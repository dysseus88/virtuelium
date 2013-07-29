/*
*  Copyright 2012 Sylvain "Greewi" Dumazet, Remi Cerise
*
*  This file is part of Virtuelium.
*
*  Virtuelium is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 3 of the License, or
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
*
*/

#ifndef _KDTREE_HPP
#define _KDTREE_HPP

#include <vector>

#include <core/Camera.hpp>

#ifdef _WIN32
	inline double log2( double n )  
	{  
		// log(n)/log(2) is log2.  
		return log( n ) / log( 2.0 );  
	}
#endif

template <typename Element> 
class KdTreeNode
{
public :

  static const unsigned char NORMAL_NODE              = 0;
  static const unsigned char EMPTY_NODE               = 1;
  static const unsigned char MEDIAN_NODE              = 2;
  static const unsigned char GREATER_THAN_MEDIAN_NODE = 3;

  /**
   * Default contructor
   */
  KdTreeNode();

  /**
   * Constructor using an element.
   */
  KdTreeNode(const Element& element);

  /**
   * Return the position of the element contained. The Element type must have a
   * "position" public field !
   */
  const Point& getPosition() const;

  /**
   * Return the element contained.
   */
  const Element& getElement() const;
  Element& getElement();

  /**
   * Return the flag of this node.
   */
  char getFlag() const;

  /**
   * Set the flag of this node.
   */
  void setFlag(char flag);

private :
  Element _element;
  char    _flag;  
};

template <typename Element>
class DimensionElementComparator{
  int _dim;
public :  
  /**
   * Set the dimension to compare
   */
  void setDimension(int dim);
  
  /**
   * Comparation function
   */
  bool operator()(const KdTreeNode<Element>* e1, const KdTreeNode<Element>* e2);
};

template <typename Element>
class DistanceElementComparator{
  Point _refPoint;
public :  
  
  /**
   * Set the refrence point
   */
  void setReferencePoint(const Point& refPoint);

  /**
   * Comparation function
   */
  bool operator()(const Element* e1, const Element* e2);
};

/**
 * 
 */
template <typename Element>
class KdTree{
public :

  /**
   * Default constructor
   */
  KdTree();

  /**
   * Put the element into the KdTree. This must be done before optimizing the 
   * tree !
   */
  void add(const Element& element);

  /**
   * Optimize the photon map. This will enable fast query in the photon map.
   * Do this only once and before any call to getNearestNeighbor() !
   * Complexity : O(n.log(n))
   */
  void optimize();

  /**
   * Initialize the kd-tree data with the given data.
   */
  void setData(Element* elements, unsigned int nbElements);

  /**
   * Export the data of the kd-tree from this data.
   */
  void getData(Element* elements);

  /**
   * Return the number of elements contained.
   */
  unsigned int getSize();

  /**
   * Get the nearest element from the given point.
   * @param origin : the point from which we search the nearest element.
   */
  Element* getNearestElement(const Point& origin);

  /**
   * Get the nearests elements of the origin point that lies into the search
   * radius and place them into the neighbors vector.
   * Optimize the KdTree before using this method !
   */
  Real getNearestNeighbor(const Point& origin, unsigned int neighbor, const Real& searchRadius , std::vector<Element*>& neighbors);

 /**
  * Get the nearests elements of the origin point that lies into the search
  * radius and place them into the neighbors vector.
  * Optimize the KdTree before using this method !
  */
  Real getNearestNeighbor(const Point& origin, unsigned int neighbor, const Real& searchRadius, const Vector& normal, std::vector<Element*>& neighbors);

private :
  std::vector<KdTreeNode<Element> > _nodes;

  /**
   * Return at max max_size element that are located into the disc centered on 
   * origin and that have a radius of the search radius. Elements founds are 
   * placed into the neighbors vector. This function return true if all elements
   * of the area have been place into the vector, false if there were more than
   * max_size elements in this area.
   */
  bool getElementWithinRadius(const Point& origin, Real searchRadius, unsigned int max_size, std::vector<Element*>& neighbors, unsigned int node, int dim);

  /**
   * Return at max max_size element that are located into the disc centered on 
   * origin and that have a radius of the search radius. Elements founds are 
   * placed into the neighbors vector. This function return true if all elements
   * of the area have been place into the vector, false if there were more than
   * max_size elements in this area.
   */
  bool getElementWithinRadius(const Point& origin, Real searchRadius, const Vector& normal, unsigned int max_size, std::vector<Element*>& neighbors, unsigned int node, int dim);

