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
        m_simFrame->Show( false );

        m_simPanel = m_simFrame->NewPlotPanel( ST_TRANSIENT );
        return true;
    }

    virtual int OnExit() override
    {
        DisposeOfWindow( reinterpret_cast<wxWindow**>( &m_simFrame ) );
        DisposeOfWindow( reinterpret_cast<wxWindow**>( &m_edaFrame ) );

        delete Kiface().KifaceSettings();
        Kiface().InitSettings( nullptr );
        Pgm().Destroy();
        return 0;
    }

    void DumpTraces();

    void AddTrace( wxString aTraceName, bool aCursor, struct PLOT_DATA_VECTOR* dataset );
    void MoveToExtremum( size_t index, enum CURSOR_CONTEXT_MENU_ID aCommand, bool aExpectedValue );
    void TestCursorValues( bool aMaximum, struct PLOT_DATA_VECTOR* dataset, int line );
    void TestExtremum( size_t aExtremumAt, enum CURSOR_CONTEXT_MENU_ID aCmd,
            size_t aStartIndex, struct PLOT_DATA_VECTOR* aDataSet );

    TRACE*  m_trace;
    CURSOR* m_cursor;
};


void TEST_APP::DumpTraces()
{
    for( auto &t : m_simPanel->GetTraces() )
        std::cout << "[SK] trace.GetMinYindex(): " << t.second->GetMinYindex() <<
                        ", trace.GetMaxYindex(): " << t.second->GetMaxYindex() <<  std::endl;

    std::cout << std::endl << std::endl;
}


void TEST_APP::AddTrace( wxString aTraceName, bool aCursor, struct PLOT_DATA_VECTOR* dataset )
{
    m_simPanel->DeleteAllTraces();
    m_simPanel->AddTrace( aTraceName, dataset->m_x->size(), &dataset->m_x->at(0),
                    &dataset->m_y->at(0), SPT_VOLTAGE );
    m_trace = m_simPanel->GetTrace( aTraceName );
    m_simPanel->EnableCursor( aTraceName, aCursor );
    RefreshWindow( m_simFrame );
}


void TEST_APP::MoveToExtremum( size_t index, enum CURSOR_CONTEXT_MENU_ID aCommand, bool aExpectedValue )
{
    m_cursor = m_trace->GetCursor();
    m_cursor->MoveToIndex( index );
    BOOST_TEST_CHECK( m_cursor->GoToExtremum( aCommand ) == aExpectedValue );

    wxQueueEvent( m_simFrame, new wxCommandEvent( EVT_SIM_CURSOR_UPDATE ) );
    RefreshWindow( m_simFrame );
    RefreshWindow( m_simFrame );
}


