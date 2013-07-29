/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#include <core/Scenery.hpp>
#include <core/Camera.hpp>

// -----------------------------------------------------------------------------
// Scenery ---------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * Constructor. The global bounding box must be set at the first time and must
 * be well sized. No resize are possible.
 * globalbounds : the overall bounding box. Must contain all the object of the scene.
 */
Scenery::Scenery(int nbObject, int nbSource, const BoundingBox& globalbounds, const Real& bias)
: _objects(globalbounds, nbObject, (std::log10((float)nbObject)>1)? (int)std::log10((float)nbObject) : 1),
  _sources(globalbounds, nbSource, (std::log10((float)nbSource)>1)? (int)std::log10((float)nbSource) : 1),
  _renderer(0), _bias(bias)
{ 
  //Nothing to do more
}

/**
 * Destructor : it doesn't free contained objects !
 */
Scenery::~Scenery()
{
  for(unsigned int i=0; i<_objects.getSize(); i++)
    delete _objects.getElement(i);
  for(unsigned int i=0; i<_sources.getSize(); i++)
    delete _sources.getElement(i);
  for(unsigned int i=0; i<_cameras.size(); i++)
    delete _cameras[i];
  for(unsigned int i=0; i<_mediums.size(); i++)
    delete _mediums[i];

  if(_renderer!=0)
    delete _renderer;
}

/**
 * Objects accessors
 */
void Scenery::addObject(Object* object)
{
  //Adding into the octree
  BoundingBox bound;
  object->getBoundingBox(bound);
  object->setIndex(_objects.getSize());
  _objects.add(object, bound);
}

unsigned int Scenery::getNbObject() const
{
  return _objects.getSize();
}

Object* Scenery::getObject(unsigned int i)
{
  return _objects.getElement(i);
}

/**
 * Sources accessors
 */
void Scenery::addSource(Source* source)
{
  //Adding into the octree
  BoundingBox bound;
  source->getBoundingBox(bound);
  _sources.add(source, bound);
}

unsigned int Scenery::getNbSource() const
{
  return _sources.getSize();
}

Source* Scenery::getSource(unsigned int i)
{
  return _sources.getElement(i);
}

/**
 * Cameras accessors
 */
void Scenery::addCamera(Camera* camera)
{
  _cameras.push_back(camera);
}

unsigned int Scenery::getNbCamera() const
{
  return _cameras.size();
}

Camera* Scenery::getCamera(unsigned int i)
{
  return _cameras[i];
}

/**
 * Medium accessors
 */
void Scenery::addMedium(Medium* medium)
{
  _mediums.push_back(medium);
}

/**
 * Texture accessors
 */
void Scenery::addTexture(Texture* texture)
{
	_textures.push_back(texture);
}

unsigned int Scenery::getNbTexture() const
{
  return _textures.size();
}

Texture* Scenery::getTexture(unsigned int i)
{
  return _textures[i];
}

/**
 * Renderer accessors
 */
void Scenery::setRenderer(Renderer* renderer)
{
  if(_renderer!=0)
    delete _renderer;
  _renderer = renderer;
}

Renderer* Scenery::getRenderer()
{
  return _renderer;
}
 
/**
 * Compute the nearest intersection and put the distance of the intersection
 * from ray.start and put the object id into object. Then it return true.
 * If no intersection were found it will return false and distance and object
 * will be undetermined.
 */
bool Scenery::getNearestIntersection(const Ray& ray, Real& distance, Object*& object, Object* startingobject)
{
  SceneryOctreeVisitor visitor(_bias);
  //Initialize visitor
  visitor.init(startingobject);
  
  //Lauch visit process
  _objects.accept(ray, visitor);
  
  //Get result
  distance = visitor.getMinDistance();
  object = visitor.getMinDistanceObject();
  
  return object!=0;
}

/**
 * Compute the nearest intersection and put the distance of the intersection
 * from ray.start and put the object id into object. Then it return true.
 * If no intersection were found it will return false and distance and object
 * will be undetermined.
 */
bool Scenery::getNearestIntersection(const Ray& ray, Real& distance, Object*& object, Basis& localBasis, Point2D& surfaceCoordinate, Object* startingobject)
{
  if(!getNearestIntersection(ray, distance, object, startingobject))
    return false;

  if(object==startingobject)
  {
    Ray tmp = ray;
    tmp.o[0] += _bias*tmp.v[0];
    tmp.o[1] += _bias*tmp.v[1];
    tmp.o[2] += _bias*tmp.v[2];
    object->getLocalBasis(tmp, distance-_bias, localBasis, surfaceCoordinate);
  }
  else
    object->getLocalBasis(ray, distance, localBasis, surfaceCoordinate);    

  return true;
}

