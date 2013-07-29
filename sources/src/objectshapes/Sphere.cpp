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

#include "objectshapes/Sphere.hpp"
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
bool Sphere::intersect(const Ray& ray, Real& distance)
{
  Vector toCenter(_center, ray.o);
  Real b = toCenter.dot(ray.v);
  Real c = toCenter.square() - _radius2;  
  Real delta = b*b - c;
  
  if(delta <0)
    return false;

  distance = - b - std::sqrt(delta);
  Real t = -b + std::sqrt(delta);
    
  if( distance <= 0 || ( t>=0 && t<distance))
    distance = t;

  return distance>0;  
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
void Sphere::getLocalBasis(const Ray& ray, const Real& distance, Basis& localBasis, Point2D& surfaceCoordinate)
{
  for(int i=0; i< 3; i++)
  {
    localBasis.o[i] = ray.o[i] + ray.v[i]*distance;
    localBasis.k[i] = localBasis.o[i] - _center[i];
  } 
  localBasis.k.normalize();
  
	// if double_sided, then k is correctly oriented to the origin of the ray
	if (_double_sided )
	{
		Vector Rv = ray.v;
		Rv.mul(-1.0f);

		if ( Rv.dot(localBasis.k) < 0 )
			localBasis.k.mul(-1.0f);
	}

  if(localBasis.k.z() > 0.999 || localBasis.k.z() < -0.999)
  {
    localBasis.j[0] = 1.0; localBasis.j[1] = 0.0; localBasis.j[2] = 0.0;
    localBasis.j.vect(localBasis.k,localBasis.i);
    localBasis.k.vect(localBasis.i,localBasis.j);
  }
  else
  {
    localBasis.j[0] = 0.0; localBasis.j[1] = 0.0; localBasis.j[2] = 1.0;
    localBasis.j.vect(localBasis.k,localBasis.i);
    localBasis.k.vect(localBasis.i,localBasis.j);
  }

  localBasis.i.normalize();
  localBasis.j.normalize();
  
  surfaceCoordinate[0] = std::atan2(localBasis.o[1]-_center[1], localBasis.o[0]-_center[0])/(2*M_PI);
  if(surfaceCoordinate[0]<0)
    surfaceCoordinate[0]+=1.0;
  surfaceCoordinate[1] = std::acos ((localBasis.o[2]-_center[2])/_radius)/M_PI;
}

/**
 * Return the bounding box of the object.
 * boundingBox : we will put the bounding box here
 */
void Sphere::getBoundingBox(BoundingBox& boundingBox)
{
  for(int i=0; i<3; i++)
  {
    boundingBox.min[i]=_center[i]-_radius;
    boundingBox.max[i]=_center[i]+_radius;
  }
}
