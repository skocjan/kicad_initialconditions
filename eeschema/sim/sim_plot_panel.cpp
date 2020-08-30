/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2016 CERN
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

#include "sim_plot_panel.h"
#include "sim_plot_frame.h"

#include <algorithm>
#include <limits>

static wxString formatFloat( double x, int nDigits )
{
    wxString rv, fmt;

    if( nDigits )
    {
        fmt = wxT( "%.0Nf" );
        fmt[3] = '0' + nDigits;
    }
    else
    {
        fmt = wxT( "%.0f" );
    }

    rv.Printf( fmt, x );

    return rv;
}


static void getSISuffix( double x, const wxString& unit, int& power, wxString& suffix )
{
    const int n_powers = 11;

    const struct
    {
        double exponent;
        char suffix;
    } powers[] =
    {
        { -18, 'a' },
        { -15, 'f' },
        { -12, 'p' },
        { -9,  'n' },
        { -6,  'u' },
        { -3,  'm' },
        { 0,   0   },
        { 3,   'k' },
        { 6,   'M' },
        { 9,   'G' },
        { 12,  'T' },
        { 14,  'P' }
    };

    power = 0;
    suffix = unit;

    if( x == 0.0 )
        return;

    for( int i = 0; i < n_powers - 1; i++ )
    {
        double r_cur = pow( 10, powers[i].exponent );

        if( fabs( x ) >= r_cur && fabs( x ) < r_cur * 1000.0 )
        {
            power = powers[i].exponent;

            if( powers[i].suffix )
                suffix = wxString( powers[i].suffix ) + unit;
            else
                suffix = unit;

            return;
        }
    }
}


static int countDecimalDigits( double x, int maxDigits )
{
    int64_t k = (int)( ( x - floor( x ) ) * pow( 10.0, (double) maxDigits ) );
    int n = 0;

    while( k && ( ( k % 10LL ) == 0LL || ( k % 10LL ) == 9LL ) )
    {
        k /= 10LL;
    }

    n = 0;

    while( k != 0LL )
    {
        n++;
        k /= 10LL;
    }

    return n;
}


static void formatSILabels( mpScaleBase* scale, const wxString& aUnit, int nDigits )
{
    double maxVis = scale->AbsVisibleMaxValue();

    wxString suffix;
    int power, digits = 0;

    getSISuffix( maxVis, aUnit, power, suffix );

    double sf = pow( 10.0, power );

    for( auto &l : scale->TickLabels() )
    {
        int k = countDecimalDigits( l.pos / sf, nDigits );

        digits = std::max( digits, k );
    }

    for( auto &l : scale->TickLabels() )
    {
        l.label = formatFloat ( l.pos / sf, digits ) + suffix;
        l.visible = true;
    }
}


class FREQUENCY_LOG_SCALE : public mpScaleXLog
{
public:
    FREQUENCY_LOG_SCALE( wxString name, int flags ) :
        mpScaleXLog( name, flags ) {};

    void formatLabels() override
    {
        const wxString unit = wxT( "Hz" );
        wxString suffix;
        int power;

        for( auto &l : TickLabels() )
        {
            getSISuffix( l.pos, unit, power, suffix );
            double sf = pow( 10.0, power );
            int k = countDecimalDigits( l.pos / sf, 3 );

            l.label = formatFloat( l.pos / sf, k ) + suffix;
            l.visible = true;
        }
    }
};


class FREQUENCY_LIN_SCALE : public mpScaleX
{
public:
    FREQUENCY_LIN_SCALE( wxString name, int flags ) :
        mpScaleX( name, flags, false , 0 ) {};

    void formatLabels() override
    {
        formatSILabels( this, wxT( "Hz" ), 3 );
    }
};


class TIME_SCALE : public mpScaleX
{
public:
    TIME_SCALE( wxString name, int flags ) :
        mpScaleX( name, flags, false, 0 ) {};

    void formatLabels() override
    {
        formatSILabels( this, wxT( "s" ), 3 );
    }
};


