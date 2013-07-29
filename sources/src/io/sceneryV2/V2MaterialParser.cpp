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
#include <io/sceneryV2/V2MaterialParser.hpp>
//!
//! @file V2MaterialParser.cpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file implements the classes declared in V2MaterialParser.hpp 
//!  @arg V2MaterialParser
//! @todo 
//! @remarks 
//!
#include <stdio.h>
#include <string>

#include <core/3DBase.hpp>
#include <exceptions/Exception.hpp>

#include <io/XMLParser.hpp>
#include <io/DataParser.hpp>
#include <io/image/ImageParser.hpp>

#include <physics/DielectricFormula.hpp>
#include <physics/ThinLayerFormula.hpp>
#include <physics/KubelkaMunkFormula.hpp>
#include <physics/LayeredSystemComputer.hpp>

#include <materials/NullBRDF.hpp>
#include <materials/LambertianBRDF.hpp>
#include <materials/RoughLambertianBRDF.hpp>
#include <materials/RegularBRDF.hpp>
#include <materials/BeckmannBRDF.hpp>
#include <materials/RefractiveBRDF.hpp>
#include "materials/BeckmannRefractiveBRDF.hpp"
#include <materials/VarnishedLambertianBRDF.hpp>
#include <materials/RoughVarnishedLambertianBRDF.hpp>
#include <materials/TwoSidedBRDF.hpp>
#include <materials/DepolarizedBRDF.hpp>
#include <materials/BlendedBRDF.hpp>
#include <materials/MappedBRDF.hpp>
#include <materials/InstanceBRDF.hpp>
#include <materials/TextureBRDF.hpp>
#include <materials/ConcentrationMap.hpp>

#include <materials/AlloyBRDF.hpp>
#include <materials/MetalB.hpp>
#include <materials/MetalW.hpp>
#include <core/VrtLog.hpp>

//////////////////////// class V2MaterialParser ////////////////////////////////
HashMap<std::string, Material*, StringHashFunctor> 
    V2MaterialParser::_materialLibrary(StringHashFunctor(), 100);
//////////////////////// class V2MaterialParser ////////////////////////////////
Material* V2MaterialParser::create(
    XMLTree* node, 
    const Medium* external, const Medium* internal, 
    const HashMap<std::string, Medium*, StringHashFunctor> &mediaMap,
  	const HashMap<std::string, Texture*, StringHashFunctor> *textureMap) {
 
  VrtLog::Write("V2MaterialParser::create");
 
  //Degenerated cases
  if(external->isOpaque && internal->isOpaque) {
    return new NullBRDF();
  }
  if(node == NULL) {
    return createLayeredInterface(NULL, external, internal, mediaMap);
  }

  //Try to get the material from the library
  std::string name = node->getAttributeValue("name");
  if(name != "" && _materialLibrary.contain(name)) {
    return new InstanceBRDF(_materialLibrary.get(name));
  }

  //Create the material
  Material* material;
  std::string type = node->getAttributeValue("type");

  if(type == "Layered") {
    material = createLayeredInterface(node, external, internal, mediaMap);
  
  } else if(type == "Alloy") {
    material = createAlloyInterface(node, external, internal, mediaMap);

  } else if(type == "MetalB") {
    material = createMetalB(node, external, internal, mediaMap);
  } else if(type == "MetalW") {
    material = createMetalW(node, external, internal, mediaMap);

  } else if(type == "Mapped") {
    material = createMappedInterface(node, external, internal, mediaMap);
  } else if(type=="Concentration") {
    if (textureMap != NULL) {
      material = createConcentrationInterface(node, external, internal, 
                                              mediaMap, *textureMap);
    } else {
     throw Exception("(V2MaterialParser::create) Texture inconnue.");
    }
  } else if(type == "Blended") {
    material = createBlendedInterface(node, external, internal, mediaMap);
  } else if(type == "Depolarized") {
    material = createDepolarizedInterface(node, external, internal, mediaMap);
  } else if(type == "Textured") {
    if (textureMap != NULL) {
      material = createTexturedInterface(node, external, internal, 
                                         mediaMap, *textureMap);
    } else {
     throw Exception("(V2MaterialParser::create) Texture inconnue.");
    }
  } else {
    throw Exception("(V2MaterialParser::create) Type de <Material> " 
                    + type + " inconnu.");
  }
  //Add the material to the library
  if(name != "") {
    _materialLibrary.add(name, material);
  }
  //Return the material
  return material;
}
//////////////////////// class V2MaterialParser ////////////////////////////////
Medium* V2MaterialParser::getMedium(
    const HashMap<std::string, Medium*, StringHashFunctor> &mediaMap, 
    std::string name) {
  if(mediaMap.contain(name)) {
    return mediaMap.get(name);
  }
  throw Exception("(V2MaterialParser::getMedium) Le milieu " 
                  + name + " n'a pas été déclaré.");
}
//////////////////////// class V2MaterialParser ////////////////////////////////

