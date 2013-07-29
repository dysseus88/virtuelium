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
#include <raytracers/MPIClients.hpp>
//!
//! @file MPIClient.cpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file implements the classes declared MPIClient.hpp 
//!  @arg MPIClient
//! @todo 
//! @remarks 
//!
#include <iostream>

#include <exceptions/Exception.hpp>
#include <raytracers/MPIServer.hpp>
#include <core/VrtLog.hpp>

////////////////////////////////////////////////////////////////////////////////
MPIClient::MPIClient(Scenery* scenery, bool debug,
                     int xmin, int xmax, int ymin, int ymax) 
    : p_scenery(scenery),
      m_xmin(xmin), m_xmax(xmax),
      m_ymin(ymin), m_ymax(ymax),
      b_debug(debug) {

  m_rank = MPI::COMM_WORLD.Get_rank();
}
////////////////////////////////////////////////////////////////////////////////
MPIClient::~MPIClient(void) {}
////////////////////////////////////////////////////////////////////////////////
void MPIClient::Init(void) {
  // reception status
	MPI::Status status;
	unsigned char* data=0;
 	unsigned int data_size;

  // datasize must be received
  MPI::COMM_WORLD.Recv(&data_size, 1, MPI_INT, 
                       0,  
                       kINITSIZE, 
                       status);			

  if (data_size > 0) {
    data = new unsigned char[data_size];
    memset(data, 0, data_size);					

    MPI::COMM_WORLD.Recv(data, data_size, MPI_UNSIGNED_CHAR, 
                         0, 
                         kINITDATA, 
                         status);

    // init slaves with data
    p_scenery->getRenderer()->InitWithData(*p_scenery, data, data_size);	
  }
}
////////////////////////////////////////////////////////////////////////////////
void MPIClient::Run(void) {
  // reception status
  MPI::Status status;
  // true if the client is closed
  bool isClosed = false;

  // the client must introduce itself before business
  Introduce();

  // while the received order is not kCLOSED
  while (isClosed == false) {
    int flag = 0, data = 0;

    // Wait for server answer
    WaitForServer(flag, data);

    // Process...
    if (flag == kSENDNCAM) {
      if (b_debug) {
        VrtLog::Write("(MPIClient[%d]::Run) Has to work", m_rank);
      }
      ComputeBand(data);

      // ... or prepare to be closed
    } else if (flag == kCLOSED) {
      if (b_debug) {
        VrtLog::Write("(MPIClient[%d]::Run) Has to leave", m_rank);
      }
      isClosed = true;
    }
  }      
}
////////////////////////////////////////////////////////////////////////////////
void MPIClient::Introduce(void) {
  int data = 0;
  MPI::COMM_WORLD.Send(&data, 1, MPI_INT, 
                      0, 
                      kINTRODUCE);
}
////////////////////////////////////////////////////////////////////////////////
void MPIClient::WaitForServer(int& flag, int& data) {
	// reception status
	MPI::Status status;

	if (b_debug) {
    VrtLog::Write("(MPIClient[%d]::WaitForServer) waiting for tasks", m_rank);
  }

  MPI::COMM_WORLD.Recv(&data, 1, MPI_INT, 
                       0, 
                       MPI::ANY_TAG, 
                       status);
	flag =  status.Get_tag();	
}
////////////////////////////////////////////////////////////////////////////////
void MPIClient::ComputeBand(int& nCam) {
	// reception status
	MPI::Status status;

  // Get camera
  if(nCam < 0 || nCam >= (int)p_scenery->getNbCamera())
    throw Exception("(MPIClient::ComputeBand) Caméra invalide !"); 
  Camera* camera = p_scenery->getCamera(nCam);

  // retrieve other info
 	int nline = 0;
 	MPI::COMM_WORLD.Recv(&nline, 1, MPI_INT, 
                       0, 
                       kSENDNLINE, 
                       status);  
  int size = 0;	
 	MPI::COMM_WORLD.Recv(&size, 1, MPI_INT, 
                       0, 
                       kSENDNSIZE, 
                       status);
	if (b_debug) {
    VrtLog::Write("(MPIClient[%d]::ComputeBand) cam=%d, line=%d; size=%d", 
                  m_rank, nCam, nline, size);
  }

  // Check that we will compute the good size
  if(camera->getWidth() * camera->getNumberOfChannels() != (unsigned int) size)
    throw Exception("(MPIClient::ComputeBand) Echange de données invalide !");

  //Compute the band
  Image image(camera->getWidth(), 1, camera->getNumberOfChannels());
  //camera->takeShot(*_scenery, 0, camera->getWidth()-1, nline, nline, image);
  camera->takeShot(*p_scenery, m_xmin, m_xmax, nline, nline, image);
    
	// commit results
  CommitComputedBand( image, nCam, nline, size );  
}
////////////////////////////////////////////////////////////////////////////////
void MPIClient::CommitComputedBand(Image& image, const int& nCam, 
                                   const int& nline, const int& size) {
	if (b_debug) {
    VrtLog::Write("(MPIClient[%d]::CommitComputedBand) Send header", m_rank);
  }

  // send header
	MPI::COMM_WORLD.Send(&nCam, 1, MPI_INT, 
                       0, 
                       kRECEIVENCAM);	

  MPI::COMM_WORLD.Send(&nline, 1, MPI_INT, 
                       0, 
                       kRECEIVENLINE);	
	
  MPI::COMM_WORLD.Send(&size, 1, MPI_INT, 
                       0, 
                       kRECEIVENSIZE);	

	// send data
	if (b_debug) {
    VrtLog::Write("(MPIClient[%d]::CommitComputedBand) Send data", m_rank);
  }
	MPI::COMM_WORLD.Send((char*)image.getRaster(), size * sizeof(float), MPI_CHAR, 
                      0, 
                      kRECEIVEDATA);		
}
////////////////////////////////////////////////////////////////////////////////