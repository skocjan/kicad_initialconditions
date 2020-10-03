/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2016 CERN
 * Copyright (C) 2016-2018 KiCad Developers, see AUTHORS.txt for contributors.
 * @author Tomasz Wlostowski <tomasz.wlostowski@cern.ch>
 * @author Maciej Suminski <maciej.suminski@cern.ch>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * https://www.gnu.org/licenses/gpl-3.0.html
 * or you may search the http://www.gnu.org website for the version 3 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include <wx/stc/stc.h>
#include <wx/clipbrd.h>
#include <wx/dataobj.h>

#include <sch_edit_frame.h>
#include <kiway.h>
#include <confirm.h>
#include <bitmaps.h>
#include <wildcards_and_files_ext.h>
#include <widgets/tuner_slider.h>
#include <dialogs/dialog_signal_list.h>
#include "netlist_exporter_pspice_sim.h"
#include <pgm_base.h>
#include "sim_plot_frame.h"
#include "sim_plot_panel.h"
#include "spice_simulator.h"
#include "spice_reporter.h"
#include <menus_helpers.h>
#include <settings/common_settings.h>
#include <tool/tool_manager.h>
#include <tools/ee_actions.h>
#include <eeschema_settings.h>

SIM_PLOT_TYPE operator|( SIM_PLOT_TYPE aFirst, SIM_PLOT_TYPE aSecond )
{
    int res = (int) aFirst | (int) aSecond;

    return (SIM_PLOT_TYPE) res;
}


class SIM_THREAD_REPORTER : public SPICE_REPORTER
{
public:
    SIM_THREAD_REPORTER( SIM_PLOT_FRAME* aParent )
        : m_parent( aParent )
    {
    }

    REPORTER& Report( const wxString& aText, SEVERITY aSeverity = RPT_SEVERITY_UNDEFINED ) override
    {
        wxCommandEvent* event = new wxCommandEvent( EVT_SIM_REPORT );
        event->SetString( aText );
        wxQueueEvent( m_parent, event );
        return *this;
    }

    bool HasMessage() const override
    {
        return false;       // Technically "indeterminate" rather than false.
    }

    void OnSimStateChange( SPICE_SIMULATOR* aObject, SIM_STATE aNewState ) override
    {
        wxCommandEvent* event = NULL;

        switch( aNewState )
        {
            case SIM_IDLE:
                event = new wxCommandEvent( EVT_SIM_FINISHED );
                break;

            case SIM_RUNNING:
                event = new wxCommandEvent( EVT_SIM_STARTED );
                break;

            default:
                wxFAIL;
                return;
        }

        wxQueueEvent( m_parent, event );
    }

private:
    SIM_PLOT_FRAME* m_parent;
};


TRACE_DESC::TRACE_DESC( const NETLIST_EXPORTER_PSPICE_SIM& aExporter, const wxString& aName,
        SIM_PLOT_TYPE aType, const wxString& aParam )
    : m_name( aName ), m_type( aType ), m_param( aParam )
{
    // Title generation
    m_title = wxString::Format( "%s(%s)", aParam, aName );

    if( aType & SPT_AC_MAG )
        m_title += " (mag)";
    else if( aType & SPT_AC_PHASE )
        m_title += " (phase)";
}

// Store the path of saved workbooks during the session
wxString SIM_PLOT_FRAME::m_savedWorkbooksPath;

SIM_PLOT_FRAME::SIM_PLOT_FRAME( KIWAY* aKiway, wxWindow* aParent )
        : SIM_PLOT_FRAME_BASE( aParent ),
          m_signalContextMenu( new wxMenu() ),
          m_lastSimPlot( nullptr ),
          m_welcomePanel( nullptr ),
          m_plotNumber( 0 )
{
    SetKiway( this, aKiway );
    m_signalsIconColorList = NULL;

    m_schematicFrame = (SCH_EDIT_FRAME*) Kiway().Player( FRAME_SCH, false );

    if( m_schematicFrame == NULL )
        throw std::runtime_error( "There is no schematic window" );

    // Give an icon
    wxIcon icon;
    icon.CopyFromBitmap( KiBitmap( simulator_xpm ) );
    SetIcon( icon );

    // Get the previous size and position of windows:
    LoadSettings( config() );

    // Prepare the color list to plot traces
    fillDefaultColorList( GetPlotBgOpt() );

    // Give icons to menuitems
    setIconsForMenuItems();

    m_simulator = SPICE_SIMULATOR::CreateInstance( "ngspice" );

    if( !m_simulator )
    {
        throw std::runtime_error( "Could not create simulator instance" );
        return;
    }

    m_simulator->Init();

    if( m_savedWorkbooksPath.IsEmpty() )
    {
        m_savedWorkbooksPath = Prj().GetProjectPath();
    }

    m_reporter = new SIM_THREAD_REPORTER( this );
    m_simulator->SetReporter( m_reporter );

    updateNetlistExporter();

    Bind( EVT_SIM_UPDATE,        &SIM_PLOT_FRAME::onSimUpdate,    this );
    Bind( EVT_SIM_REPORT,        &SIM_PLOT_FRAME::onSimReport,    this );
    Bind( EVT_SIM_STARTED,       &SIM_PLOT_FRAME::onSimStarted,   this );
    Bind( EVT_SIM_FINISHED,      &SIM_PLOT_FRAME::onSimFinished,  this );
    Bind( EVT_SIM_CURSOR_UPDATE, &SIM_PLOT_FRAME::onCursorUpdate, this );

    // Toolbar buttons
    m_toolSimulate = m_toolBar->AddTool( wxID_ANY, _( "Run/Stop Simulation" ),
            KiBitmap( sim_run_xpm ), _( "Run Simulation" ), wxITEM_NORMAL );
    m_toolAddSignals = m_toolBar->AddTool( wxID_ANY, _( "Add Signals" ),
            KiBitmap( sim_add_signal_xpm ), _( "Add signals to plot" ), wxITEM_NORMAL );
    m_toolProbe = m_toolBar->AddTool( wxID_ANY,  _( "Probe" ),
            KiBitmap( sim_probe_xpm ), _( "Probe signals on the schematic" ), wxITEM_NORMAL );
    m_toolTune = m_toolBar->AddTool( wxID_ANY, _( "Tune" ),
            KiBitmap( sim_tune_xpm ), _( "Tune component values" ), wxITEM_NORMAL );
    m_toolSettings = m_toolBar->AddTool( wxID_ANY, _( "Settings" ),
            KiBitmap( sim_settings_xpm ), _( "Simulation settings" ), wxITEM_NORMAL );
    m_toolCursors = m_toolBar->AddTool( wxID_ANY, _( "Toggle Cursors" ),
            KiBitmap( sim_settings_xpm ), _( "Toggle Cursors" ), wxITEM_CHECK );

    // Bind toolbar buttons and menus event to existing menu event handlers, so they behave the same
    Bind( wxEVT_COMMAND_TOOL_CLICKED, &SIM_PLOT_FRAME::menuRunSim,    this, m_toolSimulate->GetId() );
    Bind( wxEVT_COMMAND_TOOL_CLICKED, &SIM_PLOT_FRAME::menuAddSignal, this, m_toolAddSignals->GetId() );
    Bind( wxEVT_COMMAND_TOOL_CLICKED, &SIM_PLOT_FRAME::menuProbe,     this, m_toolProbe->GetId() );
    Bind( wxEVT_COMMAND_TOOL_CLICKED, &SIM_PLOT_FRAME::menuTune,      this, m_toolTune->GetId() );
    Bind( wxEVT_COMMAND_TOOL_CLICKED, &SIM_PLOT_FRAME::menuSettings,  this, m_toolSettings->GetId() );
    Bind( wxEVT_COMMAND_TOOL_CLICKED, &SIM_PLOT_FRAME::menuCursorToggle, this, m_toolCursors->GetId() );

    m_signals->Bind( wxEVT_CONTEXT_MENU, &SIM_PLOT_FRAME::onSignalContextMenu, this );

    m_toolBar->Realize();

    m_welcomePanel = new SIM_PANEL_BASE( ST_UNKNOWN, m_plotNotebook, wxID_ANY );
    m_plotNotebook->AddPage( m_welcomePanel, _( "Welcome!" ), 1, true );

    // the settings dialog will be created later, on demand.
    // if created in the ctor, for some obscure reason, there is an issue
    // on Windows: when open it, the simulator frame is sent to the background.
    // instead of being behind the dialog frame (as it does)
    m_settingsDlg = NULL;

    // resize the subwindows size. At least on Windows, calling wxSafeYield before
    // resizing the subwindows forces the wxSplitWindows size events automatically generated
    // by wxWidgets to be executed before our resize code.
    // Otherwise, the changes made by setSubWindowsSashSize are overwritten by one these
    // events
    wxSafeYield();
    setSubWindowsSashSize();

    // Ensure the window is on top
    Raise();
}


