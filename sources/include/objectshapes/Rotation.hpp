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
#ifndef GUARD_VRT_ROTATION_HPP
#define GUARD_VRT_ROTATION_HPP
//!
//! @file Rotation.hpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @remarks
//! @details This file defines the rotation in 3D space
//!
#include <core/Camera.hpp>
#include <objectshapes/ObjectShape.hpp>
////////////////////////////////////////////////////////////////////////////////
//! @class Scale
//! @brief Defines the rotation in 3D space
class Rotation : public ObjectShape {
 public :
  //! @brief Constructor
  //! @param axis Axis of rotation (0:x, 1:y, 2:z)
  //! @param angle Angle to rotate in radian
  //! @param shape Shape to be scaled
  Rotation(char axis, Real angle, ObjectShape* shape);
  //! @brief Destructor
  virtual ~Rotation();

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

  void to_local(Vector& v);
  void to_local(Point& p);
  void to_world(Vector& v);
  void to_world(Point& p);


private :
  //! General matrix of rotation of angle alpha around an axis u
  Real m_matrix[9]; 
  //! General  matrix of the rotation of angle -alpha around an axis u
  Real m_backmatrix[9]; 
  //! translation vector from the world origin to the local origin
  Vector world_to_local;
  //! Shape to be rotated
  ObjectShape* p_shape; 
}; // class Rotation
////////////////////////////////////////////////////////////////////////////////
#endif // GUARD_VRT_ROTATION_HPP