/**
 * Compute the nearest intersection and put the distance of the intersection
 * from ray.start and put the object id into object. Then it return true.
 * If no intersection were found it will return false and distance and object
 * will be undetermined.
 */
bool Scenery::getNearestIntersectionWithSource(const Ray& ray, Real& distance, Source*& source, Source* startingsource)
{
  ScenerySourceOctreeVisitor visitor(_bias);
  //Initialize visitor
  visitor.init(startingsource);
  
  //Lauch visit process
  _sources.accept(ray, visitor);
  
  //Get result
  distance = visitor.getMinDistance();
  source = visitor.getMinDistanceSource();
  
  return source!=0;
}

/**
 * Compute the nearest intersection and put the distance of the intersection
 * from ray.start and put the object id into object. Then it return true.
 * If no intersection were found it will return false and distance and object
 * will be undetermined.
 */
bool Scenery::getNearestIntersectionWithSource(const Ray& ray, Real& distance, Source*& source, Basis& localBasis, Point2D& surfaceCoordinate, Source* startingsource)
{
  if(!getNearestIntersectionWithSource(ray, distance, source, startingsource))
    return false;

  if(source==startingsource)
  {
    Ray tmp = ray;
    tmp.o[0] += _bias*tmp.v[0];
    tmp.o[1] += _bias*tmp.v[1];
    tmp.o[2] += _bias*tmp.v[2];
    source->getLocalBasis(tmp, distance-_bias, localBasis, surfaceCoordinate);
  }
  else
    source->getLocalBasis(ray, distance, localBasis, surfaceCoordinate);    

  return true;
}


// -----------------------------------------------------------------------------
// SceneryOctreeVisitor --------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * Constructor
 */
SceneryOctreeVisitor::SceneryOctreeVisitor(Real bias)
{
  _bias=bias;
}

/**
 * Initialize the visitor and errase the old results.
 */
void SceneryOctreeVisitor::init(Object* startingobject)
{
  //Initialize result data
  _distance=-1;
  _object=0;
  _startingobject=startingobject;
}

/**
 * Used by Octree : apply the visitor to the given object (that is able to 
 * intersect the given ray).
 */
void SceneryOctreeVisitor::apply(const Ray& ray, Object*& object)
{
  if(object==_startingobject)
  {
    Ray newray=ray;
    newray.o[0] += _bias*newray.v[0];
    newray.o[1] += _bias*newray.v[1];
    newray.o[2] += _bias*newray.v[2];
    Real distance = -1;
    if(object->intersect(newray, distance) && distance>0.0)
      if(_distance<0 || _distance>distance+_bias)
      {
        _object=object;
        _distance=distance+_bias;
      }    
  }
  else
  {
    Real distance = -1;
    if(object->intersect(ray, distance) && distance>0)
      if(_distance<0 || _distance>distance)
      {
        _distance=distance;
        _object=object;
      }
  }
}

/**
 * Return the minimal intersection distance found at the last visit of the octree.
 */
Real SceneryOctreeVisitor::getMinDistance()
{
  return _distance;
}

/**
 * Return the minimal intersection object found at the last visit of the octree.
 */
Object* SceneryOctreeVisitor::getMinDistanceObject()
{
  return _object;
}

// -----------------------------------------------------------------------------
// ScenerySourceOctreeVisitor --------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * Constructor
 */
ScenerySourceOctreeVisitor::ScenerySourceOctreeVisitor(Real bias)
{
  _bias=bias;
}

/**
 * Initialize the visitor and errase the old results.
 */
void ScenerySourceOctreeVisitor::init(Source* startingsource)
{
  //Initialize result data
  _distance=-1;
  _source=0;
  _startingsource=startingsource;
}

/**
 * Used by Octree : apply the visitor to the given object (that is able to 
 * intersect the given ray).
 */
void ScenerySourceOctreeVisitor::apply(const Ray& ray, Source*& source)
{
  if(source==_startingsource)
  {
    Ray newray=ray;
    newray.o[0] += _bias*newray.v[0];
    newray.o[1] += _bias*newray.v[1];
    newray.o[2] += _bias*newray.v[2];
    Real distance = -1;
    if(source->intersect(newray, distance) && distance>0.0)
      if(_distance<0 || _distance>distance+_bias)
      {
        _source=source;
        _distance=distance+_bias;
      }    
  }
  else
  {
    Real distance = -1;
    if(source->intersect(ray, distance) && distance>0)
      if(_distance<0 || _distance>distance)
      {
        _distance=distance;
        _source=source;
      }
  }
}

/**
 * Return the minimal intersection distance found at the last visit of the octree.
 */
Real ScenerySourceOctreeVisitor::getMinDistance()
{
  return _distance;
}

/**
 * Return the minimal intersection object found at the last visit of the octree.
 */
Source* ScenerySourceOctreeVisitor::getMinDistanceSource()
{
  return _source;
}

