/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
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
 */

#ifndef _INSTANCEOBJECTSHAPE_HPP
#define _INSTANCEOBJECTSHAPE_HPP

#include <core/Camera.hpp>
#include <objectshapes/ObjectShape.hpp>

class InstanceObjectShape : public ObjectShape{
public :
  /**
   * Virtual destructor
   */
  inline virtual ~InstanceObjectShape();

  /**
   * Constructor
   * axis : the axis of rotation (0:x, 1:y, 2:z)
   * angle : the angle to rotate in radian
   * shape : the shape to rotate
   */
  inline InstanceObjectShape(ObjectShape* shape);

  /**
   * Intersection test. Return true if the ray intersect the object.
   * In this case, distance will be the distance from the origine of
   * the ray and the nearest intersection point.
   * Otherwise, it will return false and distance will be undetermined.
   * ray : the ray we use to test the intersection with the object.
   * distance : we put the distance between the origine and the 
   *   intersection here.
   */
  inline virtual bool intersect(const Ray& ray, Real& distance);

  /**
   * Intersection test. Return true if the ray intersect the object.
   * In this case, distance will be the distance from the origine of
   * the ray and the nearest intersection point.
   * Otherwise, it will return false and distance will be undetermined.
   * ray : the ray we use to test the intersection with the object.
   * distance : the computed distance of the intersection point from the
   *   origine of the ray.
   * localBasis : we will put the local Basis of the intersection point here.
   * surfaceCoordinate : we will put the surface coordinate of the 
   *   intersection point here.
   */
  inline virtual void getLocalBasis(const Ray& ray, const Real& distance, Basis& localBasis, Point2D& surfaceCoordinate);

  /**
   * Return the bounding box of the object.
   * boundingBox : we will put the bounding box here
   */
  inline virtual void getBoundingBox(BoundingBox& boundingBox);

private :
  ObjectShape* _shape;
};

/**
 * Virtual destructor
 */
inline InstanceObjectShape::~InstanceObjectShape()
{
  //Nothing to do
}

/**
 * Constructor
 * axis : the axis of rotation (0:x, 1:y, 2:z)
 * angle : the angle to rotate in radian
 * shape : the shape to rotate
 */
inline InstanceObjectShape::InstanceObjectShape(ObjectShape* shape)
: _shape(shape)
{
  //Nothing to do more
}

/**
 * Intersection test. Return true if the ray intersect the object.
 * In this case, distance will be the distance from the origine of
 * the ray and the nearest intersection point.
 * Otherwise, it will return false and distance will be undetermined.
 * ray : the ray we use to test the intersection with the object.
 * distance : we put the distance between the origine and the 
 *   intersection here.
 */
inline bool InstanceObjectShape::intersect(const Ray& ray, Real& distance)
{
  return _shape->intersect(ray, distance);
}

/**
 * Intersection test. Return true if the ray intersect the object.
 * In this case, distance will be the distance from the origine of
 * the ray and the nearest intersection point.
 * Otherwise, it will return false and distance will be undetermined.
 * ray : the ray we use to test the intersection with the object.
 * distance : the computed distance of the intersection point from the
 *   origine of the ray.
 * localBasis : we will put the local base of the intersection point here.
 * surfaceCoordinate : we will put the surface coordinate of the 
 *   intersection point here.
 */
inline void InstanceObjectShape::getLocalBasis(const Ray& ray, const Real& distance, Basis& localBasis, Point2D& surfaceCoordinate)
{
  _shape->getLocalBasis(ray, distance, localBasis, surfaceCoordinate);
}

/**
 * Return the bounding box of the object.
 * boundingBox : we will put the bounding box here
 */
inline void InstanceObjectShape::getBoundingBox(BoundingBox& boundingBox)
{
  _shape->getBoundingBox(boundingBox);
}

#endif //_INSTANCEOBJECTSHAPE_HPP
