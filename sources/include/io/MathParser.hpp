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
 */
#ifndef GUARD_VRT_MATHPARSER_HPP
#define GUARD_VRT_MATHPARSER_HPP
//!
//! @file MathParser.hpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @remarks
//! @details This file contains miscellaneous methodes to parse various 
//!  mathemathics objects
//!
#include <string>
#include <core/3DBase.hpp>
////////////////////////////////////////////////////////////////////////////////
//! @brief Contains miscellaneous methodes to parse various mathemathics objects
//! @details
class MathParser {
 public:
  //! @brief Load a matrix from a file
  //! @param width Width of the matrix to be loaded
  //! @param height Height of the matrix to be loaded
  //! @param filename Input file 
  Real* loadMatrix(unsigned int width, unsigned int height, 
                   std::string filename);
}; // class MathParser
////////////////////////////////////////////////////////////////////////////////
#endif //GUARD_VRT_MATHPARSER_HPP
