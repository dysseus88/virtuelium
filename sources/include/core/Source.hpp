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
#ifndef GUARD_VRT_SOURCE_HPP
#define GUARD_VRT_SOURCE_HPP
//!
//! @file Source.hpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file defines a scenery light source (source)
//!
#include <core/3DBase.hpp>
#include <core/LightBase.hpp>

////////////////////////////////////////////////////////////////////////////////
//! @see LightSource
class LightSource;
////////////////////////////////////////////////////////////////////////////////
//! @see ObjectShape
class ObjectShape;
////////////////////////////////////////////////////////////////////////////////
//! @see MultispectralPhoton
class MultispectralPhoton;
////////////////////////////////////////////////////////////////////////////////
//! @class Source
//! @brief An Source matches a source tag in Virtuelium sceneries
class Source {
 public:
  //! @brief Constructor
  Source(void);
  //! @brief Constructor of complete Source
  //! @param source Light source of this Source
  //! @param spectralemiter Shape of this Source
  Source(LightSource* source, ObjectShape* shape);
  //! @brief Destructor
  //! @remarks Free also the shape and the material (if any)
  ~Source(void);

 public:
  //! @brief Set the ray caster and the spectral emiter of this object.
  //! @details If there were already a lightsource and/or a shape, the previous 
  //!  will be freed. It is possible to passe 0 for changing only one property.
  //! @param source light source of this Source
  //! @param spectralemiter Shape of this Source
  void set(LightSource* source, ObjectShape* shape);
  //! @brief Place incidents casted light data casted into the given vector
  //! @param receiver Point where we need to have the incidents lights rays
  //! @param reemited light data as reference for spectral data (generally the
  //!  one that we want to compute for the interaction that will receive the 
  //!  incidents)
  //! @param incidents incidents light data will be placed into this vector
  void getIncidentLight(const Point& receiver, 
                        const LightVector& reemited, 
                        std::vector<LightVector>& incidents);
  //! @brief Return the power of this source (the sum of all wavelenght)
  //! @return The power of the light source
  Real getPower(void);
  //! @brief Compute the emitted light for direct source view
  //! @param localBasis Local basis on the surface of the source
  //! @param surfaceCoordinate Surface coordinate on the source surface
  //! @param emitted Light data to compute
  void getEmittedLight(const Basis& localBasis, 
                       const Point2D& surfaceCoordinate, 
                       LightVector& emitted);
  //! @brief Generate a random photon according to the propriety of the source
  //! @param photon Photon to be generated
  void getRandomPhoton(MultispectralPhoton& photon);
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
  void getLocalBasis(const Ray& ray, const Real& distance, Basis& localBasis, 
                     Point2D& surfaceCoordinate);
  //! @brief Return the bounding box of the object.
  //! @param boundingBox Returned bounding box
  void getBoundingBox(BoundingBox& boundingBox);

 private :
  //! Shape of the light source
  ObjectShape* p_shape;
  //! Spectral properties of the light source
  LightSource* p_source;
}; // class Source
////////////////////////////////////////////////////////////////////////////////
#endif //GUARD_VRT_SOURCE_HPP