Texture* V2MaterialParser::getTexture(
    const HashMap<std::string, Texture*, StringHashFunctor> &textureMap,
    std::string name) {
  if(textureMap.contain(name)) {
    return textureMap.get(name);
  }
  throw Exception("(V2MaterialParser::getMedium) La texture" 
                  + name + " n'a pas été déclarée.");
}
//////////////////////// class V2MaterialParser ////////////////////////////////

Material* V2MaterialParser::createAlloyInterface(
    XMLTree* node, 
    const Medium* external, const Medium* internal, 
    const HashMap<std::string, Medium*, StringHashFunctor> &mediaMap) {
     
  //VrtLog::Write("V2MaterialParser::createAlloyInterface");

  // Be sure the node is not be null
  if (node == NULL)
    return new NullBRDF();

  // Get the roughness
  Real roughness = 0.0;
  roughness = getRealValue(node, "roughness", 0.0);

  //VrtLog::Write("  V2MaterialParser::createAlloyInterface - roughness = %f", roughness);
  
 // // Get the coeffcient map
	//Texture *tex = NULL;
	//Real tileU=1.0, tileV=1.0;
 // 
 // std::string name = node->getAttributeValue("distribution");

 // tex = getTexture( textureMap, name );
	//tileU = getRealValue( node, "tileU", 1.0);
	//tileV = getRealValue( node, "tileV", 1.0);

	//std::string modeU = node->getAttributeValue("modeU");
	//Texture::TEXTURE_REPEAT_MODE repeatModeU = Texture::REPEAT_OFF;
	//if ( modeU == "repeat" )
	//	repeatModeU = Texture::REPEAT_ON;
	//else if ( modeU == "mirror" )
	//	repeatModeU = Texture::REPEAT_MIRROR;

	//std::string modeV = node->getAttributeValue("modeV");
	//Texture::TEXTURE_REPEAT_MODE repeatModeV = Texture::REPEAT_OFF;
	//if ( modeV == "repeat" )
	//	repeatModeV = Texture::REPEAT_ON;
	//else if ( modeV == "mirror" )
	//	repeatModeV = Texture::REPEAT_MIRROR;

  // Test the number of children : only two metals are allowed for now
  if (node->getNumberOfChildren() != 2)
    throw Exception("(V2MaterialParser::createAlloyInterface): Only two metals are allowed in the alloy for now.");
  
  // Prepare medium array
  std::vector<const Medium*> media_stack(3);
  media_stack[0] = external;
  
  // Get the first child
  media_stack[1] = getMedium(mediaMap, 
                             node->getChild(0)->getAttributeValue("name"));
  Real coef_1 =  getRealValue(node->getChild(0), "coef", 0.0);
  
  //VrtLog::Write("  V2MaterialParser::createAlloyInterface - coef_1 = %f", coef_1);

  // Get the second child
  media_stack[2] = getMedium(mediaMap, 
                             node->getChild(1)->getAttributeValue("name"));
  Real coef_2 =  getRealValue(node->getChild(1), "coef", 0.0);

  //VrtLog::Write("  V2MaterialParser::createAlloyInterface - coef_2 = %f", coef_2);

  // Test if coefs are coherent (if no distrubtion map)
  if (coef_1 < 0 || coef_2 < 0 || Real(1.0) - coef_1 - coef_2 > kEPSILON) {
    VrtLog::Write("Coef_1=%f, Coef_2=%f, diff=%f", coef_1, coef_2, Real(1.0) - coef_1 - coef_2);
    throw Exception("(V2MaterialParser::createAlloyInterface): The sum of the alloy coefficients must be equal to 1.");
  }
  //// Test if child1 and child2 are metals
  //if (IsMetal(mtl_1) == false || IsMetal(mtl_2) == false)
  //  throw Exception("(V2MaterialParser::createAlloyInterface): One or more components of the alloy is not a metal.");

  // Create the alloy BRDF
  return new AlloyBRDF(media_stack, coef_1, roughness);
  //return new AlloyBRDF(media_stack, coef_1, roughness,
  //                     tex, tileU, tileV, modeU, modeV);
}
//////////////////////// class V2MaterialParser ////////////////////////////////