SIM_PLOT_FRAME::~SIM_PLOT_FRAME()
{
    m_simulator->SetReporter( nullptr );
    delete m_reporter;
    delete m_signalsIconColorList;

    if( m_settingsDlg )
        m_settingsDlg->Destroy();
}


void SIM_PLOT_FRAME::SaveSettings( APP_SETTINGS_BASE* aCfg )
{
    auto cfg = dynamic_cast<EESCHEMA_SETTINGS*>( aCfg );
    wxASSERT( cfg );

    if( cfg )
    {
        EDA_BASE_FRAME::SaveSettings( cfg );

        cfg->m_Simulator.plot_panel_width     = m_splitterLeftRight->GetSashPosition();
        cfg->m_Simulator.plot_panel_height    = m_splitterPlotAndConsole->GetSashPosition();
        cfg->m_Simulator.signal_panel_height  = m_splitterSignals->GetSashPosition();
//        cfg->m_Simulator.cursors_panel_height = m_splitterTuneValues->GetSashPosition();
        cfg->m_Simulator.white_background     = m_plotUseWhiteBg;
    }
}


void SIM_PLOT_FRAME::LoadSettings( APP_SETTINGS_BASE* aCfg )
{
    auto cfg = dynamic_cast<EESCHEMA_SETTINGS*>( aCfg );
    wxASSERT( cfg );

    if( cfg )
    {
        EDA_BASE_FRAME::LoadSettings( cfg );

        // Read subwindows sizes (should be > 0 )
        m_splitterLeftRightSashPosition      = cfg->m_Simulator.plot_panel_width;
        m_splitterPlotAndConsoleSashPosition = cfg->m_Simulator.plot_panel_height;
        m_splitterSignalsSashPosition        = cfg->m_Simulator.signal_panel_height;
//        m_splitterTuneValuesSashPosition     = cfg->m_Simulator.cursors_panel_height;
        m_plotUseWhiteBg                     = cfg->m_Simulator.white_background;
    }
}


WINDOW_SETTINGS* SIM_PLOT_FRAME::GetWindowSettings( APP_SETTINGS_BASE* aCfg )
{
    auto cfg = dynamic_cast<EESCHEMA_SETTINGS*>( aCfg );
    wxASSERT( cfg );

    return cfg ? &cfg->m_Simulator.window : nullptr;
}


// A small helper struct to handle bitmaps initialisation in menus
struct BM_MENU_INIT_ITEM
{
    wxMenuItem* m_MenuItem;
    BITMAP_DEF m_Bitmap;
};


void SIM_PLOT_FRAME::setIconsForMenuItems()
{
    // Give icons to menuitems of the main menubar
    BM_MENU_INIT_ITEM bm_list[]
    {
        // File menu:
        { m_newPlot, simulator_xpm },
        { m_openWorkbook,directory_browser_xpm },
        { m_saveWorkbook, directory_xpm},
        { m_saveImage, export_xpm},
        { m_saveCsv, export_xpm},
        { m_exitSim, exit_xpm},

        // simulator menu:
        { m_runSimulation, sim_run_xpm},
        { m_probeSignals, sim_probe_xpm},
        { m_tuneValue, sim_tune_xpm},
        { m_showNetlist, netlist_xpm},
        { m_settings, sim_settings_xpm},

        // trace menu:
        { m_addSignals, sim_add_signal_xpm},
        { m_deleteSignal, delete_xpm},
        { m_showHideMenu, delete_xpm},
        { m_copyMenu, copy_xpm},
        { m_selectAllMenu, copy_xpm},

        // View menu
        { m_zoomIn, zoom_in_xpm},
        { m_zoomOut, zoom_out_xpm},
        { m_zoomFit, zoom_fit_in_page_xpm},
        { m_showGrid, grid_xpm},
        { m_showLegend, text_xpm},
        { m_showDotted, add_dashed_line_xpm},
        { m_showWhiteBackground, swap_layer_xpm},

        { nullptr, nullptr }  // Sentinel
    };

    // wxMenuItems are already created and attached to the m_mainMenu wxMenuBar.
    // A problem is the fact setting bitmaps in wxMenuItems after they are attached
    // to a wxMenu do not work in all cases.
    // So the trick is:
    // Remove the wxMenuItem from its wxMenu
    // Set the bitmap
    // Insert the modified wxMenuItem to its previous place
    for( int ii = 0; bm_list[ii].m_MenuItem; ++ii )
    {
        if( !bm_list[ii].m_Bitmap )
            continue;

        wxMenu* menu = bm_list[ii].m_MenuItem->GetMenu();
        // Calculate the initial index of item inside the wxMenu parent
        wxMenuItemList& mlist = menu->GetMenuItems();
        int mpos = mlist.IndexOf( bm_list[ii].m_MenuItem );

        if( mpos >= 0 ) // Should be always the case
        {
            // Modify the bitmap
            menu->Remove( bm_list[ii].m_MenuItem );
            AddBitmapToMenuItem( bm_list[ii].m_MenuItem, KiBitmap( bm_list[ii].m_Bitmap ) );
            // Insert item to its the initial index
            menu->Insert( mpos, bm_list[ii].m_MenuItem );
        }
    }
}


void SIM_PLOT_FRAME::setSubWindowsSashSize()
{
    if( m_splitterLeftRightSashPosition > 0 )
        m_splitterLeftRight->SetSashPosition( m_splitterLeftRightSashPosition );

    if( m_splitterPlotAndConsoleSashPosition > 0 )
        m_splitterPlotAndConsole->SetSashPosition( m_splitterPlotAndConsoleSashPosition );

    if( m_splitterSignalsSashPosition > 0 )
        m_splitterSignals->SetSashPosition( m_splitterSignalsSashPosition );

//    if( m_splitterTuneValuesSashPosition > 0 )
//        m_splitterTuneValues->SetSashPosition( m_splitterTuneValuesSashPosition );
}


wxColor SIM_PLOT_FRAME::GetPlotColor( int aColorId )
{
    // return the wxColor selected in color list or BLACK is not in list
    if( aColorId >= 0 && aColorId < (int)m_colorList.size() )
        return m_colorList[aColorId];

    return wxColor( 0, 0, 0 );
}


