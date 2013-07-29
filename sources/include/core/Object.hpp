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
#ifndef GUARD_VRT_OBJECT_HPP
#define GUARD_VRT_OBJECT_HPP
//!
//! @file Object.hpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file defines a scenery object (surface)
//!
#include <core/3DBase.hpp>
#include <core/LightBase.hpp>
////////////////////////////////////////////////////////////////////////////////
//! @see Material
class Material;
////////////////////////////////////////////////////////////////////////////////
//! @see Medium
class Medium;
////////////////////////////////////////////////////////////////////////////////
//! @see ObjectShape
class ObjectShape;
////////////////////////////////////////////////////////////////////////////////
//! @see MultispectralPhoton
class MultispectralPhoton;
////////////////////////////////////////////////////////////////////////////////
//! @class Object
//! @brief An object matches a surface tag in Virtuelium sceneries
class Object {
 public :
  //! @brief Constructor
  Object(void);
  //! @brief Constructor of a complete object
  //! @param shape Geoemtrical definition
  //! @param material Associated material
  //! @param innerMedium Internal medium
  //! @param outerMedium External medium
  Object(ObjectShape* shape, Material* material, 
         Medium* innerMedium=0, Medium* outerMedium=0);
  //! @brief Destructor
  ~Object(void);

 public:
  //! @brief Set the shape and the material of this object. 
  //! @details If there were already a shape and / or a material, the previous 
  //!  will be freed. it is possible to passe 0 for changing only one property.
  //! @param shape Geoemtrical definition
  //! @param material Associated material
  //! @param innerMedium Internal medium
  //! @param outerMedium External medium
  void set(ObjectShape* shape, Material* material, 
           Medium* innerMedium=0, Medium* outerMedium=0);
  //! @brief Set the index of this object into the scenery
  //! @param index New index to be set
  void setIndex(unsigned int index);
  //! @brief Get the index of this object into the scenery
  //! @return Retrieved index
  unsigned int getIndex(void);
  //! @brief Compute the diffuse remited light data
  //! @param localBasis Local basis at the computation point
  //! @param surfaceCoordinate Texture coordinate of the computation point
  //! @param incident Incident light ray
  //! @param reemitedLight resulted Remited light to the view direction 
  void getDiffuseReemited(const Basis& localBasis, 
                          const Point2D& surfaceCoordinate, 
                          LightVector& incidentLight, 
                          LightVector& reemitedLight);
  //! @brief Compute the specular remited light data
  //! @param localBasis Local basis at the computation point
  //! @param surfaceCoordinate Texture coordinate of the computation point
  //! @param incident Incident light ray
  //! @param reemitedLight resulted Remited light to the view direction 
  void getSpecularReemited(const Basis& localBasis, 
                          const Point2D& surfaceCoordinate, 
                          LightVector& incidentLight, 
                          LightVector& reemitedLight);
  //! @brief Compute the remited light coming from an ambient illumination
  //! @details The ambient lighting is isotropic.
  //! @param localBasis Local basis at the computation point
  //! @param surfaceCoordinate Texture coordinate of the computation point
  //! @param reemitedLight resulted Remited light to the view direction 
  //! @param incident Incident light ray
  void getDiffuseReemitedFromAmbiant(const Basis& localBasis, 
                                     const Point2D& surfaceCoordinate, 
                                     LightVector& reemitedLight, 
                                     const Spectrum& incident);
  //! @brief Compute the secondary rays for specular reflexion
  //! @param localBasis Local basis at the computation point
  //! @param surfaceCoordinate Texture coordinate of the computation point
  //! @param reemitedLight resulted Remited light to the view direction 
  //! @param subrays Returned vector containing secondaries rays
  void getSpecularSubRays(const Basis& localBasis, 
                          const Point2D& surfaceCoordinate, 
                          LightVector& reemitedLight, 
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
  bool bouncePhoton(const Basis& localBasis, 
                    const Point2D& surfaceCoordinate, 
                    MultispectralPhoton& photon, 
                    bool& specular);
  //! @brief Compute the behavior of a ray when hitting the scattering material
  //! @details Compute the bounce of the given ray and modify it. Use the 
  //!  'russian roulette' algorithm to decide if the ray is reflected or 
  //!  absorbed. Also tells if this bounce is specular or not.
  //! @param localBasis Local basis at the computation point
  //! @param surfaceCoordinate Texture coordinate of the computation point
  //! @param reemitedLight resulted Remited light to the view direction 
  //! @param nbRays Number of output scattered rays 
  //! @param subrays List of output scattered rays 
  void getRandomDiffuseRay(const Basis& localBasis, 
                           const Point2D& surfaceCoordinate, 
                           LightVector& reemitedLight, 
                           unsigned int nbRays, 
                           std::vector<LightVector>& subrays);
  //! @brief Return true if this material has a diffuse component
  //! @return True if this material has a diffuse component
  bool isDiffuse(void) const;
  //! @brief Return true if this material has a specular component
  //! @return True if this material has a specular component
  bool isSpecular(void) const;
  //! @brief Intersection test. Return true if the ray intersect the object
  //! @details When an intersection is detected, distance will be the distance 
  //!  from the origin of the ray and the nearest intersection point. Otherwise, 
  //!  it will return false and distance will be undetermined.
  //! @param ray Ray used to test the intersection with the object
  //! @param distance Distance between the origin and the intersection point
  bool intersect(const Ray& ray, Real& distance);
  //! @brief Retrieved the local basis to the intersection point
  //! @param ray Ray used to test the intersection with the object
  //! @param distance Distance between the origin and the intersection point
  //! @param localBasis Local basis at the computation point
  //! @param surfaceCoordinate Texture coordinate of the computation point
  void getLocalBasis(const Ray& ray, const Real& distance, 
                     Basis& localBasis, Point2D& surfaceCoordinate);
  //! @brief Return the bounding box of the object.
  //! @param boundingBox Returned bounding box
  void getBoundingBox(BoundingBox& boundingBox);
  //! @brief Return the internal medium
  Medium* getInnerMedium(void);
  //! @brief Return the external medium
  Medium* getOuterMedium(void);

 private :
  //! Object geometry
  ObjectShape* p_shape;
  //! Object material 
  Material* p_material;
  //! Internal medium
  Medium* p_innerMedium;
  //! External medium 
  Medium* p_outerMedium;
  //! Index of the object in the scene
  unsigned int m_index;
}; // class Object
////////////////////////////////////////////////////////////////////////////////
#endif //GUARD_VRT_OBJECT_HPP
