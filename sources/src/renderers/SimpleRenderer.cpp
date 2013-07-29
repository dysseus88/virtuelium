/*
 *  Copyright 2013 Remi "Programmix" Cerise
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
#include <renderers/SimpleRenderer.hpp>
//!
//! @file SimpleRenderer.cpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file implements the classes declared in SimpleRenderer.hpp 
//!  @arg SimpleRenderer
//! @todo 
//! @remarks 
//!
#include <vector>
#include <iostream>
#include <core/Scenery.hpp>
#include <core/VrtLog.hpp>

#include <environments/Environment.hpp>
////////////////////////////////////////////////////////////////////////////////
SimpleRenderer::SimpleRenderer(int max_depth, Real scale, 
                               Spectrum& ambient_illum, Real ambient_power,
                               Environment* envir)
    : m_max_depth(max_depth), m_scale(scale), p_environment(NULL) {
    
  if (ambient_power >= 0) {
    for(unsigned int i = 0; i < GlobalSpectrum::nbWaveLengths(); i++)
      s_ambient[i] = ambient_illum[i] * ambient_power;
  }
  
  p_environment = envir;
}
////////////////////////////////////////////////////////////////////////////////
SimpleRenderer::~SimpleRenderer(void) { 
  if (p_environment != NULL)
    delete p_environment;
  p_environment = NULL;
}
////////////////////////////////////////////////////////////////////////////////
void SimpleRenderer::Init(Scenery& scenery, int nb_threads) {
  //nothing to do more
}
////////////////////////////////////////////////////////////////////////////////
void SimpleRenderer::InitWithData(Scenery& scenery, 
                                  unsigned char* data, 
                                  unsigned int data_size) {
  Init(scenery, 1);
}
////////////////////////////////////////////////////////////////////////////////
void SimpleRenderer::ExportData(unsigned char** data, 
                                unsigned int* data_size) {
  data = 0;
  data_size = 0;
}  
////////////////////////////////////////////////////////////////////////////////
void SimpleRenderer::CastRay(Scenery& scenery, 
                             LightVector& light_data, 
                             int depth) {

  if(depth < 0) 
    depth = m_max_depth;

  //Computing nearest intersection
  Real obj_distance = -1;
  Real src_distance = -1;
  Object* nearest_object = 0;
  Source* nearest_source = 0;
  Basis obj_local_basis;
  Basis src_local_basis;
  Point2D obj_surface_coordinate;
  Point2D src_surface_coordinate;

  bool obj_hit = scenery.getNearestIntersection(light_data.getRay(), 
                                                obj_distance, 
                                                nearest_object, 
                                                obj_local_basis, 
                                                obj_surface_coordinate);

  bool src_hit 
          = scenery.getNearestIntersectionWithSource(light_data.getRay(), 
                                                     src_distance, 
                                                     nearest_source, 
                                                     src_local_basis, 
                                                     src_surface_coordinate);
  
  //If there is no intersection, return background light
  if(!obj_hit && !src_hit) {
    //for(unsigned int i = 0; i < light_data.size(); i++)
    //  light_data[i].setRadiance(s_ambient[ light_data[i].getIndex() ] 
    //                              / (2 * M_PI));

		if (p_environment != NULL) {
  		p_environment->AddContribution(light_data);
    }

    return;
  }

  //Intersection with object
  if(obj_hit && (!src_hit || obj_distance < src_distance)) {
    light_data.setDistance(obj_distance * m_scale);
    
    //Direct illumination
    AddDirectContribution(scenery, light_data, nearest_object, 
                          obj_local_basis, obj_surface_coordinate);

    //Ambiant illumination
    AddAmbientContribution(scenery, light_data, nearest_object, 
                           obj_local_basis, obj_surface_coordinate);

    //Glossy illumination
    AddGlossyContribution(scenery, light_data, nearest_object, 
                          obj_local_basis, obj_surface_coordinate, depth - 1);  

    //Compute medium absorption    
    Medium* medium = 0;
    if(light_data.getRay().v.dot(obj_local_basis.k) < 0) {
      medium = nearest_object->getOuterMedium();
    } else {
      medium = nearest_object->getInnerMedium();
    }
    medium->transportLight(light_data);
  
  //Intersection with source
  } else {
    light_data.setDistance(src_distance * m_scale);
    AddDirectSourceContribution(scenery, light_data, nearest_source, 
                                src_local_basis, src_surface_coordinate);
  }
}
////////////////////////////////////////////////////////////////////////////////
void SimpleRenderer::AddDirectSourceContribution(
    Scenery& scenery, 
    LightVector& light_data, 
    Source* source, 
    const Basis& localBasis, 
    const Point2D& surfaceCoordinate) {
  LightVector tmp; 
  tmp.initSpectralData(light_data);
  tmp.initGeometricalData(light_data);
  source->getEmittedLight(localBasis, surfaceCoordinate, tmp);
  light_data.add(tmp);
}
////////////////////////////////////////////////////////////////////////////////
void SimpleRenderer::AddGlossyContribution(
    Scenery& scenery, 
    LightVector& light_data, 
    Object* object, 
    const Basis& localBasis, 
    const Point2D& surfaceCoordinate, 
    int depth) {
  if(depth < 0)
    return;
  
  //Getting secondarys rays to cast
  std::vector<LightVector> subrays;
  object->getSpecularSubRays(localBasis, surfaceCoordinate, light_data, subrays);
  for(unsigned int i = 0; i <  subrays.size(); i++) {
    //Advance a little to avoid intersection with starting point
    Ray propagation=subrays[i].getRay();
    propagation.o[0] += propagation.v[0] * Real(0.01);
    propagation.o[1] += propagation.v[1] * Real(0.01);
    propagation.o[2] += propagation.v[2] * Real(0.01);
    subrays[i].setRay(propagation);

    //Get incident luminance
    subrays[i].clear();
    CastRay(scenery, subrays[i], depth);
    subrays[i].flip();

    //Get the reemited luminance
    LightVector tmpr;
    tmpr.initGeometricalData(light_data);
    tmpr.initSpectralData(subrays[i]);

    object->getSpecularReemited(localBasis, surfaceCoordinate, 
                                subrays[i], tmpr);
    light_data.add(tmpr);
  }
}
////////////////////////////////////////////////////////////////////////////////
void SimpleRenderer::AddDirectContribution(
    Scenery& scenery, 
    LightVector& light_data, 
    Object* object, 
    const Basis& localBasis, 
    const Point2D& surfaceCoordinate) {
  //Adding lights contributions (no intersection !)
  LightVector tmpr; 
  tmpr.initSpectralData(light_data);
  tmpr.initGeometricalData(light_data);

  std::vector<LightVector> incidents;
  for(unsigned int i = 0; i < scenery.getNbSource(); i++) {
    //Get the incoming rays
    incidents.clear();
    scenery.getSource(i)->getIncidentLight(localBasis.o, light_data, incidents);

    //For each incoming ray
    for(unsigned int j = 0; j < incidents.size(); j++) {
      Ray incoming = incidents[j].getRay();
      incoming.v.mul(-1.0);

      Real src_distance=-1;
      Real obj_distance=-1;
      Source* source=0;
      Object* occObj=0;
      
      bool srcHit = scenery.getNearestIntersectionWithSource(incoming, 
                                                             src_distance, 
                                                             source);
      bool objHit = scenery.getNearestIntersection(incoming, obj_distance, 
                                                   occObj, object);

      if(srcHit && source!=scenery.getSource(i) 
                && src_distance<incidents[j].getDistance())
        continue;

      if(objHit && ((srcHit && obj_distance<src_distance 
                            && obj_distance<incidents[j].getDistance()) 
                    || (!srcHit && obj_distance<incidents[j].getDistance())))
        continue;

      // Get the received light, compute the reemited light and then add it to 
      // the result
      object->getDiffuseReemited(localBasis, surfaceCoordinate, 
                                 incidents[j], tmpr);
      light_data.add(tmpr);
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
void SimpleRenderer::AddAmbientContribution(
    Scenery& scenery, 
    LightVector& light_data, 
    Object* object, 
    const Basis& localBasis, 
    const Point2D& surfaceCoordinate) {
  LightVector tmpr; 
  tmpr.initSpectralData(light_data);
  tmpr.initGeometricalData(light_data);

  object->getDiffuseReemitedFromAmbiant(localBasis, surfaceCoordinate, 
                                        tmpr, s_ambient);
  light_data.add(tmpr);
}
////////////////////////////////////////////////////////////////////////////////
