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
 
#include <io/sceneryV2/V2ObjectShapeParser.hpp>
#include <io/mesh/MeshParser.hpp>
#include <io/image/ImageParser.hpp>
#include <exceptions/Exception.hpp>
#include <string>
#include <core/3DBase.hpp>

#include <objectshapes/Sphere.hpp>
#include <objectshapes/Plane.hpp>
#include <objectshapes/Triangle.hpp>
#include <objectshapes/Mesh.hpp>
#include <objectshapes/NormalMap.hpp>
#include <objectshapes/NullGeometry.hpp>
#include <objectshapes/InstanceObjectShape.hpp>

#include <objectshapes/Translation.hpp>
#include <objectshapes/Scale.hpp>
#include <objectshapes/Rotation.hpp>
#include <objectshapes/Transformation.hpp>

HashMap<std::string, ObjectShape*,StringHashFunctor> V2ObjectShapeParser::_shapeLibrary(StringHashFunctor(), 100);

/**
 * Create the object shape by using the informations of the node.
 * node : the XML node to use for the creation.
 */
ObjectShape* V2ObjectShapeParser::create(XMLTree* node)
{

  //Try to get the shape from the library
  std::string name = node->getAttributeValue("name");
  if(name!="" && _shapeLibrary.contain(name))
    return new InstanceObjectShape(_shapeLibrary.get(name));

  //Create the shape
  ObjectShape* shape;
  std::string type = node->getAttributeValue("type");
  if(type=="Null")
    shape = createNull();
  else if(type=="Sphere")
    shape = createSphere(node);
  else if(type=="Plane")
    shape = createPlane(node);
  else if(type=="Triangle")
    shape = createTriangle(node);
  else if(type=="Mesh3")
    shape = createMesh3(node);
  else if(type=="OBJ")
    shape = createOBJ(node);
  else if(type=="Translate")
    shape = createTranslate(node);
  else if(type=="Rotate")
    shape = createRotate(node);
  else if(type=="Scale")
    shape = createScale(node);
  else if(type=="NormalMap")
    shape = createNormalMap(node);
  else
    throw Exception("(V2ObjectShapeParser::create) Type de <shape> " 
                    + type + " inconnu.");

    //Add the shape to the library
  if(name!="")
    _shapeLibrary.add(name, shape);

  //Return the shape
  return shape;
}

/**
 * Create the object shape by using the informations of the node.
 * @return : the parsed ObjectShape
 */
ObjectShape* V2ObjectShapeParser::createNull(void)
{
  return new NullGeometry();
}

/**
 * Create the object shape by using the informations of the node.
 * @param node : the XML node to use for the creation.
 * @return : the parsed ObjectShape
 */
ObjectShape* V2ObjectShapeParser::createSphere(XMLTree* node)
{
  Real radius;
  Point center;
  radius = getRealValue(node, "radius", 0.0);
  center[0] = getRealValue(node, "centerx", 0.0);
  center[1] = getRealValue(node, "centery", 0.0);
  center[2] = getRealValue(node, "centerz", 0.0);

	bool double_sided = getBooleanValue( node, "backface", false);

  return new Sphere(center, radius, double_sided);
}

/**
 * Create the object shape by using the informations of the node.
 * @param node : the XML node to use for the creation.
 * @return : the parsed ObjectShape
 */
ObjectShape* V2ObjectShapeParser::createPlane(XMLTree* node)
{
  Point origin;
  Vector normal, u, v;
  origin[0] = getRealValue(node, "originx", 0.0);
  origin[1] = getRealValue(node, "originy", 0.0);
  origin[2] = getRealValue(node, "originz", 0.0);
  normal[0] = getRealValue(node, "normalx", 0.0);
  normal[1] = getRealValue(node, "normaly", 0.0);
  normal[2] = getRealValue(node, "normalz", 0.0);
  u[0] = getRealValue(node, "ux", 0.0);
  u[1] = getRealValue(node, "uy", 0.0);
  u[2] = getRealValue(node, "uz", 0.0);
  v[0] = getRealValue(node, "vx", 0.0);
  v[1] = getRealValue(node, "vy", 0.0);
  v[2] = getRealValue(node, "vz", 0.0);
  bool finite = getBooleanValue(node, "finite", false);

	bool double_sided = getBooleanValue( node, "backface", false);

  return new Plane(origin, normal, u, v, finite, double_sided); 
}

/**
 * Create the object shape by using the informations of the node.
 * @param node : the XML node to use for the creation.
 * @return : the parsed ObjectShape
 */
