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
#ifndef GUARD_VRT_GLOBALSCENERYPARSER_HPP
#define GUARD_VRT_GLOBALSCENERYPARSER_HPP
//!
//! @file GlobalSceneryParser.hpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details Load a scenery from a scenery XML file. The scenery is loaded but
//!  the renderers aren't initialized. There are several versions of the scenery
//!  file:
//!  @arg v1: Not used anymore;
//!  @arg v2: Use a specific XML description.
//!
#include <string>
#include <fstream>

#include <core/Scenery.hpp>

#include <io/Parser.hpp>
#include <io/XMLTree.hpp>

#include <renderers/Renderer.hpp>
////////////////////////////////////////////////////////////////////////////////
//! @class GlobalSceneryParser
//! @brief Parse the XML scenery file
//! @details This class manages the scenery versioning by calling the 
//!  appropriate scenery parser regarding to the version number of the <scenery>
//!  tag;
class GlobalSceneryParser : public Parser {
 public:
  //! @brief Parse a scenery file and build the corresponding scenery
  //! @param sceneryFilename Scenery filename to be loaded
  Scenery* ParseAndBuildScenery(const std::string& sceneryFilename);

 private:
  //! @brief Parse and build the XMLTree from a file
  //! @param filename Filename of the XML file to parse
  //! @return XMLTree that correspond to the file
  XMLTree* GetXMLTree(const std::string& filename);
}; // class GlobalSceneryParser
////////////////////////////////////////////////////////////////////////////////
#endif // GUARD_VRT_GLOBALSCENERYPARSER_HPP
