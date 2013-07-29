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

#ifndef _NORMALMAP_HPP
#define _NORMALMAP_HPP

#include <core/3DBase.hpp>
#include <objectshapes/ObjectShape.hpp>
#include <structures/Image.hpp>

class NormalMap : public ObjectShape{
public :
/**
 * Virtual destructor
 */
virtual inline ~NormalMap();

/**
 * Constructor
 * shape : the shape to translate
 * map : the image corresponding to the normal map
 */
NormalMap(ObjectShape* shape, Image* map, bool global=true);

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
  ObjectShape* _shape; //The shape to translate
  Image* _map;         //The normal map
  bool _isGlobal;      //Is the normal map global (or in the tangent space)
};

/**
 * Virtual destructor
 */
NormalMap::~NormalMap()
{
  if(_shape!=0)
    delete _shape;
  if(_map!=0)
    delete _map;
}

#endif //_NORMALMAP_HPP