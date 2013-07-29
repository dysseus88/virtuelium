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
 
#ifndef _OCTREE_HPP
#define _OCTREE_HPP

#include <vector>

#include "exceptions/Exception.hpp"

#include <core/Camera.hpp>

template <typename Element>
class OctreeElement{
public :
  Element element;    // Element stored in the octree
  unsigned int elementNumber;  // number of this element in the elements arrays
};

template <typename Element>
class OctreeVisitor{
public :
  virtual ~OctreeVisitor();
  virtual void apply(const Ray& ray, Element& element)=0;
};

//------------------------------------------------------------------------------
// OctreeVisitor ------------------------------------------------------------------
//------------------------------------------------------------------------------

template <typename Element> 
OctreeVisitor<Element>::~OctreeVisitor()
{
  //Nothing to do
}


template <typename Element>
class OctreeNode{  
public :

  /**
   * Default constructor
   */
  OctreeNode();

  /**
   * Initialize the child
   */
  void init(const BoundingBox& bound);

  /**
   * Destructor
   */
  ~OctreeNode();

  /**
   * Insert an element into the node.
   */
  void insert(OctreeElement<Element>* element, BoundingBox bound, int maxDepth);

  /**
   * Test if the given ray can intersect the tree node. It will not say if a 
   * contained element will intersect, but if it say that the ray can't intersect
   * no element contained by this node (If an element is contained by two or more
   * nodes it can intersect in one of the others nodes).
   */
  bool canIntersect(const Ray& ray) const;
  
  /**
   * Apply the visitor on all the element that had a chance to intersect the ray.
   * It include child of this node, and the visitor will not be applied twice on
   * the same object.
   */
  void accept(const Ray& ray, OctreeVisitor<Element>& visitor) const;

private :
  BoundingBox _bound; // Boundary of this node
  OctreeNode* _childrens; // childrens of this node. NULL -> no child
  std::vector<OctreeElement<Element>* > _elements; // Elements stored in this node
};

//------------------------------------------------------------------------------
// OctreeNode ------------------------------------------------------------------
//------------------------------------------------------------------------------

/**
 * Default constructor
 */
template <typename Element>
OctreeNode<Element>::OctreeNode()
{
  _childrens=0;
}

/**
 * Initialize the child
 */
template <typename Element>
void OctreeNode<Element>::init(const BoundingBox& bound)
{
  _bound=bound;
}

/**
 * Destructor
 */
template <typename Element>
OctreeNode<Element>::~OctreeNode()
{
  if(_childrens!=0)
    delete[] _childrens;
}

/**
 * Insert an element into the node.
 */
template <typename Element>
void OctreeNode<Element>::insert(OctreeElement<Element>* element, BoundingBox bound, int maxDepth)
{
  //Verify that this element can be inserted in this node
  for(unsigned int i=0; i<3; i++)
    if(bound.max[i] < _bound.min[i] || bound.min[i] > _bound.max[i])
      return;


  //If max depth is reached insert it (no more child can be created)
  if(maxDepth==0)
  {
    _elements.push_back(element);
    return;
  }
  
  //If there is no childrens
  if(_childrens==0)
  {
    _elements.push_back(element);

    //If too many elements are stored in this node subdivise the node
    if(_elements.size()>4)
    {
      //Create childrenss node
      _childrens = new OctreeNode<Element>[8];
      for(unsigned int i=0; i<2; i++)
      for(unsigned int j=0; j<2; j++)
      for(unsigned int k=0; k<2; k++)
      {
        //Compute child bounding box
        BoundingBox child_bound;
        child_bound.min[0]=((i==0)? (_bound.min[0]) : ((_bound.min[0]+_bound.max[0])*0.5));
        child_bound.min[1]=((j==0)? (_bound.min[1]) : ((_bound.min[1]+_bound.max[1])*0.5));
        child_bound.min[2]=((k==0)? (_bound.min[2]) : ((_bound.min[2]+_bound.max[2])*0.5));
        child_bound.max[0]=((i==1)? (_bound.max[0]) : ((_bound.min[0]+_bound.max[0])*0.5));
        child_bound.max[1]=((j==1)? (_bound.max[1]) : ((_bound.min[1]+_bound.max[1])*0.5));
        child_bound.max[2]=((k==1)? (_bound.max[2]) : ((_bound.min[2]+_bound.max[2])*0.5));
        //Init the child
        _childrens[i + j*2 + k*4].init(child_bound);
      }
    }
  }
  else
  {
    //Try to insert the element into the childrenss
    for(int i=0; i<8; i++)
      _childrens[i].insert(element, bound, maxDepth-1);
  }
  
}