Material* V2MaterialParser::createMetalB(
    XMLTree* node, 
    const Medium* external, const Medium* internal, 
    const HashMap<std::string, Medium*, StringHashFunctor> &mediaMap) {
     
  VrtLog::Write("V2MaterialParser::createMetal");

  // Be sure the node is not be null
  if (node == NULL)
    return new NullBRDF();

  // Get the roughness
  Real mij;
  mij = getRealValue(node, "roughness", 0.0);
  
  VrtLog::Write("  V2MaterialParser::createMetalB - m = (%f, %f)", mij, mij);

  // Test the number of children : only two metals are allowed for now
  if (node->getNumberOfChildren() != 1)
    throw Exception("(V2MaterialParser::createMetalB): No metals are defined.");
  
  // Prepare medium array
  std::vector<const Medium*> media_stack(3);
  media_stack[0] = external;
  
  // Get the first child
  media_stack[1] = getMedium(mediaMap, 
                             node->getChild(0)->getAttributeValue("name"));  

  // Create the alloy BRDF
  return new MetalB(media_stack, mij, mij);
}
//////////////////////// class V2MaterialParser ////////////////////////////////

Material* V2MaterialParser::createMetalW(
    XMLTree* node, 
    const Medium* external, const Medium* internal, 
    const HashMap<std::string, Medium*, StringHashFunctor> &mediaMap) {
     
  VrtLog::Write("V2MaterialParser::createMetalW");

  // Be sure the node is not be null
  if (node == NULL)
    return new NullBRDF();

  // Get the roughness
  Real mi, mj;
  mi = getRealValue(node, "tangent", 0.0);
  mj = getRealValue(node, "bitangent", 0.0);
  
  VrtLog::Write("  V2MaterialParser::createMetalW - m = (%f, %f)", mi, mj);

  // Test the number of children : only two metals are allowed for now
  if (node->getNumberOfChildren() != 1)
    throw Exception("(V2MaterialParser::createMetalW): No metals are defined.");
  
  // Prepare medium array
  std::vector<const Medium*> media_stack(3);
  media_stack[0] = external;
  
  // Get the first child
  media_stack[1] = getMedium(mediaMap, 
                             node->getChild(0)->getAttributeValue("name"));  

  // Create the alloy BRDF
  return new MetalW(media_stack, mi, mj);
}
//////////////////////// class V2MaterialParser ////////////////////////////////

Material* V2MaterialParser::createLayeredInterface(
    XMLTree* node, 
    const Medium* external, const Medium* internal, 
    const HashMap<std::string, Medium*, StringHashFunctor> &mediaMap) {
 
  //We have a two sided material to build
  Material* outermat;
  Material* innermat;

  //Create the layer stacks
  unsigned int nblayers=0;
  if(node != NULL) {
    nblayers=node->getNumberOfChildren();
  }
  std::vector<const Medium*> layers(nblayers+2);
  std::vector<Real> thickness(nblayers+2);

  //Get the roughness
  Real roughness = 0.0;
  if(node!=NULL) {
    roughness = getRealValue(node, "roughness", 0.0);
  }
  //External part of the material
  if(external->isOpaque) {
    outermat= new NullBRDF();
  } else {
    //Building the layer stack
    layers[0] = external;
    thickness[0] = 0.0;
    layers[nblayers+1] = internal;
    thickness[nblayers+1] = 0.0;
    for(unsigned int i=0; i<nblayers; i++)  {
      layers[i+1] = getMedium(mediaMap, 
                              node->getChild(i)->getAttributeValue("medium"));
      thickness[i+1] = getRealValue(node->getChild(i), "thickness", 0.0);
    }

    outermat = createLayeredMaterial(layers, thickness, roughness);
  }

  //Internal part of the material
  if(internal->isOpaque)
    innermat= new NullBRDF();
  else
  {
    //Building the layer stack
    layers[0] = internal;
    thickness[0] = 0.0;
    layers[nblayers+1] = external;
    thickness[nblayers+1] = 0.0;
    for(unsigned int i=0; i<nblayers; i++)
    {
      layers[nblayers-i] = getMedium(mediaMap, node->getChild(i)->getAttributeValue("medium"));
      thickness[nblayers-i] = getRealValue(node->getChild(i), "thickness", 0.0);
    }

    innermat = createLayeredMaterial(layers, thickness, roughness);
  }
  
  //Create the material
  return new TwoSidedBRDF(outermat, innermat);
}
//////////////////////// class V2MaterialParser ////////////////////////////////

