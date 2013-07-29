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
 
#include <io/sceneryV2/V2LightSourceParser.hpp>
#include <io/DataParser.hpp>
#include <exceptions/Exception.hpp>
#include <string>
#include <core/3DBase.hpp>

#include <lightsources/PointLightSource.hpp>
#include <lightsources/DirectionalLightSource.hpp>
#include <lightsources/SurfaceLightSource.hpp>
#include <lightsources/PlaneLightSource.hpp>

/**
 * Create the ray caster by using the informations of the node.
 * node : the XML node to use for the creation.
 */
LightSource* V2LightSourceParser::create(XMLTree* node)
{
  std::string type = node->getAttributeValue("type");
  
  if(type=="Point")
    return createPointLightSource(node);
  else if(type=="Directionnal")
    return createDirectionnalLightSource(node);
  else if(type=="Surface")
    return createSurfaceLightSource(node);
  else if(type=="Plane")
    return createPlaneLightSource(node);

  throw Exception("(V1V2LightSourceParser::create) Type de <source> " + type + " inconnu.");
}

/**
 * Create the light source by using the informations of the node.
 * @param node : the XML node to use for the creation.
 * @return : the parsed LightSource
 */
LightSource* V2LightSourceParser::createPointLightSource(XMLTree* node)
{
Point origin;
    origin[0] = getRealValue(node, "originx", 0.0);
    origin[1] = getRealValue(node, "originy", 0.0);
    origin[2] = getRealValue(node, "originz", 0.0);

    Real power;
    power = getRealValue(node, "power", 0.0);

    Real radius;
    radius = getRealValue(node, "radius", 0.1);

    Spectrum spectrum;
    DataParser parser;
    parser.loadSpectralIlluminant(node->getAttributeValue("spectrum"), spectrum);

    return new PointLightSource(spectrum, power, origin, radius);
}

/**
 * Create the light source by using the informations of the node.
 * @param node : the XML node to use for the creation.
 * @return : the parsed LightSource
 */
LightSource* V2LightSourceParser::createDirectionnalLightSource(XMLTree* node)
{
Vector direction;
    direction[0] = getRealValue(node, "directionx", 0.0);
    direction[1] = getRealValue(node, "directiony", 0.0);
    direction[2] = getRealValue(node, "directionz", 0.0);

    Real power = getRealValue(node, "power", 0.0);

    Vector u;
    u[0] = getRealValue(node, "ux", 0.0);
    u[1] = getRealValue(node, "uy", 0.0);
    u[2] = getRealValue(node, "uz", 0.0);
    Vector v;
    v[0] = getRealValue(node, "vx", 0.0);
    v[1] = getRealValue(node, "vy", 0.0);
    v[2] = getRealValue(node, "vz", 0.0);
    Point o;   
    o[0] = getRealValue(node, "ox", 0.0);
    o[1] = getRealValue(node, "oy", 0.0);
    o[2] = getRealValue(node, "oz", 0.0);

    Spectrum spectrum;
    DataParser parser;
    parser.loadSpectralIlluminant(node->getAttributeValue("spectrum"), spectrum);

    return new DirectionalLightSource(spectrum, power, direction, u, v, o);
}

/**
 * Create the light source by using the informations of the node.
 * @param node : the XML node to use for the creation.
 * @return : the parsed LightSource
 */
LightSource* V2LightSourceParser::createSurfaceLightSource(XMLTree* node)
{
Vector normal;
    normal[0] = getRealValue(node, "normalx", 0.0);
    normal[1] = getRealValue(node, "normaly", 0.0);
    normal[2] = getRealValue(node, "normalz", 0.0);

    Real power = getRealValue(node, "power", 0.0);

    Point o;   
    o[0] = getRealValue(node, "ox", 0.0);
    o[1] = getRealValue(node, "oy", 0.0);
    o[2] = getRealValue(node, "oz", 0.0);

    Spectrum spectrum;
    DataParser parser;
    parser.loadSpectralIlluminant(node->getAttributeValue("spectrum"), spectrum);

    return new SurfaceLightSource(spectrum, power, o, normal);
}

/**
 * Create the light source by using the informations of the node.
 * @param node : the XML node to use for the creation.
 * @return : the parsed LightSource
 */
LightSource* V2LightSourceParser::createPlaneLightSource(XMLTree* node)
{
Real power = getRealValue(node, "power", 0.0);
    unsigned int nbSamples = getIntegerValue(node, "samples", 10);

    Vector normal;
    normal[0] = getRealValue(node, "normalx", 0.0);
    normal[1] = getRealValue(node, "normaly", 0.0);
    normal[2] = getRealValue(node, "normalz", 0.0);

    Point o;   
    o[0] = getRealValue(node, "ox", 0.0);
    o[1] = getRealValue(node, "oy", 0.0);
    o[2] = getRealValue(node, "oz", 0.0);
    Vector u;
    u[0] = getRealValue(node, "ux", 0.0);
    u[1] = getRealValue(node, "uy", 0.0);
    u[2] = getRealValue(node, "uz", 0.0);
    Vector v;
    v[0] = getRealValue(node, "vx", 0.0);
    v[1] = getRealValue(node, "vy", 0.0);
    v[2] = getRealValue(node, "vz", 0.0);

    Spectrum spectrum;
    DataParser parser;
    parser.loadSpectralIlluminant(node->getAttributeValue("spectrum"), spectrum);

    return new PlaneLightSource(spectrum, power, o, normal, u, v, nbSamples);
}

