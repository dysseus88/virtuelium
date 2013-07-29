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
#ifndef _V2CAMERASHAPEPARSER_HPP
#define _V2CAMERASHAPEPARSER_HPP

#include <io/XMLTree.hpp>
#include <io/Parser.hpp>
#include <camerashapes/CameraShape.hpp>

/**
 * This parser load several CameraShape from a node from a XMLTree of a scenery
 * file.
 * @author Sylavin "Greewi" Dumazet
 */
class V2CameraShapeParser : public Parser{
public :
  /**
   * Create the camera shape by using the informations of the node.
   * @param node : the XML node to use for the creation.
   * @return : the parsed CameraShape
   */
  CameraShape* create(XMLTree* node, int resx, int resy);

private :
  /**
   * Create a perspective camera
   * @param node : the XML node to use for the creation.
   * @return : the parsed CameraShape
   */
  CameraShape* createPerspectiveCamera(XMLTree* node, int resx, int resy);

  /**
   * Create a Orthoscopic camera
   * @param node : the XML node to use for the creation.
   * @return : the parsed CameraShape
   */
  CameraShape* createOrthoscopicCamera(XMLTree* node, int resx, int resy);

  /**
   * Create a FishEye camera
   * @param node : the XML node to use for the creation.
   * @return : the parsed CameraShape
   */
  CameraShape* createFishEyeCamera(XMLTree* node, int resx, int resy);

  /**
   * Create a Polar camera
   * @param node : the XML node to use for the creation.
   * @return : the parsed CameraShape
   */
  CameraShape* createPolarCamera(XMLTree* node, int resx, int resy);
};


#endif //_V2CAMERASHAPEPARSER_HPP
