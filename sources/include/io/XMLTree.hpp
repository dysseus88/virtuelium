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

#ifndef _XMLTREE_HPP
#define _XMLTREE_HPP

#include <string>
#include <vector>
#include "structures/HashMap.hpp"
#include "structures/HashFunctors.hpp"

/**
 * This class will contain the information of a XML file as a tree. This is
 * generaly built by the XMLParser. The main function to access this tree are :
 * - getMarkup()
 * - getAttributeValue()
 * - getNumberOfChildren()
 * - getChild()
 * The other methods are principaly used by the XMLParser for contructing the 
 * tree.
 *
 * @author Sylvain "Greewi" Dumazet
 */
class XMLTree{
public :
  /**
   * Create the node for the given markup name.
   * @param markup : the name of the markup that correspond to this node.
   * @param nbAttributes : the maximum number of attributes that will be
   * contained if this node.
   */
  inline XMLTree(const std::string& markup, int nbAttributes = 100);

  /**
   * Destructor
   */
  inline ~XMLTree();

  /**
   * Return the markup label.
   * @return the label of this markup.
   */
  inline std::string getMarkup() const;
  
  /**
   * Add an attribute to this node.
   * @param name : the name of the attribute.
   * @param value : the value of the attribute.
   */
  inline void addAttribute(const std::string& name, const std::string& value);
  
  /**
   * Return the value of an attribute of this node. Return an empty string
   * if this attribute doesn't exist.
   * @param name : the attribute name we want for its value.
   */
  inline std::string getAttributeValue(const std::string& name)const ;
  
  /**
   * Add a child node to this node.
   * @param child : the new child to add.
   */
  inline void addChild(XMLTree* child);

  /**
   * Return the number of children.
   * @return the number of children
   */
  inline unsigned int getNumberOfChildren() const;

  /**
   * Return the i-th child of this node.
   * @param i : the number of the wanted child.
   * @return the i-th child ot this node
   */
  inline XMLTree* getChild(unsigned int i);

  /**
   * Return the i-th child of this node. (constant version)
   * @param i : the number of the wanted child.
   * @return the i-th child ot this node
   */
  inline const XMLTree* getChild(unsigned int i) const;
    
private :
  std::string _markup;
  HashMap<std::string, std::string, StringHashFunctor> _attributes;
  std::vector<XMLTree* > _children;
};

/**
 * Create the node for the given markup name.
 * @param markup : the name of the markup that correspond to this node.
 * @param nbAttributes : the maximum number of attributes that will be
 * contained if this node.
 */
inline XMLTree::XMLTree(const std::string& markup, int nbAttributes)
: _attributes(StringHashFunctor(), nbAttributes)
{
  _markup = markup;
}

/**
 * Destructor
 */
inline XMLTree::~XMLTree()
{
  //Delete all childs
  for(unsigned int i=0; i<_children.size(); i++)
    delete _children[i];
}
  
/**
 * Add an attribute to this node.
 * @param name : the name of the attribute.
 * @param value : the value of the attribute.
 */
inline void XMLTree::addAttribute(const std::string& name, const std::string& value)
{
  _attributes.add(name, value);
}
  
/**
 * Return the value of an attribute of this node. Return an empty string
 * if this attribute doesn't exist.
 * @param name : the attribute name we want for its value.
 */
inline std::string XMLTree::getAttributeValue(const std::string& name)const
{
  return _attributes.get(name);
}
  
/**
 * Add a child node to this node.
 * @param child : the new child to add.
 */
inline void XMLTree::addChild(XMLTree* child)
{
  _children.push_back(child);
}

/**
 * Return the number of children.
 */
inline unsigned int XMLTree::getNumberOfChildren() const
{
  return _children.size();
}

/**
 * Return the i-th child of this node.
 * @param i : the number of the wanted child.
 */
inline XMLTree* XMLTree::getChild(unsigned int i)
{
  if(i>=_children.size())
    return NULL;
  else
    return _children[i];
}

/**
 * Return the i-th child of this node. (constant version)
 * @param i : the number of the wanted child.
 */
inline const XMLTree* XMLTree::getChild(unsigned int i) const
{
  if(i>=_children.size())
    return NULL;
  else
    return _children[i]; 
}

/**
 * Return the markup label.
 */
inline std::string XMLTree::getMarkup() const
{
  return _markup;
}

#endif // _XMLTREE_HPP