void SIM_PLOT_FRAME::fillDefaultColorList( bool aWhiteBg )
{
    m_colorList.clear();

    if( aWhiteBg )
    {
        m_colorList.emplace_back( 255, 255, 255 );  // Bg color
        m_colorList.emplace_back( 0, 0, 0 );        // Fg color (texts)
        m_colorList.emplace_back( 130, 130, 130 );  // Axis color
        m_colorList.emplace_back( 0, 0, 0 );        // cursors color
    }
    else
    {
        m_colorList.emplace_back( 0, 0, 0 );        // Bg color
        m_colorList.emplace_back( 255, 255, 255 );  // Fg color (texts)
        m_colorList.emplace_back( 130, 130, 130 );  // Axis color
        m_colorList.emplace_back( 255, 255, 255 );  // cursors color
    }

    // Add a list of color for traces, starting at index SIM_TRACE_COLOR
    m_colorList.emplace_back( 0xE4, 0x1A, 0x1C );
    m_colorList.emplace_back( 0x37, 0x7E, 0xB8 );
    m_colorList.emplace_back( 0x4D, 0xAF, 0x4A );
    m_colorList.emplace_back( 0x98, 0x4E, 0xA3 );
    m_colorList.emplace_back( 0xFF, 0x7F, 0x00 );
    m_colorList.emplace_back( 0xFF, 0xFF, 0x33 );
    m_colorList.emplace_back( 0xA6, 0x56, 0x28 );
    m_colorList.emplace_back( 0xF7, 0x81, 0xBF );
    m_colorList.emplace_back( 0x66, 0xC2, 0xA5 );
    m_colorList.emplace_back( 0xFC, 0x8D, 0x62 );
    m_colorList.emplace_back( 0x8D, 0xA0, 0xCB );
    m_colorList.emplace_back( 0xE7, 0x8A, 0xC3 );
    m_colorList.emplace_back( 0xA6, 0xD8, 0x54 );
    m_colorList.emplace_back( 0xFF, 0xD9, 0x2F );
    m_colorList.emplace_back( 0xE5, 0xC4, 0x94 );
    m_colorList.emplace_back( 0xB3, 0xB3, 0xB3 );

}


void SIM_PLOT_FRAME::StartSimulation( const wxString& aSimCommand )
{
    STRING_FORMATTER formatter;

    if( !m_settingsDlg )
        m_settingsDlg = new DIALOG_SIM_SETTINGS( this );

    m_simConsole->Clear();
    updateNetlistExporter();

    if( aSimCommand.IsEmpty() )
    {
        SIM_PANEL_BASE* plotPanel = currentPlotWindow();
        if( plotPanel )
            m_exporter->SetSimCommand( m_plots[plotPanel].m_simCommand );
    }
    else
    {
        m_exporter->SetSimCommand( aSimCommand );
    }

    if( !m_exporter->Format( &formatter, m_settingsDlg->GetNetlistOptions() ) )
    {
        DisplayError( this, _( "There were errors during netlist export, aborted." ) );
        return;
    }

    if( m_exporter->GetSimType() == ST_UNKNOWN )
    {
        DisplayInfoMessage( this, _( "You need to select the simulation settings first." ) );
        return;
    }

    m_simulator->LoadNetlist( formatter.GetString() );
    updateTuners();
    applyTuners();
    m_simulator->Run();
}


void SIM_PLOT_FRAME::StopSimulation()
{
    m_simulator->Stop();
}


bool SIM_PLOT_FRAME::IsSimulationRunning()
{
    return m_simulator ? m_simulator->IsRunning() : false;
}


SIM_PANEL_BASE* SIM_PLOT_FRAME::NewPlotPanel( SIM_TYPE aSimType )
{
    SIM_PANEL_BASE* plotPanel;

    if( SIM_PANEL_BASE::IsPlottable( aSimType ) )
    {
        SIM_PLOT_PANEL* panel;
        panel = new SIM_PLOT_PANEL( aSimType, m_plotNotebook, this, wxID_ANY );

        panel->GetPlotWin()->EnableMouseWheelPan(
                Pgm().GetCommonSettings()->m_Input.scroll_modifier_zoom != 0 );

        plotPanel = dynamic_cast<SIM_PANEL_BASE*>( panel );
    }
    else
    {
        SIM_NOPLOT_PANEL* panel;
        panel     = new SIM_NOPLOT_PANEL( aSimType, m_plotNotebook, wxID_ANY );
        plotPanel = dynamic_cast<SIM_PANEL_BASE*>( panel );
    }

    if( m_welcomePanel )
    {
        m_plotNotebook->DeletePage( 0 );
        m_welcomePanel = nullptr;
    }

    wxString pageTitle( m_simulator->TypeToName( aSimType, true ) );
    pageTitle.Prepend( wxString::Format( _( "Plot%u - " ), (unsigned int) ++m_plotNumber ) );

    m_plotNotebook->AddPage( dynamic_cast<wxWindow*>( plotPanel ), pageTitle, true );
    m_plots[plotPanel] = PLOT_INFO();

    return plotPanel;
}


void SIM_PLOT_FRAME::AddVoltagePlot( const wxString& aNetName )
{
    addPlot( aNetName, SPT_VOLTAGE, "V" );
}


void SIM_PLOT_FRAME::AddCurrentPlot( const wxString& aDeviceName, const wxString& aParam )
{
    addPlot( aDeviceName, SPT_CURRENT, aParam );
}


void SIM_PLOT_FRAME::AddTuner( SCH_COMPONENT* aComponent )
{
    SIM_PANEL_BASE* plotPanel = currentPlotWindow();

    if( !plotPanel || plotPanel == m_welcomePanel )
        return;

    // For now limit the tuner tool to RLC components
    char primitiveType = NETLIST_EXPORTER_PSPICE::GetSpiceField( SF_PRIMITIVE, aComponent, 0 )[0];

    if( primitiveType != SP_RESISTOR && primitiveType != SP_CAPACITOR && primitiveType != SP_INDUCTOR )
        return;

    const wxString componentName = aComponent->GetField( REFERENCE )->GetText();

    // Do not add multiple instances for the same component
    auto tunerIt = std::find_if( m_tuners.begin(), m_tuners.end(), [&]( const TUNER_SLIDER* t )
        {
            return t->GetComponentName() == componentName;
        }
    );

    if( tunerIt != m_tuners.end() )
        return;     // We already have it

    try
    {
        TUNER_SLIDER* tuner = new TUNER_SLIDER( this, m_tunePanel, aComponent );
        m_tuneSizer->Add( tuner );
        m_tuners.push_back( tuner );
        m_tunePanel->Layout();
    }
    catch( const KI_PARAM_ERROR& e )
    {
        // Sorry, no bonus
        DisplayError( nullptr, e.What() );
    }
}


void SIM_PLOT_FRAME::RemoveTuner( TUNER_SLIDER* aTuner, bool aErase )
{
    if( aErase )
        m_tuners.remove( aTuner );

    aTuner->Destroy();
    m_tunePanel->Layout();
}


SIM_PLOT_PANEL* SIM_PLOT_FRAME::CurrentPlot() const
{
    SIM_PANEL_BASE* curPage = currentPlotWindow();

    return ( ( !curPage || curPage->GetType() == ST_UNKNOWN ) ?
                     nullptr :
                     dynamic_cast<SIM_PLOT_PANEL*>( curPage ) );
}


const NETLIST_EXPORTER_PSPICE_SIM* SIM_PLOT_FRAME::GetExporter() const
{
    return m_exporter.get();
}


