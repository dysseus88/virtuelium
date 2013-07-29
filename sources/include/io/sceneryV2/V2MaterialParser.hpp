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
#ifndef GUARD_VRT_V2MATERIALPARSER_HPP
#define GUARD_VRT_V2MATERIALPARSER_HPP
//!
//! @file V2MaterialParser.hpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @remarks
//! @details This parser load several Material from a node from a XMLTree of a 
//!  scenery
//!
#include <string>
#include <io/XMLTree.hpp>
#include <io/Parser.hpp>
#include <structures/Medium.hpp>
#include <structures/Texture.hpp>
#include <structures/HashMap.hpp>
#include <structures/HashFunctors.hpp>
#include <materials/Material.hpp>
////////////////////////////////////////////////////////////////////////////////
//! @class V2MaterialParser
//! @brief Parse the XML node: <material>...</material>
class V2MaterialParser : public Parser {
 public :
  //! @brief Create the material by using the informations of the node
  //! @param node XML node to use for the creation
  //! @return Parsed Material or NULL if error
  Material* create(
      XMLTree* node, 
      const Medium* external, const Medium* internal, 
			const HashMap<std::string, Medium*, StringHashFunctor> &mediaMap,
			const HashMap<std::string, Texture*, StringHashFunctor> *textureMap=NULL);

 private:
  //! @brief Return the medium
  //! @param name Name of the medium
  //! @return Medium
  Medium* getMedium(
      const HashMap<std::string, Medium*, StringHashFunctor> &mediaMap, 
      std::string name);
  //! @brief Return the texture
  //! @param name Name of the texture
  //! @return Texture
  Texture* getTexture(
      const HashMap<std::string, Texture*, StringHashFunctor> &textureMap, 
      std::string name);

 private:
  //! @brief Create a metallic material
  Material* createMetalB(
      XMLTree* node, 
      const Medium* external, const Medium* internal, 
      const HashMap<std::string, Medium*, StringHashFunctor> &mediaMap);
  //! @brief Create a metallic material
  Material* createMetalW(
      XMLTree* node, 
      const Medium* external, const Medium* internal, 
      const HashMap<std::string, Medium*, StringHashFunctor> &mediaMap);
  
  //! @brief Create a simple interface from two medium
  Material* createAlloyInterface(
      XMLTree* node, 
      const Medium* external, const Medium* internal, 
      const HashMap<std::string, Medium*, StringHashFunctor> &mediaMap);


  //! @brief Create a simple interface from two medium
  Material* createSimpleInterface(
      XMLTree* node, 
      const Medium* external, const Medium* internal, 
      const HashMap<std::string, Medium*, StringHashFunctor> &mediaMap);
  //! @brief Create a mapped interface from two media
  Material* createMappedInterface(
      XMLTree* node, 
      const Medium* external, const Medium* internal, 
      const HashMap<std::string, Medium*, StringHashFunctor> &mediaMap);
  //! @brief Create a layered interface from two media
  Material* createLayeredInterface(
      XMLTree* node, 
      const Medium* external, const Medium* internal, 
      const HashMap<std::string, Medium*, StringHashFunctor> &mediaMap);
  //! @brief Create a blended interface from two media
  Material* createBlendedInterface(
      XMLTree* node, 
      const Medium* external, const Medium* internal, 
      const HashMap<std::string, Medium*, StringHashFunctor> &mediaMap);
  //! @brief Create a depolarized interface from two media
  Material* createDepolarizedInterface(
      XMLTree* node, 
      const Medium* external, const Medium* internal, 
      const HashMap<std::string, Medium*, StringHashFunctor> &mediaMap);
  //! @brief Create a texture interface from two media
  Material* createTexturedInterface(
      XMLTree* node, 
      const Medium* external, const Medium* internal, 
			const HashMap<std::string, Medium*, StringHashFunctor> &mediaMap,
			const HashMap<std::string, Texture*, StringHashFunctor> &textureMap);
  
 private:
  //! @brief Create a thin-layered material between two media
  Material* createLayeredMaterial(
      std::vector<const Medium*>& layers, 
      std::vector<Real>& thickness, 
      Real roughness);
  //! @brief Create a diffuse interface between two media
  Material* createDiffuseLayeredMaterial(
      std::vector<const Medium*>& layers, 
      std::vector<Real>& thickness, 
      Real roughness);
  //! @brief Create a specular interface between two media
  Material* createSpecularLayeredMaterial(
      std::vector<const Medium*>& layers, 
      std::vector<Real>& thickness, 
      Real roughness);
  //! @brief Create a varnished thin-layered interface between two media
  Material* createVarnishedLayeredMaterial(
      std::vector<const Medium*>& layers, 
      std::vector<Real>& thickness, 
      Real roughness, 
      unsigned int fdl);
  //! @brief Create a Concentration map
  Material* createConcentrationInterface(
      XMLTree* node, 
	    const Medium* external, const Medium* internal, 
			const HashMap<std::string, Medium*, StringHashFunctor> &mediaMap,
      const HashMap<std::string, Texture*, StringHashFunctor> &textureMap);

 private :
  //! sammple of angles (incident, view) between [0°, 90°]
  static const unsigned int SAMPLES = 90;
  //! Material library (material_name, material_data). 
  //! Used to avoid to recompute multiple time the same material
  static HashMap<std::string, Material*, StringHashFunctor> _materialLibrary;
}; // V2MaterialParser
////////////////////////////////////////////////////////////////////////////////
#endif //GUARD_VRT_V2MATERIALPARSER_HPP
