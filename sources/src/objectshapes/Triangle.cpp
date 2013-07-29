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

#include <objectshapes/Triangle.hpp>

#include <iostream>

#include <core/VrtLog.hpp>

/**
 * Constructor
 * vertices : array that contain the three vertices of this triangle
 * normals : array that contain the three normal of this triangle
 * surfaceCoordinates : array that contain the texture corrdinates of the triangle
 */
Triangle::Triangle(Point vertices[3], Vector normals[3], Point2D surfaceCoordinates[3], bool double_sided)
{
  set(vertices, normals, surfaceCoordinates, double_sided);
}
Triangle::Triangle(Point vertices[3], Point2D surfaceCoordinates[3], bool double_sided)
{ 
  set(vertices, surfaceCoordinates, double_sided);
}

/**
 * Set the triangle properties
 * vertices : array that contain the three vertices of this triangle
 * normals : array that contain the three normal of this triangle
 * surfaceCoordinates : array that contain the texture corrdinates of the triangle
 */
void Triangle::set(Point vertices[3], Vector normals[3], Point2D surfaceCoordinates[3], bool double_sided)
{
	_double_sided = double_sided;

  //Affecting base parameters
  for(int i=0; i<3; i++)
  {
    _vertices[i]=vertices[i];
    _texCoords[i]=surfaceCoordinates[i];
    _normals[i]=normals[i];
  }

  //Computing the normal
  Vector(vertices[0], vertices[1]).vect(Vector(vertices[0], vertices[2]), _normal);
  _normal.normalize();

  //Computing the bounding box
  _boundingBox.min=_vertices[0];
  _boundingBox.max=_vertices[0];
  _boundingBox.updateWith(_vertices[1]);
  _boundingBox.updateWith(_vertices[2]);

}
void Triangle::set(Point vertices[3], Point2D surfaceCoordinates[3], bool double_sided)
{
	_double_sided = double_sided;

  //Computing the normal
  Vector(vertices[0], vertices[1]).vect(Vector(vertices[0], vertices[2]), _normal);
  _normal.normalize();

  //Affecting base parameters
  for(int i=0; i<3; i++)
  {
    _vertices[i]=vertices[i];
    _texCoords[i]=surfaceCoordinates[i];
    _normals[i]=_normal;
  }

  //Computing the bounding box
  _boundingBox.min=_vertices[0];
  _boundingBox.max=_vertices[0];
  _boundingBox.updateWith(_vertices[1]);
  _boundingBox.updateWith(_vertices[2]);

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
bool Triangle::intersect(const Ray& ray, Real& distance)
{
  if(!_boundingBox.canIntersect(ray))
    return false;
  
  // Get the intersection with the infinite plane
  Vector toPlane(ray.o, _vertices[0]);
  distance = _normal.dot(toPlane)/_normal.dot(ray.v);
  if((ray.v.dot(_normal) >- kEPSILON && ray.v.dot(_normal) < kEPSILON) || distance < 0)
    return false;  
  Point intersection(ray, distance);
  
  //Verify that the intersection point is in the triangle
  Vector vects[3];
  for(int i=0; i<3; i++)
    Vector(_vertices[i], _vertices[(i+1)%3]).vect(Vector(_vertices[i], intersection), vects[i]);

  if(vects[0].dot(vects[1])<0 || vects[1].dot(vects[2])<0 || vects[2].dot(vects[0])<0)
    return false;
  
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
void Triangle::getLocalBasis(const Ray& ray, const Real& distance, Basis& localBasis, Point2D& surfaceCoordinate)
{
  // Get the intersection point
  Point intersection(ray, distance);
  
  //Computing weights
  Real weights[3];
  for(int i=0; i<3; i++)
  {
    Vector edge(_vertices[i], _vertices[(i+1)%3]);
    Vector hyp(_vertices[i], intersection);
    Vector pvect;
    edge.vect(hyp, pvect);
    weights[(i+2)%3] = pvect.norm();
  }

  Real totalweight = weights[0] + weights[1] + weights[2];
  weights[0]/=totalweight;
  weights[1]/=totalweight;
  weights[2]/=totalweight;
  
  // Get Normal
  localBasis.o=intersection;
  localBasis.k[0]= weights[0]*_normals[0][0] + weights[1]*_normals[1][0] + weights[2]*_normals[2][0];
  localBasis.k[1]= weights[0]*_normals[0][1] + weights[1]*_normals[1][1] + weights[2]*_normals[2][1];
  localBasis.k[2]= weights[0]*_normals[0][2] + weights[1]*_normals[1][2] + weights[2]*_normals[2][2];

	// if double_sided, then k is correctly oriented to the origin of the ray
	if (_double_sided )
	{
		Vector Rv = ray.v;
		Rv.mul(-1.0f);

		if ( Rv.dot(localBasis.k) < 0 )
			localBasis.k.mul(-1.0f);
	}

  // Get tangents vector
  Vector edge1(_vertices[0], _vertices[1]);
  Vector edge2(_vertices[0], _vertices[2]);
  Point2D uvedge1 (_texCoords[1][0]-_texCoords[0][0], _texCoords[1][1]-_texCoords[0][1]);
  Point2D uvedge2 (_texCoords[2][0]-_texCoords[0][0], _texCoords[2][1]-_texCoords[0][1]);
  Real cp = uvedge1[1] * uvedge2[0] - uvedge1[0] * uvedge2[1];
  if(cp != 0.0f)
  {
    Real mul = 1.0f / cp;
    localBasis.i[0] =  (edge1[0] * -uvedge2[1] + edge2[0]*uvedge1[1])*mul;
    localBasis.i[1] =  (edge1[1] * -uvedge2[1] + edge2[1]*uvedge1[1])*mul;
    localBasis.i[2] =  (edge1[2] * -uvedge2[1] + edge2[2]*uvedge1[1])*mul;
  }
  localBasis.k.vect(localBasis.i, localBasis.j);
  localBasis.j.vect(localBasis.k, localBasis.i);

  //Normalizing vectors
  localBasis.i.normalize();
  localBasis.j.normalize();
  localBasis.k.normalize();
  
  // Get surfaceCoordinate
  surfaceCoordinate[0] =  weights[0]*_texCoords[0][0] + weights[1]*_texCoords[1][0] + weights[2]*_texCoords[2][0];
  surfaceCoordinate[1] =  weights[0]*_texCoords[0][1] + weights[1]*_texCoords[1][1] + weights[2]*_texCoords[2][1];
}

/**
 * Return the bounding box of the object.
 * boundingBox : we will put the bounding box here
 */
void Triangle::getBoundingBox(BoundingBox& boundingBox)
{
  boundingBox = _boundingBox;
}


void Triangle::Print(void) {
  VrtLog::Write("-- Sommet 1 (%f, %f, %f)", _vertices[0][0], _vertices[0][1], _vertices[0][2]);
  VrtLog::Write("-- Normale 1 (%f, %f, %f)", _normals[0][0], _normals[0][1], _normals[0][2]);
  VrtLog::Write("-- Texture 1 (%f, %f)", _texCoords[0][0], _texCoords[0][1]);

  VrtLog::Write("-- Sommet 2 (%f, %f, %f)", _vertices[1][0], _vertices[1][1], _vertices[1][2]);
  VrtLog::Write("-- Normale 2 (%f, %f, %f)", _normals[1][0], _normals[1][1], _normals[1][2]);
  VrtLog::Write("-- Texture 2 (%f, %f)", _texCoords[1][0], _texCoords[1][1]);

  VrtLog::Write("-- Sommet 3 (%f, %f, %f)", _vertices[2][0], _vertices[2][1], _vertices[2][2]);
  VrtLog::Write("-- Normale 3 (%f, %f, %f)", _normals[2][0], _normals[2][1], _normals[2][2]);
  VrtLog::Write("-- Texture 3 (%f, %f)", _texCoords[2][0], _texCoords[2][1]);

  VrtLog::Write("-- Normale moyenne (%f, %f, %f)", _normal[0], _normal[1], _normal[2]);
}