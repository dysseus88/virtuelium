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

#include <objectshapes/Mesh.hpp>
#include <iostream>

//------------------------------------------------------------------------------
// SurfaceOctreeVisitor --------------------------------------------------------
//------------------------------------------------------------------------------

/**
 * Initialize the visitor and errase the old results.
 */
void MeshOctreeVisitor::init()
{
  //Initialize result data
  _distance=-1;
  _object=0;
}

/**
 * Apply the visitor on a triangle
 */
void MeshOctreeVisitor::apply(const Ray& ray, Triangle& object)
{
  Real distance = -1;
  if(object.intersect(ray, distance) && distance>0)
    if(_distance<0 || _distance>distance)
    {
      _distance=distance;
      _object=&object;
    }
}

/**
 * Return the first intersection distance
 */
Real MeshOctreeVisitor::getMinDistance()
{
  return _distance;
}

/**
 * Return the triangle of the first intersection, NULL otherwise
 */
Triangle* MeshOctreeVisitor::getMinDistanceTriangle()
{
  return _object;
}

//------------------------------------------------------------------------------
// Mesh ------------------------------------------------------------------------
//------------------------------------------------------------------------------

/**
 * Constructor from triangles array
 * triangles : array that contain all the triangles.
 * nbTriangles : number of triangles contained in the given array
 */
Mesh::Mesh(Triangle* triangles, int nbTriangles)
{
  //Initializing bounding box
  triangles[0].getBoundingBox(_boundingbox);
  for(int i=0; i<nbTriangles; i++)
  {
    BoundingBox trianglebox;
    triangles[i].getBoundingBox(trianglebox);
    _boundingbox.updateWith(trianglebox);
  }

  //Creating octree
  int _depth = (std::log10((float)nbTriangles)>1)? (int)std::log10((float)nbTriangles) : 1;
  if(_depth<1)
    _depth=1;
  _triangles= new Octree<Triangle>(_boundingbox, nbTriangles, _depth);
  
  //Filling octree
  for(int i=0; i< nbTriangles; i++)
  {
    BoundingBox trianglebox;
    triangles[i].getBoundingBox(trianglebox);
    _triangles->add(triangles[i], trianglebox);
  }
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
bool Mesh::intersect(const Ray& ray, Real& distance)
{

  //Test the bounding box
  if(!_boundingbox.canIntersect(ray))
    return false;

  //Initialize visitor
  MeshOctreeVisitor visitor;
  visitor.init();
  
  //Applying the visitor on the octree
  _triangles->accept(ray, visitor);
  
  //Getting the intersection distance
  distance = visitor.getMinDistance();

  if(distance <0)
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
void Mesh::getLocalBasis(const Ray& ray, const Real& distance, Basis& localBasis, Point2D& surfaceCoordinate)
{
  //Initialize visitor
  MeshOctreeVisitor visitor;
  visitor.init();
  
  //Applying the visitor on the octree
  _triangles->accept(ray, visitor);

  //If there were no intersection return !
  if(visitor.getMinDistanceTriangle()==0)
    return;
  
  //Let delegate the job to the good triangle
  visitor.getMinDistanceTriangle()->getLocalBasis(ray, distance, localBasis, surfaceCoordinate);
}

/**
 * Return the bounding box of the object.
 * boundingBox : we will put the bounding box here
 */
void Mesh::getBoundingBox(BoundingBox& boundingBox)
{
  boundingBox=_boundingbox;
}
