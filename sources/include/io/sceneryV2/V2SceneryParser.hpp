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
#ifndef GUARD_VRT_V2SCENERYPARSER_HPP
#define GUARD_VRT_V2SCENERYPARSER_HPP
//!
//! @file V2SceneryParser.hpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details Load a scenery from a scenery XML file. The scenery is loaded but
//!  the renderers aren't initialized. Parse the version 2 of the xml scenery
//!  file which uses a specific XML description that allows full custom 
//!  materials and lightings, and external inclusions
//! @todo Allows inclusion of virtuelium xml files
#include <string>
#include <fstream>

#include <core/Scenery.hpp>

#include <io/XMLTree.hpp>
#include <io/Parser.hpp>

#include <structures/HashMap.hpp>
#include <structures/HashFunctors.hpp>
////////////////////////////////////////////////////////////////////////////////
//! @class V2SceneryParser
//! @brief Parse the scenery file (v2)
class V2SceneryParser : public Parser {
 public:
  //! @brief Default constructor
  V2SceneryParser(void);
  //! @brief Parse a scenery file and build the corresponding scenery
  //! @param root Root xml node of the scenery
  //! @param sceneryFilename Filename of the scenery to be loaded
  Scenery* ParseAndBuildScenery(
      XMLTree* root, 
      const std::string& sceneryFilename);

 private:
  //! @brief Add a medium into the scenery
  //! @param node Node containing data for the medium
  void AddMedium(XMLTree* node);
  //! @brief Return the texture
  //! @param name Name of the texture
  //! @return Return the texture
  Texture* GetTexture(std::string name);
  //! @brief Add a texture into the scenery
  //! @param node node containing data for the texture
  void AddTexture(XMLTree* node);
  //! @brief Return the medium
  //! @param name Name of the medium
  //! @return Return the medium
  Medium* GetMedium(std::string name);
  //! @briefAdd a surface into the scenery
  //! @param node Node containing data for the surface
  void AddSurface(XMLTree* node);
  //! @biref Add a camera into the scenery.
  //! @param node Node containing data for the camera
  void AddCamera(XMLTree* node);
  //! @brief Add a source into the scenery
  //! @param node Node containing data for the source
  void AddSource(XMLTree* node);
  //! @brief Set the renderer for the scenery (previous renderer will be freed)
  //! @param node Node containing data for the renderer
  void AddRenderer(XMLTree* node);
  //! @brief Parse an included scenery file
  //! @details Parse the file declared by the <include> tag. This file can be a
  //!  sub-xml file or an external scene description with an available file 
  //!  format.
  //! @param node Node containing data for the renderer
  void AddIncludeScenery(XMLTree* node);
  //! @brief Build the scenery with all the stored informations. 
  //! @remarks All temporary array will be freed.
  //! @return the generated scenery
  Scenery* GenerateScenery(void);

 private:
  //! List of objects in the scene
  std::vector<Object*> m_objects;
  //! List of light sources in the scene
  std::vector<Source*> m_sources;
  //! List of cameras in the scene (size of this vector mostly equal to 1)
  std::vector<Camera*> m_cameras;
  //! List of Medium instances
  std::vector<Medium*> m_mediaList;
  //! List of declared media with their names
  HashMap<std::string, Medium*, StringHashFunctor> m_mediaMap;
  //! List of Texture instances 
	std::vector<Texture*> m_textureList;
  //! List of declared textures with their names
  HashMap<std::string, Texture*, StringHashFunctor> m_textureMap;
  //! Renderer read 
  Renderer* p_renderer;
  //! Bias paramerter
  Real m_bias;
}; // class V2SceneryParser
////////////////////////////////////////////////////////////////////////////////
#endif // GUARD_VRT_V2SCENERYPARSER_HPP