void TEST_APP::TestCursorValues( bool aMaximum, struct PLOT_DATA_VECTOR* dataset, int line )
{
    SPICE_VALUE actualY( m_simFrame->GetCursors()->GetItemText(0, 2) );
    SPICE_VALUE expectedY( aMaximum ? dataset->m_maxY : dataset->m_minY );
    BOOST_TEST_CHECK( actualY.ToDouble() == expectedY.ToDouble(),
                    "Fail in seeking " << ( aMaximum ? "maximum" : "minimum" ) <<
                    ", actual Y: " <<   actualY.ToString() <<
                    ", expected Y: " << expectedY.ToString() <<
                    ", from line: " << line <<
                    ", dataset: " << (dataset - DataVectors)/sizeof( struct PLOT_DATA_VECTOR* ) );

    size_t index = ( aMaximum ? dataset->m_maxYindex : dataset->m_minYindex );
    SPICE_VALUE actualX( m_simFrame->GetCursors()->GetItemText(0, 1) );
    SPICE_VALUE expectedX( dataset->m_x->at( index ) );
    BOOST_TEST_CHECK( actualX.ToDouble() == expectedX.ToDouble(),
                    "Fail in seeking " << ( aMaximum ? "maximum" : "minimum" ) <<
                    ", actual X: " <<   actualX.ToString() <<
                    ", expected X: " << expectedX.ToString() <<
                    ", from line: " << line <<
                    ", dataset: " << (dataset - DataVectors)/sizeof( struct PLOT_DATA_VECTOR* ) );

    if( aMaximum )
    {
        BOOST_TEST_CHECK( m_cursor->GetCoords().x == dataset->m_x->at( dataset->m_maxYindex ),
            "Fail in seeking maximum, actual X coord: " <<   m_cursor->GetCoords().x <<
            ", expected X: " << dataset->m_x->at( dataset->m_maxYindex ) <<
            ", from line: " << line <<
            ", dataset: " << (dataset - DataVectors)/sizeof( struct PLOT_DATA_VECTOR* ) );

        BOOST_TEST_CHECK( m_cursor->GetCoords().y == dataset->m_maxY,
            "Fail in seeking maximum, actual Y coord: " <<   m_cursor->GetCoords().y <<
            ", expected Y: " << dataset->m_maxY << ", from line: " << line <<
            ", dataset: " << (dataset - DataVectors)/sizeof( struct PLOT_DATA_VECTOR* ) );
    }
    else
    {
        BOOST_TEST_CHECK( m_cursor->GetCoords().x == dataset->m_x->at( dataset->m_minYindex ),
            "Fail in seeking minimum, actual X coord: " <<   m_cursor->GetCoords().x <<
            ", expected X: " << dataset->m_x->at( dataset->m_minYindex ) <<
            ", from line: " << line <<
            ", dataset: " << (dataset - DataVectors)/sizeof( struct PLOT_DATA_VECTOR* ) );

        BOOST_TEST_CHECK( m_cursor->GetCoords().y == dataset->m_minY,
            "Fail in seeking minimum, actual Y coord: " <<   m_cursor->GetCoords().y <<
            ", expected Y: " << dataset->m_minY << ", from line: " << line <<
            ", dataset: " << (dataset - DataVectors)/sizeof( struct PLOT_DATA_VECTOR* ) );
    }
}


void TEST_APP::TestExtremum( size_t aExtremumAt, enum CURSOR_CONTEXT_MENU_ID aCmd,
        size_t aStartIndex, struct PLOT_DATA_VECTOR* aDataSet )
{
    // Find extremum (action under test)
    MoveToExtremum( aStartIndex, aCmd, true );

    // Test if cursor found what it should find
    bool maximum = ( aCmd & CCM_MAXIMUM ) != 0;
    struct PLOT_DATA_VECTOR localExtremum = *aDataSet;
    if( maximum )
    {
        localExtremum.m_maxYindex = aExtremumAt;
        localExtremum.m_maxY = localExtremum.m_y->at( aExtremumAt );
    }
    else
    {
        localExtremum.m_minYindex = aExtremumAt;
        localExtremum.m_minY = localExtremum.m_y->at( aExtremumAt );
    }
    TestCursorValues( maximum, &localExtremum, __LINE__ );
}


/**
 * Declare the test suite
 */
BOOST_FIXTURE_TEST_SUITE( SimPlotCursor, WX_FIXTURE_BASE<TEST_APP> )

/**
 * Check that we can get the default properties out as expected
 */