Material* V2MaterialParser::createBlendedInterface(
    XMLTree* node, 
    const Medium* external, const Medium* internal, 
    const HashMap<std::string, Medium*, StringHashFunctor> &mediaMap) {
  
  //Build the material list
  std::vector<Material*> materials(node->getNumberOfChildren());
  std::vector<Real> factors(node->getNumberOfChildren());
  for(unsigned int i=0; i<node->getNumberOfChildren(); i++) {
    materials[i] = create(node->getChild(i), external, internal, mediaMap);
    factors[i] = getRealValue(node->getChild(i), "factor", 1.0);
  }

  //Build the blended material
  return new BlendedBRDF(materials, factors);
}
//////////////////////// class V2MaterialParser ////////////////////////////////

Material* V2MaterialParser::createMappedInterface(
    XMLTree* node, 
    const Medium* external, const Medium* internal, 
    const HashMap<std::string, Medium*, StringHashFunctor> &mediaMap) {

  unsigned int nbMaterials = node->getNumberOfChildren();
  std::vector<Image*> maps(nbMaterials);
  std::vector<Material*> materials(nbMaterials);

  ImageParser parser;
  for(unsigned int i=0; i< nbMaterials; i++)  {
    maps[i] = parser.load(node->getChild(i)->getAttributeValue("map"));
    materials[i] = create(node->getChild(i), external, internal, mediaMap);
  }

  return new MappedBRDF(materials, maps);
}
//////////////////////// class V2MaterialParser ////////////////////////////////

Material* V2MaterialParser::createDepolarizedInterface(
    XMLTree* node, 
    const Medium* external, const Medium* internal, 
    const HashMap<std::string, Medium*, StringHashFunctor> &mediaMap) {
  if(node->getNumberOfChildren() == 0) {
    return new DepolarizedBRDF(create(NULL, external, internal, mediaMap));
  } else {
    return new DepolarizedBRDF(create(node->getChild(0), external, internal, 
                                      mediaMap));
  }
}
//////////////////////// class V2MaterialParser ////////////////////////////////

Material* V2MaterialParser::createTexturedInterface(
    XMLTree* node, 
    const Medium* external, const Medium* internal, 
		const HashMap<std::string, Medium*, StringHashFunctor> &mediaMap,
		const HashMap<std::string, Texture*, StringHashFunctor> &textureMap) {
	// get texture parameters
	std::string name = node->getAttributeValue("texture");
	Texture *tex = 0;
	tex = getTexture( textureMap, name );
	
	Real amount=1.0, tileU=1.0, tileV=1.0;
	amount = getRealValue( node, "amount", 1.0);
	tileU = getRealValue( node, "tileU", 1.0);
	tileV = getRealValue( node, "tileV", 1.0);

	std::string modeU = node->getAttributeValue("modeU");
	Texture::TEXTURE_REPEAT_MODE repeatModeU = Texture::REPEAT_OFF;
	if ( modeU == "repeat" )
		repeatModeU = Texture::REPEAT_ON;
	else if ( modeU == "mirror" )
		repeatModeU = Texture::REPEAT_MIRROR;

	std::string modeV = node->getAttributeValue("modeV");
	Texture::TEXTURE_REPEAT_MODE repeatModeV = Texture::REPEAT_OFF;
	if ( modeV == "repeat" )
		repeatModeV = Texture::REPEAT_ON;
	else if ( modeV == "mirror" )
		repeatModeV = Texture::REPEAT_MIRROR;
  
  std::string alpha = node->getAttributeValue("alpha");
	Texture::ALPHA_MODE alphaMode = Texture::ALPHA_OFF;
	if ( alpha == "on" )
		alphaMode = Texture::ALPHA_ON;

  if(node->getNumberOfChildren() == 0) {
	  return new TextureBRDF(createLayeredInterface(NULL, external, internal, 
                                                  mediaMap), 
                           tex, amount, tileU, tileV, 
                           repeatModeU, repeatModeV, alphaMode);
  } else {
	  return new TextureBRDF(create(node->getChild(0), external, internal, 
                                  mediaMap), 
                           tex, amount, tileU, tileV, 
                           repeatModeU, repeatModeV, alphaMode);
  }
}
//////////////////////// class V2MaterialParser ////////////////////////////////

