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
#include <io/GlobalSceneryParser.hpp>
//!
//! @file V2SceneryParser.cpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file implements classs declared in V2SceneryParser.hpp 
//!  @arg V2SceneryParser
//!
#include <io/XMLParser.hpp>
#include <io/sceneryV2/V2SceneryParser.hpp>

#include <exceptions/Exception.hpp>
//////////////////////// class GlobalSceneryParser /////////////////////////////
Scenery* GlobalSceneryParser::ParseAndBuildScenery(
    const std::string& sceneryFilename) {
  
  //Parse the XML file and get the tree
  XMLTree* root = GetXMLTree(sceneryFilename);
  
  //Verify that the root is the <scenery> markup
  if(root->getMarkup() != "scenery") {
    throw Exception("(GlobalSceneryParser::parseAndBuildScenery) Impossible de \
trouver la racine (<scenery>) du fichier " + sceneryFilename);
  }
  
  std::string version = root->getAttributeValue("version");
  Scenery * scenery=0;

  // Version 1
  if(version.compare("1") == 0) {
    throw Exception("(GlobalSceneryParser::parseAndBuildScenery) La version 1 \
du fichier de scène est devenue obsolète.");

  // Version 2
  } else if(version.compare("2") == 0) {
    V2SceneryParser parser;
    scenery = parser.ParseAndBuildScenery(root, sceneryFilename);
  
  // Unknown version
  } else {
    throw Exception("(GlobalSceneryParser::parseAndBuildScenery) Version du \
fichier de scène " + sceneryFilename + "invalide (" + version +")");
  }

  delete root;
  return scenery;
}
//////////////////////// class GlobalSceneryParser /////////////////////////////
XMLTree* GlobalSceneryParser::GetXMLTree(const std::string& filename) {
  // Open the file
  std::fstream xmlfile(filename.c_str(), std::fstream::in);
  if(!xmlfile.is_open())
    throw Exception("(GlobalSceneryParser::getXMLTree) N'a pas pu ouvrir le \
fichier " + filename);
  
  // Build parser and parse
  XMLParser parser;
  XMLTree* xmltree = parser.parseAndCreateTree(xmlfile);
  
  // Close and return the tree
  xmlfile.close();
  return xmltree;
}
////////////////////////////////////////////////////////////////////////////////