///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include "kiway_player.h"
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/toolbar.h>
#include <wx/aui/auibook.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/textctrl.h>
#include <wx/splitter.h>
#include <wx/stattext.h>
#include <wx/listctrl.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class SIM_PLOT_FRAME_BASE
///////////////////////////////////////////////////////////////////////////////
class SIM_PLOT_FRAME_BASE : public KIWAY_PLAYER
{
	private:

	protected:
		wxMenuBar* m_mainMenu;
		wxMenu* m_fileMenu;
		wxMenuItem* m_newPlot;
		wxMenuItem* m_openWorkbook;
		wxMenuItem* m_saveWorkbook;
		wxMenuItem* m_saveImage;
		wxMenuItem* m_saveCsv;
		wxMenuItem* m_exitSim;
		wxMenu* m_simulationMenu;
		wxMenuItem* m_runSimulation;
		wxMenuItem* m_probeSignals;
		wxMenuItem* m_tuneValue;
		wxMenuItem* m_showNetlist;
		wxMenuItem* m_settings;
		wxMenu* m_traceMenu;
		wxMenuItem* m_toggleCursors;
		wxMenuItem* m_addSignals;
		wxMenuItem* m_deleteSignal;
		wxMenuItem* m_showHideMenu;
		wxMenuItem* m_copyMenu;
		wxMenuItem* m_copyAllMenu;
		wxMenu* m_viewMenu;
		wxMenuItem* m_zoomIn;
		wxMenuItem* m_zoomOut;
		wxMenuItem* m_zoomFit;
		wxMenuItem* m_showGrid;
		wxMenuItem* m_showLegend;
		wxMenuItem* m_showDotted;
		wxMenuItem* m_showWhiteBackground;
		wxBoxSizer* m_sizerMain;
		wxToolBar* m_toolBar;
		wxSplitterWindow* m_splitterLeftRight;
		wxPanel* m_panelLeft;
		wxBoxSizer* m_sizer11;
		wxSplitterWindow* m_splitterPlotAndConsole;
		wxPanel* m_plotPanel;
		wxBoxSizer* m_sizerPlot;
		wxAuiNotebook* m_plotNotebook;
		wxPanel* m_panelConsole;
		wxBoxSizer* m_sizerConsole;
		wxTextCtrl* m_simConsole;
		wxPanel* m_sidePanel;
		wxBoxSizer* m_sideSizer;
		wxSplitterWindow* m_splitterSignals;
		wxPanel* m_panelSignals;
		wxStaticText* m_staticTextSignals;
		wxListView* m_signals;
		wxPanel* m_tunePanel;
		wxStaticText* m_staticTextTune;
		wxBoxSizer* m_tuneSizer;

		// Virtual event handlers, overide them in your derived class
		virtual void menuNewPlot( wxCommandEvent& event ) { event.Skip(); }
		virtual void menuOpenWorkbook( wxCommandEvent& event ) { event.Skip(); }
		virtual void menuSaveWorkbook( wxCommandEvent& event ) { event.Skip(); }
		virtual void menuSaveImage( wxCommandEvent& event ) { event.Skip(); }
		virtual void menuSaveCsv( wxCommandEvent& event ) { event.Skip(); }
		virtual void menuExit( wxCommandEvent& event ) { event.Skip(); }
		virtual void menuRunSim( wxCommandEvent& event ) { event.Skip(); }
		virtual void menuProbe( wxCommandEvent& event ) { event.Skip(); }
		virtual void menuTune( wxCommandEvent& event ) { event.Skip(); }
		virtual void menuShowNetlist( wxCommandEvent& event ) { event.Skip(); }
		virtual void menuSettings( wxCommandEvent& event ) { event.Skip(); }
		virtual void menuCursorToggle( wxCommandEvent& event ) { event.Skip(); }
		virtual void menuAddSignal( wxCommandEvent& event ) { event.Skip(); }
		virtual void menuRemoveSignal( wxCommandEvent& event ) { event.Skip(); }
		virtual void menuZoomIn( wxCommandEvent& event ) { event.Skip(); }
		virtual void menuZoomOut( wxCommandEvent& event ) { event.Skip(); }
		virtual void menuZoomFit( wxCommandEvent& event ) { event.Skip(); }
		virtual void menuShowGrid( wxCommandEvent& event ) { event.Skip(); }
		virtual void menuShowGridUpdate( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void menuShowLegend( wxCommandEvent& event ) { event.Skip(); }
		virtual void menuShowLegendUpdate( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void menuShowDotted( wxCommandEvent& event ) { event.Skip(); }
		virtual void menuShowDottedUpdate( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void menuWhiteBackground( wxCommandEvent& event ) { event.Skip(); }
		virtual void menuShowWhiteBackgroundUpdate( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void onPlotChanged( wxAuiNotebookEvent& event ) { event.Skip(); }
		virtual void onPlotClose( wxAuiNotebookEvent& event ) { event.Skip(); }
		virtual void onSignalDblClick( wxMouseEvent& event ) { event.Skip(); }
		virtual void onSignalRClick( wxListEvent& event ) { event.Skip(); }


	public:

		SIM_PLOT_FRAME_BASE( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Spice Simulator"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 564,531 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL, const wxString& name = wxT("SIM_PLOT_FRAME") );

		~SIM_PLOT_FRAME_BASE();

		void m_splitterLeftRightOnIdle( wxIdleEvent& )
		{
			m_splitterLeftRight->SetSashPosition( 700 );
			m_splitterLeftRight->Disconnect( wxEVT_IDLE, wxIdleEventHandler( SIM_PLOT_FRAME_BASE::m_splitterLeftRightOnIdle ), NULL, this );
		}

		void m_splitterPlotAndConsoleOnIdle( wxIdleEvent& )
		{
			m_splitterPlotAndConsole->SetSashPosition( 500 );
			m_splitterPlotAndConsole->Disconnect( wxEVT_IDLE, wxIdleEventHandler( SIM_PLOT_FRAME_BASE::m_splitterPlotAndConsoleOnIdle ), NULL, this );
		}

		void m_splitterSignalsOnIdle( wxIdleEvent& )
		{
			m_splitterSignals->SetSashPosition( 0 );
			m_splitterSignals->Disconnect( wxEVT_IDLE, wxIdleEventHandler( SIM_PLOT_FRAME_BASE::m_splitterSignalsOnIdle ), NULL, this );
		}

};

