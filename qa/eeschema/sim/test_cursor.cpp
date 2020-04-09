/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2020 KiCad Developers, see CHANGELOG.TXT for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

/**
 * @file
 * Test suite for CURSOR
 */

#include <unit_test_utils/unit_test_utils.h>
#include <unit_test_utils/wx_util.h>
#include <kiway.h>
#include <eeschema_settings.h>
#include <vector>
#include <mocks_eeschema.h>
#include <sim/spice_value.h>
#include <qa_utils/plot_data_vectors.h>


using namespace std;
using namespace KI_TEST;
namespace utf = boost::unit_test;

// Code under test
#include <sim/sim_plot_frame.h>
#include <sim/sim_plot_panel.h>

/* New class derived only to get access to some controls */
class TEST_SIM_PLOT_FRAME : public SIM_PLOT_FRAME
{
public:
    TEST_SIM_PLOT_FRAME( KIWAY* aKiway, wxWindow* aParent ) :
        SIM_PLOT_FRAME( aKiway, aParent )
    {};

    wxListView* GetCursors() const
    {
        return m_cursors;
    }
};


class TEST_APP : public TEST_APP_BASE
{
public:
    TEST_APP() :
        m_edaFrame( nullptr ), m_simFrame( nullptr ), m_simPanel( nullptr )
    {
    }

    virtual ~TEST_APP()
    {};

    // CURSOR, a member of SIM_PLOT_PANEL is object under test
    // SIM_PLOT_PANEL is contained in TEST_SIM_PLOT_FRAME, which will be created for test,
    // SCH_EDIT_FRAME is it's dependency.
    SCH_EDIT_FRAME *m_edaFrame;
    TEST_SIM_PLOT_FRAME *m_simFrame;
    SIM_PLOT_PANEL *m_simPanel;

    virtual bool OnInit() override
    {
        Pgm().InitPgm();
        EESCHEMA_SETTINGS *cfg = new EESCHEMA_SETTINGS();
        cfg->m_Graphics.canvas_type = EDA_DRAW_PANEL_GAL::GAL_TYPE_NONE;
        Kiface().InitSettings( cfg );

        m_edaFrame = new SCH_EDIT_FRAME( &( KiwayMock() ), nullptr );
        KiwayMock().SetFrame( FRAME_SCH, m_edaFrame );

        m_simFrame = new TEST_SIM_PLOT_FRAME( &( KiwayMock() ), m_edaFrame );
        m_simFrame->Show( true );

        m_simPanel = m_simFrame->NewPlotPanel( ST_TRANSIENT );
        return true;
    }

    virtual int OnExit() override
    {
        delete Kiface().KifaceSettings();
        Kiface().InitSettings( nullptr );
        return 0;
    }

    void RefreshWindow();
    void DumpTraces();
};

void TEST_APP::RefreshWindow()
{
    ProcessPendingEvents();
    wxYield();

    m_simFrame->Layout();
    m_simFrame->Refresh();
    m_simFrame->Update();
    ProcessPendingEvents();
    wxYield();
}


void TEST_APP::DumpTraces()
{
    for( auto &t : m_simPanel->GetTraces() )
        std::cout << "[SK] trace.GetMinYindex(): " << t.second->GetMinYindex() <<
                        ", trace.GetMaxYindex(): " << t.second->GetMaxYindex() <<  std::endl;

    std::cout << std::endl << std::endl;
}


/**
 * Declare the test suite
 */
BOOST_FIXTURE_TEST_SUITE( SimPlotCursor, WX_FIXTURE_BASE<TEST_APP> )

/**
 * Check that we can get the default properties out as expected
 */

BOOST_AUTO_TEST_CASE( GlobalMinMax, *utf::tolerance( 0.05 ) )
{
    struct PLOT_DATA_VECTOR* dataset = DataVectors;
    const wxString traceName = wxT("TestTrace");
    TRACE*  trace;
    CURSOR* cursor;

    for( ; dataset->Init != NoMoreData; dataset++ )
    {
        dataset->Init();

        App()->DumpTraces();
        App()->m_simPanel->DeleteAllTraces();
        App()->m_simPanel->AddTrace( traceName, dataset->m_x->size(), &dataset->m_x->at(0),
                        &dataset->m_y->at(0), SPT_VOLTAGE );
        trace = App()->m_simPanel->GetTrace( traceName );
        App()->m_simPanel->EnableCursor( traceName, true );
        App()->RefreshWindow();
        App()->DumpTraces();

        /* ============= TEST FOR MAX ============= */

        cursor = trace->GetCursor();
        cursor->GoToExtremum( CCM_GLOBAL_MAXIMUM );

        wxQueueEvent( App()->m_simFrame, new wxCommandEvent( EVT_SIM_CURSOR_UPDATE ) );
        App()->RefreshWindow();
        App()->RefreshWindow();

        SPICE_VALUE maximumFromCursor( App()->m_simFrame->GetCursors()->GetItemText(0, 2) );
        SPICE_VALUE maximumInData( dataset->m_maxY );
        BOOST_TEST_CHECK( maximumFromCursor.ToDouble() == maximumInData.ToDouble() );

        SPICE_VALUE maximumFromCursorAt( App()->m_simFrame->GetCursors()->GetItemText(0, 1) );
        SPICE_VALUE maximumInDataAt( dataset->m_x->at( dataset->m_maxYindex ) );
        BOOST_TEST_CHECK( maximumFromCursorAt.ToDouble() == maximumInDataAt.ToDouble() );

        BOOST_TEST_CHECK( cursor->GetCoords().x == dataset->m_x->at(dataset->m_maxYindex) );
        BOOST_TEST_CHECK( cursor->GetCoords().y == dataset->m_maxY );
        App()->DumpTraces();

        /* ============= TEST FOR MIN ============= */

        cursor = trace->GetCursor();
        cursor->GoToExtremum( CCM_GLOBAL_MINIMUM );

        wxQueueEvent( App()->m_simFrame, new wxCommandEvent( EVT_SIM_CURSOR_UPDATE ) );
        App()->RefreshWindow();
        App()->RefreshWindow();

        SPICE_VALUE minimumFromCursor( App()->m_simFrame->GetCursors()->GetItemText(0, 2) );
        SPICE_VALUE minimumInData( dataset->m_minY );
        BOOST_TEST_CHECK( minimumFromCursor.ToDouble() == minimumInData.ToDouble() );

        SPICE_VALUE minimumFromCursorAt( App()->m_simFrame->GetCursors()->GetItemText(0, 1) );
        SPICE_VALUE minimumInDataAt( dataset->m_x->at( dataset->m_minYindex ) );
        BOOST_TEST_CHECK( minimumFromCursorAt.ToDouble() == minimumInDataAt.ToDouble() );

        BOOST_TEST_CHECK( cursor->GetCoords().x == dataset->m_x->at(dataset->m_minYindex) );
        BOOST_TEST_CHECK( cursor->GetCoords().y == dataset->m_minY );
        App()->DumpTraces();
    }
}


BOOST_AUTO_TEST_SUITE_END()