ObjectShape* V2ObjectShapeParser::createTriangle(XMLTree* node)
{
  Point vertices[3];
  Vector normals[3];
  Point2D texCoords[3];
  vertices[0][0] = getRealValue(node, "vertex1x", 0.0);
  vertices[0][1] = getRealValue(node, "vertex1y", 0.0);
  vertices[0][2] = getRealValue(node, "vertex1z", 0.0);
  vertices[1][0] = getRealValue(node, "vertex2x", 0.0);
  vertices[1][1] = getRealValue(node, "vertex2y", 0.0);
  vertices[1][2] = getRealValue(node, "vertex2z", 0.0);
  vertices[2][0] = getRealValue(node, "vertex3x", 0.0);
  vertices[2][1] = getRealValue(node, "vertex3y", 0.0);
  vertices[2][2] = getRealValue(node, "vertex3z", 0.0);
  normals[0][0] = getRealValue(node, "normal1x", 0.0);
  normals[0][1] = getRealValue(node, "normal1y", 0.0);
  normals[0][2] = getRealValue(node, "normal1z", 0.0);
  normals[1][0] = getRealValue(node, "normal2x", 0.0);
  normals[1][1] = getRealValue(node, "normal2y", 0.0);
  normals[1][2] = getRealValue(node, "normal2z", 0.0);
  normals[2][0] = getRealValue(node, "normal3x", 0.0);
  normals[2][1] = getRealValue(node, "normal3y", 0.0);
  normals[2][2] = getRealValue(node, "normal3z", 0.0);
  texCoords[0][0] = getRealValue(node, "texture1u", 0.0);
  texCoords[0][1] = getRealValue(node, "texture1v", 0.0);
  texCoords[1][0] = getRealValue(node, "texture2u", 0.0);
  texCoords[1][1] = getRealValue(node, "texture2v", 0.0);
  texCoords[2][0] = getRealValue(node, "texture3u", 0.0);
  texCoords[2][1] = getRealValue(node, "texture3v", 0.0);

	bool double_sided = getBooleanValue( node, "backface", false);

  return new Triangle(vertices, normals, texCoords, double_sided);
}

/**
 * Create the object shape by using the informations of the node.
 * @param node : the XML node to use for the creation.
 * @return : the parsed ObjectShape
 */
ObjectShape* V2ObjectShapeParser::createMesh3(XMLTree* node)
{
  MeshParser parser;
  std::string filename = node->getAttributeValue("file");

	bool double_sided = getBooleanValue( node, "backface", false);

  return parser.loadMesh3(filename, double_sided);
}

/**
 * Create the object shape by using the informations of the node.
 * @param node : the XML node to use for the creation.
 * @return : the parsed ObjectShape
 */
ObjectShape* V2ObjectShapeParser::createOBJ(XMLTree* node)
{
  MeshParser parser;
  std::string filename = node->getAttributeValue("file");

	bool double_sided = getBooleanValue( node, "backface", false);

  return parser.loadOBJ(filename, double_sided);
}

/**
 * Create the object shape by using the informations of the node.
 * @param node : the XML node to use for the creation.
 * @return : the parsed ObjectShape
 */
ObjectShape* V2ObjectShapeParser::createTranslate(XMLTree* node)
{
  if(node->getNumberOfChildren() < 1 
      || node->getChild(0)->getMarkup() != "geometry") {
    throw Exception("(V2ObjectShapeParser::create) Pas de <geometry> a translater.");
  }
  ObjectShape* shape = create(node->getChild(0));

  Vector translation;
  translation[0] = getRealValue(node, "x", 0.0);
  translation[1] = getRealValue(node, "y", 0.0);
  translation[2] = getRealValue(node, "z", 0.0);

  //return new Translation(translation, shape);
  return new Transformation(shape, translation);
}

/**
 * Create the object shape by using the informations of the node.
 * @param node : the XML node to use for the creation.
 * @return : the parsed ObjectShape
 */
ObjectShape* V2ObjectShapeParser::createScale(XMLTree* node)
{
  if(node->getNumberOfChildren() < 1 
      || node->getChild(0)->getMarkup() != "geometry") {
    throw Exception("(V2ObjectShapeParser::create) Pas de <geometry> pour l'homothétie.");
  }
  ObjectShape* shape = create(node->getChild(0));

  Real x, y, z;
  x = getRealValue(node, "x", 1.0);
  y = getRealValue(node, "y", 1.0);
  z = getRealValue(node, "z", 1.0);

  return new Scale(x, y, z, shape);
}
/**
 * Create the object shape by using the informations of the node.
 * @param node : the XML node to use for the creation.
 * @return : the parsed ObjectShape
 */
ObjectShape* V2ObjectShapeParser::createRotate(XMLTree* node)
{
  if(node->getNumberOfChildren()<1 || node->getChild(0)->getMarkup()!="geometry")
    throw Exception("(V2ObjectShapeParser::create) Pas de <geometry> pour la rotation.");
  ObjectShape* shape = create(node->getChild(0));

  char axis;
  if(node->getAttributeValue("axis") == "x")
    axis=0;
  else if(node->getAttributeValue("axis") == "y")
    axis=1;
  else
    axis=2;

  Real angle = getRealValue(node, "angle", 0.0) * M_PI/180.0;

  return new Rotation(axis, angle, shape);

  //Vector axis;
  //axis[0] = getRealValue(node, "x", 0.0);
  //axis[1] = getRealValue(node, "y", 0.0);
  //axis[2] = getRealValue(node, "z", 0.0);

  //Real angle = getRealValue(node, "angle", 0.0) * M_PI / Real(180.0);

  //printf ("\n  axis = (%f, %f, %f) angle = %f\n", axis[0], axis[1], axis[2], angle);
  //
  //return new Transformation(shape, axis, angle);
}

/**
 * Create the object shape by using the informations of the node.
 * @param node : the XML node to use for the creation.
 * @return : the parsed ObjectShape
 */
ObjectShape* V2ObjectShapeParser::createNormalMap(XMLTree* node)
{
  if(node->getNumberOfChildren()<1 || node->getChild(0)->getMarkup()!="geometry")
    throw Exception("(V2ObjectShapeParser::create) Pas de <geometry> pour le mappage de normale.");
  ObjectShape* shape = create(node->getChild(0));

  //Loading image map
  ImageParser imageParser;
  Image* image = imageParser.load(node->getAttributeValue("map"));

  bool global = getBooleanValue(node, "global", true);

  return new NormalMap(shape, image, global);
}

