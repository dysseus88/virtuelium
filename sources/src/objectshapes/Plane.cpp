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

#include "objectshapes/Plane.hpp"
#include <iostream>

/**
 * Intersection test. Return true if the ray intersect the object.
 * In this case, distance will be the distance from the origine of
 * the ray and the nearest intersection point.
 * Otherwise, it will return false and distance will be undetermined.
 * ray : the ray we use to test the intersection with the object.
 * distance : we put the distance between the origine and the 
 *   intersection here.
 */
bool Plane::intersect(const Ray& ray, Real& distance)
{
  // Get the intersection with the infinite Planee
  Vector toPlanee(ray.o, _origin);
  distance = toPlanee.dot(_normal)/ray.v.dot(_normal);
  if(ray.v.dot(_normal) == 0 || distance < 0)
    return false;

  if(_finite)
  {
    Vector OM;
    OM[0] = -toPlanee[0] + distance*ray.v[0];
    OM[1] = -toPlanee[1] + distance*ray.v[1];
    OM[2] = -toPlanee[2] + distance*ray.v[2];
    Real x = OM.dot(_u)/(_normu);
    Real y = OM.dot(_v)/(_normv);
    if(x<0.0 || x>1.0 || y<0.0 || y>1.0)
      return false;
  }

  return true; 
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
void Plane::getLocalBasis(const Ray& ray, const Real& distance, Basis& localBasis, Point2D& surfaceCoordinate)
{
  //Build intersection point
  localBasis.o[0] = ray.o[0] + distance * ray.v[0];
  localBasis.o[1] = ray.o[1] + distance * ray.v[1];
  localBasis.o[2] = ray.o[2] + distance * ray.v[2];
  
	// if double_sided, then k is correctly oriented to the origin of the ray
	if (_double_sided )
	{
		Vector Rv = ray.v;
		Rv.mul(-1.0f);

		if ( Rv.dot(localBasis.k) < 0 )
			localBasis.k.mul(-1.0f);
	}

  //Build base vector
  localBasis.k = _normal;
  localBasis.i = _u;
  localBasis.j = _v;

  //Build surface coordinate
  Vector OM(_origin, localBasis.o);
  surfaceCoordinate[0] = OM.dot(_u)/(_normu);
  surfaceCoordinate[1] = OM.dot(_v)/(_normv);
}

/**
 * Return the bounding box of the object.
 * boundingBox : we will put the bounding box here
 */
void Plane::getBoundingBox(BoundingBox& boundingBox)
{
  boundingBox.min = _origin;
  boundingBox.max = _origin;

  if(_finite)
  {
    boundingBox.updateWith(Point(_origin[0]+_u[0]*_normu, _origin[1]+_u[1]*_normu, _origin[2]+_u[2]*_normu));
    boundingBox.updateWith(Point(_origin[0]+_v[0]*_normv, _origin[1]+_v[1]*_normv, _origin[2]+_v[2]*_normv));
    boundingBox.updateWith(Point(_origin[0]+_u[0]*_normu+_v[0]*_normv, _origin[1]+_u[1]*_normu+_v[1]*_normv, _origin[2]+_u[2]*_normu+_v[2]*_normv));
  }
  else
  {
    for(int i=0; i<3; i++)
    {
      boundingBox.min[i]=_origin[i]-1000;
      boundingBox.max[i]=_origin[i]+1000;
    }
  }
}
