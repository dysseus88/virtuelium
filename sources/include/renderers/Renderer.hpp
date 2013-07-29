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
#ifndef GUARD_VRT_RENDERER_HPP
#define GUARD_VRT_RENDERER_HPP
//!
//! @file Renderer.hpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @remarks
//! @details This file defines the base class all rendering engines must inherit
//!
#include <core/3DBase.hpp>
#include <core/LightBase.hpp>

////////////////////////////////////////////////////////////////////////////////
//! @see Scenery
class Scenery;
//! @class Renderer
//! @brief Defines the base class for rendering engines
class Renderer {
 public :
  //! @brief Destructor  
  virtual inline ~Renderer(void) { }
  
 public:
  //! @brief Initialize the renderer
  //! @details This method will all precomputations by itself
  //! @remarks This method is abstract and must implemented by derivated classes
  //! @param[in, out] scenery Scenery ready for rendering
  //! @param[in] nb_threads Number of threads used by the rendering process
  virtual void Init(Scenery& scenery, int nb_threads) = 0;
  //! @brief Initialize the renderer with precomputed data 
  //! @remarks The precomputed data can be obtained from netwotk for example
  //! @remarks This method is abstract and must implemented by derivated classes
  //! @param[in, out] scenery Scenery ready for rendering
  //! @param[in] data Pointer on the precomputed data buffer
  //! @param[in] data_size Size of the precomputed data buffer
   virtual void InitWithData(Scenery& scenery, 
                            unsigned char* data, unsigned int data_size) = 0;
  //! @brief Export precomputed data 
  //! @remarks The renderer must have been initialized first !
  //! @remarks Dont forget to free the data pointer after exporting ! 
  //! @remarks This method is abstract and must implemented by derivated classes
  //! @param[out] data Pointer that will contain the adress of the exported 
  //!  data buffer; May be equal to NULL
  //! @param[out] data_size : Size of the exported data buffer
  virtual void ExportData(unsigned char** data, unsigned int* data_size) = 0;

 public:
  //! @brief Compute the light data for a given ray
  //! @remarks This method is abstract and must implemented by derivated classes
  //! @param[in, out] scenery Scenery ready for rendering
  //! @param[out] light_data results of computation will be here
  //! @param[in, out] depth Counter for recursions
  virtual void CastRay(Scenery& scenery, LightVector& light_data, 
                       int depth = -1) = 0;
}; // class Renderer

#endif // GUARD_VRT_RENDERER_HPP
