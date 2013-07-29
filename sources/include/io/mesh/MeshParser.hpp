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
#ifndef GUARD_VRT_MESHPARSER_HPP
#define GUARD_VRT_MESHPARSER_HPP
//!
//! @file MeshParser.hpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details Parse several formats of files for 3D meshes
//!
#include <string>
#include <istream>

#include <objectshapes/Mesh.hpp>
////////////////////////////////////////////////////////////////////////////////
//! @sruct Face 
//! @brief This structure is used by the MeshParser for the loading
struct Face {
 public :
  Point vertices[3];
  Vector normals[3];
  Point2D texcoords[3];
}; // struct Face
////////////////////////////////////////////////////////////////////////////////
//! @class MeshParser
//! @brief This class is used to load meshes from various file formats
class MeshParser {
 public:
  //! @brief Load a mesh from a Mesh3 file (VirtueliumIII files)
  //! @param filename File of the mesh to be loaded
  //! @deprecated
  Mesh* loadMesh3(std::string filename, bool double_sided);
  //! @brief Load a mesh from a wavefront OBJ file
  //! @param filename File of the mesh to be loaded
  Mesh* loadOBJ(std::string filename, bool double_sided);

 protected:
  //! @brief Jump to the next character that is not a whitespace
  //! @param stream : the istream in wich we jump.
  void eatWhiteSpace(std::istream& stream);
}; // class MeshParser
////////////////////////////////////////////////////////////////////////////////
#endif //GUARD_VRT_MESHPARSER_HPP