void SIM_PLOT_FRAME::addPlot( const wxString& aName, SIM_PLOT_TYPE aType, const wxString& aParam )
{
    SIM_TYPE simType = m_exporter->GetSimType();

    if( simType == ST_UNKNOWN )
    {
        m_simConsole->AppendText( _( "Error: simulation type not defined!\n" ) );
        m_simConsole->SetInsertionPointEnd();
        return;
    }
    else if( !SIM_PANEL_BASE::IsPlottable( simType ) )
    {
        m_simConsole->AppendText( _( "Error: simulation type doesn't support plotting!\n" ) );
        m_simConsole->SetInsertionPointEnd();
        return;
    }

    // Create a new plot if the current one displays a different type
    SIM_PLOT_PANEL* plotPanel = CurrentPlot();

    if( !plotPanel || plotPanel->GetType() != simType )
        plotPanel = dynamic_cast<SIM_PLOT_PANEL*>( NewPlotPanel( simType ) );

    wxASSERT( plotPanel );

    if( !plotPanel )    // Something is wrong
        return;

    TRACE_DESC descriptor( *m_exporter, aName, aType, aParam );

    bool updated = false;
    SIM_PLOT_TYPE xAxisType = GetXAxisType( simType );

    if( xAxisType == SPT_LIN_FREQUENCY || xAxisType == SPT_LOG_FREQUENCY )
    {
        int baseType = descriptor.GetType() & ~( SPT_AC_MAG | SPT_AC_PHASE );

        // Add two plots: magnitude & phase
        TRACE_DESC mag_desc( *m_exporter, descriptor, (SIM_PLOT_TYPE)( baseType | SPT_AC_MAG ) );
        TRACE_DESC phase_desc( *m_exporter, descriptor, (SIM_PLOT_TYPE)( baseType | SPT_AC_PHASE ) );

        updated |= updatePlot( mag_desc, plotPanel );
        updated |= updatePlot( phase_desc, plotPanel );
    }
    else
    {
        updated = updatePlot( descriptor, plotPanel );
    }

    if( updated )
    {
        updateSignalList();
    }
}


void SIM_PLOT_FRAME::removePlot( const wxString& aPlotName, bool aErase )
{
    SIM_PLOT_PANEL* plotPanel = CurrentPlot();

    if( !plotPanel )
        return;

    if( aErase )
    {
        auto& traceMap = m_plots[plotPanel].m_traces;
        auto traceIt = traceMap.find( aPlotName );
        wxASSERT( traceIt != traceMap.end() );
        traceMap.erase( traceIt );
    }

    wxASSERT( plotPanel->TraceShown( aPlotName ) );
    plotPanel->DeleteTrace( aPlotName );
    plotPanel->GetPlotWin()->Fit();

    updateSignalList();
}


void SIM_PLOT_FRAME::updateNetlistExporter()
{
    m_exporter.reset( new NETLIST_EXPORTER_PSPICE_SIM( &m_schematicFrame->Schematic() ) );
}


bool SIM_PLOT_FRAME::updatePlot( const TRACE_DESC& aDescriptor, SIM_PLOT_PANEL* aPanel )
{
    SIM_TYPE simType = m_exporter->GetSimType();
    wxString spiceVector = m_exporter->ComponentToVector(
            aDescriptor.GetName(), aDescriptor.GetType(), aDescriptor.GetParam() );

    if( !SIM_PANEL_BASE::IsPlottable( simType ) )
    {
        // There is no plot to be shown
        m_simulator->Command( wxString::Format( "print %s", spiceVector ).ToStdString() );

        return false;
    }

    // First, handle the x axis
    wxString xAxisName( m_simulator->GetXAxis( simType ) );

    if( xAxisName.IsEmpty() )
        return false;

    auto data_x = m_simulator->GetMagPlot( (const char*) xAxisName.c_str() );
    unsigned int size = data_x.size();

    if( data_x.empty() )
        return false;

    SIM_PLOT_TYPE plotType = aDescriptor.GetType();
    std::vector<double> data_y;

    // Now, Y axis data
    switch( m_exporter->GetSimType() )
    {
        case ST_AC:
        {
            wxASSERT_MSG( !( ( plotType & SPT_AC_MAG ) && ( plotType & SPT_AC_PHASE ) ),
                    "Cannot set both AC_PHASE and AC_MAG bits" );

            if( plotType & SPT_AC_MAG )
                data_y = m_simulator->GetMagPlot( (const char*) spiceVector.c_str() );
            else if( plotType & SPT_AC_PHASE )
                data_y = m_simulator->GetPhasePlot( (const char*) spiceVector.c_str() );
            else
                wxASSERT_MSG( false, "Plot type missing AC_PHASE or AC_MAG bit" );
        }
        break;

        case ST_NOISE:
        case ST_DC:
        case ST_TRANSIENT:
        {
            data_y = m_simulator->GetMagPlot( (const char*) spiceVector.c_str() );
        }
        break;

        default:
            wxASSERT_MSG( false, "Unhandled plot type" );
            return false;
    }

    if( data_y.size() != size )
        return false;

    // If we did a two-source DC analysis, we need to split the resulting vector and add traces
    // for each input step
    SPICE_DC_PARAMS source1, source2;

    if( m_exporter->GetSimType() == ST_DC &&
        m_exporter->ParseDCCommand( m_exporter->GetUsedSimCommand(), &source1, &source2 ) )
    {
        if( !source2.m_source.IsEmpty() )
        {
            // Source 1 is the inner loop, so lets add traces for each Source 2 (outer loop) step
            SPICE_VALUE v = source2.m_vstart;
            wxString name;

            size_t offset = 0;
            size_t outer = ( size_t )( ( source2.m_vend - v ) / source2.m_vincrement ).ToDouble();
            size_t inner = data_x.size() / ( outer + 1 );

            wxASSERT( data_x.size() % ( outer + 1 ) == 0 );

            for( size_t idx = 0; idx <= outer; idx++ )
            {
                name = wxString::Format( "%s (%s = %s V)", aDescriptor.GetTitle(),
                                         source2.m_source, v.ToString() );

                std::vector<double> sub_x( data_x.begin() + offset,
                                           data_x.begin() + offset + inner );
                std::vector<double> sub_y( data_y.begin() + offset,
                                           data_y.begin() + offset + inner );

                if( aPanel->AddTrace( name, inner,
                                      sub_x.data(), sub_y.data(), aDescriptor.GetType() ) )
                {
                    m_plots[aPanel].m_traces.insert( std::make_pair( name, aDescriptor ) );
                }

                v = v + source2.m_vincrement;
                offset += inner;
            }

            return true;
        }
    }

    if( aPanel->AddTrace( aDescriptor.GetTitle(), size,
                data_x.data(), data_y.data(), aDescriptor.GetType() ) )
    {
        m_plots[aPanel].m_traces.insert( std::make_pair( aDescriptor.GetTitle(), aDescriptor ) );
    }

    return true;
}


