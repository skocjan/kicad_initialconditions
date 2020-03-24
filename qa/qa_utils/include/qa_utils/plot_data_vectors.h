/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2019 KiCad Developers, see CHANGELOG.TXT for contributors.
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

#ifndef PLOT_DATA_VECTORS___H
#define PLOT_DATA_VECTORS___H

#include <vector>

namespace KI_TEST
{

using namespace std;

const size_t DATA_VECTOR_LENGTH = 50;

struct PLOT_DATA_VECTOR
{
    pair<vector<size_t>, vector<size_t>> (*Init)(void);
    vector<double> *m_x, *m_y;
    double m_minX, m_maxX, m_minY, m_maxY;
    size_t m_minYindex, m_maxYindex;
};


enum PLOT_DATA_VECTOR_ITEMS
{
    PDV_CONST_PLOT = 0,
    PDV_SINGLE_EXTREMUM_BEGIN = 1,
    PDV_LINEAR_RISING = 2,
    PDV_LINEAR_FALLING = 3,
    PDV_SINGLE_EXTREMUM_END = 4,
    PDV_MULTIPLE_EXTREMA_BEGIN = 4,
    PDV_MULTIPLE_EXTREMA_END = 8,
    PDV_ITEMS_MAX = 8
};

extern struct PLOT_DATA_VECTOR DataVectors[];

} // namespace KI_TEST

#endif // PLOT_DATA_VECTORS___H
