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
#ifndef GUARD_VRT_MPICLIENT_HPP
#define GUARD_VRT_MPICLIENT_HPP
//!
//! @file MPIClient.hpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @remarks
//! @details This file defines the behavior of a MPI client 
//!
#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

#include <core/Scenery.hpp>
#include <structures/Image.hpp>
////////////////////////////////////////////////////////////////////////////////
//! @class MPIClient
//! @brief Defines the behavior of a MPI client
//! @details
class MPIClient {
 public:
  //! @brief Default constructor
  //! @param scenery Scenery to be rendered
  //! @param xmin, xmax, ymin, ymax Area of the image to be rendred
  MPIClient(Scenery* scenery, bool debug,
            int xmin, int xmax, int ymin, int ymax);
  //! @brief Destructor
  ~MPIClient(void);
  //! @brief Initialize the client
  void Init(void);
  //! @brief Run the client
  void Run(void);

 private:
  //! Introduce to server
  void Introduce(void);
  //! Wait for the server's response
  //! @param flag Type of received message
  //! @param data Received data
  void WaitForServer(int& flag, int& data);
  //! @brief Get the next band to compute from the server
  //! @param nCam Camera index
  void ComputeBand(int& nCam);
  //! @brief Send the computed band to the server
  //! @param nCam Camera index
  //! @param nline Line index
  //! @param size Size of the computed band
  void CommitComputedBand(Image& image, const int& nCam, 
                          const int&  nline, 
                          const int&  size);

 private:
  //! Pointer to the scenery object to be renderer 
  //! Must not be deleted or use a reference count.
  Scenery* p_scenery;
  //! Bounding box area
  int m_xmin, m_xmax, m_ymin, m_ymax;
  //! Debug mode
  bool b_debug;
  //! Thread ID
  int m_rank;
}; // class MPIClient
////////////////////////////////////////////////////////////////////////////////
#endif //GUARD_VRT_MPICLIENT_HPP
