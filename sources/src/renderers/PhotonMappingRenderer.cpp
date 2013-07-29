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
#include <renderers/PhotonMappingRenderer.hpp>
//!
//! @file PhotonMappingRenderer.cpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file implements the classes declared in PhotonMappingRenderer.hpp 
//!  @arg PhotonMappingRenderer
//! @todo 
//! @remarks 
//!
#include <vector>
#include <iostream>
#include <core/Scenery.hpp>

#include <environments/Environment.hpp>
////////////////////////////////////////////////////////////////////////////////
PhotonMappingRenderer::PhotonMappingRenderer(
    int maxDepth, Real scale, 
    unsigned int nb_global_photon, 
    unsigned int nb_caustic_photon, 
    unsigned int nb_sample_global_photon, 
    unsigned int nb_sample_caustic_photon, 
    Real search_global_radius, 
    Real search_caustic_radius, 
    unsigned int nb_samples, 
    Real estimation_min_distance,
    Environment* envir)
    // Initialization list
    : m_max_depth(maxDepth), 
      m_scale(scale), 
      m_estimation_min_distance(estimation_min_distance), 
      m_nb_global_photon(nb_global_photon), 
      m_nb_global_sample_photon(nb_sample_global_photon), 
      m_global_search_radius(search_global_radius), 
      m_nb_caustic_photon(nb_caustic_photon), 
      m_nb_caustic_sample_photon(nb_sample_caustic_photon), 
      m_caustic_search_radius(search_caustic_radius), 
      m_nb_samples(nb_samples),
      p_environment(NULL) {

  p_environment = envir;
}
////////////////////////////////////////////////////////////////////////////////
PhotonMappingRenderer::~PhotonMappingRenderer(void)
{
  for(unsigned int i = 0; i < m_global_map_in.size(); i++)
  {
    delete m_global_map_in[i];
    delete m_global_map_out[i];
    delete m_caustic_map[i];
  }

  if (p_environment != NULL)
    delete p_environment;
  p_environment = NULL;
}
////////////////////////////////////////////////////////////////////////////////
void PhotonMappingRenderer::BuildGlobalPhotonMaps(Scenery& scenery)
{
  for(unsigned int i = 0; i < scenery.getNbSource(); i++) {
    Source& source = *scenery.getSource(i);
    unsigned int nb_photon = (unsigned int)(source.getPower() 
                                              / m_global_photon_power);

    for(unsigned int j = 0; j < nb_photon; j++) {
      MultispectralPhoton photon;
      source.getRandomPhoton(photon);
      CastGlobalPhoton(scenery, photon, true, 20);
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
void PhotonMappingRenderer::CastGlobalPhoton(Scenery& scenery, 
                                             MultispectralPhoton& photon, 
                                             bool direct, int depth, 
                                             Object* last_object_hit) {
  if(depth <= 0)
    return;

  //Computing nearest intersection
  Real distance = -1;
  Object* nearest_object = 0;
  Point intersection;

  //If there is no intersection photon is lost
  Ray ray; 
  ray.v = photon.direction; 
  ray.o = photon.position;
  Basis local_basis;
  Point2D surface_coordinate;
  if(!scenery.getNearestIntersection(ray, distance, nearest_object, 
                                     local_basis, surface_coordinate, 
                                     last_object_hit))
    return;

  //Adding photon to global map
  photon.position = local_basis.o;
  photon.distance = distance * m_scale;
  photon.normal = local_basis.k;

  //Compute medium absorption    
  Medium* medium = 0;
  if(photon.direction.dot(local_basis.k) < 0) {
    medium = nearest_object->getOuterMedium();
  } else {
    medium = nearest_object->getInnerMedium();
  }
  if(!medium->transportPhoton(photon))
    return;

  if(!direct || m_nb_samples > 0) {
    if(photon.direction.dot(local_basis.k) < 0) {
      m_global_map_out[nearest_object->getIndex()]->addPhoton(photon);
    } else {
      m_global_map_in[nearest_object->getIndex()]->addPhoton(photon);
    }
  }

  //Photon bounce
  bool specular;
  if(nearest_object->bouncePhoton(local_basis, surface_coordinate, 
                                  photon, specular)) {
    CastGlobalPhoton(scenery, photon, false, depth - 1, nearest_object);
  }
}
////////////////////////////////////////////////////////////////////////////////
void PhotonMappingRenderer::BuildCausticPhotonMaps(Scenery& scenery) {
  for(unsigned int i = 0; i < scenery.getNbSource(); i++) {
    Source& source = *scenery.getSource(i);
    unsigned int nb_photon = (unsigned int)(source.getPower() 
                                              / m_caustic_photon_power);

    for(unsigned int j = 0; j < nb_photon; j++)
    {
      MultispectralPhoton photon;
      source.getRandomPhoton(photon);
      CastCausticPhoton(scenery, photon, true, 20);
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
void PhotonMappingRenderer::CastCausticPhoton(Scenery& scenery, 
                                              MultispectralPhoton& photon, 
                                              bool direct, int depth, 
                                              Object* last_object_hit) {
  if(depth<=0)
    return;

  //Computing nearest intersection
  Real distance = -1;
  Object* nearest_object = 0;
  Point intersection;

  //If there is no intersection photon is lost
  Ray ray; 
  ray.v = photon.direction;
  ray.o = photon.position;
  Basis local_basis;
  Point2D surface_coordinate;
  if(!scenery.getNearestIntersection(ray, distance, nearest_object, 
                                     local_basis, surface_coordinate, 
                                     last_object_hit))
    return;

  //Adding photon to global map
  photon.position = local_basis.o;
  photon.distance = distance * m_scale;
  photon.normal = local_basis.k;

  //Compute medium absorption    
  Medium* medium = 0;
  if(photon.direction.dot(local_basis.k) < 0) {
    medium = nearest_object->getOuterMedium();
  } else {
    medium = nearest_object->getInnerMedium();
  } 
  if(!medium->transportPhoton(photon))
    return;

  if(!direct && nearest_object->isDiffuse())
    m_caustic_map[nearest_object->getIndex()]->addPhoton(photon);
   
  //Photon bounce
  bool specular;
  if(nearest_object->isSpecular() 
       && nearest_object->bouncePhoton(local_basis, surface_coordinate, 
                                         photon, specular) 
       && specular) {
    CastCausticPhoton(scenery, photon, false, depth - 1, nearest_object);
  }
}
////////////////////////////////////////////////////////////////////////////////
void PhotonMappingRenderer::InitWithData(Scenery& scenery, 
                                         unsigned char* data, 
                                         unsigned int data_size) {
  unsigned char* buffer = data;

  //Load photon powers
  m_global_photon_power  = *(float*)buffer; 
  buffer += 4;
  m_caustic_photon_power = *(float*)buffer; 
  buffer += 4;
  
  //Load the number of maps
  unsigned int number_of_map = *(unsigned int*)buffer; 
  buffer += 4;

  if(number_of_map != scenery.getNbObject()) {
    throw Exception("(PhotonMappingRenderer::InitWithData) Nombre d'objet \
                    incorect.");
  }
  for(unsigned int i = 0; i < number_of_map; i++) {
    //Loading inner global map
    unsigned int nb_photons = *(unsigned int*)buffer; 
    buffer += 4;
    MultispectralPhotonMap* photonMap 
      = new MultispectralPhotonMap(m_global_photon_power);
    photonMap->setData((MultispectralPhoton*)buffer, nb_photons); 
    buffer += nb_photons * sizeof(MultispectralPhoton);
    m_global_map_in.push_back(photonMap);

    //Loading outer global map
    nb_photons = *(unsigned int*)buffer; 
    buffer += 4;
    photonMap = new MultispectralPhotonMap(m_global_photon_power);
    photonMap->setData((MultispectralPhoton*)buffer, nb_photons); 
    buffer += nb_photons * sizeof(MultispectralPhoton);
    m_global_map_out.push_back(photonMap);

    //Loading caustic map
    nb_photons = *(unsigned int*)buffer; 
    buffer += 4;
    photonMap = new MultispectralPhotonMap(m_caustic_photon_power);
    photonMap->setData((MultispectralPhoton*)buffer, nb_photons); 
    buffer += nb_photons*sizeof(MultispectralPhoton);
    m_caustic_map.push_back(photonMap);
  }
}
////////////////////////////////////////////////////////////////////////////////
void PhotonMappingRenderer::ExportData(unsigned char** data, 
                                       unsigned int* data_size) {
  //Get the header size (3 floats or int for header + 1 int by photons maps) 
  *data_size = 12 + 3 * 4 * m_global_map_in.size();

  //Get the size of photons
  for(unsigned int i = 0; i < m_global_map_in.size(); i++) {
    *data_size += (m_global_map_in[i]->getSize() 
                      + m_global_map_out[i]->getSize() 
                      + m_caustic_map[i]->getSize()) 
                   * sizeof(MultispectralPhoton);
  }
  //Allocating memory
  unsigned char * buffer = new unsigned char[*data_size];
  *data = buffer;

  //Filling header
  *(float*)buffer = m_global_photon_power;
  buffer += 4;
  *(float*)buffer = m_caustic_photon_power;
  buffer += 4;
  *(unsigned int*)buffer = m_global_map_in.size();
  buffer += 4;

  //Filling maps
  for(unsigned int i = 0; i < m_global_map_in.size(); i++) {
    *(unsigned int*)buffer = m_global_map_in[i]->getSize();
    buffer += 4;
    m_global_map_in[i]->getData((MultispectralPhoton*)buffer);
    buffer += m_global_map_in[i]->getSize() * sizeof(MultispectralPhoton);

    *(unsigned int*)buffer = m_global_map_out[i]->getSize();
    buffer += 4;
    m_global_map_out[i]->getData((MultispectralPhoton*)buffer);
    buffer += m_global_map_out[i]->getSize() * sizeof(MultispectralPhoton);

    *(unsigned int*)buffer = m_caustic_map[i]->getSize();
    buffer += 4;
    m_caustic_map[i]->getData((MultispectralPhoton*)buffer);
    buffer += m_caustic_map[i]->getSize() * sizeof(MultispectralPhoton);
  }
}
////////////////////////////////////////////////////////////////////////////////
void PhotonMappingRenderer::Init(Scenery& scenery, int nb_threads) {
  Real totalPower = 0;
  for(unsigned int i = 0; i < scenery.getNbSource(); i++) {
    totalPower += scenery.getSource(i)->getPower();
  }
  m_global_photon_power  = totalPower / m_nb_global_photon;
  for(unsigned int i = 0; i < scenery.getNbObject(); i++) {
    m_global_map_in.push_back(
      new MultispectralPhotonMap(m_global_photon_power));
    m_global_map_out.push_back(
      new MultispectralPhotonMap(m_global_photon_power));
  }
  BuildGlobalPhotonMaps(scenery);
  for(unsigned int i = 0; i < scenery.getNbObject(); i++) {
    m_global_map_in[i]->optimize();
    m_global_map_out[i]->optimize();
    m_global_map_in[i]->convertToIrradianceMap(m_global_search_radius, 
                                               m_nb_global_sample_photon);
    m_global_map_out[i]->convertToIrradianceMap(m_global_search_radius, 
                                                m_nb_global_sample_photon);
  }

  m_caustic_photon_power = totalPower / m_nb_caustic_photon;
  for(unsigned int i = 0; i < scenery.getNbObject(); i++) {
    m_caustic_map.push_back(
      new MultispectralPhotonMap(m_caustic_photon_power));
  }
  if(m_nb_samples > 0) {
    BuildCausticPhotonMaps(scenery);
    for(unsigned int i = 0; i < scenery.getNbObject(); i++) {
      m_caustic_map[i]->optimize();
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
void PhotonMappingRenderer::AddGlossyContribution(
    Scenery& scenery, 
    LightVector& light_data, 
    Object* object, 
    const Basis& local_basis, 
    const Point2D& surface_coordinate, 
    int depth, bool precise) {
  if(depth < 0) 
    return;
  
  //Getting secondarys rays to cast
  std::vector<LightVector> subrays;
  object->getSpecularSubRays(local_basis, surface_coordinate, 
                             light_data, subrays);
  for(unsigned int i = 0; i <  subrays.size(); i++) {
    //Get incident luminance
    subrays[i].clear();
    CastRay(scenery, subrays[i], depth, object, precise);
    subrays[i].flip();

    //Get the reemited luminance
    LightVector tmpr;
    tmpr.initGeometricalData(light_data);
    tmpr.initSpectralData(subrays[i]);

    object->getSpecularReemited(local_basis, surface_coordinate, 
                                subrays[i], tmpr);
    light_data.add(tmpr);
  }
}
////////////////////////////////////////////////////////////////////////////////
void PhotonMappingRenderer::AddDirectSourceContribution(
    Scenery& scenery, 
    LightVector& light_data, 
    Source* source, 
    const Basis& local_basis, 
    const Point2D& surface_coordinate) {
  LightVector tmp; 
  tmp.initSpectralData(light_data);
  tmp.initGeometricalData(light_data);
  source->getEmittedLight(local_basis, surface_coordinate, tmp);
  light_data.add(tmp);
}
////////////////////////////////////////////////////////////////////////////////
void PhotonMappingRenderer::AddDirectContribution(
    Scenery& scenery, 
    LightVector& light_data, 
    Object* object, 
    const Basis& local_basis, 
    const Point2D& surface_coordinate) {
  //Adding lights contributions (no intersection !)
  LightVector tmpr; 
  tmpr.initSpectralData(light_data);
  tmpr.initGeometricalData(light_data);

  std::vector<LightVector> incidents;
  for(unsigned int i = 0; i < scenery.getNbSource(); i++) {
    //Get the incoming rays
    incidents.clear();
    scenery.getSource(i)->getIncidentLight(local_basis.o, light_data, 
                                           incidents);

    //For each incoming ray
    for(unsigned int j = 0; j < incidents.size(); j++) {
      Ray incoming = incidents[j].getRay();
      incoming.v.mul(-1.0);

      Real src_distance = -1;
      Real obj_distance = -1;
      Source* source = 0;
      Object* occ_obj = 0;
      
      bool srcHit = scenery.getNearestIntersectionWithSource(incoming, 
                                                             src_distance, 
                                                             source);
      bool objHit = scenery.getNearestIntersection(incoming, obj_distance, 
                                                   occ_obj, object);

      if(srcHit && source!=scenery.getSource(i) 
                && src_distance<incidents[j].getDistance())
        continue;

      if(objHit && ((srcHit && obj_distance<src_distance 
                            && obj_distance<incidents[j].getDistance()) 
                    || (!srcHit && obj_distance<incidents[j].getDistance())))
        continue;

      //Get the received light, compute the reemited light and then add it 
      // to the result
      object->getDiffuseReemited(local_basis, surface_coordinate, 
                                 incidents[j], tmpr);
      light_data.add(tmpr);
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
void PhotonMappingRenderer::AddDiffuseContribution(
    Scenery& scenery, 
    LightVector& light_data, 
    Object* object, 
    const Basis& local_basis, 
    const Point2D& surface_coordinate, 
    int depth, 
    bool precise) {
  if(depth < 0) 
    return;

  std::vector<LightVector> incidents;
  if(precise) {
    object->getRandomDiffuseRay(local_basis, surface_coordinate, 
                                light_data, m_nb_samples, incidents);
  } else {
    object->getRandomDiffuseRay(local_basis, surface_coordinate, light_data, 
                                1, incidents);
  }
  for(unsigned int i = 0; i < incidents.size(); i++) {
    //Get incident luminance
    incidents[i].clear();
    CastRay(scenery, incidents[i], depth, object, false);
    incidents[i].flip();

    //Get the reemited luminance
    LightVector tmpr;
    tmpr.initGeometricalData(light_data);
    tmpr.initSpectralData(incidents[i]);
    object->getDiffuseReemited(local_basis, surface_coordinate, 
                               incidents[i], tmpr);
    tmpr.mul(Real(1.0) / (incidents[i].getWeight() * incidents.size()));
    light_data.add(tmpr);    
  }
}
////////////////////////////////////////////////////////////////////////////////
void PhotonMappingRenderer::AddCausticContribution(
    Scenery& scenery, 
    LightVector& light_data, 
    Object* object, 
    const Basis& local_basis, 
    const Point2D& surface_coordinate) {
  //Get the estimation
  LightVector tmp;
  tmp.initGeometricalData(light_data);
  tmp.initSpectralData(light_data);
  m_caustic_map[object->getIndex()]->getEstimation(local_basis, 
                                                   surface_coordinate, 
                                                   *object, 
                                                   m_caustic_search_radius, 
                                                   m_nb_caustic_sample_photon, 
                                                   tmp);
  light_data.add(tmp);
}
////////////////////////////////////////////////////////////////////////////////
void PhotonMappingRenderer::AddDiffuseEstimation(
    Scenery& scenery, 
    LightVector& light_data, 
    Object* object, 
    const Basis& local_basis, 
    const Point2D& surface_coordinate) {
  //Building the incident ray
  LightVector incident;
  Spectrum irradiance;
  incident.initSpectralData(light_data);
  incident.setDistance(0);

  //Get the reemited luminance
  LightVector tmpr;
  tmpr.initGeometricalData(light_data);
  tmpr.initSpectralData(light_data);

  //Outer irradiance  
  incident.setRay(
    local_basis.o, 
    Vector(-local_basis.k[0], -local_basis.k[1], -local_basis.k[2]));
  m_global_map_out[object->getIndex()]->getIrradiance(local_basis.o, 
                                                      irradiance);
  for(unsigned int l = 0; l < incident.size(); l++) {
    incident[l].setRadiance(irradiance[incident[l].getIndex()]);
  } 
  object->getDiffuseReemited(local_basis, surface_coordinate, incident, tmpr);
  light_data.add(tmpr);

  //Inner irradiance  
  incident.setRay(
    local_basis.o, 
    Vector(local_basis.k[0], local_basis.k[1], local_basis.k[2]));
  m_global_map_in[object->getIndex()]->getIrradiance(local_basis.o, irradiance);
  for(unsigned int l = 0; l < incident.size(); l++) {
    incident[l].setRadiance(irradiance[incident[l].getIndex()]);
  }
  object->getDiffuseReemited(local_basis, surface_coordinate, incident, tmpr);
  light_data.add(tmpr);
}
////////////////////////////////////////////////////////////////////////////////
void PhotonMappingRenderer::CastRay(Scenery& scenery, 
                                    LightVector& light_data, 
                                    int depth){
  CastRay(scenery, light_data, depth, 0, true);
}
////////////////////////////////////////////////////////////////////////////////
void PhotonMappingRenderer::CastRay(Scenery& scenery, 
                                    LightVector& light_data, 
                                    int depth, 
                                    Object* last_object, 
                                    bool precise) {
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
                                                obj_surface_coordinate, 
                                                last_object);
  bool srcHitted = false;
  if(precise || m_nb_samples <= 0) {
    srcHitted = scenery.getNearestIntersectionWithSource(
                          light_data.getRay(), 
                          src_distance, 
                          nearest_source, 
                          src_local_basis, 
                          src_surface_coordinate);
  }
  //If there is no intersection return black data
  if(!obj_hit && !srcHitted) {

		if (p_environment != NULL) {
  		p_environment->AddContribution(light_data);
    }

    return;
  }

  //Intersection with object
  if(obj_hit && (!srcHitted || obj_distance<src_distance)) {
    light_data.setDistance(obj_distance * m_scale);  

    if(m_nb_samples <= 0) {
      //Direct illumination
      AddDirectContribution(scenery, light_data, nearest_object, 
                            obj_local_basis, obj_surface_coordinate);

      //Glossy illumination
      if(nearest_object->isSpecular()) {
        AddGlossyContribution(scenery, light_data, nearest_object, 
                              obj_local_basis, obj_surface_coordinate, 
                              depth - 1, precise);  
      }
      //Diffuse illumination
      if(nearest_object->isDiffuse()) {
        AddDiffuseEstimation(scenery, light_data, nearest_object, 
                             obj_local_basis, obj_surface_coordinate);    
      }
    } else if(precise) {
      //Direct illumination
      if(nearest_object->isDiffuse()) {
        AddDirectContribution(scenery, light_data, nearest_object, 
                              obj_local_basis, obj_surface_coordinate);
      }
      //Glossy illumination
      if(nearest_object->isSpecular()) {
        AddGlossyContribution(scenery, light_data, nearest_object, 
                              obj_local_basis, obj_surface_coordinate, 
                              depth - 1, precise);  
      }
      //Indirect diffuse illumination
      if(nearest_object->isDiffuse()) {
        AddCausticContribution(scenery, light_data, nearest_object, 
                               obj_local_basis, obj_surface_coordinate);
      }
      //Indirect diffuse illumination
      if(nearest_object->isDiffuse()) {
        AddDiffuseContribution(scenery, light_data, nearest_object, 
                               obj_local_basis, obj_surface_coordinate, 
                               depth - 1, true);
      }
    } else {
      //Glossy illumination
      if(nearest_object->isSpecular()) {
        AddGlossyContribution(scenery, light_data, nearest_object, 
                              obj_local_basis, obj_surface_coordinate, 
                              depth - 1, precise);  
      }
      //Diffuse illumination
      if(nearest_object->isDiffuse()) {
        if(obj_distance>m_estimation_min_distance || depth <= 0) {
          AddDiffuseEstimation(scenery, light_data, nearest_object, 
                               obj_local_basis, obj_surface_coordinate);
        } else {
          AddCausticContribution(scenery, light_data, nearest_object, 
                                 obj_local_basis, obj_surface_coordinate);
          AddDiffuseContribution(scenery, light_data, nearest_object, 
                                 obj_local_basis, obj_surface_coordinate, 
                                 depth - 1, false);
          AddDirectContribution(scenery, light_data, nearest_object, 
                                obj_local_basis, obj_surface_coordinate);
        }
      }
    }

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
    if(precise || m_nb_samples <= 0) {
      light_data.setDistance(src_distance * m_scale);
      AddDirectSourceContribution(scenery, light_data, nearest_source, 
                                  src_local_basis, src_surface_coordinate);
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