Material* V2MaterialParser::createLayeredMaterial(
    std::vector<const Medium*>& layers, 
    std::vector<Real>& thickness, 
    Real roughness) {

  //Search for the first diffuse layer (fdl)
  unsigned int fdl = 1;
  while(fdl < layers.size() 
          && !layers[fdl]->useLambertianModel 
          && !layers[fdl]->useKubelkaMunkModel)
    fdl++;

  //Is the material specular
  bool specular = false;
  if(layers[1]->useFresnelModel)
    specular = true;

  //Is the material diffuse
  bool diffuse = false;
  if(fdl<layers.size())
    diffuse = true;

  //Diffuse only
  if(layers[0]->useLambertianModel 
        || layers[0]->useKubelkaMunkModel 
        || (diffuse && !specular))
    return createDiffuseLayeredMaterial(layers, thickness, roughness);

  //Specular only
  if(specular && !diffuse)
    return createSpecularLayeredMaterial(layers, thickness, roughness);

  //Varnished diffuse material
  return createVarnishedLayeredMaterial(layers, thickness, roughness, fdl);
}
//////////////////////// class V2MaterialParser ////////////////////////////////

Material* V2MaterialParser::createDiffuseLayeredMaterial(
    std::vector<const Medium*>& layers, 
    std::vector<Real>& thickness, Real roughness) {

  Spectrum R, T;
  LayeredSystemComputer::computeDiffuseLayerSystemReflectance(R, T, 
                                                              layers, 
                                                              thickness);

  for(unsigned int i=0; i<GlobalSpectrum::nbWaveLengths(); i++) {
    if(!(R[i] < 1.0001)) {
      printf("\n -- ATTENTION --  ");
      printf("(V2MaterialParser::createDiffuseLayeredMaterial) ");
      printf("La réflectance de la bande %d vaut (%f).\n", i, R[i]);
    }
    if(!(T[i] < 1.0001)) {
      printf("\n -- ATTENTION --  ");
      printf("(V2MaterialParser::createDiffuseLayeredMaterial) ");
      printf("La transmittance de la bande %d vaut (%f).\n", i, T[i]);
    }
  }

  if(roughness > 0.0001) {
    return new RoughLambertianBRDF(R, roughness);  
  } else {
    return new LambertianBRDF(
                  R, T, 
                  LayeredSystemComputer::isLayerSystemOpaque(layers));
  }
}
//////////////////////// class V2MaterialParser ////////////////////////////////

