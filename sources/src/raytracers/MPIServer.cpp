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
#include <raytracers/MPIServer.hpp>
//!
//! @file MPIServer.cpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file implements the classes declared in MPIServer.hpp 
//!  @arg MPIServer
//! @todo 
//! @remarks 
//!
#include <exceptions/Exception.hpp>
#include <core/VrtLog.hpp>

////////////////////////////////////////////////////////////////////////////////
MPIServer::MPIServer(Scenery* scenery,
                     int num_clients,
                     bool debug,
                     int xmin, int xmax, 
                     int ymin, int ymax)
    : p_taskmanager(NULL),
      p_scenery(scenery),
      p_clients(NULL),
      m_num_clients(num_clients),
      m_xmin(xmin), m_xmax(xmax),
      m_ymin(ymin), m_ymax(ymax),
      b_debug(debug) {

  p_taskmanager = new TaskManager(p_scenery, m_xmin, m_xmax, m_ymin, m_ymax);
  
  p_clients = new int[m_num_clients - 1];
  memset(p_clients, 0, (m_num_clients - 1) * sizeof(int));
  
  m_rank = MPI::COMM_WORLD.Get_rank();
}
////////////////////////////////////////////////////////////////////////////////
MPIServer::~MPIServer(void) {
  if (p_taskmanager != NULL) {
    delete p_taskmanager;
    p_taskmanager = NULL;
  }
  if (p_clients != NULL) {
    delete[] p_clients;
    p_clients = NULL;
  }
}
////////////////////////////////////////////////////////////////////////////////
void MPIServer::Init(void) {
  unsigned char* data = NULL;
  unsigned int data_size = 0;

  p_taskmanager->exportRendererData(&data, &data_size);

  for (int c = 1; c < m_num_clients; c++) {
    MPI::COMM_WORLD.Send(&data_size, 1, MPI_INT, 
                         c, kINITSIZE);			
    if (data_size > 0) {			
      MPI::COMM_WORLD.Send(data, data_size, MPI_UNSIGNED_CHAR, 
                           c, 
                           kINITDATA);
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
void MPIServer::Run(void) {
  if (p_taskmanager == NULL)
    return;
   
  while(!p_taskmanager->workFinished()) {
    int ncam = -1, flag = -1, client_id = -1;
 
    // Wait for clients
    WaitForEvent(flag, client_id, ncam);
    
    // Treat received messages
    switch (flag) {     
      
      // A client has introduced itself, give some band to compute
    case kINTRODUCE:
				if (b_debug) {
          VrtLog::Write("(MPIServer::Run): New client registered wwith SID=%d",
                        client_id);
        }
        SendNewBandToCompute(client_id);
        break;

			// A client has finished its previous works, treat results, then give 
      // another work  
			case kRECEIVENCAM:
				if (b_debug) {
          VrtLog::Write("(MPIServer::Run): SID=%d has returned results", client_id);
        }
        ReceiveResults(client_id, ncam);
				SendNewBandToCompute(client_id);
				break;

      ///////
      default: 
        break;
    }

    // The server also works
    //Work();

  } // while

  if (b_debug) {
    VrtLog::Write("(MPIServer::Run): No work anymore");
  }

  // Receive last results then kill slave
  for (int c = 1; c < m_num_clients; c++) {
    if ( p_clients[c - 1] != 0 ) {
      if (b_debug) {
        VrtLog::Write("(MPIServer::Run): SID=%d must returns result", c);
      }
      ReceiveLastSlaveResults(c);
    }
      if (b_debug) {
        VrtLog::Write("(MPIServer::Run): Kill=%d", c);
      }
      KillClient(c);
  }
}
////////////////////////////////////////////////////////////////////////////////
void MPIServer::WaitForEvent(int& flag, int& client_id, int& nCam) {
  // reception status
  MPI::Status status;

  MPI::COMM_WORLD.Recv(&nCam, 1, MPI_INT, 
                       MPI::ANY_SOURCE, 
                       MPI::ANY_TAG, 
                       status);	
	client_id = status.Get_source();
	flag = status.Get_tag();
}
////////////////////////////////////////////////////////////////////////////////
void MPIServer::SendNewBandToCompute(const int& client_id) {
  int ncam = -1, nline = -1, size = -1;
  
	if (b_debug) {
    VrtLog::Write("(MPIServer::SendNewBandToCompute): Send new band to \
client %d", client_id); 
  }

  if(!p_taskmanager->nextBand(ncam, nline, size))
    return;

	if (b_debug) {
    VrtLog::Write("(MPIServer::SendNewBandToCompute): cam=%d, line=%d, size=%d", 
                  ncam, nline, size);
  }

  p_clients[client_id - 1] = 1;
  MPI::COMM_WORLD.Send(&ncam, 1, MPI_INT, 
                       client_id, 
                       kSENDNCAM);	
	MPI::COMM_WORLD.Send(&nline, 1, MPI_INT, 
                       client_id, 
                       kSENDNLINE);	
	MPI::COMM_WORLD.Send(&size, 1, MPI_INT, 
                       client_id, 
                       kSENDNSIZE);	
}
////////////////////////////////////////////////////////////////////////////////
void MPIServer::ReceiveResults(const int& client_id, int& nCam) {
  int nline = -1, size = -1;

  // reception status
	MPI::Status status;

  // receive other data
  MPI::COMM_WORLD.Recv(&nline, 1, MPI_INT, 
                       client_id, 
                       kRECEIVENLINE, 
                       status);	
  MPI::COMM_WORLD.Recv(&size, 1, MPI_INT, 
                       client_id, 
                       kRECEIVENSIZE, 
                       status);
	if (b_debug) {
    VrtLog::Write("(MPIServer::ReceiveResults): cam=%d, line=%d, size=%d", 
                  nCam, nline, size);
  }

 	p_clients[client_id - 1] = 0;

	// wait for image data
  char* image_data = NULL;
  image_data = new char[size * sizeof(float)];
 	MPI::COMM_WORLD.Recv(image_data, size * sizeof(float), MPI_CHAR, 
                       client_id, 
                       kRECEIVEDATA, 
                       status);		
	if (b_debug) {
    VrtLog::Write("(MPIServer::ReceiveResults): Commit image data");
  }

  //Commit the data to the task manager
  if(!p_taskmanager->commitBand((float*)image_data, nCam, nline, size)) {
    if (image_data != NULL) { 
      delete[] image_data;
      image_data = NULL;
    }
    throw Exception("(MPIServer::ReceiveResults)Impossible d'intégrer les \
données images reçues !"); 
  }
  
  if (image_data != NULL) { 
    delete[] image_data;
    image_data = NULL;
  }
}
////////////////////////////////////////////////////////////////////////////////
void MPIServer::ReceiveLastSlaveResults(const int& client_id) {
	// reception status
	MPI::Status status;
	int ncam = -1;

	if (b_debug) {
    VrtLog::Write("(MPIServer::ReceiveLastSlaveResults): force SID=%d \
to return results", client_id);
  }
	// force reception of tag: RETURNED_IMAGEINFO
	MPI::COMM_WORLD.Recv(&ncam, 1, MPI_INT, 
                       client_id, 
                       kRECEIVENCAM, 
                       status);	
	
	// commit result normally
	ReceiveResults(client_id, ncam);
}
////////////////////////////////////////////////////////////////////////////////
void MPIServer::KillClient(const int& client_id) {
  int data =  -1;

  MPI::COMM_WORLD.Send(&data, 1, MPI_INT, 
                       client_id, 
                       kCLOSED);	
}
////////////////////////////////////////////////////////////////////////////////
void MPIServer::Work(void) {
  int ncam = -1, nline = -1, size = -1;
 
  if(!p_taskmanager->nextBand(ncam, nline, size))
    return;

	if (b_debug) {
    VrtLog::Write("(MPIServer::Work): Compute a part of the image (ncam=%d, \
nline=%d, size=%d)", ncam, nline, size);
  }

  // Get camera
  if(ncam < 0 || ncam >= (int)p_scenery->getNbCamera())
    throw Exception("(MPIClient::ComputeBand) Caméra invalide !"); 
  Camera* camera = p_scenery->getCamera(ncam);

  // Compute the band
  Image image(camera->getWidth(), 1, camera->getNumberOfChannels());
  //camera->takeShot(*_scenery, 0, camera->getWidth()-1, nline, nline, image);
  camera->takeShot(*p_scenery, m_xmin, m_xmax, nline, nline, image);

  // Commit the data to the task manager
  if(!p_taskmanager->commitBand((float*)image.getRaster(), ncam, nline, size)) {
    throw Exception("(MPIServer::ReceiveResults)Impossible d'intégrer les \
données images reçues !"); 
  }

}