class VOLTAGE_SCALE_X : public mpScaleX
{
public:
    VOLTAGE_SCALE_X( wxString name, int flags ) :
        mpScaleX( name, flags, false, 0 ) {};

    void formatLabels() override
    {
        formatSILabels( this, wxT( "V" ), 3 );
    }
};


class GAIN_SCALE : public mpScaleY
{
public:
    GAIN_SCALE( wxString name, int flags ) :
        mpScaleY( name, flags, false ) {};

    void formatLabels() override
    {
        formatSILabels( this, wxT( "dBV" ), 3 );
    }

};


class PHASE_SCALE : public mpScaleY
{
public:
    PHASE_SCALE( wxString name, int flags ) :
        mpScaleY( name, flags, false ) {};

    void formatLabels() override
    {
        formatSILabels( this, wxT( "\u00B0" ), 3 );     // degree sign
    }
};


class VOLTAGE_SCALE_Y : public mpScaleY
{
public:
    VOLTAGE_SCALE_Y( wxString name, int flags ) :
        mpScaleY( name, flags, false ) {};

    void formatLabels() override
    {
        formatSILabels( this, wxT( "V" ), 3 );
    }
};


class CURRENT_SCALE : public mpScaleY
{
public:
    CURRENT_SCALE( wxString name, int flags ) :
        mpScaleY( name, flags, false ) {};

    void formatLabels() override
    {
        formatSILabels( this, wxT( "A" ), 3 );
    }
};


void CURSOR::Plot( wxDC& aDC, mpWindow& aWindow )
{
//    if( !m_window )
//        m_window = &aWindow;

    wxPrintf("[SK] CURSOR::Plot() m_plotPanel: %p\n", m_plotPanel);
    wxPrintf("[SK] CURSOR::Plot() m_plotPanel: %p\n", &m_plotPanel->GetTraces());

    if( !m_visible || m_plotPanel->GetTraces().empty() )
        return;

    TRACE* firstTrace = m_plotPanel->GetTraces().begin()->second;

    wxPrintf("[SK} trace addr0: %p\n", firstTrace);

    if( m_updateRequired )
    {
        // assume first traces contains the same x data as any other

        const auto& dataX = firstTrace->GetDataX();

        if( dataX.size() <= 1 )
            return;

        m_x = firstTrace->s2x( aWindow.p2x( m_dim.x ) );

        // Find the closest point coordinates
        auto maxXIt = std::upper_bound( dataX.begin(), dataX.end(), m_x );
        int maxIdx = maxXIt - dataX.begin();
        int minIdx = maxIdx - 1;

        // Out of bounds checks
        if( minIdx < 0 )
        {
            minIdx = 0;
            maxIdx = 1;
            m_x = dataX[0];
        }
        else if( maxIdx >= (int) dataX.size() )
        {
            maxIdx = dataX.size() - 1;
            minIdx = maxIdx - 1;
            m_x = dataX[maxIdx];
        }

        const double leftX = dataX[minIdx];
        const double rightX = dataX[maxIdx];

        for( auto trace : m_plotPanel->GetTraces() )
        {
            const auto& dataY = trace.second->GetDataY();

            const double leftY = dataY[minIdx];
            const double rightY = dataY[maxIdx];

            // Linear interpolation
            double curValue= leftY + ( rightY - leftY ) / ( rightX - leftX ) * ( m_x - leftX );
            m_y[trace.first] = curValue;

            wxPrintf("[SK} Cursor Y value: %f %f\n", m_y[trace.first], curValue);
        }

        // Notify the parent window about the changes
        wxQueueEvent( aWindow.GetParent(), new wxCommandEvent( EVT_SIM_CURSOR_UPDATE ) );
        m_updateRequired = false;
    }
    else
    {
        m_updateRef = true;
    }

    if( m_updateRef )
    {
        UpdateReference();
        m_updateRef = false;
    }

    draw( aDC, aWindow );
}