void SIM_PLOT_FRAME::updateSignalList()
{
    m_signals->ClearAll();

    SIM_PLOT_PANEL* plotPanel = CurrentPlot();

    if( !plotPanel )
        return;

    wxSize size = m_signals->GetClientSize();

    wxChar firstCursorLabel, secondCursorLabel;
    double cursorValue[2];

    // Set the labels on the column headers
    bool drawCursors = plotPanel->AreCursorsActive( firstCursorLabel, secondCursorLabel );
    if( drawCursors )
    {
        wxString label;

        m_signals->AppendColumn( _( "Signal" ), wxLIST_FORMAT_LEFT, size.x / 4 );

        label.Printf( _( "%s at %c: " ), plotPanel->GetLabelX(), firstCursorLabel );
        cursorValue[0] = plotPanel->GetCursorPos( firstCursorLabel );
        label.Append( SPICE_VALUE( cursorValue[0] ).ToSpiceString() );
        label.Append( plotPanel->GetUnitX() );
        m_signals->AppendColumn( label, wxLIST_FORMAT_LEFT, size.x / 4 );

        label.Printf( _( "%s at %c: " ), plotPanel->GetLabelX(), secondCursorLabel );
        cursorValue[1] = plotPanel->GetCursorPos( secondCursorLabel );
        label.Append( SPICE_VALUE( cursorValue[1] ).ToSpiceString() );
        label.Append( plotPanel->GetUnitX() );
        m_signals->AppendColumn( label, wxLIST_FORMAT_LEFT, size.x / 4 );

        label.Printf( _( "Difference (%c - %c): " ), plotPanel->GetLabelX(), secondCursorLabel, firstCursorLabel );
        label.Append( SPICE_VALUE( cursorValue[1] - cursorValue[0] ).ToSpiceString() );
        label.Append( plotPanel->GetUnitX() );
        m_signals->AppendColumn( label, wxLIST_FORMAT_LEFT, size.x / 4 );
    }
    else
    {
        m_signals->AppendColumn( _( "Signal" ), wxLIST_FORMAT_LEFT, size.x );
    }

    // Build an image list, to show the color of the corresponding trace
    // in the plot panel
    // This image list is used for trace and cursor lists
    wxMemoryDC bmDC;
    const int isize = bmDC.GetCharHeight();

    if( m_signalsIconColorList == NULL )
        m_signalsIconColorList = new wxImageList( isize, isize, false );
    else
        m_signalsIconColorList->RemoveAll();

    for( const auto& trace : plotPanel->GetTraces() )
    {
        wxBitmap bitmap( isize, isize );
        bmDC.SelectObject( bitmap );
        //wxColour tcolor = trace.second->GetTraceColour();
        wxColour tcolor = trace.second->GetPen().GetColour();

        wxColour bgColor = m_signals->wxWindow::GetBackgroundColour();
        bmDC.SetPen( wxPen( bgColor ) );
        bmDC.SetBrush( wxBrush( bgColor ) );
        bmDC.DrawRectangle( 0, 0, isize, isize ); // because bmDC.Clear() does not work in wxGTK

        bmDC.SetPen( wxPen( tcolor ) );
        bmDC.SetBrush( wxBrush( tcolor ) );
        bmDC.DrawRectangle( 0, isize / 4 + 1, isize, isize / 2 );

        bmDC.SelectObject( wxNullBitmap );  // Needed to initialize bitmap

        bitmap.SetMask( new wxMask( bitmap, *wxBLACK ) );
        m_signalsIconColorList->Add( bitmap );
    }

    if( bmDC.IsOk() )
    {
        bmDC.SetBrush( wxNullBrush );
        bmDC.SetPen( wxNullPen );
    }

    m_signals->SetImageList( m_signalsIconColorList, wxIMAGE_LIST_SMALL );

    // Fill the signals listctrl. Keep the order of names and
    // the order of icon color identical, because the icons
    // are also used in cursor list, and the color index is
    // calculated from the trace name index
    int imgidx = 0;

    for( const auto& trace : plotPanel->GetTraces() )
    {
        m_signals->InsertItem( imgidx, trace.first, imgidx );

        if( drawCursors )
        {
            cursorValue[0] = trace.second->GetCursorValue( 'A' );
            cursorValue[1] = trace.second->GetCursorValue( 'B' );

            m_signals->SetItem( imgidx, 1, SPICE_VALUE( cursorValue[0] ).ToSpiceString() );
            m_signals->SetItem( imgidx, 2, SPICE_VALUE( cursorValue[1] ).ToSpiceString() );
            m_signals->SetItem( imgidx, 3, SPICE_VALUE( cursorValue[1] - cursorValue[0] )
                            .ToSpiceString() );
        }

        imgidx++;
    }
}


void SIM_PLOT_FRAME::updateTuners()
{
    const auto& spiceItems = m_exporter->GetSpiceItems();

    for( auto it = m_tuners.begin(); it != m_tuners.end(); /* iteration inside the loop */ )
    {
        const wxString& ref = (*it)->GetComponentName();

        if( std::find_if( spiceItems.begin(), spiceItems.end(), [&]( const SPICE_ITEM& item )
                {
                    return item.m_refName == ref;
                }) == spiceItems.end() )
        {
            // The component does not exist anymore, remove the associated tuner
            TUNER_SLIDER* tuner = *it;
            it = m_tuners.erase( it );
            RemoveTuner( tuner, false );
        }
        else
        {
            ++it;
        }
    }
}


void SIM_PLOT_FRAME::applyTuners()
{
    for( auto& tuner : m_tuners )
    {
        /// @todo no ngspice hardcoding
        std::string command( "alter @" + tuner->GetSpiceName()
                + "=" + tuner->GetValue().ToSpiceString() );

        m_simulator->Command( command );
    }
}


bool SIM_PLOT_FRAME::loadWorkbook( const wxString& aPath )
{
    m_plots.clear();
    m_plotNotebook->DeleteAllPages();

    wxTextFile file( aPath );

    if( !file.Open() )
        return false;

    long plotsCount;

    if( !file.GetFirstLine().ToLong( &plotsCount ) )        // GetFirstLine instead of GetNextLine
        return false;

    for( long i = 0; i < plotsCount; ++i )
    {
        long plotType, tracesCount;

        if( !file.GetNextLine().ToLong( &plotType ) )
            return false;

        SIM_PANEL_BASE* plotPanel  = NewPlotPanel( (SIM_TYPE) plotType );
        m_plots[plotPanel].m_simCommand = file.GetNextLine();
        StartSimulation( m_plots[plotPanel].m_simCommand );

        // Perform simulation, so plots can be added with values
        do
        {
            wxThread::This()->Sleep( 50 );
        }
        while( IsSimulationRunning() );

        if( !file.GetNextLine().ToLong( &tracesCount ) )
            return false;

        for( long j = 0; j < tracesCount; ++j )
        {
            long traceType;
            wxString name, param;

            if( !file.GetNextLine().ToLong( &traceType ) )
                return false;

            name = file.GetNextLine();
            param = file.GetNextLine();

            if( name.IsEmpty() || param.IsEmpty() )
                return false;

            addPlot( name, (SIM_PLOT_TYPE) traceType, param );
        }
    }

    return true;
}


bool SIM_PLOT_FRAME::saveWorkbook( const wxString& aPath )
{

    wxString savePath = aPath;

    if( !savePath.Lower().EndsWith(".wbk"))
    {
        savePath += ".wbk";
    };


    wxTextFile file( savePath );

    if( file.Exists() )
    {
        if( !file.Open() )
            return false;

        file.Clear();
    }
    else
    {
        file.Create();
    }

    file.AddLine( wxString::Format( "%llu", m_plots.size() ) );

    for( const auto& plot : m_plots )
    {
        if( plot.first )
        {
            file.AddLine( wxString::Format( "%d", plot.first->GetType() ) );
            file.AddLine( plot.second.m_simCommand );
            file.AddLine( wxString::Format( "%llu", plot.second.m_traces.size() ) );

            for( const auto& trace : plot.second.m_traces )
            {
                file.AddLine( wxString::Format( "%d", trace.second.GetType() ) );
                file.AddLine( trace.second.GetName() );
                file.AddLine( trace.second.GetParam() );
            }
        }
    }

    bool res = file.Write();
    file.Close();

    return res;
}


SIM_PLOT_TYPE SIM_PLOT_FRAME::GetXAxisType( SIM_TYPE aType ) const
{
    switch( aType )
    {
        case ST_AC:
            return SPT_LIN_FREQUENCY;
            /// @todo SPT_LOG_FREQUENCY

        case ST_DC:
            return SPT_SWEEP;

        case ST_TRANSIENT:
            return SPT_TIME;

        default:
            wxASSERT_MSG( false, "Unhandled simulation type" );
            return (SIM_PLOT_TYPE) 0;
    }
}


