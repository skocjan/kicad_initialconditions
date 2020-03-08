/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2016 CERN
 * @author Sylwester Kocjan <s.kocjan@o2.pl>
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

#include "sim_plot_panel_base.h"
#include "sim_plot_frame.h"

#include <wx/sizer.h>


SIM_PLOT_PANEL_BASE::SIM_PLOT_PANEL_BASE()
    : m_type( ST_INVALID )
{
}


SIM_PLOT_PANEL_BASE::SIM_PLOT_PANEL_BASE( enum SIM_TYPE aType )
    : m_type( aType )
{
}


SIM_PLOT_PANEL_BASE::~SIM_PLOT_PANEL_BASE()
{
}


bool SIM_PLOT_PANEL_BASE::IsPlottable( SIM_TYPE aSimType )
{
    switch( aSimType )
    {
        case ST_AC:
        case ST_DC:
        case ST_TRANSIENT:
            return true;

        default:
            return false;
    }
}


SIM_NOPLOT_PANEL::SIM_NOPLOT_PANEL( SIM_TYPE aType, wxWindow* parent, //SIM_PLOT_FRAME* aMainFrame, wxWindowID id,
                const wxPoint& pos, const wxSize& size, long style, const wxString& name )
    : SIM_PLOT_PANEL_BASE( aType ),
      wxPanel( parent, wxID_ANY, pos, size, style, name )
{
    //m_panel = new wxPanel();
    m_sizer = new wxBoxSizer( wxVERTICAL );
    m_textInfo = new wxStaticText( parent, wxID_ANY,
                    _( "This simulation provide no plots. Please refer to console window for results" ),
                    wxDefaultPosition, wxDefaultSize );
    m_sizer->Add( m_textInfo, 0, wxALL, 10 );
}


SIM_NOPLOT_PANEL::~SIM_NOPLOT_PANEL()
{
    delete m_textInfo;
    delete m_sizer;
    //delete m_panel;
}