void CURSOR::draw( wxDC& aDC, mpWindow& aWindow  )
{
    wxCoord xCursorPosPx = m_dim.x;

    wxCoord leftPx   = m_drawOutsideMargins ? 0 : aWindow.GetMarginLeft();
    wxCoord rightPx  = m_drawOutsideMargins ? aWindow.GetScrX() : aWindow.GetScrX() - aWindow.GetMarginRight();
    wxCoord topPx    = m_drawOutsideMargins ? 0 : aWindow.GetMarginTop();
    wxCoord bottomPx = m_drawOutsideMargins ? aWindow.GetScrY() : aWindow.GetScrY() - aWindow.GetMarginBottom();

    wxPrintf("[SK] leftPx: %d, rightPx: %d, xCursorPosPx: %d\n", leftPx, rightPx, xCursorPosPx);

    if( leftPx < xCursorPosPx && xCursorPosPx < rightPx )
    {
        updatePen  ( aDC, SIM_CURSOR_COLOR );
        updateBrush( aDC, SIM_CURSOR_COLOR );

        aDC.DrawLine( xCursorPosPx, topPx, xCursorPosPx, bottomPx );

        wxPoint triangle[] = { wxPoint( xCursorPosPx, topPx + TRIANGLE_DIM ),
                               wxPoint( xCursorPosPx - ( 2* TRIANGLE_DIM / 3 ), topPx ),
                               wxPoint( xCursorPosPx + ( 2* TRIANGLE_DIM / 3 ), topPx ) };
        aDC.DrawPolygon( 3, triangle );

        aDC.SetTextForeground( m_plotPanel->GetPlotColor( SIM_BG_COLOR ) );
        aDC.SetTextBackground( m_plotPanel->GetPlotColor( SIM_CURSOR_COLOR ) );
        wxCoord textWidth, textHeight;
        aDC.GetTextExtent( m_label, &textWidth, &textHeight );
        aDC.DrawText( m_label, xCursorPosPx - ( textWidth / 2 ) , topPx );

        for( auto trace : m_plotPanel->GetTraces() )
        {
            wxCoord yCursorPosPx = aWindow.y2p( trace.second->y2s( m_y[trace.first] ) );

            if( topPx < yCursorPosPx && yCursorPosPx < bottomPx )
            {
                updatePen  ( aDC, trace.second->GetTraceColour() );
                updateBrush( aDC, trace.second->GetTraceColour() );

                aDC.DrawCircle( xCursorPosPx, yCursorPosPx, TRACE_DOT_RADIUS );
            }
            else
                continue;
        }
    }
}


void CURSOR::updatePen( wxDC& aDC, enum SIM_COLOR_SET aColour )
{
    wxPen* pen =  wxThePenList->FindOrCreatePen( m_plotPanel->GetPlotColor( aColour ), 1,
                       m_continuous ? wxPENSTYLE_SOLID : wxPENSTYLE_LONG_DASH );
    aDC.SetPen( *pen );
}


void CURSOR::updateBrush( wxDC& aDC, enum SIM_COLOR_SET aColour )
{
    wxBrush* brush =  wxTheBrushList->FindOrCreateBrush(
            m_plotPanel->GetPlotColor( aColour ), wxBRUSHSTYLE_SOLID  );
    aDC.SetBrush( *brush );
}


bool CURSOR::Inside( wxPoint& aPoint )
{
//    if( !m_window )
//        return false;

    return ( std::abs( (double) aPoint.x - m_reference.x ) <= DRAG_MARGIN )
        || ( std::abs( (double) aPoint.y - m_reference.y ) <= DRAG_MARGIN );
}


//void CURSOR::UpdateReference()
//{
//    if( !m_window )
//        return;
//
//    m_reference.x = m_window->x2p( m_plotPanel->GetTraces().begin()->second->x2s( m_x ) );
//    //m_reference.y = m_window->y2p( m_trace->y2s( m_y ) );
//}