Material* V2MaterialParser::createSpecularLayeredMaterial(
    std::vector<const Medium*>& layers, 
    std::vector<Real>& thickness, Real roughness) {

  //Compute reflectance and transmittance
  std::vector<Spectrum> Rp(SAMPLES), Rs(SAMPLES), Tp(SAMPLES), Ts(SAMPLES);
  for(unsigned int iOi = 0; iOi < SAMPLES; iOi++) {
    Real Oi = iOi*M_PI*0.5/(Real)SAMPLES;
    Real cosOi = std::cos(Oi);
    LayeredSystemComputer::computeSpecularLayerSystem(Rs[iOi], Rp[iOi], Ts[iOi], 
                                                      Tp[iOi], cosOi, 
                                                      layers, thickness);

    for(unsigned int i=0; i<GlobalSpectrum::nbWaveLengths(); i++) { 
      if(!(Rp[iOi][i] < 1.0001)) {
        printf("\n -- ATTENTION --  ");
        printf("(V2MaterialParser::createSpecularLayeredMaterial) ");
        printf("La réflectance R de la bande %d vaut (%f) ", i, Rp[iOi][i]);
        printf("pour l'angle d'incidence %f°\n", (Oi*180.0/M_PI));
      }
      if(!(Rs[iOi][i] < 1.0001)) {
        printf("\n -- ATTENTION --  ");
        printf("(V2MaterialParser::createSpecularLayeredMaterial) ");
        printf("La réflectance S de la bande %d vaut (%f) ", i, Rs[iOi][i]);
        printf("pour l'angle d'incidence %f°\n", (Oi*180.0/M_PI));
      }
      if(!(Tp[iOi][i] < 1.0001)) {
        printf("\n -- ATTENTION --  ");
        printf("(V2MaterialParser::createSpecularLayeredMaterial) ");
        printf("La transmittance R de la bande %d vaut (%f) ", i, Tp[iOi][i]);
        printf("pour l'angle d'incidence %f°\n", (Oi*180.0/M_PI));
      }
      if(!(Ts[iOi][i] < 1.0001)) {
        printf("\n -- ATTENTION --  ");
        printf("(V2MaterialParser::createSpecularLayeredMaterial) ");
        printf("La transmittance S de la bande %d vaut (%f) ", i, Ts[iOi][i]);
        printf("pour l'angle d'incidence %f°\n", (Oi*180.0/M_PI));
      }
    }
  }

  //If the substrate is opaque
  if(layers[layers.size()-1]->isOpaque) {
    if(roughness <= 0.000001) {
      return new RegularBRDF(Rp, Rs);
    } else {
      return new BeckmannBRDF(Rp, Rs, roughness);
    }
  }

  //If the substrate is non-opaque
  Spectrum n;
  for(unsigned int i = 0; i < GlobalSpectrum::nbWaveLengths(); i++) {
    n[i]=layers[layers.size()-1]->n[i]/layers[0]->n[i];
  }
  return new RefractiveBRDF(Rp, Rs, Tp, Ts, n, 
                            layers[layers.size()-1]->hasDispersion, 
                            layers[layers.size()-1]->hasPhotonDispersion);
}
//////////////////////// class V2MaterialParser ////////////////////////////////

