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
#include <io/sceneryV2/V2EnvironmentParser.hpp>
//!
//! @file V2EnvironmentParser.cpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file implements the classes declared in V2EnvironmentParser.hpp 
//!  @arg V2EnvironmentParser
//! @todo 
//! @remarks 
//!
#include <string>
#include <core/3DBase.hpp>
#include <exceptions/Exception.hpp>

#include <io/DataParser.hpp>

#include <environments/Environment.hpp>
#include <environments/SphericalEnvironment.hpp>
#include <environments/SpectralEnvironment.hpp>
////////////////////////////////////////////////////////////////////////////////
Environment* V2EnvironmentParser::Create(
    XMLTree* node,
    const HashMap<std::string, Texture*, StringHashFunctor> &textureMap) {

  std::string type = node->getAttributeValue("type");
  
  // Simple environment
  if(type.compare("Spherical") == 0) 
    return CreateSphericalEnvironment(node, textureMap);

  // Simple environment
  if(type.compare("Spectral") == 0) 
    return CreateSpectralEnvironment(node);

  // error case
  throw Exception("(V2EnvironmentParser::create) Type de <environment> " 
                  + type + " inconnu.");
}
////////////////////////////////////////////////////////////////////////////////
Environment* V2EnvironmentParser::CreateSphericalEnvironment(
    XMLTree* node,
    const HashMap<std::string, Texture*, StringHashFunctor> &textureMap) {

  Texture* environment = NULL;

  std::string name = node->getAttributeValue("name");
  environment = getTexture(textureMap, name);

  Real environment_power = getRealValue(node, "envpower", Real(0.01));

  return new SphericalEnvironment(environment, environment_power);
}
////////////////////////////////////////////////////////////////////////////////
Environment* V2EnvironmentParser::CreateSpectralEnvironment(XMLTree* node) {
  Spectrum *s_envir = new Spectrum;

  DataParser parser;
  parser.loadSpectralIlluminant(node->getAttributeValue("spectrum"), 
                                *s_envir);

  Real environment_power = getRealValue(node, "envpower", Real(0.01));

  return new SpectralEnvironment(s_envir, environment_power);
}
////////////////////////////////////////////////////////////////////////////////
Texture* V2EnvironmentParser::getTexture(
    const HashMap<std::string, Texture*, StringHashFunctor> &textureMap,
    std::string name) {
  if(textureMap.contain(name)) {
    return textureMap.get(name);
  }
  throw Exception("(V2EnvironmentParser::getTexture) La texture " 
                  + name + " n'a pas été déclarée.");
}
////////////////////////////////////////////////////////////////////////////////



