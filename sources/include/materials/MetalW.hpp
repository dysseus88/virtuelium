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
#ifndef VRT_GUARD_MetalW_HPP
#define VRT_GUARD_MetalW_HPP
//!
//! @file MetalW.hpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file is responsible of simulating an alloy between two metals 
//! @remarks This version of virtuelium is MPI-only. please refer to MPI docs!
//!
#include <vector>

#include <core/3DBase.hpp>
#include <core/LightBase.hpp>


#include <structures/Medium.hpp>

#include <materials/Material.hpp>
////////////////////////////////////////////////////////////////////////////////
//! @class MetalW
//! @brief The alloy class must concerns only metals
class MetalW : public Material {
 public:
  //! @brief Constructor
  //! @param media_stack Array of media: external (air), internal (metal)
  //! @param mi Material roughness - direction i
  //! @param mj Material roughness - direction j
  MetalW(std::vector<const Medium*>& media_stack, 
            Real mi, Real mj); 
	//! @brief Virtual destructor
  virtual ~MetalW(void);
  
 public:
  //! @brief Compute the diffuse remited light data
  //! @param localBasis Local basis at the computation point
  //! @param surfaceCoordinate Texture coordinate of the computation point
  //! @param incident Incident light ray
  //! @param reemitedLight resulted Remited light to the view direction 
  virtual void getSpecularReemited(const Basis& localBasis, 
                                   const Point2D& surfaceCoordinate, 
                                   const LightVector& incidentLight, 
                                   LightVector& reemitedLight);
  //! @brief Compute the diffuse remited light data
  //! @param localBasis Local basis at the computation point
  //! @param surfaceCoordinate Texture coordinate of the computation point
  //! @param reemitedLight resulted Remited light to the view direction 
  //! @param subrays List of output scattered rays 
  virtual void getSpecularSubRays(const Basis& localBasis, 
                                  const Point2D& surfaceCoordinate, 
                                  LightVector& reemitedLight, 
                                  std::vector<LightVector>& subrays);
  //! @brief Compute the diffuse remited light data
  //! @param localBasis Local basis at the computation point
  //! @param surfaceCoordinate Texture coordinate of the computation point
  //! @param incident Incident light ray
  //! @param reemitedLight resulted Remited light to the view direction 
  virtual void getDiffuseReemited(const Basis& localBasis,
	                                const Point2D& surfaceCoordinate, 
                                  const LightVector& incidentLight,
	                                LightVector& reemitedLight);
  //! @brief Compute the behavior of a ray when hitting the scattering material
  //! @details Compute the bounce of the given ray and modify it. Use the 
  //!  'russian roulette' algorithm to decide if the ray is reflected or 
  //!  absorbed. Also tells if this bounce is specular or not.
  //! @param localBasis Local basis at the computation point
  //! @param surfaceCoordinate Texture coordinate of the computation point
  //! @param reemitedLight resulted Remited light to the view direction 
  //! @param nbRays Number of output scattered rays 
  //! @param subrays List of output scattered rays 
  virtual void getRandomDiffuseRay(const Basis& localBasis, 
	                                 const Point2D& surfaceCoordinate, 
                                   LightVector& reemitedLight,
                                   unsigned int nbRays, 
                                   std::vector<LightVector>& subrays);
  //! @brief This method compute a random reflexion for the given photon
  //! @details Uusing the 'russian roulette' algorithm for handeling the 
  //!  absorption of the photon. This method also tells if the reflected photon 
  //!  is specular or not.  
  //! @param localBasis Local basis at the computation point
  //! @param surfaceCoordinate Texture coordinate of the computation point
  //! @param photon Photon to be treated (it would be modified directly) 
  //! @param specular : True if the bounced photon is specular, false otherwise
  //! @return True if the photon had been reemited or false if it was absorbed
  virtual bool bouncePhoton(const Basis& localBasis,
	                          const Point2D& surfaceCoordinate, 
                            MultispectralPhoton& photon, bool& specular);
  //! @brief Compute the remited light coming from an ambient illumination
  //! @details The ambient lighting is isotropic.
  //! @param localBasis Local basis at the computation point
  //! @param surfaceCoordinate Texture coordinate of the computation point
  //! @param reemitedLight resulted Remited light to the view direction 
  //! @param incident Incident light ray
  virtual void getDiffuseReemitedFromAmbiant(const Basis& localBasis, 
	                                           const Point2D& surfaceCoordinate, 
                                             LightVector& reemitedLight, 
                                             const Spectrum& incident);

 private:
  //! @brief Get the reflectance in the reflection direction
  //! @param cosOi Cosinus of the angle between the normal and incident
  //! @param index We want the spectral value at a specfific wavelength
  //! @param ROrth Orthogonal polarized computed Reflectance will be placed here
  //! @param Rpara Parallel polarized computed Reflectance will be placed here
  void getReflectance(const Real& cosOi, const int& index, 
                      Real& ROrth, Real& RPara);

 private:
  //! Reflected energies parallel to the direction of the polarized light. 
  //!  One spectrum by specular angle
  std::vector<Spectrum> p_RPara;
  //! Reflected energies orithogonal to the direction of the polarized light. 
  //!  One spectrum by specular angle
  std::vector<Spectrum> p_ROrth;
  //! Material roughness - direction i
  Real m_mi;
  //! Material roughness - direction j
  Real m_mj;
}; // class MetalW
////////////////////////////////////////////////////////////////////////////////
#endif //VRT_GUARD_MetalW_HPP
