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
  
#ifndef _SCENERY_HPP
#define _SCENERY_HPP

#include <structures/Octree.hpp>

class Camera;
class Renderer;
class Object;
class Medium;
class Source;
class Texture;

class SceneryOctreeVisitor : public OctreeVisitor<Object*> {
public :
  /**
   * Constructor
   */
  SceneryOctreeVisitor(Real bias);

  /**
   * Initialize the visitor and errase the old results.
   */
  void init(Object* startingobject=0);
  
  /**
   * Used by Octree : apply the visitor to the given object (that is able to 
   * intersect the given ray).
   */
  void apply(const Ray& ray, Object*& object);
  
  /**
   * Return the minimal intersection distance found at the last visit of the octree.
   */
  Real getMinDistance();
  
  /**
   * Return the minimal intersection object found at the last visit of the octree.
   */
  Object* getMinDistanceObject();

private :
  Real _distance;
  Real _bias;
  Object* _object;
  Object* _startingobject;
};

class ScenerySourceOctreeVisitor : public OctreeVisitor<Source*> {
public :
  /**
   * Constructor
   */
  ScenerySourceOctreeVisitor(Real bias);

  /**
   * Initialize the visitor and errase the old results.
   */
  void init(Source* startingsource=0);
  
  /**
   * Used by Octree : apply the visitor to the given object (that is able to 
   * intersect the given ray).
   */
  void apply(const Ray& ray, Source*& source);
  
  /**
   * Return the minimal intersection distance found at the last visit of the octree.
   */
  Real getMinDistance();
  
  /**
   * Return the minimal intersection object found at the last visit of the octree.
   */
  Source* getMinDistanceSource();

private :
  Real _distance;
  Real _bias;
  Source* _source;
  Source* _startingsource;
};

class Scenery{
public :
  /**
   * Constructor. The global bounding box must be set at the first time and must
   * be well sized. No resize are possible.
   * globalbounds : the overall bounding box. Must contain all the object of the scene.
   */
  Scenery(int nbObject, int nbSource, const BoundingBox& globalbounds, const Real& bias);  

  /**
   * Destructor : it doesn't free contained objects !
   */
  ~Scenery();
  
  /**
   * Objects accessors
   */
  void addObject(Object* object);
  unsigned int getNbObject() const;
  Object* getObject(unsigned int i);

  /**
   * Sources accessors
   */
  void addSource(Source* source);
  unsigned int getNbSource() const;
  Source* getSource(unsigned int i);

  /**
   * Cameras accessors
   */
  void addCamera(Camera* camera);
  unsigned int getNbCamera() const;
  Camera* getCamera(unsigned int i);
  
  /**
   * Medium accessors
   */
  void addMedium(Medium* medium);

  /**
   * Renderer accessors
   */
  void setRenderer(Renderer* renderer);
  Renderer* getRenderer();

  /**
   * Texture accessors
   */
  void addTexture(Texture* texture);
  unsigned int getNbTexture() const;
  Texture* getTexture(unsigned int i);

  /**
   * Compute the nearest intersection and put the distance of the intersection
   * from ray.start and put the object id into object. Then it return true.
   * If no intersection were found it will return false and distance and object
   * will be undetermined.
   */
  bool getNearestIntersection(const Ray& ray, Real& distance, Object*& object, Object* startingobject=0);

  /**
   * Compute the nearest intersection and put the distance of the intersection
   * from ray.start and put the object id into object. Then it return true.
   * If no intersection were found it will return false and distance and object
   * will be undetermined.
   */
  bool getNearestIntersection(const Ray& ray, Real& distance, Object*& object, Basis& localBasis, Point2D& surfaceCoordinate, Object* startingobject=0);

  /**
   * Compute the nearest intersection and put the distance of the intersection
   * from ray.start and put the object id into object. Then it return true.
   * If no intersection were found it will return false and distance and object
   * will be undetermined.
   */
  bool getNearestIntersectionWithSource(const Ray& ray, Real& distance, Source*& source, Source* startingsource=0);

  /**
   * Compute the nearest intersection and put the distance of the intersection
   * from ray.start and put the object id into object. Then it return true.
   * If no intersection were found it will return false and distance and object
   * will be undetermined.
   */
  bool getNearestIntersectionWithSource(const Ray& ray, Real& distance, Source*& source, Basis& localBasis, Point2D& surfaceCoordinate, Source* startingsource=0);

private :
  Octree<Object*> _objects;
  Octree<Source*> _sources;
  std::vector<Medium*> _mediums;
	std::vector<Texture*> _textures;
  std::vector<Camera*> _cameras;
  Renderer* _renderer;
  Real _bias;
};

#include <core/Object.hpp>

#include <core/Camera.hpp>
#include <core/Source.hpp>

#include <structures/Medium.hpp>
#include <renderers/Renderer.hpp>

#endif //_SCENERY_HPP
