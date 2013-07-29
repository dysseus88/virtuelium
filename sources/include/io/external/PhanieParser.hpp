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
#ifndef GUARD_VRT_PHANIEPARSER_HPP
#define GUARD_VRT_PHANIEPARSER_HPP
//!
//! @file PhanieParser.hpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details Parse Phanie file format
//!
#include <string>
#include <istream>
#include <vector>

#include <io/mesh/MeshParser.hpp>
////////////////////////////////////////////////////////////////////////////////
//! @see Object
class Object;
//! @see Source
class Source;
////////////////////////////////////////////////////////////////////////////////
//! @class PhanieParser
//! @brief This class is used to load meshes from Phanie file format
//! @desc Phanie is a software which physically simulates lighting, produced by 
//!  the Centre Technique et Scientifique des Batiments (CSTB). Phanie file 
//!  format (.pha) is used by the CALLISTO-SARI project.
class PhanieParser: public MeshParser {
 public:
  //! @brief Load a mesh from a PHA file
  //! @details The .pha file format contains all
  //!  the geometrical and material info for a whole 3D scene.
  //! @param filename File of the mesh to be loaded
  //! @param objects List of resulted objects
  //! @param sources List of resulted sources
  //!  data
  bool LoadPhanie(std::string filename, 
                  std::vector<Object*>& objects,
                  std::vector<Source*>& sources);

 private:
  //! @brief Calculate texture coordinates from XYZ coordinates
  //! @details tu = m11*Px + m12*Py + m13*Pz + m14;\n
  //!          tv = m21*Px + m22*Py + m23*Pz + m24; 
  inline void ComputeTextureCoordinate(const Point& p, Point2D& t, Real m[8]) {
    t[0] = p[0] * m[0] + p[1] * m[1] + p[2] * m[2] + m[3];
    t[1] = p[0] * m[4] + p[1] * m[5] + p[2] * m[6] + m[3];
  }
}; // class PhanieParser
////////////////////////////////////////////////////////////////////////////////
#endif // GUARD_VRT_PHANIEPARSER_HPP
