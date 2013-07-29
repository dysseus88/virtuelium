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
#include <io/sceneryV2/V2SceneryParser.hpp>
//!
//! @file V2SceneryParser.cpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file implements classs declared in V2SceneryParser.hpp 
//!  @arg V2SceneryParser
//!
#include <exceptions/Exception.hpp>
#include <core/VrtLog.hpp>

#include <io/XMLParser.hpp>
#include <io/DataParser.hpp>

#include <io/sceneryV2/V2RendererParser.hpp>
#include <io/sceneryV2/V2CameraShapeParser.hpp>
#include <io/sceneryV2/V2ColorHandlerParser.hpp>

#include <io/sceneryV2/V2ObjectShapeParser.hpp>
#include <io/sceneryV2/V2MaterialParser.hpp>
#include <io/sceneryV2/V2LightSourceParser.hpp>

#include <io/external/PhanieParser.hpp>
////////////////////////////// class V2SceneryParser /////////////////////////////
V2SceneryParser::V2SceneryParser(void)
    : m_mediaMap(StringHashFunctor(), 100),
      m_textureMap(StringHashFunctor(), 100),
      p_renderer(NULL) {
  //Nothing to do more
}
////////////////////////////// class V2SceneryParser /////////////////////////////
Scenery* V2SceneryParser::ParseAndBuildScenery(
    XMLTree* root, 
    const std::string& sceneryFilename) {
  VrtLog::Write("V2SceneryParser::parseAndBuildScenery");

  //Getting spectrum of the scene
  std::vector<Real> wavelengths;
  DataParser dataparser;
  dataparser.loadWaveLenghts(root->getAttributeValue("spectrum"), wavelengths);
  GlobalSpectrum::init(wavelengths);

  m_bias = getRealValue(root, "bias", 0.01);

  //Set the default medium
  Medium* defaultmedium = new Medium();
  defaultmedium->useFresnelModel = true;
  for(unsigned int i = 0; i < GlobalSpectrum::nbWaveLengths(); i++)
    defaultmedium->n[i] = 1.0;
  m_mediaMap.add("default", defaultmedium);
  m_mediaList.push_back(defaultmedium);

  //Loading all elements of the scene
  for(unsigned int i = 0; i < root->getNumberOfChildren(); i++) {
    XMLTree* child = root->getChild(i);
    
    if(child->getMarkup() == "camera") {
      AddCamera(child);
    } else if(child->getMarkup() == "renderer") {
      AddRenderer(child);
    } else if(child->getMarkup() == "medium") {
      AddMedium(child);
 		} else if(child->getMarkup() == "texture") {
			AddTexture(child);
    } else if(child->getMarkup() == "source") {
      AddSource(child);
    } else if(child->getMarkup() == "surface") {
      AddSurface(child);
    } else if(child->getMarkup() == "include") {
      AddIncludeScenery(child);
    } else {
      throw Exception("(V2SceneryParser::parseAndBuildScenery) Balise " 
                        + child->getMarkup() 
                        +  " inconnue dans la racine dans le fichier " 
                        + sceneryFilename);
    }
  }

  return GenerateScenery();
}
////////////////////////////// class V2SceneryParser /////////////////////////////
void V2SceneryParser::AddCamera(XMLTree* node) {
  //Verify the number of child
  if(node->getNumberOfChildren() != 2) {
    throw Exception("(V2SceneryParser::addCamera) La camera " 
                      + node->getAttributeValue("name") 
                      + " n'a pas le bon nombre de fils.");
  }
  unsigned int resx = getIntegerValue(node, "resx", 100);
  unsigned int resy = getIntegerValue(node, "resy", 100);

  //Parse and build the informations of the camera
  CameraShape* geometry=0;
  ColorHandler* colorhandler=0;
  for(int i = 0; i < 2; i++) {
    XMLTree* child = node->getChild(i);
    
    // Geometry part
    if(child->getMarkup() == "geometry") {
      if(geometry != NULL) {
        throw Exception("(V2SceneryParser::addCamera) Balise <geometry> en \
double dans le noeud de la camera " + node->getAttributeValue("name"));
      }
      V2CameraShapeParser parser;
      geometry = parser.create(child, resx, resy);      
    }

    // Behaviopr  part
    else if(child->getMarkup() == "colorhandler") {
      if(colorhandler != NULL) {
        throw Exception("(V2SceneryParser::addCamera) Balise <colorhandler> en \
double dans le noeud de la camera " + node->getAttributeValue("name"));
      }      
      V2ColorHandlerParser parser;
      colorhandler = parser.create(child);      
    
    // Error case
    } else {
      throw Exception("(V2SceneryParser::addCamera) Balise <" 
                      + child->getMarkup() 
                      +  "> inconnue dans le noeud de la camera " 
                      + node->getAttributeValue("name"));
    }
  }
  
  //Build the camera
  std::string outputfile=node->getAttributeValue("name") + ".png";
  if(node->getAttributeValue("filename") != "")
    outputfile=node->getAttributeValue("filename");
  m_cameras.push_back(new Camera(geometry, 
                                 colorhandler, 
                                 node->getAttributeValue("name"), 
                                 outputfile)); 
}
////////////////////////////// class V2SceneryParser /////////////////////////////
void V2SceneryParser::AddRenderer(XMLTree* child) {
  if(p_renderer != NULL) {
    delete p_renderer;
    p_renderer = NULL;
  }

  V2RendererParser parser;
  p_renderer = parser.Create(child,  &m_textureMap);
}
////////////////////////////// class V2SceneryParser /////////////////////////////
void V2SceneryParser::AddMedium(XMLTree* node) {
  Medium* medium = new Medium();
  
  //Load attributes
  DataParser parser;
  if(node->getAttributeValue("spectrum") != "") {
    parser.loadSpectralMaterial(node->getAttributeValue("spectrum"), 
                                medium->r);
    medium->isOpaque = true;
    medium->useLambertianModel = true;
  }
  if(node->getAttributeValue("transmittance") != "") {
    parser.loadSpectralMaterial(node->getAttributeValue("transmittance"), 
                                medium->t);
    medium->isOpaque = false;
    medium->useLambertianModel = true;
    for(unsigned int i = 0; i < GlobalSpectrum::nbWaveLengths(); i++)
      if(medium->r[i] > Real(0.0))
        medium->t[i] /= Real(1.0) - medium->r[i];
  }
  if(node->getAttributeValue("indices") != "") {
    parser.loadDielectricMaterial(node->getAttributeValue("indices"), 
                                  medium->n, medium->k);
    medium->useFresnelModel = true;
    for(unsigned int i = 0; i < GlobalSpectrum::nbWaveLengths(); i++)
      //Transform kappa into k
      medium->k[i] *= medium->n[i]; 
  } 
  if(node->getAttributeValue("pigments") != "") {
    parser.loadPigmentMaterial(node->getAttributeValue("pigments"), 
                               medium->K, medium->S);
    medium->useKubelkaMunkModel = true;
  }  

  if(! medium->useLambertianModel 
        && ! medium->useFresnelModel 
        && ! medium->useKubelkaMunkModel) {
    throw Exception("(V2SceneryParser::addMedium) Le medium " 
                    + node->getAttributeValue("name") 
                    + " n'est pas correctement défini.");
  }
  //Set the medium opaque if forced by the scenery file
  if(getBooleanValue(node, "opaque", medium->isOpaque))
    medium->isOpaque = true;

  //Set the medium dispersion
  if(getBooleanValue(node, "dispersion", false))
    medium->hasDispersion = true;

  //Set the medium dispersion
  if(getBooleanValue(node, "photondispersion", false))
    medium->hasPhotonDispersion = true;

  //Add the medium into the media map and the media list
  m_mediaMap.add(node->getAttributeValue("name"), medium);
  m_mediaList.push_back(medium);
}
////////////////////////////// class V2SceneryParser /////////////////////////////
Medium* V2SceneryParser::GetMedium(std::string name) {
  if(m_mediaMap.contain(name))
    return m_mediaMap.get(name);
  
  throw Exception("(V2SceneryParser::getMedium) Le milieu " 
                  + name 
                  + " n'a pas été déclaré.");
}
////////////////////////////// class V2SceneryParser /////////////////////////////
void V2SceneryParser::AddTexture(XMLTree* node) {
  Texture* tex = new Texture();

	// texture file
	if(node->getAttributeValue("file") != "") {
    tex->SetImage(node->getAttributeValue("file").c_str());
	}

	// texture name
	tex->SetTextureName(node->getAttributeValue("name").c_str());

	//// texture alpha mode
	//tex->SetAlphaMode( static_cast<unsigned int>(getIntegerValue(node, 
  //                                                             "alpha", 0)) );

	//Add the texture into the media map and the media list
	m_textureMap.add(tex->GetTextureName(), tex);
	m_textureList.push_back(tex);
}
////////////////////////////// class V2SceneryParser /////////////////////////////
Texture* V2SceneryParser::GetTexture(std::string name) {
	if(m_textureMap.contain(name))
		return m_textureMap.get(name);
	
  throw Exception("(V2SceneryParser::getTexture) La texture " 
                  + name 
                  + " n'a pas été déclarée.");
}
////////////////////////////// class V2SceneryParser /////////////////////////////
void V2SceneryParser::AddSource(XMLTree* node) {
  //Verify the number of child
  if(node->getNumberOfChildren() != 2) {
    throw Exception("(V2SceneryParser::addSource) La source " 
                    + node->getAttributeValue("name") 
                    + " n'a pas le bon nombre de fils.");
  }

  //Parse and build the informations of the object
  ObjectShape* geometry=0;
  LightSource* source=0;
  for(int i = 0; i < 2; i++) {
    XMLTree* child = node->getChild(i);

    // Geometry part
    if(child->getMarkup() == "geometry") {
      if(geometry != NULL) {
        throw Exception("(V2SceneryParser::addSurface) Balise <geometry> en \
double dans le noeud de l'objet " + node->getAttributeValue("name"));
      }
      V2ObjectShapeParser parser;
      geometry = parser.create(child);
    
    // Behavior part
    } else if(child->getMarkup() == "source") {
      if(source != NULL) {
        throw Exception("(V2SceneryParser::addSource) Balise <source> en \
double dans le noeud de l'objet " + node->getAttributeValue("name"));
      }
      V2LightSourceParser parser;
      source = parser.create(child);      

    // Error case
    } else {
      throw Exception("(V2SceneryParser::addSource) Balise <" 
                      + child->getMarkup() 
                      +  "> inconnue dans le noeud de l'objet " 
                      + node->getAttributeValue("name"));
    }
  }

  //Build the object
  m_sources.push_back(new Source(source, geometry));
}
////////////////////////////// class V2SceneryParser /////////////////////////////
void V2SceneryParser::AddSurface(XMLTree* node)
{
  //Verify the number of child
  if(node->getNumberOfChildren() < 2) {
    throw Exception("(V2SceneryParser::addSurface) La surface " 
                    + node->getAttributeValue("name") 
                    + " n'a pas le bon nombre de fils.");
  }
  //Parse and build the geometrique information of the surface
  XMLTree* geonode = node->getChild(0);
  if (geonode->getMarkup() != "geometry") {
    throw Exception("(V2SceneryParser::addSurface) Balise <geometry> \
manquante dans le noeud de la surface " + node->getAttributeValue("name"));
  }
  V2ObjectShapeParser shapeparser;
  ObjectShape* geometry=shapeparser.create(geonode);

  XMLTree* material = NULL; 
  Medium* external = GetMedium("default");
  Medium* internal = GetMedium("default");

  //Case 1 : Juste an material declared
  if (node->getNumberOfChildren() == 2) {
    if(node->getChild(1)->getMarkup() != "material" 
        && node->getChild(1)->getMarkup() != "medium" ) {
      throw Exception("(V2SceneryParser::addSurface) Balise <material> \
manquante dans le noeud de la surface " + node->getAttributeValue("name"));
    }

    if(node->getChild(1)->getMarkup() == "material")
      material = node->getChild(1);
    else
      internal = GetMedium(node->getChild(1)->getAttributeValue("name"));

  //Case 2 : A material decalred and one medium (internal or external)
  } else if (node->getNumberOfChildren() == 3) {
    if(node->getChild(1)->getMarkup() == "material" 
        && node->getChild(2)->getMarkup() == "material") {
      throw Exception("(V2SceneryParser::addSurface) La surface " 
                      + node->getAttributeValue("name")
                      + "a deux matériaux de déclarés.");
    }
    if (node->getChild(1)->getMarkup() != "medium" 
         && node->getChild(1)->getMarkup() != "material") {
      throw Exception("(V2SceneryParser::addSurface) Balise inconnue (" 
                      + node->getChild(1)->getMarkup() 
                      + ") pour la surface " 
                      + node->getAttributeValue("name"));
    }
    if (node->getChild(2)->getMarkup() != "medium" 
         && node->getChild(2)->getMarkup() != "material") {
      throw Exception("(V2SceneryParser::addSurface) Balise inconnue (" 
                      + node->getChild(2)->getMarkup() 
                      + ") pour la surface " 
                      + node->getAttributeValue("name"));
    }
    if (node->getChild(1)->getMarkup() == "material") {
      material = node->getChild(1);
      internal = GetMedium(node->getChild(2)->getAttributeValue("name"));
    
    } else if (node->getChild(2)->getMarkup() == "material") {
      external = GetMedium(node->getChild(1)->getAttributeValue("name"));
      material = node->getChild(2);
    
    } else {
      external = GetMedium(node->getChild(1)->getAttributeValue("name"));
      internal = GetMedium(node->getChild(2)->getAttributeValue("name"));
    }

  //Case 3 : A material declared and both internal and external medium are set
  } else if (node->getNumberOfChildren() == 4) {
    if (node->getChild(2)->getMarkup() != "material") {
      throw Exception("(V2SceneryParser::addSurface) Balise <material> \
manquante dans le noeud de la surface " + node->getAttributeValue("name"));
    } 
    if (node->getChild(1)->getMarkup() != "medium" 
        || node->getChild(3)->getMarkup() != "medium") {
      throw Exception("(V2SceneryParser::addSurface) Balise incorrecte dans le \
noeud de la surface " + node->getAttributeValue("name"));
    }
    external = GetMedium(node->getChild(1)->getAttributeValue("name"));
    internal = GetMedium(node->getChild(3)->getAttributeValue("name"));
    material = node->getChild(2);
  }

  //Build the object
  VrtLog::Write("V2SceneryParser::addSurface");
  V2MaterialParser parser;
  m_objects.push_back(new Object(geometry, 
                                 parser.create(material, external, internal, 
                                               m_mediaMap, &m_textureMap), 
                                 internal, external));
}
////////////////////////////// class V2SceneryParser /////////////////////////////
void V2SceneryParser::AddIncludeScenery(XMLTree* child) {
  std::string format = child->getAttributeValue("format");
  std::string filename = child->getAttributeValue("filename");
  
  // Phanie file format
  if (format.compare("phanie") == 0) {
    PhanieParser parser;
    if (parser.LoadPhanie(filename, m_objects, m_sources) == false) {
      throw Exception("(V2SceneryParser::AddIncludeScenery) Echec dans la \
lecture du fichier de scene de format " + format + " : " + filename);
    }

  // Error case
  } else {
      throw Exception("(V2SceneryParser::AddIncludeScenery) Format de fichier "
                      + format + " inconnu (fichier : " + filename + ")");
  }
}
////////////////////////////// class V2SceneryParser /////////////////////////////
Scenery* V2SceneryParser::GenerateScenery() {
  //Compute the global bounding box
  BoundingBox globalBounds(0, 0, 0, 0, 0, 0);
  for(unsigned int i = 0; i < m_objects.size(); i++) {
    BoundingBox objectBound;
    m_objects[i]->getBoundingBox(objectBound);
    globalBounds.updateWith(objectBound);
  }

  for(unsigned int i = 0; i < m_sources.size(); i++) {
    BoundingBox sourceBound;
    m_sources[i]->getBoundingBox(sourceBound);
    globalBounds.updateWith(sourceBound);
  }
  
  //Build the scenery
  Scenery* scenery = new Scenery(m_objects.size(), 
                                 m_sources.size(), 
                                 globalBounds, m_bias);
  
  //Fill the scenery
  for(unsigned int i = 0; i < m_objects.size(); i++)
    scenery->addObject(m_objects[i]);
  for(unsigned int i = 0; i < m_sources.size(); i++)
    scenery->addSource(m_sources[i]);
  for(unsigned int i = 0; i < m_cameras.size(); i++)
    scenery->addCamera(m_cameras[i]);
  for(unsigned int i = 0; i < m_mediaList.size(); i++)
    scenery->addMedium(m_mediaList[i]);
  scenery->setRenderer(p_renderer); 
 
  //Done
  return scenery;
}
////////////////////////////////////////////////////////////////////////////////
