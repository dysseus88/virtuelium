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
#include <io/sceneryV2/V2RendererParser.hpp>
//!
//! @file V2RendererParser.cpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file implements the classes declared in V2RendererParser.hpp 
//!  @arg V2RendererParser
//! @todo 
//! @remarks 
//!
#include <string>

#include <core/3DBase.hpp>

#include <exceptions/Exception.hpp>

#include <io/DataParser.hpp>
#include <io/sceneryV2/V2EnvironmentParser.hpp>

#include <renderers/TestRenderer.hpp>
#include <renderers/SimpleRenderer.hpp>
#include <renderers/PhotonMappingRenderer.hpp>

#include <environments/Environment.hpp>
/////////////////////// class V2RendererParser /////////////////////////////////
Renderer* V2RendererParser::Create(
    XMLTree* node,
    const HashMap<std::string, Texture*, StringHashFunctor> *textureMap) {

  std::string type = node->getAttributeValue("type");
  
  // Simple renderer
  if(type == "SimpleRenderer") {
    return CreateSimpleRenderer(node, *textureMap);

  // photon mapping
  } else if(type == "PhotonMapping") {
    return CreatePhotonMapping(node, *textureMap);

  // Test
  } else if(type == "Test") {
    return CreateTestRenderer(node, *textureMap);
  }

  // error case
  throw Exception("(V2RendererParser::create) Type de <Renderer> " 
                  + type + " inconnu.");
}
/////////////////////// class V2RendererParser /////////////////////////////////
Renderer* V2RendererParser::CreateTestRenderer(
    XMLTree* node,
    const HashMap<std::string, Texture*, StringHashFunctor> &textureMap) {

  int max_depth = getIntegerValue(node, "maxdepth", 10);
  Real scale = getRealValue(node, "scale", 1.0);

  // Create Renderer object
  return new TestRenderer(max_depth, scale);
}
/////////////////////// class V2RendererParser /////////////////////////////////
Renderer* V2RendererParser::CreateSimpleRenderer(
    XMLTree* node,
    const HashMap<std::string, Texture*, StringHashFunctor> &textureMap) {

  int max_depth = getIntegerValue(node, "maxdepth", 10);
  Real scale = getRealValue(node, "scale", 1.0);

  // Ambient lighting is defined by 2 parameters:
  // - power
  // - illuminant
  Real ambient_power = -1.0;
  Spectrum ambient_illum;
  if(node->getAttributeValue("ambientpower") != "") {
    ambient_power = getRealValue(node, "ambientpower", Real(0.01));
    DataParser parser;
    parser.loadSpectralIlluminant(node->getAttributeValue("ambientillum"), 
                                  ambient_illum);
  } else {
    for(unsigned int i = 0; i < GlobalSpectrum::nbWaveLengths(); i++)
      ambient_illum[i] = 0.0;
  }
  
  // Environment: see child node
  Environment* environment = NULL;
  if(node->getNumberOfChildren() > 0 ) {
    XMLTree* envnode = node->getChild(0);
    
    // parse environment
    if (envnode != NULL && envnode->getMarkup() == "environment") {
      V2EnvironmentParser parser;
      environment = parser.Create(node->getChild(0), textureMap); 

    // error case
    } else {
      throw Exception("(V2RendererParser::CreateSimpleRenderer) Balise <"
                        + envnode->getMarkup()
                        + "> inconnue dans le noeud du renderer.");
    }
  }

  // Create Renderer object
  return new SimpleRenderer(max_depth, scale, 
                            ambient_illum, ambient_power, environment);
}
/////////////////////// class V2RendererParser /////////////////////////////////
Renderer* V2RendererParser::CreatePhotonMapping(
    XMLTree* node,
    const HashMap<std::string, Texture*, StringHashFunctor> &textureMap) {

  int max_depth = getIntegerValue(node, "maxdepth", 10);
  Real scale = getRealValue(node, "scale", 1.0);
  Real search_global_radius = getRealValue(node, "globalphotonradius", 1.0);
  Real search_caustic_radius = getRealValue(node, "causticphotonradius", 1.0);
  unsigned int nb_global_photon = getIntegerValue(node, "nbglobalphotons", 1000);
  unsigned int nb_sample_global_photon = getIntegerValue(node, 
                                                         "nbglobalsamples", 30);
  unsigned int nb_caustic_photon = getIntegerValue(node, "nbcausticphotons", 0);
  unsigned int nb_sample_caustic_photon= getIntegerValue(node, 
                                                         "nbcausticsamples", 0);
  unsigned int nb_samples = getIntegerValue(node, "nbsamples", 0);
  Real estimation_min_distance = getRealValue(node, "estimationdistance", 0.2);

  // Environment: see child node
  Environment* environment = NULL;
  if(node->getNumberOfChildren() > 0 ) {
    XMLTree* envnode = node->getChild(0);
    
    // parse environment
    if (envnode != NULL && envnode->getMarkup() == "environment") {
      V2EnvironmentParser parser;
      environment = parser.Create(node->getChild(0), textureMap); 

    // error case
    } else {
      throw Exception("(V2RendererParser::CreateSimpleRenderer) Balise <"
                        + envnode->getMarkup()
                        + "> inconnue dans le noeud du renderer.");
    }
  }

  // Create Renderer object
  return new PhotonMappingRenderer(max_depth, scale, 
                                   nb_global_photon, 
                                   nb_caustic_photon, 
                                   nb_sample_global_photon, 
                                   nb_sample_caustic_photon, 
                                   search_global_radius, 
                                   search_caustic_radius, 
                                   nb_samples, 
                                   estimation_min_distance, 
                                   environment);
}
////////////////////////////////////////////////////////////////////////////////

