/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2020 KiCad Developers, see AUTHORS.TXT for contributors.
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
 * Several data vectors used for testing plotting
 */

#include <qa_utils/plot_data_vectors.h>


namespace KI_TEST
{
using namespace std;


static vector<double> temp(DATA_VECTOR_LENGTH);

// Length of vectors below is 50 elements (DATA_VECTOR_LENGTH):
static vector<double> linearZero2Pi = { 0.00, 0.13, 0.25, 0.38, 0.50, 0.63, 0.75, 0.88, 1.01, 1.13,
1.26, 1.38, 1.51, 1.63, 1.76, 1.88, 2.01, 2.14, 2.26, 2.39, 2.51, 2.64, 2.76, 2.89, 3.02, 3.14,
3.27, 3.39, 3.52, 3.64, 3.77, 3.90, 4.02, 4.15, 4.27, 4.40, 4.52, 4.65, 4.78, 4.90, 5.03, 5.15,
5.28, 5.40, 5.53, 5.65, 5.78, 5.91, 6.03, 6.16 };

static vector<double> sinus = { 0.00, 0.13, 0.25, 0.37, 0.48, 0.59, 0.68, 0.77, 0.84, 0.90, 0.95,
0.97, 0.99, 1.00, 0.98, 0.95, 0.90, 0.84, 0.77, 0.68, 0.59, 0.48, 0.37, 0.25, 0.13, 0.00,
-0.13, -0.25, -0.37, -0.48, -0.59, -0.68, -0.77, -0.84, -0.90, -0.95, -0.97, -0.99, -1.00,
-0.98, -0.95, -0.90, -0.84, -0.77, -0.68, -0.59, -0.48, -0.37, -0.25, -0.13 };


static vector<double> cosinus3dot5XplusX = { 1.00, 1.03, 0.89, 0.63, 0.32, 0.04, -0.14, -0.12,
0.08, 0.45, 0.95, 1.51, 2.04, 2.48, 2.75, 2.84, 2.74, 2.50, 2.20, 1.91, 1.70, 1.66, 1.80, 2.12,
2.59, 3.14, 3.69, 4.16, 4.49, 4.63, 4.58, 4.38, 4.08, 3.78, 3.54, 3.45, 3.53, 3.81, 4.24, 4.78,
5.34, 5.84, 6.21, 6.40, 6.41, 6.24, 5.97, 5.66, 5.39, 5.25 };

size_t MIN_cosinus3dot5XplusX[] = {0, 6, 21, 35, 49};
size_t MAX_cosinus3dot5XplusX[] = {1, 15, 29, 44};


static vector<double> sinus3XminusX = { 0.00, 0.24, 0.43, 0.53, 0.50, 0.32, 0.02, -0.40, -0.88,
-1.38, -1.84, -2.23, -2.49, -2.62, -2.60, -2.47, -2.26, -2.01, -1.78, -1.62, -1.56, -1.64, -1.86,
-2.21, -2.65, -3.14, -3.64, -4.08, -4.42, -4.64, -4.72, -4.67, -4.50, -4.27, -4.02, -3.81, -3.68,
-3.67, -3.79, -4.06, -4.44, -4.90, -5.40, -5.89, -6.30, -6.61, -6.78, -6.81, -6.72, -6.53 };

size_t MIN_sinus3XminusX[] = {0, 13, 30, 47};
size_t MAX_sinus3XminusX[] = {3, 20, 37, 49};


static vector<double> cosinus3XdivX = { 0.32, 0.31, 0.25, 0.15, 0.02, -0.12, -0.27, -0.39, -0.46,
-0.48, -0.43, -0.30, -0.11, 0.12, 0.39, 0.64, 0.86, 0.99, 1.00, 0.85, 0.49, -0.12, -1.13, -2.90,
-7.40, 0.00, 7.40, 2.90, 1.13, 0.12, -0.49, -0.85, -1.00, -0.99, -0.86, -0.64, -0.39, -0.12, 0.11,
0.30, 0.43, 0.48, 0.46, 0.39, 0.27, 0.12, -0.02, -0.15, -0.25, -0.31 };

size_t MIN_cosinus3XdivX[] = {9, 24, 32, 49};
size_t MAX_cosinus3XdivX[] = {0, 18, 26, 41};


static vector<double> cosinus3XdivXWoZero = { 0.55, 0.32, 0.31, 0.25, 0.15, 0.02, -0.12, -0.27,
-0.39, -0.46, -0.48, -0.43, -0.30, -0.11, 0.12, 0.39, 0.64, 0.86, 0.99, 1.00, 0.85, 0.49, -0.12,
-1.13, -2.90, -7.40, 7.40, 2.90, 1.13, 0.12, -0.49, -0.85, -1.00, -0.99, -0.86, -0.64, -0.39,
-0.12, 0.11, 0.30, 0.43, 0.48, 0.46, 0.39, 0.27, 0.12, -0.02, -0.15, -0.25, -0.31 };

size_t MIN_cosinus3XdivXWoZero[] = {10, 25, 32, 49};
size_t MAX_cosinus3XdivXWoZero[] = {0, 19, 26, 41};


static pair<vector<size_t>, vector<size_t>> DoNothing()
{
    // dummy retval
    vector<size_t> retVal;
    return make_pair( retVal, retVal );
}


static pair<vector<size_t>, vector<size_t>> MinusX()
{
    for(vector<size_t>::size_type i = 0; i < linearZero2Pi.size(); i++)
        temp[i] = -linearZero2Pi[i];

    // dummy retval
    vector<size_t> retVal;
    return make_pair( retVal, retVal );
}


static const double M_CONST = 17.75;
static pair<vector<size_t>, vector<size_t>> Constans()
{
    for(vector<double>::size_type i = 0; i < linearZero2Pi.size(); i++)
        temp[i] = M_CONST;

    // dummy retval
    vector<size_t> retVal;
    return make_pair( retVal, retVal );
}


static pair<vector<size_t>, vector<size_t>> Cosinus3dot5XplusX()
{
    vector<size_t> mins( MIN_cosinus3dot5XplusX,
                    MIN_cosinus3dot5XplusX
                    + sizeof( MIN_cosinus3dot5XplusX ) / sizeof( MIN_cosinus3dot5XplusX[0] ) );
    vector<size_t> maxes( MAX_cosinus3dot5XplusX,
                    MAX_cosinus3dot5XplusX
                    + sizeof( MAX_cosinus3dot5XplusX ) / sizeof( MAX_cosinus3dot5XplusX[0] ) );

    return make_pair( mins, maxes );
}


static pair<vector<size_t>, vector<size_t>> Sinus3XminusX()
{
    vector<size_t> mins( MIN_sinus3XminusX,
                    MIN_sinus3XminusX
                    + sizeof( MIN_sinus3XminusX ) / sizeof( MIN_sinus3XminusX[0] ) );
    vector<size_t> maxes( MAX_sinus3XminusX,
                    MAX_sinus3XminusX
                    + sizeof( MAX_sinus3XminusX ) / sizeof( MAX_sinus3XminusX[0] ) );

    return make_pair( mins, maxes );
}


static pair<vector<size_t>, vector<size_t>> Cosinus3XdivX()
{
    vector<size_t> mins( MIN_cosinus3XdivX,
                    MIN_cosinus3XdivX
                    + sizeof( MIN_cosinus3XdivX ) / sizeof( MIN_cosinus3XdivX[0] ) );
    vector<size_t> maxes( MAX_cosinus3XdivX,
                    MAX_cosinus3XdivX
                    + sizeof( MAX_cosinus3XdivX ) / sizeof( MAX_cosinus3XdivX[0] ) );

    return make_pair( mins, maxes );
}


static pair<vector<size_t>, vector<size_t>> Cosinus3XdivXWoZero()
{
    vector<size_t> mins( MIN_cosinus3XdivXWoZero,
                    MIN_cosinus3XdivXWoZero
                    + sizeof( MIN_cosinus3XdivXWoZero ) / sizeof( MIN_cosinus3XdivXWoZero[0] ) );
    vector<size_t> maxes( MAX_cosinus3XdivXWoZero,
                    MAX_cosinus3XdivXWoZero
                    + sizeof( MAX_cosinus3XdivXWoZero ) / sizeof( MAX_cosinus3XdivXWoZero[0] ) );

    return make_pair( mins, maxes );
}


struct PLOT_DATA_VECTOR DataVectors [] =
{
        // f(x) = const
    { .Init = Constans, .m_x = &linearZero2Pi, .m_y = &temp, .m_minX = 0.0,
      .m_maxX = 6.16, .m_minY = M_CONST, .m_maxY = M_CONST, .m_minYindex = 0, .m_maxYindex = 0
    },
        // f(x) = sin(x)
    { .Init = DoNothing, .m_x = &linearZero2Pi, .m_y = &sinus, .m_minX = 0.0, .m_maxX = 6.16,
      .m_minY = -1.0, .m_maxY = 1.00, .m_minYindex = 38, .m_maxYindex = 13
    },
        // f(x) = x
    { .Init = DoNothing, .m_x = &linearZero2Pi, .m_y = &linearZero2Pi, .m_minX = 0.0,
      .m_maxX = 6.16, .m_minY = 0.0, .m_maxY = 6.16, .m_minYindex = 0, .m_maxYindex = 49
    },
        // f(x) = -x
    { .Init = MinusX, .m_x = &linearZero2Pi, .m_y = &temp, .m_minX = 0.0,
      .m_maxX = 6.16, .m_minY = -6.16, .m_maxY = 0.0, .m_minYindex = 49, .m_maxYindex = 0
    },
        // f(x) = cos(3.5*x)+x
    { .Init = Cosinus3dot5XplusX, .m_x = &linearZero2Pi, .m_y = &cosinus3dot5XplusX, .m_minX = 0.0,
      .m_maxX = 6.16, .m_minY = -0.14, .m_maxY = 6.41, .m_minYindex = 6, .m_maxYindex = 44
    },
        // f(x) = sin(3*x)-x
    { .Init = Sinus3XminusX, .m_x = &linearZero2Pi, .m_y = &sinus3XminusX, .m_minX = 0.0,
      .m_maxX = 6.16, .m_minY = -6.81, .m_maxY = 0.53, .m_minYindex = 47, .m_maxYindex = 3
    },
        // f(x) = cos(3*(x-M_PI)) / x, with zero in the centre of plot
    { .Init = Cosinus3XdivX, .m_x = &linearZero2Pi, .m_y = &cosinus3XdivX, .m_minX = 0.0,
      .m_maxX = 6.16, .m_minY = -7.40, .m_maxY = 7.40, .m_minYindex = 24, .m_maxYindex = 26
    },
        // f(x) = cos(3*(x-M_PI)) / x, without zero in the centre of plot (non-contiguous)
    { .Init = Cosinus3XdivXWoZero, .m_x = &linearZero2Pi, .m_y = &cosinus3XdivXWoZero, .m_minX = 0.0,
      .m_maxX = 6.16, .m_minY = -7.40, .m_maxY = 7.40, .m_minYindex = 25, .m_maxYindex = 26
    }
};




} // namespace KI_TEST
