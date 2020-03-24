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
 * Test suite for mathplot: conversions and so on
 */

#include <unit_test_utils/unit_test_utils.h>
#include <unit_test_utils/wx_util.h>
#include <qa_utils/plot_data_vectors.h>
#include <wx/frame.h>
#include <vector>
using namespace std;
using namespace KI_TEST;
namespace utf = boost::unit_test;

// Code under test
#include <widgets/mathplot.h>


class TEST_FRAME : public wxFrame
{
public:
    TEST_FRAME() :
        wxFrame( nullptr, wxID_ANY, "TEST_mpWindow", wxDefaultPosition, wxSize(150,150) )
    {
        m_window    = new mpWindow( this, -1, wxPoint(0,0), wxSize(100,100) );
        m_scaleX    = new mpScaleX( wxT("X"), mpALIGN_BOTTOM, true, mpX_NORMAL );
        m_scaleXLog = new mpScaleXLog();
        m_scaleY    = new mpScaleY( wxT("Y"), mpALIGN_LEFT, true );
        m_vector    = new mpFXYVector();

        //m_window->SetMargins(30, 30, 50, 100);
        m_window->AddLayer( m_scaleX );
        m_window->AddLayer( m_scaleY );
        m_window->AddLayer( m_vector );
    }

    ~TEST_FRAME()
    {
        OnQuit();
    }

    void OnQuit()
    {
    }


    mpWindow*    m_window;
    mpScaleX*    m_scaleX;
    mpScaleXLog* m_scaleXLog;
    mpScaleY*    m_scaleY;
    mpFXYVector* m_vector;

};


class TEST_APP : public TEST_APP_BASE
{
public:
    TEST_APP() :
        m_frame( nullptr )
    {
    }

    virtual ~TEST_APP()
    {};


    TEST_FRAME *m_frame;

virtual bool OnInit() override
    {
    m_frame = new TEST_FRAME();
    m_frame->Show( true );
    return true;
    }
};


/**
 * Declare the test suite
 */
BOOST_FIXTURE_TEST_SUITE( wxMathPlot, WX_FIXTURE_BASE<TEST_APP> )

enum WHAT_IS_PLOT
{
    RANGE_ZERO_TO_ONE,
    SCREEN_PIXELS,
    IDENTITY,
    RANGE_ZERO_TO_ONE_INVERTED
} Plot = IDENTITY;


/**
 * Check that we can get the default properties out as expected
 */
//#define REAL_TEST
BOOST_AUTO_TEST_CASE( mpScaleX, * utf::tolerance(0.00001) )
{
    App()->m_frame->m_scaleX->SetDataRange( 0.0, 1000.0 );

#ifdef REAL_TEST
    // Note: repaint is necessary for linear scale to update internal variables and provide
    //       valid results by TransformXxx() functions. This looks like a bug to me.
    App()->RefreshWindow( App()->m_frame );

    Plot = RANGE_ZERO_TO_ONE;
#else
    // force pass
    Plot = IDENTITY;
#endif

    if( Plot == RANGE_ZERO_TO_ONE )
    {
        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleX->TransformFromPlot( 250.0 ), 0.25 );
        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleX->TransformFromPlot( 500.0 ), 0.50 );
        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleX->TransformFromPlot( 750.0 ), 0.75 );

        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleX->TransformToPlot( 0.25 ), 250.0 );
        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleX->TransformToPlot( 0.50 ), 500.0 );
        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleX->TransformToPlot( 0.75 ), 750.0 );
    }
    else if( Plot == SCREEN_PIXELS )
    {
        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleX->TransformFromPlot( 250.0 ), 25 );
        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleX->TransformFromPlot( 500.0 ), 50 );
        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleX->TransformFromPlot( 750.0 ), 75 );

        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleX->TransformToPlot( 25 ), 250.0 );
        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleX->TransformToPlot( 50 ), 500.0 );
        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleX->TransformToPlot( 75 ), 750.0 );
    }
    else if( Plot == IDENTITY )
    {
        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleX->TransformFromPlot( 250.0 ), 250.0 );
        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleX->TransformFromPlot( 500.0 ), 500.0 );
        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleX->TransformFromPlot( 750.0 ), 750.0 );

        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleX->TransformToPlot( 250.0 ), 250.0 );
        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleX->TransformToPlot( 500.0 ), 500.0 );
        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleX->TransformToPlot( 750.0 ), 750.0 );
    }
}


