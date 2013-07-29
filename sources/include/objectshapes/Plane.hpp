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

#ifndef _Plane_HPP
#define _Plane_HPP

#include <core/Camera.hpp>
#include <objectshapes/ObjectShape.hpp>

class Plane : public ObjectShape{
public :
/**
 * Virtual destructor
 */
virtual inline ~Plane();

/**
 * Constructor
 * origin : a point of the Plane
 * normal : the normal to the Plane
 */
inline Plane(const Point& origin, const Vector& normal, const Vector& u, 
             const Vector& v, bool finite, bool double_sided = false);

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
  Point _origin; //The origin of the Plane
  Vector _normal;  //The normal
  Vector _u;  //The vector that define u coordinates
  Vector _v;  //The vector that define v coordinates
  Real _normu; //The norm of the vector that define u coordinates
  Real _normv; //The norm of the vector that define v coordinates
  bool _finite; //Is the Plane finite ?
	bool _double_sided;
};

/**
 * Virtual destructor
 */
Plane::~Plane()
{
  //Nothing to do
}

/**
 * Constructor
 * center : the center of the sphere
 * radius : the radius of the sphere
 */
inline Plane::Plane(const Point& origin, const Vector& normal, const Vector& u, 
                    const Vector& v, bool finite, bool double_sided)
: _origin(origin), _normal(normal), _u(u), _v(v), _finite(finite), 
  _double_sided(double_sided)
{
  //Building u and v if they are null
  if((_u[0]==0 && _u[1]==0 && _u[2]==0) || (_v[0]==0 && _v[1]==0 && _v[2]==0))
  {
    if(_normal[0]<0.9 && _normal[0]>-0.9)
      _u[0]=1.0;
    else
      _u[1]=1.0;

    _normal.vect(_u, _v);
    _v.vect(_normal, _u);
  }

  //Compute the norm of u and v.
  _normu = _u.norm();
  _normv = _v.norm();

  _u.normalize();
  _v.normalize();
  _normal.normalize();
}

#endif //_SPHERE_HPP