void SIM_PLOT_FRAME::menuNewPlot( wxCommandEvent& aEvent )
{
    SIM_TYPE type = m_exporter->GetSimType();

    if( SIM_PANEL_BASE::IsPlottable( type ) )
    {
        SIM_PLOT_PANEL* prevPlot = CurrentPlot();
        SIM_PLOT_PANEL* newPlot  = dynamic_cast<SIM_PLOT_PANEL*>( NewPlotPanel( type ) );

        // If the previous plot had the same type, copy the simulation command
        if( prevPlot )
            m_plots[newPlot].m_simCommand = m_plots[prevPlot].m_simCommand;
    }
}


void SIM_PLOT_FRAME::menuOpenWorkbook( wxCommandEvent& event )
{
    wxFileDialog openDlg( this, _( "Open simulation workbook" ), m_savedWorkbooksPath, "",
                          WorkbookFileWildcard(), wxFD_OPEN | wxFD_FILE_MUST_EXIST );

    if( openDlg.ShowModal() == wxID_CANCEL )
        return;

    m_savedWorkbooksPath = openDlg.GetDirectory();

    if( !loadWorkbook( openDlg.GetPath() ) )
        DisplayError( this, _( "There was an error while opening the workbook file" ) );
}


void SIM_PLOT_FRAME::menuSaveWorkbook( wxCommandEvent& event )
{
    if( !CurrentPlot() )
        return;

    wxFileDialog saveDlg( this, _( "Save Simulation Workbook" ), m_savedWorkbooksPath, "",
                          WorkbookFileWildcard(), wxFD_SAVE | wxFD_OVERWRITE_PROMPT );

    if( saveDlg.ShowModal() == wxID_CANCEL )
        return;

    m_savedWorkbooksPath = saveDlg.GetDirectory();

    if( !saveWorkbook( saveDlg.GetPath() ) )
        DisplayError( this, _( "There was an error while saving the workbook file" ) );
}


void SIM_PLOT_FRAME::menuSaveImage( wxCommandEvent& event )
{
    if( !CurrentPlot() )
        return;

    wxFileDialog saveDlg( this, _( "Save Plot as Image" ), "", "",
                          PngFileWildcard(), wxFD_SAVE | wxFD_OVERWRITE_PROMPT );

    if( saveDlg.ShowModal() == wxID_CANCEL )
        return;

    CurrentPlot()->GetPlotWin()->SaveScreenshot( saveDlg.GetPath(), wxBITMAP_TYPE_PNG );
}


void SIM_PLOT_FRAME::menuSaveCsv( wxCommandEvent& event )
{
    if( !CurrentPlot() )
        return;

    const wxChar SEPARATOR = ';';

    wxFileDialog saveDlg( this, _( "Save Plot Data" ), "", "",
                          CsvFileWildcard(), wxFD_SAVE | wxFD_OVERWRITE_PROMPT );

    if( saveDlg.ShowModal() == wxID_CANCEL )
        return;

    wxFile out( saveDlg.GetPath(), wxFile::write );
    bool timeWritten = false;

    for( const auto& t : CurrentPlot()->GetTraces() )
    {
        const TRACE* trace = t.second;

        if( !timeWritten )
        {
            out.Write( wxString::Format( "Time%c", SEPARATOR ) );

            for( double v : trace->GetDataX() )
                out.Write( wxString::Format( "%f%c", v, SEPARATOR ) );

            out.Write( "\r\n" );
            timeWritten = true;
        }

        out.Write( wxString::Format( "%s%c", t.first, SEPARATOR ) );

        for( double v : trace->GetDataY() )
            out.Write( wxString::Format( "%f%c", v, SEPARATOR ) );

        out.Write( "\r\n" );
    }

    out.Close();
}


void SIM_PLOT_FRAME::menuZoomIn( wxCommandEvent& event )
{
    if( CurrentPlot() )
        CurrentPlot()->GetPlotWin()->ZoomIn();
}


void SIM_PLOT_FRAME::menuZoomOut( wxCommandEvent& event )
{
    if( CurrentPlot() )
        CurrentPlot()->GetPlotWin()->ZoomOut();
}


void SIM_PLOT_FRAME::menuZoomFit( wxCommandEvent& event )
{
    if( CurrentPlot() )
        CurrentPlot()->GetPlotWin()->Fit();
}


void SIM_PLOT_FRAME::menuShowGrid( wxCommandEvent& event )
{
    SIM_PLOT_PANEL* plot = CurrentPlot();

    if( plot )
        plot->ShowGrid( !plot->IsGridShown() );
}


void SIM_PLOT_FRAME::menuShowGridUpdate( wxUpdateUIEvent& event )
{
    SIM_PLOT_PANEL* plot = CurrentPlot();

    event.Check( plot ? plot->IsGridShown() : false );
}


void SIM_PLOT_FRAME::menuShowLegend( wxCommandEvent& event )
{
    SIM_PLOT_PANEL* plot = CurrentPlot();

    if( plot )
        plot->ShowLegend( !plot->IsLegendShown() );
}


void SIM_PLOT_FRAME::menuShowLegendUpdate( wxUpdateUIEvent& event )
{
    SIM_PLOT_PANEL* plot = CurrentPlot();
    event.Check( plot ? plot->IsLegendShown() : false );
}


void SIM_PLOT_FRAME::menuShowDotted( wxCommandEvent& event )
{
    SIM_PLOT_PANEL* plot = CurrentPlot();

    if( plot )
        plot->SetDottedCurrentPhase( !plot->GetDottedCurrentPhase() );
}


void SIM_PLOT_FRAME::menuShowDottedUpdate( wxUpdateUIEvent& event )
{
    SIM_PLOT_PANEL* plot = CurrentPlot();

    event.Check( plot ? plot->GetDottedCurrentPhase() : false );
}


void SIM_PLOT_FRAME::menuWhiteBackground( wxCommandEvent& event )
{
    m_plotUseWhiteBg = not m_plotUseWhiteBg;

    // Rebuild the color list to plot traces
    fillDefaultColorList( GetPlotBgOpt() );

    // Now send changes to all SIM_PLOT_PANEL
    for( size_t page = 0; page < m_plotNotebook->GetPageCount(); page++ )
    {
        wxWindow* curPage = m_plotNotebook->GetPage( page );

        if( curPage == m_welcomePanel )
            continue;

        static_cast<SIM_PLOT_PANEL*>( curPage )->UpdatePlotColors();
    }
}


void SIM_PLOT_FRAME::onPlotClose( wxAuiNotebookEvent& event )
{
    int idx = event.GetSelection();

    if( idx == wxNOT_FOUND )
        return;

    SIM_PANEL_BASE* plotPanel =
            dynamic_cast<SIM_PANEL_BASE*>( m_plotNotebook->GetPage( idx ) );

    m_plots.erase( plotPanel );
    updateSignalList();
}


void SIM_PLOT_FRAME::onPlotChanged( wxAuiNotebookEvent& event )
{
    updateSignalList();
}


void SIM_PLOT_FRAME::onSignalDblClick( wxMouseEvent& event )
{
    // Remove signal from the plot panel when double clicked
    wxCommandEvent dummy;
    menuRemoveSignal( dummy );
}


