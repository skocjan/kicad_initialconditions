/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2016 CERN
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

#include "netlist_exporter_pspice_sim.h"
#include <wx/tokenzr.h>

wxString NETLIST_EXPORTER_PSPICE_SIM::GetSpiceVector( const wxString& aName, SIM_PLOT_TYPE aType,
        const wxString& aParam ) const
{
    wxString res;

    // Some of the flags should exclude mutually
    assert( ( ( aType & SPT_VOLTAGE ) == 0 ) != ( ( aType & SPT_CURRENT ) == 0 ) );
    assert( ( ( aType & SPT_AC_PHASE ) == 0 ) || ( ( aType & SPT_AC_MAG ) == 0 ) );

    if( aType & SPT_VOLTAGE )
    {
        // netnames are escaped (can contain "{slash}" for '/') Unscape them:
        wxString spicenet = UnescapeString( aName );

        // Spice netlist netnames does not accept some chars, whicyh are replaced
        // by eeschema netlist generator.
        // Replace these forbidden chars to find the actual spice net name
        NETLIST_EXPORTER_PSPICE::ReplaceForbiddenChars( spicenet );

        return wxString::Format( "V(%s)", spicenet );
    }

    else if( aType & SPT_CURRENT )
    {
        return wxString::Format( "@%s[%s]", GetSpiceDevice( aName ).Lower(),
                aParam.IsEmpty() ? "i" : aParam.Lower() );
    }

    return res;
}


const std::vector<wxString>& NETLIST_EXPORTER_PSPICE_SIM::GetCurrents( SPICE_PRIMITIVE aPrimitive )
{
    static const std::vector<wxString> passive = { "I" };
    static const std::vector<wxString> diode = { "Id" };
    static const std::vector<wxString> bjt = { "Ib", "Ic", "Ie" };
    static const std::vector<wxString> mos = { "Ig", "Id", "Is" };
    static const std::vector<wxString> empty;

    switch( aPrimitive )
    {
        case SP_RESISTOR:
        case SP_CAPACITOR:
        case SP_INDUCTOR:
        case SP_VSOURCE:
            return passive;

        case SP_DIODE:
            return diode;

        case SP_BJT:
            return bjt;

        case SP_MOSFET:
            return mos;

        default:
            return empty;
    }
}


wxString NETLIST_EXPORTER_PSPICE_SIM::GetSheetSimCommand()
{
    wxString simCmd;

    UpdateDirectives();

    for( const auto& dir : GetDirectives() )
    {
        if( IsSimCommand( dir ) )
            simCmd += wxString::Format( "%s\r\n", dir );
    }

    return simCmd;
}


wxString NETLIST_EXPORTER_PSPICE_SIM::GetUsedSimCommand()
{
    return m_simCommand.IsEmpty() ? GetSheetSimCommand() : m_simCommand;
}


SIM_TYPE NETLIST_EXPORTER_PSPICE_SIM::GetSimType()
{
    return CommandToSimType( GetUsedSimCommand() );
}


SIM_TYPE NETLIST_EXPORTER_PSPICE_SIM::CommandToSimType( const wxString& aCmd )
{
    const std::map<wxString, SIM_TYPE> simCmds = {
        { ".ac", ST_AC }, { ".dc", ST_DC }, { ".disto", ST_DISTORTION }, { ".noise", ST_NOISE },
        { ".op", ST_OP }, { ".pz", ST_POLE_ZERO }, { ".sens", ST_SENSITIVITY }, { ".tf", ST_TRANS_FUNC },
        { ".tran", ST_TRANSIENT }
    };
    wxString lcaseCmd = aCmd.Lower();

    for( const auto& c : simCmds )
    {
        if( lcaseCmd.StartsWith( c.first ) )
            return c.second;
    }

    return ST_UNKNOWN;
}


