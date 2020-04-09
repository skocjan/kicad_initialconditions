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

#include <page_info.h>
#include <title_block.h>
#include <eda_draw_frame.h>
#include <sch_edit_frame.h>
#include <kiface_i.h>
#include <pgm_base.h>



struct KIWAY_MOCK : public KIWAY
{
    KIWAY_MOCK( PGM_BASE* aProgram, int aCtlBits, wxFrame* aTop = NULL ) :
        KIWAY( aProgram, aCtlBits, aTop ),
        m_frames( FRAME_T_COUNT )
    {};

    VTBL_ENTRY KIWAY_PLAYER* Player( FRAME_T aFrameType, bool doCreate = true,
                                     wxTopLevelWindow* aParent = NULL ) override;

    std::vector<KIWAY_PLAYER*> m_frames;

    void SetFrame( FRAME_T aFrameType, KIWAY_PLAYER* aPlayer )
    {
        m_frames[aFrameType] = aPlayer;
    }
};


KIWAY_MOCK& KiwayMock();
PGM_BASE& Pgm();