  /**
   * Build the array for the optimization.
   * Complexity : O(n)
   */
  void fillArrays(KdTreeNode<Element>** dimArray, KdTreeNode<Element>** linkArray, unsigned int capacity);

  /**
   * Sort the array for the optimization.
   * Complexity : O(n.log(n))
   */
  void sortArrays(KdTreeNode<Element>** dimArray);
  
  /**
   * Rearange the array from bip to bsq.
   * Complexity : O(n)
   */
  void rearrangeArrays(KdTreeNode<Element>** dimArray);

  /**
   * Search the median of the given array and mark all photon that are higher
   * Complexity : O(size)
   */
  KdTreeNode<Element>* searchMedian(KdTreeNode<Element>** dimArray, unsigned int size, int dim);

  /**
   * Split the array and put all the value marked in the second table
   * Complexity : O(n)
   */
  void splitArrays(KdTreeNode<Element>** dimArray, unsigned int* size, KdTreeNode<Element>*** upDimArray, unsigned int* upsize);

  /**
   * Create balanced sub-kd-tree in linkArray that should have his root at node.
   * Complexity : O(n.log(n))
   */
  void balanceTree(KdTreeNode<Element>** dimArray, unsigned int size, KdTreeNode<Element>** linkArray, int dim, int node);
};

//------------------------------------------------------------------------------
// KdTreeNode
//------------------------------------------------------------------------------
/**
 * Default constructor
 */
template <typename Element>
KdTreeNode<Element>::KdTreeNode()
{
  _flag = KdTreeNode::NORMAL_NODE;
}


/**
 * Constructor using an element.
 */
template <typename Element>
KdTreeNode<Element>::KdTreeNode(const Element& element)
{
  _element = element;
  _flag = KdTreeNode::NORMAL_NODE;
}

/**
 * Return the position of the element contained. The Element type must have a
 * "position" public field !
 */
template <typename Element>
const Point& KdTreeNode<Element>::getPosition() const
{
  return _element.position;
}

/**
 * Return the element contained.
 */
template <typename Element>
const Element& KdTreeNode<Element>::getElement() const
{
  return _element;  
}

/**
 * Return the element contained.
 */
template <typename Element>
Element& KdTreeNode<Element>::getElement()
{
  return _element;  
}

/**
 * Return the flag of this node.
 */
template <typename Element>
char KdTreeNode<Element>::getFlag() const
{
  return _flag;
}

/**
 * Set the flag of this node.
 */
template <typename Element>
void KdTreeNode<Element>::setFlag(char flag)
{
  _flag=flag;
}

//------------------------------------------------------------------------------
// DimensionElementComparator
//------------------------------------------------------------------------------

/**
 * Set the dimention to compare
 */
template <typename Element>
void DimensionElementComparator<Element>::setDimension(int dim)
{
  _dim = dim;
}
  
/**
 * Comparation function
 */
template <typename Element>
bool DimensionElementComparator<Element>::operator()(const KdTreeNode<Element>* e1, const KdTreeNode<Element>* e2)
{
  return e2->getFlag()==KdTreeNode<Element>::EMPTY_NODE
      || e1->getElement().position[_dim] < e2->getElement().position[_dim];
}

//------------------------------------------------------------------------------
// DistanceElementComparator
//------------------------------------------------------------------------------

/**
 * Set the refrence point
 */
template <typename Element>
void DistanceElementComparator<Element>::setReferencePoint(const Point& refPoint)
{
  _refPoint=refPoint;
}

/**
 * Comparation function
 */
template <typename Element>
bool DistanceElementComparator<Element>::operator()(const Element* e1, const Element* e2)
{
  Real d1 = (e1->position[0]-_refPoint[0])*(e1->position[0]-_refPoint[0])
          + (e1->position[1]-_refPoint[1])*(e1->position[1]-_refPoint[1])
          + (e1->position[2]-_refPoint[2])*(e1->position[2]-_refPoint[2]);

  Real d2 = (e2->position[0]-_refPoint[0])*(e2->position[0]-_refPoint[0])
          + (e2->position[1]-_refPoint[1])*(e2->position[1]-_refPoint[1])
          + (e2->position[2]-_refPoint[2])*(e2->position[2]-_refPoint[2]);

  return d1<d2;
}

//------------------------------------------------------------------------------
// KdTree
//------------------------------------------------------------------------------

/**
 * Default constructor
 */
template <typename Element>
KdTree<Element>::KdTree()
{
  _nodes.clear();
}

