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
#ifndef GUARD_VRT_MPISERVER_HPP
#define GUARD_VRT_MPISERVER_HPP
//!
//! @file MPIServer.hpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @remarks
//! @details This file defines the behavior of the MPI server
//!
#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

#include <core/Scenery.hpp>
#include <structures/Image.hpp>
#include <raytracers/OldTaskManager.hpp>
////////////////////////////////////////////////////////////////////////////////
//! @enum Communication flags
enum {
  // The server initializes the client with size of renderer data
  kINITSIZE,
  // The server initializes the client with renderer data if non null
  kINITDATA,
  // The client is introduce itself to server
  kINTRODUCE,
  // The server sends ncam to client 
  kSENDNCAM,
  // The server sends nline to client 
  kSENDNLINE,
  // The server sends nsize to client
  kSENDNSIZE,
  // The server receives ncam from client 
  kRECEIVENCAM,
  // The server receives nline from client 
  kRECEIVENLINE,
  // The server receives nsize from client
  kRECEIVENSIZE,
  // The server receives image data from client
  kRECEIVEDATA,
  // The client must be closed
  kCLOSED
};
////////////////////////////////////////////////////////////////////////////////
//! @class MPIServer
//! @brief Defines the behavior of the MPI server
//! @details
class MPIServer {
 public:
  //! @brief Constructor
  //! @param scenery Scenery to be rendered
  //! @param num_clients Maximum number of clients that can be connected
  //! @param xmin, xmax, ymin, ymax Area of the image to be rendred
  MPIServer(Scenery* scenery, int num_clients, bool debug,
            int xmin, int xmax, int ymin, int ymax);
  //! @brief Destructor
  ~MPIServer(void);
  //! @brief Initialize the server
  void Init(void);
  //! @brief Run the server.
  void Run(void);

 private:
  //! @brief Wait for the next event (new client or data to read)
  //! @param flag Identifier of the client to be disconnected
  //! @param client_id Identifier of the client to be disconnected
  //! @param nCam Camera index
  void WaitForEvent(int& flag, int& client_id, int& nCam);
  //! @brief Disconnect all the clients
  //! @param client_id Identifier of the client to be disconnected
  void KillClient(const int& client_id);
  //! @brief Receive the data from a client, then treat it
  //! @param client_id Identifier of the client to be disconnected
  //! @param nCam Camera index
  void ReceiveResults(const int& client_id, int& nCam);
  //! @brief Sent the next task to compute to the client
  //! @param client_id Identifier of the client to be disconnected
  void SendNewBandToCompute(const int& client_id);
  //! @brief receive the last result of a clients 
  //! @param client_id Identifier of the client to be disconnected
  void ReceiveLastSlaveResults(const int& client_id);

private:
  //! @brief The server also gets some work
  void Work(void);

 private:
  //! Pointer to the task manager who says how to parallelize
  TaskManager* p_taskmanager;
  //! Pointer to the scenery object to be renderer 
  //! Must not be deleted or use a reference count.
  Scenery* p_scenery;
  //! Array of clients identifier
  int* p_clients;
  //! Maximum number of clients that can be connected
  int m_num_clients;
  //! Bounding box area
  int m_xmin, m_xmax, m_ymin, m_ymax;
  //! Debug mode
  bool b_debug;
  //! Thread ID
  int m_rank;
}; // class MPIServer
////////////////////////////////////////////////////////////////////////////////
#endif // GUARD_VRT_MPISERVER_HPP
