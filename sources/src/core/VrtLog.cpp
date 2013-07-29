/*
 *  Copyright 2013 Remi "Programmix" Cerise
 *
 *  This file is part of Virtuelium.
 *
 *  Virtuelium is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */
#include <core/VrtLog.hpp>
//!
//! @file VrtLog.cpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file instanciates the class declared in VrtLog.hpp:
//!  @arg VrtLog
//!
////////////////////////////////////////////////////////////////////////////////
std::ofstream VrtLog::m_stream;
////////////////////////////////////////////////////////////////////////////////
bool VrtLog::b_ready = false;