Material* V2MaterialParser::createVarnishedLayeredMaterial(
    std::vector<const Medium*>& layers, 
    std::vector<Real>& thickness, Real roughness, unsigned int fdl) {

  unsigned int lsl = fdl; //Last Specular Layer
  if(!layers[fdl]->useFresnelModel)
    lsl = fdl-1;

  //Build layers system for the substrate and the varnish
  std::vector<const Medium*> var_layers(lsl+1);
  std::vector<Real> var_thickness(lsl+1);
  std::vector<const Medium*> ivar_layers(lsl+1);
  std::vector<Real> ivar_thickness(lsl+1);
  std::vector<const Medium*> sub_layers(layers.size()-fdl+1);
  std::vector<Real> sub_thickness(layers.size()-fdl+1);
  for(unsigned int i = 0; i < layers.size(); i++) {
    if(i <= lsl) {
      var_layers[i]=layers[i];
      var_thickness[i]=thickness[i];
      ivar_layers[i]=layers[lsl-i];
      ivar_thickness[i]=thickness[lsl-i];
    }
    if(i >= fdl-1) {
      sub_layers[i-fdl+1]=layers[i];
      sub_thickness[i-fdl+1]=thickness[i];
    }
  }

  //Compute specular reflectance and transmittance
  std::vector<Spectrum> R12p(SAMPLES), R12s(SAMPLES);
  std::vector<Spectrum> T12p(SAMPLES), T12s(SAMPLES);
  std::vector<Spectrum> R21p(SAMPLES), R21s(SAMPLES);
  std::vector<Spectrum> T21p(SAMPLES), T21s(SAMPLES);
  std::vector<Spectrum> R13p(SAMPLES), R13s(SAMPLES);
  std::vector<Spectrum> T13p(SAMPLES), T13s(SAMPLES);
  
  for(unsigned int iOi = 0; iOi < SAMPLES; iOi++) {
    Real Oi = iOi*M_PI*0.5/(Real)SAMPLES;
    Real cosOi = std::cos(Oi);
    LayeredSystemComputer::computeSpecularLayerSystem(
                            R12s[iOi], R12p[iOi], T12s[iOi], T12p[iOi], 
                            cosOi, var_layers, var_thickness);
    LayeredSystemComputer::computeSpecularLayerSystem(
                            R21s[iOi], R21p[iOi], T21s[iOi], T21p[iOi], 
                            cosOi, ivar_layers, ivar_thickness);
    LayeredSystemComputer::computeSpecularLayerSystem(
                            R13s[iOi], R13p[iOi], T13s[iOi], T13p[iOi], 
                            cosOi, layers, thickness);

    for(unsigned int i = 0; i < GlobalSpectrum::nbWaveLengths(); i++) {
      if(!(R12p[iOi][i] < 1.0001)) {
        printf("\n -- ATTENTION --  ");
        printf("(V2MaterialParser::createVarnishedLayeredMaterial) ");
        printf("La réflectance 1-2 P de la bande %d vaut (%f) ", 
               i, R12p[iOi][i]);
        printf("pour l'angle d'incidence %f°\n", (Oi*180.0/M_PI));
      }
      if(!(R12s[iOi][i] < 1.0001)) {
        printf("\n -- ATTENTION --  ");
        printf("(V2MaterialParser::createVarnishedLayeredMaterial) ");
        printf("La réflectance 1-2 S de la bande %d vaut (%f) ", 
               i, R12s[iOi][i]);
        printf("pour l'angle d'incidence %f°\n", (Oi*180.0/M_PI));
      }
      if(!(T12p[iOi][i] < 1.0001)) {
        printf("\n -- ATTENTION --  ");
        printf("(V2MaterialParser::createVarnishedLayeredMaterial) ");
        printf("La transmittance 1-2 P de la bande %d vaut (%f) ", 
               i, T12p[iOi][i]);
        printf("pour l'angle d'incidence %f°\n", (Oi*180.0/M_PI));
      }
      if(!(T12s[iOi][i] < 1.0001)) {
        printf("\n -- ATTENTION --  ");
        printf("(V2MaterialParser::createVarnishedLayeredMaterial) ");
        printf("La transmittance 1-2 S de la bande %d vaut (%f) ", 
               i, T12s[iOi][i]);
        printf("pour l'angle d'incidence %f°\n", (Oi*180.0/M_PI));
      }

      if(!(R21p[iOi][i] < 1.0001)) {
        printf("\n -- ATTENTION --  ");
        printf("(V2MaterialParser::createVarnishedLayeredMaterial) ");
        printf("La réflectance 2-1 P de la bande %d vaut (%f) ", 
               i, R21p[iOi][i]);
        printf("pour l'angle d'incidence %f°\n", (Oi*180.0/M_PI));
      }
      if(!(R21s[iOi][i] < 1.0001)) {
        printf("\n -- ATTENTION --  ");
        printf("(V2MaterialParser::createVarnishedLayeredMaterial) ");
        printf("La réflectance 2-1 S de la bande %d vaut (%f) ", 
               i, R21s[iOi][i]);
        printf("pour l'angle d'incidence %f°\n", (Oi*180.0/M_PI));
      }
      if(!(T21p[iOi][i] < 1.0001)) {
        printf("\n -- ATTENTION --  ");
        printf("(V2MaterialParser::createVarnishedLayeredMaterial) ");
        printf("La transmittance 2-1 P de la bande %d vaut (%f) ", 
               i, T21p[iOi][i]);
        printf("pour l'angle d'incidence %f°\n", (Oi*180.0/M_PI));
      }
      if(!(T21s[iOi][i] < 1.0001)) {
        printf("\n -- ATTENTION --  ");
        printf("(V2MaterialParser::createVarnishedLayeredMaterial) ");
        printf("La transmittance 2-1 S de la bande %d vaut (%f) ", 
               i, T21s[iOi][i]);
        printf("pour l'angle d'incidence %f°\n", (Oi*180.0/M_PI));
      }

      if(!(R13p[iOi][i] < 1.0001)) {
        printf("\n -- ATTENTION --  ");
        printf("(V2MaterialParser::createVarnishedLayeredMaterial) ");
        printf("La réflectance 1-3 P de la bande %d vaut (%f) ", 
               i, R13p[iOi][i]);
        printf("pour l'angle d'incidence %f°\n", (Oi*180.0/M_PI));
      }
      if(!(R13s[iOi][i] < 1.0001)) {
        printf("\n -- ATTENTION --  ");
        printf("(V2MaterialParser::createVarnishedLayeredMaterial) ");
        printf("La réflectance 1-3 S de la bande %d vaut (%f) ", 
               i, R13s[iOi][i]);
        printf("pour l'angle d'incidence %f°\n", (Oi*180.0/M_PI));
      }
      if(!(T13p[iOi][i] < 1.0001)) {
        printf("\n -- ATTENTION --  ");
        printf("(V2MaterialParser::createVarnishedLayeredMaterial) ");
        printf("La transmittance 1-3 P de la bande %d vaut (%f) ", 
               i, T13p[iOi][i]);
        printf("pour l'angle d'incidence %f°\n", (Oi*180.0/M_PI));
      }
      if(!(T13s[iOi][i] < 1.0001)) {
        printf("\n -- ATTENTION --  ");
        printf("(V2MaterialParser::createVarnishedLayeredMaterial) ");
        printf("La transmittance 1-3 S de la bande %d vaut (%f) ", 
               i, T13s[iOi][i]);
        printf("pour l'angle d'incidence %f°\n", (Oi*180.0/M_PI));
      }
    }
  }

  //Compute diffuse reflectance
  Spectrum Rsub, TSub;
  LayeredSystemComputer::computeDiffuseLayerSystemReflectance(Rsub, TSub, 
                                                              sub_layers, 
                                                              sub_thickness);

  // Get the index of refraction (and the thickness) of the last layer of 
  // the varnish
  Spectrum n2, k2;
  Real t2;
  if(layers[fdl]->useFresnelModel) {
    t2 = 0;
    n2 = layers[fdl]->n;
    k2 = layers[fdl]->k;
  } else {
    t2 = thickness[fdl-1];
    n2 = layers[fdl-1]->n;
    k2 = layers[fdl-1]->k;
  }

  //If the substrate is opaque
  if(layers[layers.size()-1]->isOpaque) {
    if(roughness <= 0.000001) {
      return new VarnishedLambertianBRDF(R13p, R13s, T12p, T12s, 
                                         R21p, R21s, n2, k2, t2, Rsub);
    } else {
      return new RoughVarnishedLambertianBRDF(R13p, R13s, T12p, T12s, 
                                              R21p, R21s, n2, k2, t2, 
                                              roughness, Rsub);
    }
  } else {
    return new VarnishedLambertianBRDF(R13p, R13s, T12p, T12s, 
                                       R21p, R21s, n2, k2, t2, Rsub, TSub);
  }
  //Build the material
}
//////////////////////// class V2MaterialParser ////////////////////////////////