BOOST_AUTO_TEST_CASE( ConstPlot, *utf::tolerance( 0.05 ) )
{
    struct PLOT_DATA_VECTOR* dataset = &DataVectors[PDV_CONST_PLOT];

    dataset->Init();

    App()->AddTrace( wxT("TestTrace"), true, dataset );

    /* ============= TEST FOR GLOBAL EXTREMA ============= */
    App()->MoveToExtremum( -1, CCM_GLOBAL_MAXIMUM, true );
    SPICE_VALUE actualY( App()->m_simFrame->GetCursors()->GetItemText(0, 2) );
    SPICE_VALUE expectedY( dataset->m_maxY );
    BOOST_TEST_CHECK( actualY.ToDouble() == expectedY.ToDouble() );

    App()->MoveToExtremum( -1, CCM_GLOBAL_MINIMUM, true );
    actualY = App()->m_simFrame->GetCursors()->GetItemText(0, 2);
    expectedY = dataset->m_minY;
    BOOST_TEST_CHECK( actualY.ToDouble() == expectedY.ToDouble() );

    /* ============= TEST FOR LOCAL EXTREMA ============= */
    struct TEST_DATA_TYPE
    {
        size_t startIndex;
        enum CURSOR_CONTEXT_MENU_ID whereToGo;
    } testSteps[] =
    {
        {0,                        CCM_LOCAL_MAXIMUM_ASCENDING},
        {0,                        CCM_LOCAL_MINIMUM_ASCENDING},
        {dataset->m_x->size() / 2, CCM_LOCAL_MAXIMUM_ASCENDING},
        {dataset->m_x->size() / 2, CCM_LOCAL_MINIMUM_ASCENDING},
        {dataset->m_x->size() - 1, CCM_LOCAL_MAXIMUM_ASCENDING},
        {dataset->m_x->size() - 1, CCM_LOCAL_MINIMUM_ASCENDING},
        {0,                        CCM_LOCAL_MAXIMUM_DESCENDING},
        {0,                        CCM_LOCAL_MINIMUM_DESCENDING},
        {dataset->m_x->size() / 2, CCM_LOCAL_MAXIMUM_DESCENDING},
        {dataset->m_x->size() / 2, CCM_LOCAL_MINIMUM_DESCENDING},
        {dataset->m_x->size() - 1, CCM_LOCAL_MAXIMUM_DESCENDING},
        {dataset->m_x->size() - 1, CCM_LOCAL_MINIMUM_DESCENDING}
    };

    for( unsigned int i = 0; i < sizeof(testSteps)/sizeof(struct TEST_DATA_TYPE); i++)
    {
        // with const plot cursor shouldn't go anywhere
        App()->MoveToExtremum( testSteps[i].startIndex, testSteps[i].whereToGo, false );
        BOOST_TEST_CHECK( App()->m_cursor->GetCoords().x
                        == dataset->m_x->at( testSteps[i].startIndex ) );
    }
}


BOOST_AUTO_TEST_CASE( GlobalMinMax, *utf::tolerance( 0.05 ) )
{
    struct PLOT_DATA_VECTOR* dataset = &DataVectors[PDV_SINGLE_EXTREMUM_BEGIN];

    for( ; dataset < &DataVectors[PDV_ITEMS_MAX]; dataset++ )
    {
        dataset->Init();

        App()->AddTrace( wxT("TestTrace"), true, dataset );

        /* ============= TEST FOR MAX ============= */
        App()->MoveToExtremum( -1, CCM_GLOBAL_MAXIMUM, true );
        App()->TestCursorValues( true, dataset, __LINE__ );

        /* ============= TEST FOR MIN ============= */

        App()->MoveToExtremum( -1, CCM_GLOBAL_MINIMUM, true );
        App()->TestCursorValues( false, dataset, __LINE__ );
    }
}