void SIM_PLOT_FRAME::menuRemoveSignal( wxCommandEvent& event )
{
    long idx = m_signals->GetFirstSelected();
    std::vector<wxString> itemsToDelete;

    while( idx != wxNOT_FOUND )
    {
        itemsToDelete.push_back( m_signals->GetItemText( idx, 0 ) );
        idx = m_signals->GetNextSelected( idx );
    }

    for( auto signal : itemsToDelete )
        removePlot( signal );
}


void SIM_PLOT_FRAME::menuCopySignal( wxCommandEvent& event )
{
    int noOfColumns = m_signals->GetColumnCount();
    wxString clipboardData( wxEmptyString );
    wxString line( wxEmptyString );

    // Attach info from columns
    for( int col = 0; col < noOfColumns; col++ )
    {
        wxListItem item;
        item.SetColumn( col );
        item.SetMask( wxLIST_MASK_TEXT );

        m_signals->GetColumn( col , item );
        line.Append( item.GetText() + wxT( ";" ) );
    }

    clipboardData = line.Append( wxT( "\n" ) );
    line.Clear();

    // Attach info from selected signals
    long row = m_signals->GetFirstSelected();
    while( row != wxNOT_FOUND )
    {
        for( int col = 0; col < noOfColumns; col++ )
            line.Append( m_signals->GetItemText( row, col ) + wxT( ";" ) );

        clipboardData.Append( line.Append( wxT( "\n" ) ) );
        line.Clear();

        row = m_signals->GetNextSelected( row );
    }

    // Move data to clipboard
    if( wxTheClipboard->Open() )
    {
        wxTheClipboard->SetData( new wxTextDataObject( clipboardData ) );
        wxTheClipboard->Close();
    }
}


void SIM_PLOT_FRAME::menuSelectAllSignals( wxCommandEvent& event )
{
    int noOfSignals = m_signals->GetItemCount();
    for( int i = 0; i < noOfSignals; i++ )
        m_signals->SetItemState( i, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
}


void SIM_PLOT_FRAME::onSignalContextMenu( wxContextMenuEvent& event )
{
    int idx = m_signals->GetFocusedItem();

    if( idx != wxNOT_FOUND )
    {
        const wxString& netName = m_signals->GetItemText( idx, 0 );
        prepareSignalContextMenu( netName );
        m_signals->PopupMenu( m_signalContextMenu );
    }
}


void SIM_PLOT_FRAME::onSignalListResize( wxSizeEvent& event )
{
    wxSize size = m_signals->GetClientSize();

    SIM_PLOT_PANEL* plotPanel = CurrentPlot();
    if( plotPanel )
    {
        wxChar dummy1, dummy2;
        int noOfColumns;

        if( plotPanel->AreCursorsActive( dummy1, dummy2) )
        {
            noOfColumns = 4;
        }
        else
        {
            noOfColumns = 1;
        }

        for( int i = 0; i < noOfColumns; i++ )
            m_signals->SetColumnWidth( i, size.x / noOfColumns );
    }
}


void SIM_PLOT_FRAME::menuRunSim( wxCommandEvent& event )
{
    if( IsSimulationRunning() )
        StopSimulation();
    else
        StartSimulation();
}


void SIM_PLOT_FRAME::menuCursorToggle( wxCommandEvent& event )
{
    SIM_PLOT_PANEL* plotPanel = CurrentPlot();
    if( plotPanel )
        plotPanel->ToggleCursors();
}


void SIM_PLOT_FRAME::menuSettings( wxCommandEvent& event )
{
    SIM_PANEL_BASE* plotPanelWindow = currentPlotWindow();

    // Initial processing is required to e.g. display a list of power sources
    updateNetlistExporter();

    if( !m_exporter->ProcessNetlist( NET_ALL_FLAGS ) )
    {
        DisplayError( this, _( "There were errors during netlist export, aborted." ) );
        return;
    }

    if( !m_settingsDlg )
        m_settingsDlg = new DIALOG_SIM_SETTINGS( this );

    if( plotPanelWindow != m_welcomePanel )
        m_settingsDlg->SetSimCommand( m_plots[plotPanelWindow].m_simCommand );

    m_settingsDlg->SetNetlistExporter( m_exporter.get() );

    if( m_settingsDlg->ShowModal() == wxID_OK )
    {
        wxString newCommand = m_settingsDlg->GetSimCommand();
        SIM_TYPE newSimType = NETLIST_EXPORTER_PSPICE_SIM::CommandToSimType( newCommand );

        // If it is a new simulation type, open a new plot
        if( !plotPanelWindow || ( plotPanelWindow && plotPanelWindow->GetType() != newSimType ) )
        {
            plotPanelWindow = NewPlotPanel( newSimType );
        }

        m_plots[plotPanelWindow].m_simCommand = newCommand;
    }
}


void SIM_PLOT_FRAME::menuAddSignal( wxCommandEvent& event )
{
    SIM_PLOT_PANEL* plotPanel = CurrentPlot();

    if( !plotPanel || !m_exporter || plotPanel->GetType() != m_exporter->GetSimType() )
    {
        DisplayInfoMessage( this, _( "You need to run plot-providing simulation first." ) );
        return;
    }

    DIALOG_SIGNAL_LIST dialog( this, m_exporter.get() );
    dialog.ShowModal();
}


void SIM_PLOT_FRAME::menuProbe( wxCommandEvent& event )
{
    if( m_schematicFrame == NULL )
        return;

    m_schematicFrame->GetToolManager()->RunAction( EE_ACTIONS::simProbe );
    m_schematicFrame->Raise();
}


void SIM_PLOT_FRAME::menuTune( wxCommandEvent& event )
{
    if( m_schematicFrame == NULL )
        return;

    m_schematicFrame->GetToolManager()->RunAction( EE_ACTIONS::simTune );
    m_schematicFrame->Raise();
}

void SIM_PLOT_FRAME::menuShowNetlist( wxCommandEvent& event )
{
    class NETLIST_VIEW_DIALOG : public wxDialog
    {
    public:
        enum
        {
            MARGIN_LINE_NUMBERS
        };

        void onClose( wxCloseEvent& evt )
        {
            EndModal( GetReturnCode() );
        }

        NETLIST_VIEW_DIALOG(wxWindow* parent, wxString source) :
            wxDialog(parent, wxID_ANY, "SPICE Netlist",
                     wxDefaultPosition, wxSize(1500,900),
                     wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
        {
            wxStyledTextCtrl* text = new wxStyledTextCtrl( this, wxID_ANY );

            text->SetMarginWidth( MARGIN_LINE_NUMBERS, 50 );
            text->StyleSetForeground( wxSTC_STYLE_LINENUMBER, wxColour( 75, 75, 75 ) );
            text->StyleSetBackground( wxSTC_STYLE_LINENUMBER, wxColour( 220, 220, 220 ) );
            text->SetMarginType( MARGIN_LINE_NUMBERS, wxSTC_MARGIN_NUMBER );

            text->SetWrapMode( wxSTC_WRAP_WORD );

            text->SetText( source );

            text->StyleClearAll();
            text->SetLexer( wxSTC_LEX_SPICE );

            wxBoxSizer* sizer = new wxBoxSizer( wxVERTICAL );
            sizer->Add( text, 1, wxEXPAND );
            SetSizer( sizer );

            Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( NETLIST_VIEW_DIALOG::onClose ), NULL,
                    this );
        }
    };

    if( m_schematicFrame == NULL || m_simulator == NULL )
        return;

    NETLIST_VIEW_DIALOG dlg( this, m_simulator->GetNetlist() );
    dlg.ShowModal();
}


void SIM_PLOT_FRAME::doCloseWindow()
{
    SaveSettings( config() );

    if( IsSimulationRunning() )
        m_simulator->Stop();

    // Cancel a running simProbe or simTune tool
    m_schematicFrame->GetToolManager()->RunAction( ACTIONS::cancelInteractive );

    Destroy();
}


void SIM_PLOT_FRAME::onCursorUpdate( wxCommandEvent& event )
{
    updateSignalList();
}


void SIM_PLOT_FRAME::onSimStarted( wxCommandEvent& aEvent )
{
    m_toolBar->SetToolNormalBitmap( m_runSimulation->GetId(), KiBitmap( sim_stop_xpm ) );
    SetCursor( wxCURSOR_ARROWWAIT );
}


void SIM_PLOT_FRAME::onSimFinished( wxCommandEvent& aEvent )
{
    m_toolBar->SetToolNormalBitmap( m_runSimulation->GetId(), KiBitmap( sim_run_xpm ) );
    SetCursor( wxCURSOR_ARROW );

    SIM_TYPE simType = m_exporter->GetSimType();

    if( simType == ST_UNKNOWN )
        return;

    SIM_PANEL_BASE* plotPanelWindow = currentPlotWindow();

    if( !plotPanelWindow || plotPanelWindow->GetType() != simType )
        plotPanelWindow = NewPlotPanel( simType );

    if( IsSimulationRunning() )
        return;

    // If there are any signals plotted, update them
    if( SIM_PANEL_BASE::IsPlottable( simType ) )
    {
        TRACE_MAP&      traceMap  = m_plots[plotPanelWindow].m_traces;
        SIM_PLOT_PANEL* plotPanel = dynamic_cast<SIM_PLOT_PANEL*>( plotPanelWindow );

        wxCHECK_RET( plotPanel, "not a SIM_PLOT_PANEL"  );

        for( auto it = traceMap.begin(); it != traceMap.end(); /* iteration occurs in the loop */)
        {
            if( !updatePlot( it->second, plotPanel ) )
            {
                removePlot( it->first, false );
                it = traceMap.erase( it );       // remove a plot that does not exist anymore
            }
            else
            {
                ++it;
            }
        }

        updateSignalList();
        plotPanel->GetPlotWin()->UpdateAll();
        plotPanel->ResetScales();
    }
    else if( simType == ST_OP )
    {
        m_simConsole->AppendText( _( "\n\nSimulation results:\n\n" ) );
        m_simConsole->SetInsertionPointEnd();

        for( const auto& vec : m_simulator->AllPlots() )
        {
            double val = m_simulator->GetRealPlot( vec, 1 ).at( 0 );

            wxString      outLine, signal;
            SIM_PLOT_TYPE type = m_exporter->VectorToSignal( vec, signal );

            const size_t tab     = 25; //characters
            size_t padding = ( signal.length() < tab ) ? ( tab - signal.length() ) : 1;

            outLine.Printf( wxT( "%s%s" ), ( signal + wxT( ":" ) ).Pad( padding, wxUniChar( ' ' ) ),
                    SPICE_VALUE( val ).ToSpiceString() );

            outLine.Append( type == SPT_CURRENT ? "A\n" : "V\n" );

            m_simConsole->AppendText( outLine );
            m_simConsole->SetInsertionPointEnd();

            // @todo display calculated values on the schematic
        }
    }
}


void SIM_PLOT_FRAME::onSimUpdate( wxCommandEvent& aEvent )
{
    if( IsSimulationRunning() )
        StopSimulation();

    if( CurrentPlot() != m_lastSimPlot )
    {
        // We need to rerun simulation, as the simulator currently stores
        // results for another plot
        StartSimulation();
    }
    else
    {
        // Incremental update
        m_simConsole->Clear();
        // Do not export netlist, it is already stored in the simulator
        applyTuners();
        m_simulator->Run();
    }
}


void SIM_PLOT_FRAME::onSimReport( wxCommandEvent& aEvent )
{
    m_simConsole->AppendText( aEvent.GetString() + "\n" );
    m_simConsole->SetInsertionPointEnd();
}


//SIM_PLOT_FRAME::SIGNAL_CONTEXT_MENU::SIGNAL_CONTEXT_MENU( const wxString& aSignal,
//        SIM_PLOT_FRAME* aPlotFrame )
//    : m_signal( aSignal ), m_plotFrame( aPlotFrame )
void SIM_PLOT_FRAME::prepareSignalContextMenu( const wxString& aSignal )
{
//    SIM_PLOT_PANEL* plot = m_plotFrame->CurrentPlot();
    wxMenuItem* item;

    // remove all from context menu
    while( m_signalContextMenu->GetMenuItemCount() > 0 )
    {
        item = m_signalContextMenu->FindItemByPosition( 0 );
        m_signalContextMenu->Destroy( item );
    }

    item = AddMenuItem( m_signalContextMenu, wxID_ANY,
                    m_deleteSignal->GetItemLabel(),
                    m_deleteSignal->GetHelp(),
                    m_deleteSignal->GetBitmap() );
    m_signalContextMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &SIM_PLOT_FRAME::menuRemoveSignal,
                    this, item->GetId() );
#if 1
    item = AddMenuItem( m_signalContextMenu, wxID_ANY,
                    m_showHideMenu->GetItemLabel(),
                    m_showHideMenu->GetHelp(),
                    m_showHideMenu->GetBitmap() );
    m_signalContextMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &SIM_PLOT_FRAME::menuShowHideSignal,
                    this, item->GetId() );

    item = AddMenuItem( m_signalContextMenu, wxID_ANY,
                    m_copyMenu->GetItemLabel(),
                    m_copyMenu->GetHelp(),
                    m_copyMenu->GetBitmap() );
    m_signalContextMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &SIM_PLOT_FRAME::menuCopySignal,
                    this, item->GetId() );

    item = AddMenuItem( m_signalContextMenu, wxID_ANY,
                    m_selectAllMenu->GetItemLabel(),
                    m_selectAllMenu->GetHelp(),
                    m_selectAllMenu->GetBitmap() );
    m_signalContextMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &SIM_PLOT_FRAME::menuSelectAllSignals,
                    this, item->GetId() );