/**
 * Put the element into the KdTree. This must be done before optimizing the 
 * tree !
 */
template <typename Element>
void KdTree<Element>::add(const Element& element)
{
  _nodes.push_back(KdTreeNode<Element>(element));
}

/**
 * Build the array for the optimization.
 * Complexity : O(n)
 */
template <typename Element>
void KdTree<Element>::fillArrays(KdTreeNode<Element>** dimArray, KdTreeNode<Element>** linkArray, unsigned int capacity)
{
  for(unsigned int i=0 ; i<_nodes.size() ; i++)
  {
    //Filling dimmentions array
    for(unsigned int k=0; k<3; k++)
      dimArray[i + k*_nodes.size()] = &(_nodes[i]);
  }
  for(unsigned int i=0; i<capacity; i++)
    linkArray[i]=NULL;
}

/**
 * Sort the array for the optimization.
 * Complexity : O(n.log(n))
 */
template <typename Element>
void KdTree<Element>::sortArrays(KdTreeNode<Element>** dimArray)
{
  DimensionElementComparator<Element> comparator;
  
  //Sorting dimentions arrays
  for(int k=0 ; k<3 ; k++)
  {
    comparator.setDimension(k);
    std::sort(&(dimArray[k*_nodes.size()]), &(dimArray[k*_nodes.size()])+_nodes.size(), comparator);
  }
}

/**
 * Rearange the array from bip to bsq.
 * Complexity : O(n)
 */
template <typename Element>
void KdTree<Element>::rearrangeArrays(KdTreeNode<Element>** dimArray)
{
  KdTreeNode<Element>** tmp = new KdTreeNode<Element>*[_nodes.size()*3];
  memcpy(tmp, dimArray, sizeof(KdTreeNode<Element>*)*_nodes.size()*3);
  for(unsigned int i=0 ; i< _nodes.size() ; i++)
    for(unsigned int k=0 ; k<3 ; k++)
      dimArray[i*3 + k] = tmp[i+k*_nodes.size()];
  delete[] tmp;
}

/**
 * Search the median of the given array and mark all photon that are higher
 * Complexity : O(size)
 */
template <typename Element>
KdTreeNode<Element>* KdTree<Element>::searchMedian(KdTreeNode<Element>** dimArray, unsigned int size, int dim)
{
  //Verifying that the array is not empty
  if(size==0)
    return NULL;
  
  //Getting the median
  KdTreeNode<Element>* median = dimArray[(size/2)*3 + dim];
  median->setFlag(KdTreeNode<Element>::MEDIAN_NODE);
  
  //Marking all 
  for(unsigned int i=(size/2)+1; i<size; i++)
    dimArray[i*3 + dim]->setFlag(KdTreeNode<Element>::GREATER_THAN_MEDIAN_NODE);
  
  return median;
}

/**
 * Split the array and put all the value marked in the second table
 * Complexity : O(n)
 */
template <typename Element>
void KdTree<Element>::splitArrays(KdTreeNode<Element>** dimArray, unsigned int* size, KdTreeNode<Element>*** upDimArray, unsigned int* upsize)
{ 
  //Creating temporary variables
  KdTreeNode<Element> ** tmp = new KdTreeNode<Element>*[((*size)/2+1)*3];
  
  //Initalizing size of sub-tables
  unsigned int size_down[3] = {0, 0, 0};
  unsigned int size_up[3]   = {0, 0, 0};
  
  //Filling sub-table
  for(unsigned int i=0 ; i<*size ; i++)
    for(unsigned int k=0 ; k<3; k++)
    {
      KdTreeNode<Element>* element = dimArray[i*3 + k];
      if(element->getFlag() != KdTreeNode<Element>::MEDIAN_NODE)
      {
        if(element->getFlag() == KdTreeNode<Element>::GREATER_THAN_MEDIAN_NODE)
        {
          tmp[size_up[k]*3 + k]=element;
          size_up[k]++;
        }
        else
        {
          dimArray[size_down[k]*3 + k]=element;
          size_down[k]++;
        }
      }
    }

  //Reset all GREATER_THAN_MEDIAN_NODE markers
  for(unsigned int i=0; i<size_up[0]; i++)
    tmp[i*3]->setFlag(KdTreeNode<Element>::NORMAL_NODE);
    
  //Putting the temporary subtable after the first and free it
  memcpy(&dimArray[size_down[0]*3], tmp, sizeof(KdTreeNode<Element>*)*size_up[0]*3);
  delete[] tmp;

  //Update sizes
  *size=size_down[0];
  *upsize=size_up[0];  
  *upDimArray = &dimArray[size_down[0]*3];
}