Material* V2MaterialParser::createConcentrationInterface(
    XMLTree* node, 
    const Medium* external, 
    const Medium* internal, 
	  const HashMap<std::string, Medium*, StringHashFunctor> &mediaMap, 
	  const HashMap<std::string, Texture*, StringHashFunctor> &textureMap) {
  
  unsigned int nbMaterials = node->getNumberOfChildren();
	Image* map;
  std::vector<Material*> materials(nbMaterials);

  Real tileU=1.0, tileV=1.0;
  tileU = getRealValue( node, "tileU", 1.0);
  tileV = getRealValue( node, "tileV", 1.0);

  std::string modeU = node->getAttributeValue("modeU");
  std::string modeV = node->getAttributeValue("modeV");
  Texture::TEXTURE_REPEAT_MODE repeatModeU = Texture::REPEAT_OFF;
  Texture::TEXTURE_REPEAT_MODE repeatModeV = Texture::REPEAT_OFF;

  /// repeatMode sur U
  if ( modeU == "repeat" )
    repeatModeU = Texture::REPEAT_ON;
  else if ( modeU == "mirror" )
    repeatModeU = Texture::REPEAT_MIRROR;

  /// repeatMode sur V
  if ( modeV == "repeat" )
    repeatModeV = Texture::REPEAT_ON;
  else if ( modeV == "mirror" )
    repeatModeV = Texture::REPEAT_MIRROR;

  ImageParser parser;
  map = parser.load(node->getAttributeValue("map"));

  if (nbMaterials - map->getNumberOfChannels() >= 2) {
    throw Exception("(V2MaterialParser::createConcentrationInterface) Too many \
                    materials require more channels in the concentration map.");
  }

  for(unsigned int i=0; i< nbMaterials; i++) {
    materials[i] = create(node->getChild(i), external, internal, 
                          mediaMap, &textureMap);
  }

  return new ConcentrationMap(materials, map, tileU, tileV, 
                              repeatModeU, repeatModeV);
}
////////////////////////////////////////////////////////////////////////////////

