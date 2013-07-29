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

#ifndef _TRIANGLE_HPP
#define _TRIANGLE_HPP

#include <core/Camera.hpp>

#include <objectshapes/ObjectShape.hpp>

class Triangle : public ObjectShape{
public :
/**
 * Default constructor
 */
inline Triangle();

/**
 * Constructor
 * vertices : array that contain the three vertices of this triangle
 * normals : array that contain the three normal of this triangle
 * surfaceCoordinates : array that contain the texture corrdinates of the triangle
 */
Triangle(Point vertices[3], Vector normals[3], Point2D surfaceCoordinates[3], bool double_sided = false);
Triangle(Point vertices[3], Point2D surfaceCoordinates[3], bool double_sided = false);

/**
 * Virtual destructor
 */
virtual inline ~Triangle();

/**
 * Set the triangle properties
 * vertices : array that contain the three vertices of this triangle
 * normals : array that contain the three normal of this triangle
 * surfaceCoordinates : array that contain the texture corrdinates of the triangle
 */
virtual void set(Point vertices[3], Vector normals[3], Point2D surfaceCoordinates[3], bool double_sided = false);
virtual void set(Point vertices[3], Point2D surfaceCoordinates[3], bool double_sided = false);

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

void Print(void);

private : 
  Point _vertices[3];
  Point2D _texCoords[3];
  Vector _normals[3];
  Vector _normal;
  BoundingBox _boundingBox;
	bool _double_sided;
};

/**
 * Default constructor
 */
inline Triangle::Triangle()
{
  //Nothing to do
}

/**
 * Virtual destructor
 */
inline Triangle::~Triangle()
{
  //Nothing to do
}

#endif //_TRIANGLE_HPP