#endif
    /*
    TRACE* trace = plot->GetTrace( m_signal );

    if( trace->IsVisible() )
        AddMenuItem( this, SHOW_HIDE_SIGNAL, _( "Hide Signal" ),
                _( "Show the hidden signal" ),
                KiBitmap( delete_xpm ) );
    else
        AddMenuItem( this, SHOW_SIGNAL, _( "Show Signal" ),
                _( "Hide the signal on a plot screen" ),
                KiBitmap( delete_xpm ) );
*/
//    Connect( wxEVT_COMMAND_MENU_SELECTED, wxMenuEventHandler( SIGNAL_CONTEXT_MENU::onMenuEvent ), NULL, this );
}


void SIM_PLOT_FRAME::menuShowHideSignal( wxCommandEvent& event )
{
    SIM_PLOT_PANEL* plot = CurrentPlot();
    if( plot )
    {
        int idx = m_signals->GetFirstSelected();

        while( idx != wxNOT_FOUND )
        {
            const wxString& netName = m_signals->GetItemText( idx, 0 );
            TRACE* trace = plot->GetTrace( netName );

            if( trace )
            {
                trace->SetVisible( !trace->IsVisible() );
                plot->Refresh();
            }

            idx = m_signals->GetNextSelected( idx );
        }
    }
}

wxDEFINE_EVENT( EVT_SIM_UPDATE, wxCommandEvent );
wxDEFINE_EVENT( EVT_SIM_REPORT, wxCommandEvent );

wxDEFINE_EVENT( EVT_SIM_STARTED, wxCommandEvent );
wxDEFINE_EVENT( EVT_SIM_FINISHED, wxCommandEvent );
