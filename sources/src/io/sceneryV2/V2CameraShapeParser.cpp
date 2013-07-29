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
#include <io/sceneryV2/V2CameraShapeParser.hpp>
#include <exceptions/Exception.hpp>
#include <string>
#include <core/3DBase.hpp>

#include <camerashapes/PerspectiveCameraShape.hpp>
#include <camerashapes/FishEyeCameraShape.hpp>
#include <camerashapes/PolarCameraShape.hpp>
#include <camerashapes/OrthoscopicCameraShape.hpp>

/**
 * Create the object shape by using the informations of the node.
 * node : the XML node to use for the creation.
 */
CameraShape* V2CameraShapeParser::create(XMLTree* node, int resx, int resy)
{
  std::string type = node->getAttributeValue("type");
  
  if(type=="Perspective")
    return createPerspectiveCamera(node, resx, resy);
  else if(type=="Orthoscopic")
    return createOrthoscopicCamera(node, resx, resy);
  else if(type=="FishEye")
    return createFishEyeCamera(node, resx, resy);
  else if(type=="Polar")
    return createPolarCamera(node, resx, resy);
  else
    throw Exception("(V2CameraShapeParser::create) Géometrie de caméra innconue : " + type + ".");
}

/**
 * Create a perspective camera
 * @param node : the XML node to use for the creation.
 * @return : the parsed CameraShape
 */
CameraShape* V2CameraShapeParser::createPerspectiveCamera(XMLTree* node, int resx, int resy)
{
  Real fov = getRealValue(node, "fov", 45.0);

  Point origin;
  origin[0] = getRealValue(node, "originx", 0.0);
  origin[1] = getRealValue(node, "originy", 0.0);
  origin[2] = getRealValue(node, "originz", 0.0);

  Point lookat;
  lookat[0] = getRealValue(node, "lookatx", 1.0);
  lookat[1] = getRealValue(node, "lookaty", 0.0);
  lookat[2] = getRealValue(node, "lookatz", 0.0);

  Vector up;
  up[0] = getRealValue(node, "upx", 0.0);
  up[1] = getRealValue(node, "upy", 0.0);
  up[2] = getRealValue(node, "upz", 1.0);

  return new PerspectiveCameraShape(resx, resy, fov, origin, lookat, up);
}

/**
 * Create a Orthoscopic camera
 * @param node : the XML node to use for the creation.
 * @return : the parsed CameraShape
 */
CameraShape* V2CameraShapeParser::createOrthoscopicCamera(XMLTree* node, int resx, int resy)
{
  Real height = getRealValue(node, "height", 1.0);

  Point origin;
  origin[0] = getRealValue(node, "originx", 0.0);
  origin[1] = getRealValue(node, "originy", 0.0);
  origin[2] = getRealValue(node, "originz", 0.0);

  Point lookat;
  lookat[0] = getRealValue(node, "lookatx", 1.0);
  lookat[1] = getRealValue(node, "lookaty", 0.0);
  lookat[2] = getRealValue(node, "lookatz", 0.0);

  Vector up;
  up[0] = getRealValue(node, "upx", 0.0);
  up[1] = getRealValue(node, "upy", 0.0);
  up[2] = getRealValue(node, "upz", 1.0);
  return new OrthoscopicCameraShape(resx, resy, height, origin, lookat, up);
}

/**
 * Create a FishEye camera
 * @param node : the XML node to use for the creation.
 * @return : the parsed CameraShape
 */
CameraShape* V2CameraShapeParser::createFishEyeCamera(XMLTree* node, int resx, int resy)
{
  Point origin;
  origin[0] = getRealValue(node, "originx", 0.0);
  origin[1] = getRealValue(node, "originy", 0.0);
  origin[2] = getRealValue(node, "originz", 0.0);

  return new FishEyeCameraShape(resx, resy, origin);
}

/**
 * Create a Polar camera
 * @param node : the XML node to use for the creation.
 * @return : the parsed CameraShape
 */
CameraShape* V2CameraShapeParser::createPolarCamera(XMLTree* node, int resx, int resy)
{
  Point origin;
  origin[0] = getRealValue(node, "originx", 0.0);
  origin[1] = getRealValue(node, "originy", 0.0);
  origin[2] = getRealValue(node, "originz", 0.0);

  return new PolarCameraShape(resx, resy, origin);
}

