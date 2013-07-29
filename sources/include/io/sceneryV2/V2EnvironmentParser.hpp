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
#ifndef GUARD_VRT_V2ENVIRONMENTPARSER_HPP
#define GUARD_VRT_V2ENVIRONMENTPARSER_HPP
//!
//! @file V2EnvironmentParser.hpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @remarks
//! @details This file is charged to retrieve environment information from XML 
//!  node in Virtuelium scene file (<environment>...</environment>)
//!
#include <io/XMLTree.hpp>
#include <io/Parser.hpp>

#include <structures/Texture.hpp>
#include <structures/HashMap.hpp>
#include <structures/HashFunctors.hpp>
////////////////////////////////////////////////////////////////////////////////
//! @see Environment
class Environment;
////////////////////////////////////////////////////////////////////////////////
//! @class V2EnvironmentParser
//! @brief Parses renderer node of Virtuelium scene file (V2)
//! @details This parser loads several V2EnvironmentParser while parsing the XML 
//!  node.
//! @todo Say what the 'Photon Mapping Estimation' rendering engine actually is 
class V2EnvironmentParser : public Parser {
 public:
  //! @brief Create a environment by using the node information
  //! @paramnode XML node to be read
  //! @return Pointer to the created environment object
	Environment* Create(
      XMLTree* node,
      const HashMap<std::string, Texture*, StringHashFunctor> &textureMap);

private:
  //! @brief Create an environment based on spherical mapping 
  //! @param node XML node to be read
  //! @param textureMap List of decleared textures
  //! @return Pointer to the created Renderer object
	Environment* CreateSphericalEnvironment(
      XMLTree* node,
      const HashMap<std::string, Texture*, StringHashFunctor> &textureMap);
  //! @brief Create an environment based on a uniform spectrum 
  //! @param node XML node to be read
  //! @return Pointer to the created Renderer object
	Environment* CreateSpectralEnvironment(
      XMLTree* node);

 private:
  //! @brief Return the texture
  //! @param name Name of the texture
  //! @return Texture
  Texture* getTexture(
      const HashMap<std::string, Texture*, StringHashFunctor> &textureMap, 
      std::string name);
}; // class V2EnvironmentParser
////////////////////////////////////////////////////////////////////////////////
#endif // GUARD_VRT_V2ENVIRONMENTPARSER_HPP