BOOST_AUTO_TEST_CASE( LocalMinMaxSingle, *utf::tolerance( 0.05 ) )
{
    struct TEST_DATA_TYPE
    {
        size_t startIndex;
        enum PLOT_DATA_VECTOR_ITEMS datasetIdx;
        enum CURSOR_CONTEXT_MENU_ID whereToGo;
        bool shouldBeFound;
    } testSteps[] =
    {
        {0,                      PDV_SINGLE_EXTREMUM_BEGIN, CCM_LOCAL_MAXIMUM_ASCENDING,  true},
        {0,                      PDV_SINGLE_EXTREMUM_BEGIN, CCM_LOCAL_MINIMUM_ASCENDING,  true},
        {DATA_VECTOR_LENGTH - 1, PDV_SINGLE_EXTREMUM_BEGIN, CCM_LOCAL_MAXIMUM_DESCENDING, true},
        {DATA_VECTOR_LENGTH - 1, PDV_SINGLE_EXTREMUM_BEGIN, CCM_LOCAL_MINIMUM_DESCENDING, true},
        {0,                      PDV_LINEAR_RISING,         CCM_LOCAL_MAXIMUM_ASCENDING,  true},
        {0,                      PDV_LINEAR_RISING,         CCM_LOCAL_MINIMUM_ASCENDING,  false},
        {DATA_VECTOR_LENGTH - 1, PDV_LINEAR_RISING,         CCM_LOCAL_MAXIMUM_DESCENDING, false},
        {DATA_VECTOR_LENGTH - 1, PDV_LINEAR_RISING,         CCM_LOCAL_MINIMUM_DESCENDING, true},
        {0,                      PDV_LINEAR_FALLING,        CCM_LOCAL_MAXIMUM_ASCENDING,  false},
        {0,                      PDV_LINEAR_FALLING,        CCM_LOCAL_MINIMUM_ASCENDING,  true},
        {DATA_VECTOR_LENGTH - 1, PDV_LINEAR_FALLING,        CCM_LOCAL_MAXIMUM_DESCENDING, true},
        {DATA_VECTOR_LENGTH - 1, PDV_LINEAR_FALLING,        CCM_LOCAL_MINIMUM_DESCENDING, false}
    };

    for( unsigned int i = 0; i < sizeof(testSteps)/sizeof(struct TEST_DATA_TYPE); i++)
    {
        struct PLOT_DATA_VECTOR* dataset = &DataVectors[testSteps[i].datasetIdx];

        dataset->Init();

        App()->AddTrace( wxT("TestTrace"), true, dataset );

        App()->MoveToExtremum( testSteps[i].startIndex,
                        testSteps[i].whereToGo, testSteps[i].shouldBeFound );
        App()->TestCursorValues( ( testSteps[i].whereToGo & CCM_MAXIMUM ) != 0, dataset, __LINE__ );
    }

}


BOOST_FIXTURE_TEST_CASE( LocalMinMaxMulti, WX_FIXTURE_BASE<TEST_APP>, *utf::tolerance( 0.05 ) )
{
    struct PLOT_DATA_VECTOR* dataset = &DataVectors[PDV_MULTIPLE_EXTREMA_BEGIN];

    auto testAllExtremaFromList = [&dataset, this]
        ( vector<size_t> list, enum CURSOR_CONTEXT_MENU_ID cmd ) -> void
    {
        size_t startIndex = ( ( cmd & CCM_ASCENDING ) != 0 ) ? 0 : dataset->m_x->size() - 1;

        if( ( cmd & CCM_ASCENDING ) != 0 )
            for( auto extremumAt = list.begin(); extremumAt != list.end(); extremumAt++ )
            {
                // Extremum cannot be found, when cursor is already located on it.
                // Therefore extrema at beginning of plot in asc. order or at end in desc. order
                // should be skipped.
                if( ( ( cmd & CCM_ASCENDING  ) && ( *extremumAt == startIndex ) ) ||
                    ( ( cmd & CCM_DESCENDING ) && ( *extremumAt == startIndex ) ) )
                {
                    continue;
                }

                App()->TestExtremum( *extremumAt, cmd, startIndex, dataset );

                // For every extremum except first one cursor should not be moved,
                // hence -1 as starting index in all remaining iterations
                startIndex = -1;
            }
        else
            for( auto extremumAt = list.rbegin(); extremumAt != list.rend(); extremumAt++ )
            {
                // See comments above
                if( ( ( cmd & CCM_ASCENDING  ) && ( *extremumAt == startIndex ) ) ||
                    ( ( cmd & CCM_DESCENDING ) && ( *extremumAt == startIndex ) ) )
                {
                    continue;
                }

                App()->TestExtremum( *extremumAt, cmd, startIndex, dataset );
                startIndex = -1;
            }
    };

    for( ; dataset < &DataVectors[PDV_ITEMS_MAX]; dataset++ )
    {
        auto extrema = dataset->Init();

        App()->AddTrace( wxT("TestTrace"), true, dataset );

        testAllExtremaFromList( extrema.first,  CCM_LOCAL_MINIMUM_ASCENDING );
        testAllExtremaFromList( extrema.second, CCM_LOCAL_MAXIMUM_ASCENDING );
        testAllExtremaFromList( extrema.first,  CCM_LOCAL_MINIMUM_DESCENDING );
        testAllExtremaFromList( extrema.second, CCM_LOCAL_MAXIMUM_DESCENDING );
    }
}