/**
 * Create balanced sub-kd-tree in linkArray that should have his root at node.
 * Complexity : O(n.log(n))
 */
template <typename Element>
void KdTree<Element>::balanceTree(KdTreeNode<Element>** dimArray, unsigned int size, KdTreeNode<Element>** linkArray, int dim, int node)
{
  // Stoping condition we are in a empty node.
  if(size==0)
    return;
  
  //Setting median as the root of the subtree
  linkArray[node]=searchMedian(dimArray, size, dim);
  
  //Splitting the array
  KdTreeNode<Element>** upDimArray;
  unsigned int upsize;
  splitArrays(dimArray, &size, &upDimArray, &upsize);
  
  //Compute right and left child
  balanceTree(dimArray, size, linkArray, (dim+1)%3, (node+1)*2 - 1 );
  balanceTree(upDimArray, upsize, linkArray, (dim+1)%3, (node+1)*2 );
}

/**
 * Optimize the photon map. This will enable fast query in the photon map.
 * Do this only once and before any call to getNearestNeighbor() !
 * Complexity : O(n.log(n))
 */
template <typename Element>
void KdTree<Element>::optimize()
{
  //Computing the capacity in order to be able to store the tree
  //There is 2^(h+1)-1 node in an balanced complete tree, with h the height of
  //the tree. So the height h = log2 (n+1) -1 with n the number of nodes
  int height = (int)ceil(log2(_nodes.size() + 1.0)) - 1;
  unsigned int capacity = ( 1<<(height+1) ) -1; // 1<<x is equivalent to 2^x
  
  //Build arrays
  KdTreeNode<Element>** dimArray = new KdTreeNode<Element>*[_nodes.size()*3];
  KdTreeNode<Element>** linkArray = new KdTreeNode<Element>*[capacity];
  fillArrays(dimArray, linkArray, capacity);

  //Sort arrays
  sortArrays(dimArray);
  rearrangeArrays(dimArray);

  //Create balanced tree
  balanceTree(dimArray, _nodes.size(), linkArray, 0, 0);

  //We dont need dimArray any more and we need to build a second tamporary map so delete[]
  delete[] dimArray;
  
  //Filling the new map
  std::vector<KdTreeNode<Element> > tmp(capacity);
  for(unsigned int i=0 ; i<capacity; i++)
    if(linkArray[i]!=NULL)
      tmp[i]=*linkArray[i];
    else
      tmp[i].setFlag(KdTreeNode<Element>::EMPTY_NODE);
    
  //Free temporary memory and rebind the map
  delete[] linkArray;
  _nodes=tmp;
}

/**
 * Get the nearest element from the given point.
 * @param origin : the point from which we search the nearest element.
 */
template <typename Element>
Element* KdTree<Element>::getNearestElement(const Point& origin)
{
  if(_nodes.size()==0)
    return NULL;
  
  unsigned node = 0;
  Element* element = &_nodes[node].getElement();
  Real d2 = (origin[0] - element->position[0])*(origin[0] - element->position[0])
          + (origin[1] - element->position[1])*(origin[1] - element->position[1])
          + (origin[2] - element->position[2])*(origin[2] - element->position[2]);
  
  int dim=0;
  while(node < _nodes.size())
  {
    Real tmpd2 = (origin[0] - _nodes[node].getPosition()[0])*(origin[0] - _nodes[node].getPosition()[0])
               + (origin[1] - _nodes[node].getPosition()[1])*(origin[1] - _nodes[node].getPosition()[1])
               + (origin[2] - _nodes[node].getPosition()[2])*(origin[2] - _nodes[node].getPosition()[2]);
    if(tmpd2 < d2)
    {
      d2 = tmpd2;
      element = &_nodes[node].getElement();
    }
    
    if(origin[dim]<_nodes[node].getPosition()[dim])
      node = node*2 + 1;
    else
      node = node*2 + 2;
    
    dim = (dim+1)%3;
  }
  
  return element;
}

/**
 * Get the nearests elements of the origin point that lies into the search
 * radius and place them into the neighbors vector.
 * Optimize the KdTree before using this method !
 */
