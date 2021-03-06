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
#ifndef _V2LIGHTSOURCEPARSER_HPP
#define _V2LIGHTSOURCEPARSER_HPP

#include <io/XMLTree.hpp>
#include <io/Parser.hpp>
#include <lightsources/LightSource.hpp>

/**
 * This parser load several LightSource from a node from a XMLTree of a scenery
 * file.
 * @author Sylavin "Greewi" Dumazet
 */
class V2LightSourceParser : public Parser{
public :
  /**
   * Create the light source by using the informations of the node.
   * @param node : the XML node to use for the creation.
   * @return : the parsed LightSource
   */
  LightSource* create(XMLTree* node);

private :

  /**
   * Create the light source by using the informations of the node.
   * @param node : the XML node to use for the creation.
   * @return : the parsed LightSource
   */
  LightSource* createPointLightSource(XMLTree* node);

  /**
   * Create the light source by using the informations of the node.
   * @param node : the XML node to use for the creation.
   * @return : the parsed LightSource
   */
  LightSource* createDirectionnalLightSource(XMLTree* node);

  /**
   * Create the light source by using the informations of the node.
   * @param node : the XML node to use for the creation.
   * @return : the parsed LightSource
   */
  LightSource* createSurfaceLightSource(XMLTree* node);

  /**
   * Create the light source by using the informations of the node.
   * @param node : the XML node to use for the creation.
   * @return : the parsed LightSource
   */
  LightSource* createPlaneLightSource(XMLTree* node);
};


#endif //_V2LIGHTSOURCEPARSER_HPP
