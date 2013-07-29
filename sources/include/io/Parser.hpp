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
  
#ifndef _PARSER_HPP
#define _PARSER_HPP

#include <common.hpp>
#include <string>
#include <io/XMLTree.hpp>
#include <cstdlib>

class Parser{
public :
  /**
   * Extract a real value from an XMLTree node.
   * @param node : the node from wich we will extract the value
   * @param value : the name of the attribute that contain the value we want
   * @param defaultvalue : the default value to return if the attribute is not
   *   found.
   * @return : the extracted value if the node contained the attribute of the
   *   default one otherwise. 
   */
  inline static Real getRealValue(XMLTree* node, const std::string& value, Real defaultvalue);

  /**
   * Extract an integer value from an XMLTree node.
   * @param node : the node from wich we will extract the value
   * @param value : the name of the attribute that contain the value we want
   * @param defaultvalue : the default value to return if the attribute is not
   *   found.
   * @return : the extracted value if the node contained the attribute of the
   *   default one otherwise. 
   */
  inline static int getIntegerValue(XMLTree* node, const std::string& value, int defaultvalue);

  /**
   * Extract a boolean value from an XMLTree node.
   * @param node : the node from wich we will extract the value
   * @param value : the name of the attribute that contain the value we want
   * @param defaultvalue : the default value to return if the attribute is not
   *   found.
   * @return : the extracted value if the node contained the attribute of the
   *   default one otherwise. 
   */
  inline bool getBooleanValue(XMLTree* node, const std::string& value, bool defaultvalue);
};

/**
 * Extract a real value from an XMLTree node.
 * @param node : the node from wich we will extract the value
 * @param value : the name of the attribute that contain the value we want
 * @param defaultvalue : the default value to return if the attribute is not
 *   found.
 * @return : the extracted value if the node contained the attribute of the
 *   default one otherwise. 
 */
inline Real Parser::getRealValue(XMLTree* node, const std::string& value, Real defaultvalue)
{
  if(node->getAttributeValue(value)=="")
    return defaultvalue;
  return strtod(node->getAttributeValue(value).c_str(), NULL);
}

/**
 * Extract an integer value from an XMLTree node.
 * @param node : the node from wich we will extract the value
 * @param value : the name of the attribute that contain the value we want
 * @param defaultvalue : the default value to return if the attribute is not
 *   found.
 * @return : the extracted value if the node contained the attribute of the
 *   default one otherwise. 
 */
inline int Parser::getIntegerValue(XMLTree* node, const std::string& value, int defaultvalue)
{
  if(node->getAttributeValue(value)=="")
    return defaultvalue;
  return strtol(node->getAttributeValue(value).c_str(), NULL, 10);
}

/**
 * Extract a boolean value from an XMLTree node.
 * @param node : the node from wich we will extract the value
 * @param value : the name of the attribute that contain the value we want
 * @param defaultvalue : the default value to return if the attribute is not
 *   found.
 * @return : the extracted value if the node contained the attribute of the
 *   default one otherwise. 
 */
inline bool Parser::getBooleanValue(XMLTree* node, const std::string& value, bool defaultvalue)
{
  if(node->getAttributeValue(value)=="")
    return defaultvalue;
  if(   node->getAttributeValue(value)=="true"
     || node->getAttributeValue(value)=="TRUE"
     || node->getAttributeValue(value)=="True"
     || node->getAttributeValue(value)=="1")
    return true;
  return false;
}

#endif //_PARSER_HPP
