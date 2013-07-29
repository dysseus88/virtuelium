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

#include <objectshapes/NormalMap.hpp>

/**
 * Constructor
 * shape : the shape to translate
 * map : the image corresponding to the normal map
 */
NormalMap::NormalMap(ObjectShape* shape, Image* map, bool global)
: _shape(shape), _map(map), _isGlobal(global)
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
bool NormalMap::intersect(const Ray& ray, Real& distance)
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
void NormalMap::getLocalBasis(const Ray& ray, const Real& distance, Basis& localBasis, Point2D& surfaceCoordinate)
{
  _shape->getLocalBasis(ray, distance, localBasis, surfaceCoordinate);

  Real x = surfaceCoordinate[0]*_map->getWidth();
  Real y = surfaceCoordinate[1]*_map->getHeight();
  Pixel pixel = _map->getInterpolatedPixel(x, y);

  if(_isGlobal)
  {
    localBasis.k[0]= 2.0*pixel[0] - 1.0;
    localBasis.k[1]= 2.0*pixel[1] - 1.0;
    localBasis.k[2]= 2.0*pixel[2] - 1.0;
  }
  else
  {
    Vector n(localBasis.k), u(localBasis.i), v(localBasis.j);
    Real x = 2.0*pixel[0] - 1.0;
    Real y = 2.0*pixel[1] - 1.0;
    Real z = 2.0*pixel[2] - 1.0;
    localBasis.k[0]= u[0]*x + v[0]*y + n[0]*z;
    localBasis.k[1]= u[1]*x + v[1]*y + n[1]*z;
    localBasis.k[2]= u[2]*x + v[2]*y + n[2]*z;
  }

  localBasis.k.vect(localBasis.i, localBasis.j);
  localBasis.j.vect(localBasis.k, localBasis.i);
  localBasis.i.normalize();
  localBasis.j.normalize();
  localBasis.k.normalize();
}

/**
 * Return the bounding box of the object.
 * boundingBox : we will put the bounding box here
 */
void NormalMap::getBoundingBox(BoundingBox& boundingBox)
{
  _shape->getBoundingBox(boundingBox);
}
