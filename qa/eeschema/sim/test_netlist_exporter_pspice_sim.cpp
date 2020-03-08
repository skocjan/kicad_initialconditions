/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2020 S.Kocjan <s.kocjan@o2.pl>
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
 * Test suite for NETLIST_EXPORTER_PSPICE_SIM
 */

#include <string.h>
#include <unit_test_utils/unit_test_utils.h>
#include <vector>
#include <wx/string.h>

// Code under test
#include <sim/netlist_exporter_pspice_sim.h>

class TEST_NETLIST_EXPORTER_PSPICE_SIM
{
public:
    TEST_NETLIST_EXPORTER_PSPICE_SIM()
            : m_netlist( new NETLIST_OBJECT_LIST ), m_exporter( m_netlist )
    {
    }

    NETLIST_OBJECT_LIST*        m_netlist;
    NETLIST_EXPORTER_PSPICE_SIM m_exporter;
};


/**
 * Declare the test suite
 */
BOOST_FIXTURE_TEST_SUITE( NetlistExporterPspiceSim, TEST_NETLIST_EXPORTER_PSPICE_SIM )


/**
 * Check if simulation command is recognised properly
 */
BOOST_AUTO_TEST_CASE( CommandToSimType )
{
    struct TEST_DATA
    {
        wxString command;
        SIM_TYPE type;
    };

    std::vector<struct TEST_DATA> testData = {
        { ".op", ST_OP },
        { ".option TEMP=27", ST_UNKNOWN },
        { ".tran 0 1 0.1", ST_TRANSIENT },
        { ".tran 0 1 0.1 UIC", ST_TRANSIENT },
        { ".ac dec 10 1 10K", ST_AC },
        { ".ac dec 10 1K 100MEG", ST_AC },
        { ".ac lin 100 1 100HZ", ST_AC },
        { ".dc VIN 0.25 5.0 0.25", ST_DC },
        { ".dc VDS 0 10 .5 VGS 0 5 1", ST_DC },
        { ".dc VCE 0 10 .25 IB 0 10u 1u", ST_DC },
        { ".dc RLoad 1k 2k 100", ST_DC },
        { ".dc TEMP -15 75 5", ST_DC },
        { ".disto dec 10 1kHz 100MEG", ST_DISTORTION },
        { ".disto dec 10 1kHz 100MEG 0.9", ST_DISTORTION },
        { ".noise v(5) VIN dec 10 1kHz 100MEG", ST_NOISE },
        { ".noise v(5,3) V1 oct 8 1.0 1.0e6 1", ST_NOISE },
        { ".pz 1 0 3 0 cur pol", ST_POLE_ZERO },
        { ".pz 2 3 5 0 vol zer", ST_POLE_ZERO },
        { ".pz 4 1 4 1 cur pz", ST_POLE_ZERO },
        { ".SENS V(1,OUT)", ST_SENSITIVITY },
        { ".SENS V(OUT) AC DEC 10 100 100k", ST_SENSITIVITY },
        { ".SENS I(VTEST)", ST_SENSITIVITY },
        { ".tf v(5, 3) VIN", ST_TRANS_FUNC },
        { ".tf i(VLOAD) VIN", ST_TRANS_FUNC },
    };

    for( auto& step : testData )
    {
        BOOST_CHECK_EQUAL( m_exporter.CommandToSimType( wxString( step.command ) ), step.type );
    }

    for( auto& step : testData )
    {
        step.command.Append( "\n" );
        BOOST_CHECK_EQUAL( m_exporter.CommandToSimType( wxString( step.command ) ), step.type );
    }
}


BOOST_AUTO_TEST_SUITE_END()
