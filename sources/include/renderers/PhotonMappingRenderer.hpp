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
#ifndef GUARD_VRT_PHOTONMAPPINGRENDERER_HPP
#define GUARD_VRT_PHOTONMAPPINGRENDERER_HPP
//!
//! @file PhotonMappingRenderer.hpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @remarks
//! @details This file defines the behaviors of the "Photon Mapping" engine
//!
#include <core/3DBase.hpp>
#include <core/LightBase.hpp>

#include <core/Object.hpp>

#include <core/Source.hpp>

#include <structures/MultispectralPhotonMap.hpp>
#include <renderers/Renderer.hpp>
////////////////////////////////////////////////////////////////////////////////
//! @see Environment
class Environment;
//! @see Scenery
class Scenery;
////////////////////////////////////////////////////////////////////////////////
//! @class PhotonMappingRenderer
//! @brief Defines the "Photon Mapping" engine
//! @details All the behaviors of the "Photon Mapping" engine are defined here.
class PhotonMappingRenderer: public Renderer {
 public :
  //! @brief Constructor
  //! @param max_depth Maximum number of recursions for ray-tracing algorithm
  //! @param scale Scale of the 3D scene (1 = 1 meter)
	//! @param nb_global_photon Number of photons to be shot to compute the 
  //!  global photon maps
	//! @param nb_caustic_photon Number of photons to be shot to compute the 
  //!  caustic photon map
	//! @param nb_sample_global_photon Number of photons used to compute the 
  //!  global photon maps
	//! @param nb_sample_caustic_photon	Number of photons used to compute the
  //!  caustic photon map
	//! @param search_global_radius	Maximum length from the hit point for 
  //!  photon search in global photon maps
	//! @param search_caustic_radius Maximum length from the hit point for 
  //!  photon search in caustic photon map
	//! @param nb_samples	Number of secondary rays to compute diffuse lighting
	//! @param estimation_min_distance Minimum length	between 2 bounces in the
  //!  ray-tracing algorithm
  //! @param envir Pointer to the environment mapping object
  PhotonMappingRenderer(int max_depth, Real scale, 
                        unsigned int nb_global_photon, 
                        unsigned int nb_caustic_photon, 
                        unsigned int nb_sample_global_photon, 
                        unsigned int nb_sample_caustic_photon, 
                        Real search_global_radius, 
                        Real search_caustic_radius, 
                        unsigned int nb_samples, 
                        Real estimation_min_distance,
                        Environment* envir);
  //! @brief Destructor
  virtual ~PhotonMappingRenderer(void);
 
 public:
  //! @brief Initialize the renderer
  //! @details This method will all precomputations by itself
  //! @param scenery Scenery ready for rendering
  //! @param nb_threads Number of threads used by the rendering process
  virtual void Init(Scenery& scenery, int nb_threads);
  //! @brief Initialize the renderer with precomputed data 
  //! @remarks The precomputed data can be obtained from netwotk for example
  //! @param scenery Scenery ready for rendering
  //! @param data Pointer on the precomputed data buffer
  //! @param data_size Size of the precomputed data buffer
  virtual void InitWithData(Scenery& scenery, 
                            unsigned char* data, unsigned int data_size);
  //! @brief Export precomputed data 
  //! @remarks The renderer must have been initialized first !
  //! @remarks Dont forget to free the data pointer after exporting ! 
  //! @param data Pointer that will contain the adress of the exported 
  //!  data buffer; May be equal to NULL
  //! @param data_size : Size of the exported data buffer
  virtual void ExportData(unsigned char** data, unsigned int* data_size);

 public:
  //! @brief Compute the light data for a given ray
  //! @param scenery Scenery ready for rendering
  //! @param light_data results of computation will be here
  //! @param depth Counter for recursions
  virtual void CastRay(Scenery& scenery, LightVector& light_data, 
                       int depth = -1);
                       
 private:
  //! @brief Compute the light data for the given ray
  //! @param scenery Scenery ready for rendering
  //! @param light_data results of computation will be here
  //! @param depth Counter for recursions
  //! @param last_object Last object hit
  //! @param precise If false, a fast estimation will be done; this 
  //!  paramater can be used for secondary rays
  void CastRay(Scenery& scenery, LightVector& light_data, int depth = -1, 
               Object* last_object = 0, bool precise = true);
  //! @brief Build the global photon maps
  //! @param scenery Scenery ready for rendering
  void BuildGlobalPhotonMaps(Scenery& scenery);
  //! @brief Cast a photon for adding it into the global photon maps
  //! @param scenery Scenery ready for rendering
  //! @param photon Photon to be cast
  //! @brief direct True if this photon directlty come from a light
  //! @param depth Counter for recursions
  //! @param last_object_hit Last object hit
  void CastGlobalPhoton(Scenery& scenery, MultispectralPhoton& photon, 
                        bool direct, int depth, Object* last_object_hit = 0);