BOOST_FIXTURE_TEST_CASE( LocalIntereaved, WX_FIXTURE_BASE<TEST_APP>, *utf::tolerance( 0.05 ) )
{
    struct PLOT_DATA_VECTOR* dataset = &DataVectors[PDV_MULTIPLE_EXTREMA_BEGIN];

    auto testAllExtremaFromList = [&dataset, this]
        ( vector<size_t> mins, vector<size_t> maxes, enum CURSOR_CONTEXT_MENU_ID aCmd ) -> void
    {
        vector<pair<size_t,bool>> extremaList;
        for( auto minAt : mins )
            extremaList.push_back( make_pair( minAt, false ) );
        for( auto maxAt : maxes )
            extremaList.push_back( make_pair( maxAt, true ) );

        sort( extremaList.begin(), extremaList.end(), []( pair<size_t,bool> a, pair<size_t,bool> b )
                ->bool { return a.first < b.first; } );

        size_t startIndex = ( ( aCmd & CCM_ASCENDING ) != 0 ) ? 0 : dataset->m_x->size() - 1;

        if( ( aCmd & CCM_ASCENDING ) != 0 )
            for( auto extremumAt = extremaList.begin();
                    extremumAt != extremaList.end(); extremumAt++ )
            {
                // Extremum cannot be found, when cursor is already located on it.
                // Therefore extrema at beginning of plot in asc. order or at end in desc. order
                // should be skipped.
                if( ( ( aCmd & CCM_ASCENDING  ) && ( extremumAt->first == startIndex ) ) ||
                    ( ( aCmd & CCM_DESCENDING ) && ( extremumAt->first == startIndex ) ) )
                {
                    continue;
                }

                int cmd = aCmd | CCM_LOCAL;
                cmd |= extremumAt->second ? CCM_MAXIMUM : CCM_MINIMUM;
                App()->TestExtremum( extremumAt->first,
                        static_cast<enum CURSOR_CONTEXT_MENU_ID>( cmd ), startIndex, dataset );

                // For every extremum except first one cursor should not be moved,
                // hence -1 as starting index in all remaining iterations
                startIndex = -1;
            }
        else
            for( auto extremumAt = extremaList.rbegin();
                    extremumAt != extremaList.rend(); extremumAt++ )
            {
                // See comments above
                if( ( ( aCmd & CCM_ASCENDING  ) && ( extremumAt->first == startIndex ) ) ||
                    ( ( aCmd & CCM_DESCENDING ) && ( extremumAt->first == startIndex ) ) )
                {
                    continue;
                }

                int cmd = aCmd | CCM_LOCAL;
                cmd |= extremumAt->second ? CCM_MAXIMUM : CCM_MINIMUM;
                App()->TestExtremum( extremumAt->first,
                        static_cast<enum CURSOR_CONTEXT_MENU_ID>( cmd ), startIndex, dataset );
                startIndex = -1;
            }
    };

    for( ; dataset < &DataVectors[PDV_ITEMS_MAX]; dataset++ )
    {
        auto extrema = dataset->Init();

        App()->AddTrace( wxT("TestTrace"), true, dataset );

        testAllExtremaFromList( extrema.first, extrema.second,  CCM_ASCENDING );
        testAllExtremaFromList( extrema.first, extrema.second,  CCM_DESCENDING );
    }
}

BOOST_AUTO_TEST_SUITE_END()
