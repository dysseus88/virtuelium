/*
 *  Copyright 2013 Remi "programmix" Cerise
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
#ifndef GUARD_VRT_CONCENTRATIONMAP_HPP
#define GUARD_VRT_CONCENTRATIONMAP_HPP
//!
//! @file ConcentrationMap.hpp
//! @author Bin Yu, Stephane Shao
//! @version 5.0.0
//! @date 2013
//! @remarks
//! @details Defines a concentration map between n materials
//!
#include <vector>
#include <core/3DBase.hpp>
#include <core/LightBase.hpp>

#include <materials/Material.hpp>
#include <structures/Image.hpp>
#include <structures/Texture.hpp>
////////////////////////////////////////////////////////////////////////////////
//! @class ConcentrationMap
//! @brief Defines a concentration map between n materials
//! @details Syntax:\n
//!  <material name="..." type="Concentration" map="...">\n
//!  \t<mtl 1 />\n
//!  \t...\n
//!  </material>\n
class ConcentrationMap : public Material {
 public:
	//! @brief Constructor #1
  ConcentrationMap(std::vector<Material*> materials, Image* maps);  
	//! @brief Constructor #2
	ConcentrationMap(
      std::vector<Material*> materials, Image* maps, 
		  const Real& tileU, const Real& tileV,
			Texture::TEXTURE_REPEAT_MODE texRepeatModeU = Texture::REPEAT_OFF,
			Texture::TEXTURE_REPEAT_MODE texRepeatModeV = Texture::REPEAT_OFF);
	//! @brief Destructor
  virtual ~ConcentrationMap(void);
	
 public:
  //! @brief Compute the specularly reemited light data 
  virtual void getSpecularReemited(const Basis& localBasis, 
                                   const Point2D& surfaceCoordinate, 
                                   const LightVector& incidentLight, 
                                   LightVector& reemitedLight);
  //! @brief Compute the secondary rays for specular reflexion 
  virtual void getSpecularSubRays(const Basis& localBasis, 
                                  const Point2D& surfaceCoordinate, 
                                  LightVector& reemitedLight, 
                                  std::vector<LightVector>& subrays);
  //! @brief Compute the diffuse part of a reflexion
  virtual void getDiffuseReemited(const Basis& localBasis, 
                                  const Point2D& surfaceCoordinate, 
                                  const LightVector& incidentLight, 
                                  LightVector& reemitedLight);
  //! @brief Compute secondary ray for sampling the diffuse reflexion
  virtual void getRandomDiffuseRay(const Basis& localBasis, 
                                   const Point2D& surfaceCoordinate, 
                                   LightVector& reemitedLight, 
                                   unsigned int nbRays, 
                                   std::vector<LightVector>& subrays);
  //! @brief compute a random reflexion for the given photon
  //! @details This method uses the russian roulette algorithm for handeling 
  //!  the absorption of the photon
  virtual bool bouncePhoton(const Basis& localBasis, 
                            const Point2D& surfaceCoordinate, 
                            MultispectralPhoton& photon, bool& specular);

  //! @brief Compute the remited light from an istrotropic ambiant illumination
  virtual void getDiffuseReemitedFromAmbiant(const Basis& localBasis, 
                                             const Point2D& surfaceCoordinate, 
                                             LightVector& reemitedLight, 
                                             const Spectrum& incident);

 private :
  //! @brief Says if the material is specular
  bool hasDiffuseMaterial(const std::vector<Material*>& materials);
  //! @brief Says if the material is diffuse
  bool hasSpecularMaterial(const std::vector<Material*>& materials);
  //! @brief Read a pixel in the map
  Pixel& getPixel(const Point2D& surfaceCoordinate);

 private :
  //! Array of embedded materials 
  std::vector<Material*> m_materials;
  //! Array of factors (same size than the m_materials
  Real* p_factor;
  //! The map from where factors are extracted
  Image* p_map;
  //! Scale u
  Real m_tileU;
  //! Scale v
  Real m_tileV;
  //! Repeat mode on u
  Texture::TEXTURE_REPEAT_MODE m_texRepeatModeU;
  //! Repeat mode on v
  Texture::TEXTURE_REPEAT_MODE m_texRepeatModeV;
}; // class ConcentrationMap
////////////////////////////////////////////////////////////////////////////////
#endif // GUARD_VRT_CONCENTRATIONMAP_HPP
