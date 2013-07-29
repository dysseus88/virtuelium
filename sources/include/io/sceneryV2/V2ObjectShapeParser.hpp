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
#ifndef _V2OBJECTSHAPEPARSER_HPP
#define _V2OBJECTSHAPEPARSER_HPP

#include <io/XMLTree.hpp>
#include <io/Parser.hpp>
#include <objectshapes/ObjectShape.hpp>
#include <structures/HashMap.hpp>
#include <structures/HashFunctors.hpp>

/**
 * This parser load several ObjectShape from a node from a XMLTree of a scenery
 * file.
 * @author Sylavin "Greewi" Dumazet
 */
class V2ObjectShapeParser : public Parser{
public :
  /**
   * Create the object shape by using the informations of the node.
   * @param node : the XML node to use for the creation.
   * @return : the parsed ObjectShape
   */
  ObjectShape* create(XMLTree* node);

private :

  static HashMap<std::string, ObjectShape*, StringHashFunctor> _shapeLibrary;

  /**
   * Create the object shape by using the informations of the node.
   * @return : the parsed ObjectShape
   */
  ObjectShape* createNull(void);

  /**
   * Create the object shape by using the informations of the node.
   * @param node : the XML node to use for the creation.
   * @return : the parsed ObjectShape
   */
  ObjectShape* createSphere(XMLTree* node);

  /**
   * Create the object shape by using the informations of the node.
   * @param node : the XML node to use for the creation.
   * @return : the parsed ObjectShape
   */
  ObjectShape* createPlane(XMLTree* node);

  /**
   * Create the object shape by using the informations of the node.
   * @param node : the XML node to use for the creation.
   * @return : the parsed ObjectShape
   */
  ObjectShape* createTriangle(XMLTree* node);

  /**
   * Create the object shape by using the informations of the node.
   * @param node : the XML node to use for the creation.
   * @return : the parsed ObjectShape
   */
  ObjectShape* createMesh3(XMLTree* node);

  /**
   * Create the object shape by using the informations of the node.
   * @param node : the XML node to use for the creation.
   * @return : the parsed ObjectShape
   */
  ObjectShape* createOBJ(XMLTree* node);

  /**
   * Create the object shape by using the informations of the node.
   * @param node : the XML node to use for the creation.
   * @return : the parsed ObjectShape
   */
  ObjectShape* createTranslate(XMLTree* node);  

  /**
   * Create the object shape by using the informations of the node.
   * @param node : the XML node to use for the creation.
   * @return : the parsed ObjectShape
   */
  ObjectShape* createRotate(XMLTree* node);

  /**
   * Create the object shape by using the informations of the node.
   * @param node : the XML node to use for the creation.
   * @return : the parsed ObjectShape
   */
  ObjectShape* createScale(XMLTree* node);

  /**
   * Create the object shape by using the informations of the node.
   * @param node : the XML node to use for the creation.
   * @return : the parsed ObjectShape
   */
  ObjectShape* createNormalMap(XMLTree* node);
};


#endif //_V2OBJECTSHAPEPARSER_HPP