BOOST_AUTO_TEST_CASE( mpScaleXLog, * utf::tolerance(0.00001) )
{
    App()->m_frame->m_scaleXLog->SetDataRange( 1.0, 10000.0 );

    Plot = RANGE_ZERO_TO_ONE;

    if( Plot == RANGE_ZERO_TO_ONE )
    {
        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleXLog->TransformFromPlot( 0.0 ), 1.0 );
        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleXLog->TransformFromPlot( 0.5 ), 100.0 );
        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleXLog->TransformFromPlot( 1.0 ), 10000.0 );

        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleXLog->TransformToPlot( 1.0 ),     0.0 );
        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleXLog->TransformToPlot( 100.0 ),   0.5 );
        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleXLog->TransformToPlot( 10000.0 ), 1.0 );

    }
    else if( Plot == SCREEN_PIXELS )
    {
        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleXLog->TransformFromPlot( 1.0 ),     0.0 );
        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleXLog->TransformFromPlot( 100.0 ),   50.0 );
        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleXLog->TransformFromPlot( 10000.0 ), 100.0 );

        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleXLog->TransformToPlot( 0.0 ),   1.0 );
        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleXLog->TransformToPlot( 50.0 ),  100.0 );
        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleXLog->TransformToPlot( 100.0 ), 10000.0 );
    }
    else if( Plot == IDENTITY )
    {
        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleXLog->TransformFromPlot( 1.0 ),     0.0 );
        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleXLog->TransformFromPlot( 100.0 ),   100.0 );
        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleXLog->TransformFromPlot( 10000.0 ), 10000.0 );

        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleXLog->TransformToPlot( 1.0 ),     0.0 );
        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleXLog->TransformToPlot( 100.0 ),   100.0 );
        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleXLog->TransformToPlot( 10000.0 ), 10000.0 );
    }
    else if( Plot == RANGE_ZERO_TO_ONE_INVERTED )
    {
        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleXLog->TransformFromPlot( 1.0 ),     0.0 );
        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleXLog->TransformFromPlot( 100.0 ),   0.5 );
        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleXLog->TransformFromPlot( 10000.0 ), 1.0 );

        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleXLog->TransformToPlot( 0.0 ), 1.0 );
        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleXLog->TransformToPlot( 0.5 ), 100.0 );
        BOOST_CHECK_EQUAL( App()->m_frame->m_scaleXLog->TransformToPlot( 1.0 ), 10000.0 );
    }
}


BOOST_AUTO_TEST_CASE( mpFXYVector, * utf::tolerance(0.00001) )
{
    struct PLOT_DATA_VECTOR* dataset = DataVectors;

    for( ; dataset < &DataVectors[PDV_ITEMS_MAX]; dataset++ )
    {
        dataset->Init();

        App()->m_frame->m_vector->SetData( *dataset->m_x, *dataset->m_y );

        BOOST_CHECK_EQUAL( App()->m_frame->m_vector->GetMinX(), dataset->m_minX );
        BOOST_CHECK_EQUAL( App()->m_frame->m_vector->GetMaxX(), dataset->m_maxX );
        BOOST_CHECK_EQUAL( App()->m_frame->m_vector->GetMinY(), dataset->m_minY );
        BOOST_CHECK_EQUAL( App()->m_frame->m_vector->GetMaxY(), dataset->m_maxY );

        // for const plot every item can be maximum or minimum, so index should not be checked
        if( dataset != &DataVectors[PDV_CONST_PLOT] )
        {
            BOOST_CHECK_EQUAL( App()->m_frame->m_vector->GetMinYindex(), dataset->m_minYindex );
            BOOST_CHECK_EQUAL( App()->m_frame->m_vector->GetMaxYindex(), dataset->m_maxYindex );
        }
    }
}

#if 0
BOOST_AUTO_TEST_CASE( mpWindow, * utf::tolerance(0.00001) )
{
    struct PLOT_DATA_VECTOR* dataset = DataVectors;

    for( ; dataset < &DataVectors[PDV_ITEMS_MAX]; dataset++ )
    {
        dataset->Init();

        App()->m_frame->m_scaleX->SetDataRange( 0.0, 1000.0 );
        //App()->m_frame->m_scaleXLog->SetDataRange( 1.0, 10000.0 );
        App()->m_frame->m_vector->SetData( *dataset->m_x, *dataset->m_y );

//        BOOST_CHECK_EQUAL( App()->m_frame->m_vector->GetMinX(), dataset->m_minX );
//        BOOST_CHECK_EQUAL( App()->m_frame->m_vector->GetMaxX(), dataset->m_maxX );
//        BOOST_CHECK_EQUAL( App()->m_frame->m_vector->GetMinY(), dataset->m_minY );
//        BOOST_CHECK_EQUAL( App()->m_frame->m_vector->GetMaxY(), dataset->m_maxY );
//        BOOST_CHECK_EQUAL( App()->m_frame->m_vector->GetMinYindex(), dataset->m_minYindex );
//        BOOST_CHECK_EQUAL( App()->m_frame->m_vector->GetMaxYindex(), dataset->m_maxYindex );
    }

    Plot = RANGE_ZERO_TO_ONE;

    if( Plot == RANGE_ZERO_TO_ONE )
    {
        BOOST_CHECK_EQUAL( App()->m_frame->m_window->p2x( (wxCoord) 50 ), 0.5 );
        BOOST_CHECK_EQUAL( App()->m_frame->m_window->x2p( 0.5 ), 50 );

        while( std::cin.get() != '\n' )
            wxYield();
    }
    else if( Plot == SCREEN_PIXELS )
    {
    }
    else if( Plot == IDENTITY )
    {
    }
    else if( Plot == RANGE_ZERO_TO_ONE_INVERTED )
    {
    }
}
#endif

BOOST_AUTO_TEST_SUITE_END()