template <typename Element>
Real KdTree<Element>::getNearestNeighbor(const Point& origin, unsigned int neighborsNb, const Real& searchRadius , std::vector<Element*>& neighbors)
{
  //We will do a dicotomic search using the radius of the searching area.
  Real maxRadius = searchRadius*2.0;
  Real curRadius = searchRadius;
  Real minRadius = 0;

  //Will the area have too many or too few elements we continue to search
  int i=0;
  while(!getElementWithinRadius(origin, curRadius, neighborsNb*2, neighbors, 0, 0) || neighbors.size()<neighborsNb)
  {
    ++i;
    if(i >1000)
      break; 

    //We will abort search if there is too few elements int the area.
    if(curRadius > searchRadius)
      break;

    //If we get too few element enlage the radius
    if(neighbors.size()<neighborsNb)
    {
      minRadius = curRadius;
      curRadius = (curRadius+maxRadius)*0.5;
    }
    //If we get too many element reduce the radius
    else
    {
      maxRadius = curRadius;
      curRadius = (curRadius+minRadius)*0.5;
    }

    //Reset the search
    neighbors.clear();
  }

  //Sort and keep nearest elements
  DistanceElementComparator<Element> cmp;
  cmp.setReferencePoint(origin);
  std::sort(neighbors.begin(), neighbors.end(), cmp);
  if(neighbors.size() > neighborsNb)
    neighbors.resize(neighborsNb);


  //Return the square radius that effectively contain theses elements.
  if(neighbors.size() > 0)
  {
    const Point& farthest = neighbors[neighbors.size()-1]->position;
    Real radius = (farthest[0]-origin[0])*(farthest[0]-origin[0])
                + (farthest[1]-origin[1])*(farthest[1]-origin[1])
                + (farthest[2]-origin[2])*(farthest[2]-origin[2]);
    return radius;
  }
  else
    return 1.0;
}

 /**
  * Get the nearests elements of the origin point that lies into the search
  * radius and place them into the neighbors vector.
  * Optimize the KdTree before using this method !
  */
template <typename Element>
Real KdTree<Element>:: getNearestNeighbor(const Point& origin, unsigned int neighborsNb, const Real& searchRadius, const Vector& normal, std::vector<Element*>& neighbors)
{
  //We will do a dicotomic search using the radius of the searching area.
  Real maxRadius = searchRadius*2.0;
  Real curRadius = searchRadius;
  Real minRadius = 0;

  //Will the area have too many or too few elements we continue to search
  int iter = 0;
  while(!getElementWithinRadius(origin, curRadius, normal, neighborsNb*2, neighbors, 0, 0) || neighbors.size()<neighborsNb)
  {
    //We will abort search if there is too few elements int the area.
    if(curRadius > searchRadius)
      break;

    if(iter++ > 20)
    {
      neighbors.clear();
      return 0.0;
    }

    //If we get too few element enlage the radius
    if(neighbors.size()<neighborsNb)
    {
      minRadius = curRadius;
      curRadius = (curRadius+maxRadius)*0.5;
    }
    //If we get too many element reduce the radius
    else
    {
      maxRadius = curRadius;
      curRadius = (curRadius+minRadius)*0.5;
    }

    //Reset the search
    neighbors.clear();
  }

  //Sort and keep nearest elements
  DistanceElementComparator<Element> cmp;
  cmp.setReferencePoint(origin);
  std::sort(neighbors.begin(), neighbors.end(), cmp);
  if(neighbors.size() > neighborsNb)
    neighbors.resize(neighborsNb);


  //Return the square radius that effectively contain theses elements.
  if(neighbors.size() > 0)
  {
    const Point& farthest = neighbors[neighbors.size()-1]->position;
    Real radius = (farthest[0]-origin[0])*(farthest[0]-origin[0])
                + (farthest[1]-origin[1])*(farthest[1]-origin[1])
                + (farthest[2]-origin[2])*(farthest[2]-origin[2]);
    return radius;
  }
  else
    return 0.0;
}

/**
 * Return at max max_size element that are located into the disc centered on 
 * origin and that have a radius of the search radius. Elements founds are 
 * placed into the neighbors vector. This function return true if all elements
 * of the area have been place into the vector, false if there were more than
 * max_size elements in this area.
 */