  //! @brief Build the caustic photon map
  //! @param scenery Scenery ready for rendering
  void BuildCausticPhotonMaps(Scenery& scenery);
  //! @brief Cast a photon for adding it into the caustic photon map
  //! @param scenery Scenery ready for rendering
  //! @param photon Photon to be cast
  //! @brief direct True if this photon directlty come from a light
  //! @param depth Counter for recursions
  //! @param last_object_hit Last object hit
  void CastCausticPhoton(Scenery& scenery, MultispectralPhoton& photon, 
                         bool direct, int depth, Object* last_object_hit = 0);

  //! @brief Get ad estimation of the photon mapping for the given ray
  //! @param scenery Scenery ready for rendering
  //! @param light_data results of computation will be here
  void EstimateRay(Scenery& scenery, LightVector& light_data);
  //! @brief Add the contribution of direct viewed source
  void AddDirectSourceContribution(Scenery& scenery, 
                                   LightVector& light_data, 
                                   Source* source, 
                                   const Basis& local_basis, 
                                   const Point2D& surface_coordinate);
  //! @brief Add the contribution of glossiness and reflections 
  void AddGlossyContribution(Scenery& scenery, 
                             LightVector& light_data, 
                             Object* object, 
                             const Basis& local_basis, 
                             const Point2D& surface_coordinate, 
                             int depth, bool precise);
  //! @brief Add the contribution ot the direct light
  void AddDirectContribution(Scenery& scenery, 
                             LightVector& light_data, 
                             Object* object, 
                             const Basis& local_basis, 
                             const Point2D& surface_coordinate);
  //! @brief Estimate the caustic contribution
  void AddCausticContribution(Scenery& scenery, 
                             LightVector& light_data, 
                             Object* object, 
                             const Basis& local_basis, 
                             const Point2D& surface_coordinate);
  //! @brief Add the indirect diffuse light contribution
  void AddDiffuseContribution(Scenery& scenery, 
                             LightVector& light_data, 
                             Object* object, 
                             const Basis& local_basis, 
                             const Point2D& surface_coordinate, 
                             int depth, bool precise);
  //! @brief Estimate the diffuse contribution
  void AddDiffuseEstimation(Scenery& scenery, 
                             LightVector& light_data, 
                             Object* object, 
                             const Basis& local_basis, 
                             const Point2D& surface_coordinate);
                       
 private :
  //! Maximum number of recursions for ray-tracing algorithm
  int m_max_depth;
  //! Scale of the 3D scene (1 = 1 meter)
  Real m_scale;
  //! Minimum length	between 2 bounces in the ray-tracing algorithm
  Real m_estimation_min_distance;
  //! Global photon map for light going inside an object
  std::vector<MultispectralPhotonMap*> m_global_map_in;
  //! Global photon map for light going outside an object
  std::vector<MultispectralPhotonMap*> m_global_map_out;
  //! Number of photons to be shot to compute the global photon maps
  unsigned int m_nb_global_photon;
  //! Enery of a photon in the global photon maps
  Real m_global_photon_power;
  //! Number of photons used to compute the global photon maps
  unsigned int m_nb_global_sample_photon;
  //! Maximum length from the hit point for photon search in global photon maps
  Real m_global_search_radius;
  //! Caustic photon map
  std::vector<MultispectralPhotonMap*> m_caustic_map;
  //! Number of photons to be shot to compute the caustic photon map
  unsigned int m_nb_caustic_photon;
  //! Enery of a photon in the caustic photon map
  Real m_caustic_photon_power;
  //! Number of photons used to compute the caustic photon map
  unsigned int m_nb_caustic_sample_photon;
  //! Maximum length from the hit point for photon search in caustic photon map
  Real m_caustic_search_radius;
  //! Number of secondary rays to compute diffuse lighting
  unsigned int m_nb_samples;
  //! Environment
  Environment* p_environment;
}; // class PhotonMappingRenderer

#endif // GUARD_VRT_PHOTONMAPPINGRENDERER_HPP