/**
 * Test if the given ray can intersect the tree node. It will not say if a 
 * contained element will intersect, but if it say that the ray can't intersect
 * no element contained by this node (If an element is contained by two or more
 * nodes it can intersect in one of the others nodes).
 */
template <typename Element>
bool OctreeNode<Element>::canIntersect(const Ray& ray) const
{
  return _bound.canIntersect(ray);
}

/**
 * Apply the visitor on all the element that had a chance to intersect the ray.
 * It include child of this node, and the visitor will not be applied twice on
 * the same object.
 */
template <typename Element>
void OctreeNode<Element>::accept(const Ray& ray, OctreeVisitor<Element>& visitor) const
{
  //Test if contained element can be intersect the ray
  if(!canIntersect(ray))
    return;

  //Apply visitor
  for(unsigned int i=0; i<_elements.size(); i++)
    visitor.apply(ray, _elements[i]->element);
  
  //Recurse on the childrens
  if(_childrens!=0)
    for(int i=0; i<8; i++)
      _childrens[i].accept(ray, visitor);
}

template <typename Element>
class Octree{
public :
  /**
   * Constructor of empty tree
   */
  Octree(BoundingBox bound, int maxElement, int maxDepth=4);

  /**
   * Destructor
   */
  ~Octree();

  /**
   * Add an element into the tree
   */
  void add(Element& element, BoundingBox bound);

  /**
   * Get the number of element contained in the octree
   */
  unsigned int getSize() const;
  
  /**
   * Return the i-th element.
   */
  const Element& getElement(unsigned int i) const;

  /**
   * Apply the visitor on all the element that had a chance to intersect the ray.
   * The visitor will not be applied twice on the same object.
   */
  void accept(const Ray& ray, OctreeVisitor<Element>& visitor) const;

private :
  //Element storing
  OctreeElement<Element>* _elements; // All stored elements
  unsigned int _capacity;
  unsigned int _size;
  
  //Tree managment
  OctreeNode<Element>* _root;                     // Root of the tree
  int _maxDepth;
};

//------------------------------------------------------------------------------
// Octree ----------------------------------------------------------------------
//------------------------------------------------------------------------------
/**
 * Constructor of empty tree
 */
template <typename Element>
Octree<Element>::Octree(BoundingBox bound, int maxElements, int maxDepth)
{
  _maxDepth=maxDepth;
  _root=new OctreeNode<Element>();
  _root->init(bound);
  _elements = new OctreeElement<Element>[maxElements];
  _capacity=maxElements;
  _size=0;
}

/**
 * Destructor
 */
template <typename Element>
Octree<Element>::~Octree()
{
  delete _root;
  delete[] _elements;
}

/**
 * Add an element into the tree
 */
template <typename Element>
void Octree<Element>::add(Element& element, BoundingBox bound)
{
  //Verify that the octree is not full
  if(_size==_capacity)
    throw Exception("(Octree<Element>::add) Octree plein.");

  //Add element into the Octree elements
  _elements[_size].element=element;
  _elements[_size].elementNumber=_size;
  _size++;
  
  //Insert element into the tree
  _root->insert(&_elements[_size-1], bound, _maxDepth);
}

/**
 * Get the number of element contained in the octree
 */
template <typename Element>
unsigned int Octree<Element>::getSize() const
{
  return _size;
}

/**
 * Return the i-th element.
 */
template <typename Element>
const Element& Octree<Element>::getElement(unsigned int i) const
{
  return _elements[i].element;
}

/**
 * Apply the visitor on all the element that had a chance to intersect the ray.
 * The visitor will not be applied twice on the same object.
 */
template <typename Element>
void Octree<Element>::accept(const Ray& ray, OctreeVisitor<Element>& visitor) const
{
  //Is there any node ?
  if(_root==NULL)
    return;

  //visit the tree
  _root->accept(ray, visitor);
}

#endif /* _OCTREE_HPP */
