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
#ifndef GUARD_VRT_TEXTUREBRDF_HPP
#define GUARD_VRT_TEXTUREBRDF_HPP

#include <vector>

#include <core/3DBase.hpp>
#include <core/LightBase.hpp>


#include <structures/Texture.hpp>
#include <structures/Medium.hpp>

#include <materials/Material.hpp>
//!
//! @file TextureBRDF.hpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @remarks
//! @details A simple texturized material with lambertian behavior
//!
////////////////////////////////////////////////////////////////////////////////
//! @class Texture
//! @brief Texturized material
//! @details This material comes over another embedded material
//! 
class TextureBRDF : public Material {
 public :
  //! @brief Constructor
  TextureBRDF(Material* material, Texture* map, const Real& amount, 
              const Real& tileU, const Real& tileV, 
              Texture::TEXTURE_REPEAT_MODE texReapeatModeU=Texture::REPEAT_OFF, 
              Texture::TEXTURE_REPEAT_MODE texReapeatModeV=Texture::REPEAT_OFF, 
              Texture::ALPHA_MODE alphaMode=Texture::ALPHA_OFF);  

  //! @brief Destructor
  virtual ~TextureBRDF(void);

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
  //! @brief Randomly generate random secondary rays for diffuse material
  void generateRandomeDiffuseRay(const Vector& normal, const Point& origin, 
                                 unsigned int nbRays, 
                                 LightVector& reemitedLight, 
                                 std::vector<LightVector>& subrays);
  //! @brief Generate photons for diffuse material (photon mapping only)
  void generatePhoton(const Vector& normal, MultispectralPhoton& photon, 
                      Real mean);

 private :
  //! Embedded material
  Material* p_mtl;
  //! Texture map
  Texture* p_map;
  //! Illumination factor (multiplies the texture spectrum)
  Real m_amount;
  //! Scale u
  Real m_tileU;
  //! Scale v
  Real m_tileV;
  //! Repeat mode on u
  Texture::TEXTURE_REPEAT_MODE m_texRepeatModeU;
  //! Repeat mode on v
  Texture::TEXTURE_REPEAT_MODE m_texRepeatModeV;
  /// Alpha mode
  Texture::ALPHA_MODE  m_alphaMode;
}; // class TextureBRDF
////////////////////////////////////////////////////////////////////////////////
#endif // GUARD_VRT_TEXTUREBRDF_HPP
