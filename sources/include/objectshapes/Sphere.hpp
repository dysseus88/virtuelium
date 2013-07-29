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

#ifndef _SPHERE_HPP
#define _SPHERE_HPP

#include <core/Camera.hpp>
#include <objectshapes/ObjectShape.hpp>

class Sphere : public ObjectShape{
public :
/**
 * Virtual destructor
 */
virtual inline ~Sphere();

/**
 * Constructor
 * center : the center of the sphere
 * radius : the radius of the sphere
 */
inline Sphere(const Point& center, const Real& radius, bool double_sided = false);

/**
 * Intersection test. Return true if the ray intersect the object.
 * In this case, distance will be the distance from the origine of
 * the ray and the nearest intersection point.
 * Otherwise, it will return false and distance will be undetermined.
 * ray : the ray we use to test the intersection with the object.
 * distance : we put the distance between the origine and the 
 *   intersection here.
 */
virtual bool intersect(const Ray& ray, Real& distance);

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
virtual void getLocalBasis(const Ray& ray, const Real& distance, Basis& localBasis, Point2D& surfaceCoordinate);

/**
 * Return the bounding box of the object.
 * boundingBox : we will put the bounding box here
 */
virtual void getBoundingBox(BoundingBox& boundingBox);

private :
  Point _center; //The center of the sphere 
  Real _radius;  //The sphere radius
  Real _radius2; //The square of the sphere radius
	bool _double_sided;
};

/**
 * Virtual destructor
 */
Sphere::~Sphere()
{
  //Nothing to do
}

/**
 * Constructor
 * center : the center of the sphere
 * radius : the radius of the sphere
 */
inline Sphere::Sphere(const Point& center, const Real& radius, bool double_sided)
  : _center(center), _radius(radius), _radius2(radius*radius), _double_sided(double_sided)
{
  //Nothing to do
}

#endif //_SPHERE_HPP