SIM_PLOT_PANEL::SIM_PLOT_PANEL( SIM_TYPE aType, wxWindow* parent, SIM_PLOT_FRAME* aMainFrame,
        wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
        : SIM_PANEL_BASE( aType, parent, id, pos, size, style, name ),
          m_colorIdx( 0 ),
          m_plotWin( new mpWindow( this, wxID_ANY, pos, size, style ) ),
          m_cursors ( std::make_pair<CURSOR, CURSOR>( CURSOR( this ), CURSOR( this ) ) ),
          m_axis_x( nullptr ),
          m_axis_y1( nullptr ),
          m_axis_y2( nullptr ),
          m_dotted_cp( false ),
          m_masterFrame( aMainFrame )
{
    m_sizer   = new wxBoxSizer( wxVERTICAL );
    //m_plotWin = new mpWindow( this, wxID_ANY, pos, size, style );

    m_plotWin->LimitView( true );
    m_plotWin->SetMargins( 50, 80, 50, 80 );

    m_plotWin->SetColourTheme( GetPlotColor( SIM_BG_COLOR ), GetPlotColor( SIM_FG_COLOR ),
            GetPlotColor( SIM_AXIS_COLOR ) );

    switch( GetType() )
    {
        case ST_AC:
            m_axis_x = new FREQUENCY_LOG_SCALE( _( "Frequency" ), mpALIGN_BOTTOM );
            m_axis_y1 = new GAIN_SCALE( _( "Gain" ), mpALIGN_LEFT );
            m_axis_y2 = new PHASE_SCALE( _( "Phase" ), mpALIGN_RIGHT );
            m_axis_y2->SetMasterScale( m_axis_y1 );
            break;

        case ST_DC:
            m_axis_x = new VOLTAGE_SCALE_X( _( "Voltage (swept)" ), mpALIGN_BOTTOM );
            m_axis_y1 = new VOLTAGE_SCALE_Y( _( "Voltage (measured)" ), mpALIGN_LEFT );
            m_axis_y2 = new CURRENT_SCALE( _( "Current" ), mpALIGN_RIGHT );
            break;

        case ST_NOISE:
            m_axis_x = new FREQUENCY_LOG_SCALE( _( "Frequency" ), mpALIGN_BOTTOM );
            m_axis_y1 = new mpScaleY( _( "noise [(V or A)^2/Hz]" ), mpALIGN_LEFT );
            break;

        case ST_TRANSIENT:
            m_axis_x = new TIME_SCALE( _( "Time" ), mpALIGN_BOTTOM );
            m_axis_y1 = new VOLTAGE_SCALE_Y( _( "Voltage" ), mpALIGN_LEFT );
            m_axis_y2 = new CURRENT_SCALE( _( "Current" ), mpALIGN_RIGHT );
            m_axis_y2->SetMasterScale( m_axis_y1 );
            break;

        default:
            // suppress warnings
            break;
    }

    if( m_axis_x )
    {
        m_axis_x->SetTicks( false );
        m_axis_x->SetNameAlign ( mpALIGN_BOTTOM );

        m_plotWin->AddLayer( m_axis_x );
    }

    if( m_axis_y1 )
    {
        m_axis_y1->SetTicks( false );
        m_axis_y1->SetNameAlign ( mpALIGN_LEFT );
        m_plotWin->AddLayer( m_axis_y1 );
    }

    if( m_axis_y2 )
    {
        m_axis_y2->SetTicks( false );
        m_axis_y2->SetNameAlign ( mpALIGN_RIGHT );
        m_plotWin->AddLayer( m_axis_y2 );
    }

    // a mpInfoLegend displays le name of traces on the left top panel corner:
    m_legend = new mpInfoLegend( wxRect( 0, 40, 200, 40 ), wxTRANSPARENT_BRUSH );
    m_legend->SetVisible( false );
    m_plotWin->AddLayer( m_legend );

    m_plotWin->AddLayer( &m_cursors.first  );
    m_plotWin->AddLayer( &m_cursors.second );

    m_plotWin->EnableDoubleBuffer( true );
    m_plotWin->UpdateAll();

    m_sizer->Add( m_plotWin, 1, wxALL | wxEXPAND, 1 );
    SetSizer( m_sizer );
}


SIM_PLOT_PANEL::~SIM_PLOT_PANEL()
{
    // ~mpWindow destroys all the added layers, so there is no need to destroy m_traces contents
}


void SIM_PLOT_PANEL::UpdatePlotColors()
{
    // Update bg and fg colors:
    m_plotWin->SetColourTheme( GetPlotColor( SIM_BG_COLOR ), GetPlotColor( SIM_FG_COLOR ),
            GetPlotColor( SIM_AXIS_COLOR ) );

    m_plotWin->UpdateAll();
}


wxColour SIM_PLOT_PANEL::GetPlotColor( int aIndex )
{
    return m_masterFrame->GetPlotColor( aIndex );
}


void SIM_PLOT_PANEL::UpdateTraceStyle( TRACE* trace )
{
    int        flags    = trace->GetFlags();
    wxPenStyle penStyle = ( ( flags & SPT_AC_PHASE || flags & SPT_CURRENT ) && m_dotted_cp ) ?
                                  wxPENSTYLE_DOT :
                                  wxPENSTYLE_SOLID;
    trace->SetPen( wxPen( m_masterFrame->GetPlotColor( trace->GetTraceColour() ), 2, penStyle ) );
}


bool SIM_PLOT_PANEL::AddTrace( const wxString& aName, int aPoints,
        const double* aX, const double* aY, SIM_PLOT_TYPE aFlags )
{
    TRACE* trace = NULL;

    // Find previous entry, if there is one
    auto prev = m_traces.find( aName );
    bool addedNewEntry = ( prev == m_traces.end() );

    if( addedNewEntry )
    {
        if( GetType() == ST_TRANSIENT )
        {
            bool hasVoltageTraces = false;

            for( const auto& tr : m_traces )
            {
                if( !( tr.second->GetFlags() & SPT_CURRENT ) )
                {
                    hasVoltageTraces = true;
                    break;
                }
            }

            if( !hasVoltageTraces )
                m_axis_y2->SetMasterScale( nullptr );
            else
                m_axis_y2->SetMasterScale( m_axis_y1 );
        }

        // New entry
        trace = new TRACE( aName, generateColor() );
        UpdateTraceStyle( trace );
        m_traces[aName] = trace;

        // It is a trick to keep legend & coords always on the top
        for( mpLayer* l : m_topLevel )
            m_plotWin->DelLayer( l );

        m_plotWin->AddLayer( (mpLayer*) trace );

        for( mpLayer* l : m_topLevel )
            m_plotWin->AddLayer( l );
    }
    else
    {
        trace = prev->second;
    }

    std::vector<double> tmp( aY, aY + aPoints );

    if( GetType() == ST_AC )
    {
        if( aFlags & SPT_AC_PHASE )
        {
            for( int i = 0; i < aPoints; i++ )
                tmp[i] = tmp[i] * 180.0 / M_PI;                 // convert to degrees
        }
        else
        {
            for( int i = 0; i < aPoints; i++ )
                tmp[i] = 20 * log( tmp[i] ) / log( 10.0 );      // convert to dB
        }
    }

    trace->SetData( std::vector<double>( aX, aX + aPoints ), tmp );

    if( ( aFlags & SPT_AC_PHASE ) || ( aFlags & SPT_CURRENT ) )
        trace->SetScale( m_axis_x, m_axis_y2 );
    else
        trace->SetScale( m_axis_x, m_axis_y1 );

    trace->SetFlags( aFlags );

    m_plotWin->UpdateAll();

    return addedNewEntry;
}


bool SIM_PLOT_PANEL::DeleteTrace( const wxString& aName )
{
    auto it = m_traces.find( aName );

    if( it != m_traces.end() )
    {
        TRACE* trace = it->second;
        m_traces.erase( it );

        if( CURSOR* cursor = trace->GetCursor() )
            m_plotWin->DelLayer( cursor, true );

        m_plotWin->DelLayer( trace, true, true );
        ResetScales();

        return true;
    }

    return false;
}


void SIM_PLOT_PANEL::DeleteAllTraces()
{
    for( auto& t : m_traces )
    {
        DeleteTrace( t.first );
    }

    m_colorIdx = 0;
    m_traces.clear();
}


bool SIM_PLOT_PANEL::HasCursorEnabled( const wxString& aName ) const
{
    TRACE* t = GetTrace( aName );

    return t ? t->HasCursor() : false;
}


void SIM_PLOT_PANEL::EnableCursor( const wxString& aName, bool aEnable )
{
    TRACE* t = GetTrace( aName );

    if( t == nullptr || t->HasCursor() == aEnable )
        return;

    if( aEnable )
    {
        CURSOR* c = nullptr; //new CURSOR( t, this );
        int     plotCenter = GetPlotWin()->GetMarginLeft()
                         + ( GetPlotWin()->GetXScreen() - GetPlotWin()->GetMarginLeft()
                                   - GetPlotWin()->GetMarginRight() )
                                   / 2;
        c->SetX( plotCenter );
        t->SetCursor( c );
        m_plotWin->AddLayer( c );
    }
    else
    {
        CURSOR* c = t->GetCursor();
        t->SetCursor( NULL );
        m_plotWin->DelLayer( c, true );
    }

    // Notify the parent window about the changes
    wxQueueEvent( GetParent(), new wxCommandEvent( EVT_SIM_CURSOR_UPDATE ) );
}



bool SIM_PLOT_PANEL::ToggleCursors()
{
    if( GetTraces().size() > 0 )
    {
        bool visible = m_cursors.first.IsVisible();
        wxPrintf( "[SK] Setting cursors %svisible\n", visible ? "" : "in" );
        m_cursors.first .SetVisible( !visible );
        m_cursors.second.SetVisible( !visible );

        // Configure diff cursors
        //TODO temporary, move it to proper constructor
        int     quarterOfScreenWidth = ( m_plotWin->GetXScreen() -
                m_plotWin->GetMarginLeft() - m_plotWin->GetMarginRight() )
                                   / 4;
        wxPrintf("[SK] Setting cursors at: %d, %d",
                m_plotWin->GetMarginLeft() +   quarterOfScreenWidth, m_plotWin->GetMarginLeft() + 3*quarterOfScreenWidth);
        m_cursors.first .SetX( m_plotWin->GetMarginLeft() +   quarterOfScreenWidth );
        m_cursors.second.SetX( m_plotWin->GetMarginLeft() + 3*quarterOfScreenWidth );

        m_plotWin->UpdateAll();

        // Notify the parent window about the changes
        wxQueueEvent( GetParent(), new wxCommandEvent( EVT_SIM_CURSOR_UPDATE ) );
        return !visible;
    }
    else
        return false;
}


bool SIM_PLOT_PANEL::AreCursorsActive( char& aLabelFirst, char& aLabelSecond )
{
    aLabelFirst  = m_cursors.first .GetLabel();
    aLabelSecond = m_cursors.second.GetLabel();
    return m_cursors.first.IsVisible();
}


void SIM_PLOT_PANEL::ResetScales()
{
    if( m_axis_x )
        m_axis_x->ResetDataRange();

    if( m_axis_y1 )
        m_axis_y1->ResetDataRange();

    if( m_axis_y2 )
        m_axis_y2->ResetDataRange();

    for( auto t : m_traces )
        t.second->UpdateScales();
}


enum SIM_COLOR_SET SIM_PLOT_PANEL::generateColor()
{
    const unsigned int colorCount = m_masterFrame->GetPlotColorCount() - SIM_TRACE_COLOR;

    for( int i = SIM_TRACE_COLOR; i < (int)colorCount - 1; i++ )
    {
        bool hasColor = false;

        for( auto& t : m_traces )
        {
            TRACE* trace = t.second;

            if( trace->GetTraceColour() == i )
            {
                hasColor = true;
                break;
            }
        }

        if( !hasColor )
            return static_cast<enum SIM_COLOR_SET>( i );
    }

    // If all colors are in use, choose a suitable color in list
    int idx = m_traces.size() % colorCount;
    return static_cast<enum SIM_COLOR_SET>( idx + SIM_TRACE_COLOR );
}

wxDEFINE_EVENT( EVT_SIM_CURSOR_UPDATE, wxCommandEvent );