bool NETLIST_EXPORTER_PSPICE_SIM::ParseDCCommand( const wxString& aCmd, SPICE_DC_PARAMS* aSource1,
                                                  SPICE_DC_PARAMS* aSource2 )
{
    if( !aCmd.Lower().StartsWith( ".dc" ) )
        return false;

    wxString cmd = aCmd.Mid( 3 ).Trim().Trim( false );

    wxStringTokenizer tokens( cmd );

    size_t num = tokens.CountTokens();

    if( num != 4 && num != 8 )
        return false;

    aSource1->m_source = tokens.GetNextToken();
    aSource1->m_vstart = SPICE_VALUE( tokens.GetNextToken() );
    aSource1->m_vend = SPICE_VALUE( tokens.GetNextToken() );
    aSource1->m_vincrement = SPICE_VALUE( tokens.GetNextToken() );

    if( num == 8 )
    {
        aSource2->m_source = tokens.GetNextToken();
        aSource2->m_vstart = SPICE_VALUE( tokens.GetNextToken() );
        aSource2->m_vend = SPICE_VALUE( tokens.GetNextToken() );
        aSource2->m_vincrement = SPICE_VALUE( tokens.GetNextToken() );
    }

    return true;
}


void NETLIST_EXPORTER_PSPICE_SIM::writeDirectives( OUTPUTFORMATTER* aFormatter ) const
{
    if( m_simCommand.IsEmpty() )
    {
        // Fallback to the default behavior and just write all directives
        NETLIST_EXPORTER_PSPICE::writeDirectives( aFormatter );
    }
    else
    {
        // Add here simulation options from DIALOG_SIM_SETTINGS
        for( const auto& opt : m_simOptions )
        {
            aFormatter->Print( 0, "%s\n", (const char*) opt.c_str() );
        }

        // Dump all directives but simulation commands
        for( const auto& dir : GetDirectives() )
        {
            if( !IsSimCommand( dir ) )
                aFormatter->Print( 0, "%s\n", (const char*) dir.c_str() );
        }

        // Finish with our custom simulation command
        aFormatter->Print( 0, "%s\n", (const char*) m_simCommand.c_str() );
    }
}


void NETLIST_EXPORTER_PSPICE_SIM::addNewOptionToList(const char* aOptionText, const wxString& aOptionValue)
{
    const char optionCard[] = ".option ";
    m_simOptions.push_back( wxString(optionCard) + wxString(aOptionText) + aOptionValue );
}


void NETLIST_EXPORTER_PSPICE_SIM::SetSimOptions( const struct PSPICE_SIM_OPTIONS& aOptions, bool aCustomCommand )
{
    ClearSimOptions();

    // Global options
    addNewOptionToList( "temp=", aOptions.m_temp );
    addNewOptionToList( "tnom=", aOptions.m_tnom );

    if( aOptions.m_flags & OPT_SIM_SAVE_CURRENTS )
    {
        addNewOptionToList( "", wxString( "savecurrents" ) );
        m_areCurrentsSaved = true;
    }
    else
    {
        m_areCurrentsSaved = false;
    }

    if( !aCustomCommand )
    {
        switch( GetSimType() )
        {
        case ST_OP:
        case ST_DC:
        case ST_AC:
        case ST_TRANSIENT:
            // add some options affecting OP analysis (three other ones will be affected too)
            addNewOptionToList( "abstol=", aOptions.m_absTol );
            addNewOptionToList( "reltol=", aOptions.m_relTol );
            addNewOptionToList( "vntol=",  aOptions.m_vnTol );

            if( aOptions.m_flags & OPT_SIM_USE_RSHUNT )
                addNewOptionToList( "rshunt=", aOptions.m_rShunt );

            break;

        case ST_UNKNOWN:
        case ST_DISTORTION:
        case ST_NOISE:
        case ST_POLE_ZERO:
        case ST_SENSITIVITY:
        case ST_TRANS_FUNC:
        default:
            //do nothing
            break;
        }

        switch( GetSimType() )
        {
        case ST_AC:
            if( aOptions.m_flags & OPT_SIM_AC_NO_OPERATING_POINT )
                addNewOptionToList( "", wxString( "noopac" ) );
            break;

        case ST_TRANSIENT:
            addNewOptionToList( "method=",
                wxString( aOptions.m_flags & OPT_SIM_TRAN_METHOD_GEAR ? "Gear" : "trapezoidal" ));
            addNewOptionToList( "chgtol=", aOptions.m_chgTol );

            if( aOptions.m_flags & OPT_SIM_USE_TRTOL )
                addNewOptionToList( "trtol=", aOptions.m_trTol );

            break;
        case ST_OP:
        case ST_DC:
            //they are already handled in previous switch
        case ST_DISTORTION:
        case ST_NOISE:
        case ST_POLE_ZERO:
        case ST_SENSITIVITY:
        case ST_TRANS_FUNC:
        case ST_UNKNOWN:
        default:
            //do nothing
            break;
        }
    }
}
