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
#ifndef GUARD_VRT_TESTRENDERER_HPP
#define GUARD_VRT_TESTRENDERER_HPP
//!
//! @file TestRenderer.hpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @remarks
//! @details This file defines the behaviors of the "Test Renderer" engine
//!
#include <core/3DBase.hpp>
#include <core/LightBase.hpp>

#include <core/Object.hpp>

#include <core/Source.hpp>

#include <renderers/Renderer.hpp>
////////////////////////////////////////////////////////////////////////////////
//! @see Environment
class Environment;
//! @see Scenery
class Scenery;
////////////////////////////////////////////////////////////////////////////////
//! @class TestRenderer
//! @brief Defines the "Test Renderer" engine
//! @details In this renderer, all objects are painted in white.
class TestRenderer: public Renderer {
 public:
  //! @brief Constructor
  //! @param max_depth Maximum number of recursions for ray-tracing algorithm
  //! @param scale Scale of the 3D scene (1 = 1 meter)
  TestRenderer(int max_depth, Real scale);

  //! @brief Destructor
  virtual ~TestRenderer(void);
 
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
  //! @paramSampleDiagram data Pointer that will contain the adress of the exported 
  //!  data buffer; May be equal to NULL
  //! @paramSampleDiagram data_size : Size of the exported data buffer
  virtual void ExportData(unsigned char** data, unsigned int* data_size);
    
 public:
  //! @brief Compute the light data for a given ray
  //! @param scenery Scenery ready for rendering
  //! @paramSampleDiagram light_data results of computation will be here
  //! @param depth Counter for recursions
  virtual void CastRay(Scenery& scenery, LightVector& light_data, 
                       int depth = -1);

 private:
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
                             int depth);
  //! @brief Add the contribution ot the direct light
  void AddDirectContribution(Scenery& scenery, 
                             LightVector& light_data, 
                             Object* object, 
                             const Basis& local_basis, 
                             const Point2D& surface_coordinate);
  //! @brief Add the contribution of the ambient light
  void AddAmbientContribution(Scenery& scenery, 
                              LightVector& light_data, 
                              Object* object, 
                              const Basis& local_basis, 
                              const Point2D& surface_coordinate);
 
 private : 
  //! Maximum number of recursions for ray-tracing algorithm
  int m_max_depth;
  //! Scale of the 3D scene (1 = 1 meter)
  Real m_scale;
}; // class TestRenderer
////////////////////////////////////////////////////////////////////////////////
#endif //GUARD_VRT_TESTRENDERER_HPP
