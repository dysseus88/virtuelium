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
#ifndef GUARD_VRT_SCALE_HPP
#define GUARD_VRT_SCALE_HPP
//!
//! @file Scale.hpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @remarks
//! @details This file defines the scale in 3D space
//!
#include <core/Camera.hpp>
#include <objectshapes/ObjectShape.hpp>
////////////////////////////////////////////////////////////////////////////////
//! @class Scale
//! @brief Defines the scale in 3D space
class Scale : public ObjectShape {
 public :
  //! @brief Constructor
  //! @param xfactor Scale on x-axis
  //! @param yfactor Scale on y-axis
  //! @param zfactor Scale on z-axis
  //! @param shape Shape to be scaled
  Scale(Real xfactor, Real yfactor, Real zfactor, ObjectShape* shape);
  //! @brief Destructor
  virtual ~Scale();

 public:
  //! @brief Intersection test with p_shape
  //! @param ray Input ray used by the test
  //! @param distance If Intersection is detected, then distance will match the 
  //!  following length : [ray.o, first hit point]; otherwise the distance will  
  //!  be undetermined
  //! @return True if the ray intersect the object.
  virtual bool intersect(const Ray& ray, Real& distance);
  //! @brief Get the local basis of the scaled object
  virtual void getLocalBasis(const Ray& ray, const Real& distance, Basis& localBasis, Point2D& surfaceCoordinate);
  //! @brief Get the bounding box of the scaled object
  virtual void getBoundingBox(BoundingBox& boundingBox);

 private:
  //! @brief Transformation of a vector
  void transform(Vector& v);
  //! @brief Transformation of a point
  void transform(Point& p);
  //! @brief Inverse transformation of a vector
  void inverse_transform(Vector& v);
  //! @brief Inverse transformation of a point
  void inverse_transform(Point& p);

 private :
  //! Scale on x-axis
  Real m_xfactor;
  //! Scale on y-axis
  Real m_yfactor;
  //! Scale on z-axis
  Real m_zfactor;
  //! Shape to be scaled
  ObjectShape* p_shape; 
}; // class Scale
////////////////////////////////////////////////////////////////////////////////
#endif //GUARD_VRT_SCALE_HPP