template <typename Element>
bool KdTree<Element>::getElementWithinRadius(const Point& origin, Real searchRadius, unsigned int max_size, std::vector<Element*>& neighbors, unsigned int node, int dim)
{
  //Verify that the node is valid
  if(node >= _nodes.size() || _nodes[node].getFlag()==KdTreeNode<Element>::EMPTY_NODE)
    return true;

  //Verify that we have not too many selected elements
  if(neighbors.size() >= max_size)
    return false;

  Real dist = origin[dim] - _nodes[node].getPosition()[dim];
  if(dist<0) //Position is left to this node
  {
    if(!getElementWithinRadius(origin, searchRadius, max_size, neighbors, node*2 + 1, (dim+1)%3))
      return false;
    if(dist*dist< searchRadius*searchRadius && !getElementWithinRadius(origin, searchRadius, max_size, neighbors, node*2 + 2, (dim+1)%3))
      return false;
  }
  else //Position is right to this node
  {
    if(!getElementWithinRadius(origin, searchRadius, max_size, neighbors, node*2 + 2, (dim+1)%3))
      return false;
    if(dist*dist< searchRadius*searchRadius && !getElementWithinRadius(origin, searchRadius, max_size, neighbors, node*2 + 1, (dim+1)%3))
      return false;
  }
  
  //Compute distance beetewn the element and the position
  dist = 0.0;
  for(int i=0; i<3; i++)
    dist+= (origin[i]-_nodes[node].getPosition()[i])*(origin[i]-_nodes[node].getPosition()[i]);
  
  //Put element if it is into the search radius
  if(dist<searchRadius*searchRadius)
    neighbors.push_back(&(_nodes[node].getElement()));
  
  return true;
}

  /**
   * Return at max max_size element that are located into the disc centered on 
   * origin and that have a radius of the search radius. Elements founds are 
   * placed into the neighbors vector. This function return true if all elements
   * of the area have been place into the vector, false if there were more than
   * max_size elements in this area.
   */
template <typename Element>
bool KdTree<Element>::getElementWithinRadius(const Point& origin, Real searchRadius, const Vector& normal, unsigned int max_size, std::vector<Element*>& neighbors, unsigned int node, int dim)
{
  //Verify that the node is valid
  if(node >= _nodes.size() || _nodes[node].getFlag()==KdTreeNode<Element>::EMPTY_NODE)
    return true;

  //Verify that we have not too many selected elements
  if(neighbors.size() >= max_size)
    return false;

  Real dist = origin[dim] - _nodes[node].getPosition()[dim];
  if(dist<0) //Position is left to this node
  {
    if(!getElementWithinRadius(origin, searchRadius, normal, max_size, neighbors, node*2 + 1, (dim+1)%3))
      return false;
    if(dist*dist< searchRadius*searchRadius && !getElementWithinRadius(origin, searchRadius, normal, max_size, neighbors, node*2 + 2, (dim+1)%3))
      return false;
  }
  else //Position is right to this node
  {
    if(!getElementWithinRadius(origin, searchRadius, normal, max_size, neighbors, node*2 + 2, (dim+1)%3))
      return false;
    if(dist*dist< searchRadius*searchRadius && !getElementWithinRadius(origin, searchRadius, normal, max_size, neighbors, node*2 + 1, (dim+1)%3))
      return false;
  }
  
  //Compute distance beetewn the element and the position
  Real d2 = 0.0;
  Real x2 = 0.0;
  Real y2 = 0.0;
  for(int i=0; i<3; i++)
  {
    d2 += (origin[i]-_nodes[node].getPosition()[i])*(origin[i]-_nodes[node].getPosition()[i]);
    y2 += (_nodes[node].getPosition()[i]-origin[i])*normal[i];
  }

  y2 = y2;
  x2 = d2 - y2;

  //Put element if it is into the search radius
  if(d2 < searchRadius*searchRadius && y2/x2 < 0.25)
    neighbors.push_back(&(_nodes[node].getElement()));
  
  return true;
}

/**
 * Initialize the kd-tree data with the given data.
 */
template <typename Element>
void KdTree<Element>::setData(Element* elements, unsigned int nbElements)
{
  _nodes.clear();
  for(unsigned int i=0; i<nbElements; i++)
    _nodes.push_back(KdTreeNode<Element>(elements[i]));
}

/**
 * Export the data of the kd-tree from this data.
 */
template <typename Element>
void KdTree<Element>::getData(Element* elements)
{
  for(unsigned int i=0; i<_nodes.size(); i++)
  {
    elements[i]=_nodes[i].getElement();
    if(_nodes[i].getFlag()==KdTreeNode<Element>::EMPTY_NODE)
      elements[i].position[0]=-1000000;
  }
}

/**
 * Return the number of elements contained.
 */
template <typename Element>
unsigned int KdTree<Element>::getSize()
{
  return _nodes.size();
}

#endif //_KDTREE_HPP
