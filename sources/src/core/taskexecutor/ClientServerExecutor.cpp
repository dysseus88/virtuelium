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
#include <core/taskexecutor/ClientServerExecutor.hpp>
//!
//! @file ClientServerExecutor.cpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file implements classs declared in ClientServerExecutor.hpp 
//!  @arg ClientServerExecutor
//!
#include <cstdarg>
#include <omp.h>
#include <mpi.h>
#include <map>

#include <core/VrtLog.hpp>

#include <core/Scenery.hpp>
#include "io/image/ImageParser.hpp"
#include <structures/Image.hpp>
#include <exceptions/Exception.hpp>
////////////////////////////// class TaskBlock /////////////////////////////////
TaskBlock::TaskBlock(void) 
    : m_blk_orig(0), m_blk_size(0), m_status(0) {}
////////////////////////////// class TaskBlock /////////////////////////////////
TaskBlock::~TaskBlock(void) {}
////////////////////////////// class TaskBlock /////////////////////////////////
TaskBlock::TaskBlock(const TaskBlock& src) {
  m_blk_orig = src.m_blk_orig;
  m_blk_size = src.m_blk_size;
  m_status = src.m_status;
}
////////////////////////////// class TaskBlock /////////////////////////////////
TaskBlock& TaskBlock::operator=(const TaskBlock& src) {
  if (this != &src) {
    m_blk_orig = src.m_blk_orig;
    m_blk_size = src.m_blk_size;
    m_status = src.m_status;
  }
  return *this;
}
////////////////////////////// class TaskBlock /////////////////////////////////
void TaskBlock::Print(void) {
  VrtLog::Write(" -- Task Block: origin = %u, size %u, status = %u", 
                m_blk_orig, m_blk_size, m_status);
}
//////////////////////////// class ClientServerExecutor ////////////////////////
ClientServerExecutor::ClientServerExecutor(void)
  : m_mpi_rank(0),
    m_nb_mpi_process(1),
    m_nb_openmp_process(1), 
    p_counter(NULL) {
}
//////////////////////////// class ClientServerExecutor ////////////////////////
ClientServerExecutor::~ClientServerExecutor(void) {
  EraseBlocks();

  if (p_counter) {
    delete [] p_counter;
    p_counter = NULL;
  }
}
//////////////////////////// class ClientServerExecutor ////////////////////////
void ClientServerExecutor::SetAdditionalParameters(int nb_params, ...) { 
  va_list(params);
  va_start(params, nb_params);

  if (nb_params == 3) {
    m_mpi_rank = va_arg(params, int);
    m_nb_mpi_process = va_arg(params, int);
    m_nb_openmp_process = va_arg(params, int);

    if (m_chunk < m_nb_openmp_process) {
      VrtLog::Write("WARNING: m_chunk( %d ) < m_nb_openmp_process( %d )",
                    m_chunk, m_nb_openmp_process);
    }
  
  // Bad number of parameters
  } else {
   va_end(params);
   throw Exception("(ClientServerExecutor::SetAdditionalParameters) Nombre \
incorrect de paramètres");
  }

  va_end(params);
}
//////////////////////////// class ClientServerExecutor ////////////////////////
void ClientServerExecutor::Initialize(Scenery* scenery, 
                                      const std::string& manager_class,
                                      int nb_params, ...) {
  // Assign Scenery
  p_scenery = scenery;

  // Server part
  if (m_mpi_rank == 0) {
    // Send renderer data to client
    SendRendererData();      

  // Client part
  } else {
    // Receive renderer data from the server
    ReceiveRendererData();
  }

  // Initialize the image
  InitializeImage();

  // Get the list of parameters
  std::vector<int> list_params;
  va_list(params);
  va_start(params, nb_params);
  for (int arg = 0; arg < nb_params; arg++)
    list_params.push_back(va_arg(params, int));
  va_end(params);
  // Initialize the task manager
  CreateTaskManager(manager_class, list_params);

  // Initialize the list of blocks
  InitializeBlocks();
  PrintBlocks();

  p_counter = new int [m_nb_mpi_process];
  for (int i = 0; i < m_nb_mpi_process; i++) p_counter[i] = 0;
  VrtLog::WriteArray("p_counter before Allgather", p_counter, m_nb_mpi_process);
  MPI::COMM_WORLD.Allgather(&m_blocks[0].m_blk_orig, 1, MPI_INT, p_counter, 1, MPI_INT);
  VrtLog::WriteArray("p_counter after Allgather", p_counter, m_nb_mpi_process);
}  
//////////////////////////// class ClientServerExecutor ////////////////////////
void ClientServerExecutor::CreateTaskManager(
    const std::string& manager_class,
    const std::vector<int>& manager_params) {

  // Create a new task manager (and delete the old one)
  EraseTaskManager();

  // Line Algorithm 
  if (manager_class.compare("Line") == 0) {
    p_task_mngr = new TaskManagerLine;
    // Spiral Algorithm
  } else if (manager_class.compare("Spiral") == 0) {
    p_task_mngr = new TaskManagerSpiral;
    // Unknown
  } else {
    throw Exception("This Task manager has not been implemented yet: " 
      + manager_class);
  }

  // Base paramaters
  p_task_mngr->SetBaseParameters(m_img_ulx, m_img_uly, 
    m_img_width, m_img_height,
    m_tsk_width, m_tsk_height);
  // Additional parameters
  for (unsigned int arg = 0; arg < manager_params.size(); arg++)
    p_task_mngr->SetAdditionalParameters((int)arg, manager_params[arg]);

  // Create Task list
  p_task_mngr->CreateTaskList();
  //p_task_mngr->PrintTaskList();
}
//////////////////////////// class ClientServerExecutor ////////////////////////
void ClientServerExecutor::SendRendererData(void) {
  unsigned char* data = NULL;
  unsigned int data_size = 0;

  p_scenery->getRenderer()->ExportData(&data, &data_size);

  // Traverse the list of clients
  for (int c = 1; c < m_nb_mpi_process; c++) {
    MPI::COMM_WORLD.Send(&data_size, 1, MPI_INT, 
                         c, kSIG_INITSIZE);			
    if (data_size > 0) {			
      MPI::COMM_WORLD.Send(data, data_size, MPI_UNSIGNED_CHAR, 
                           c, kSIG_INITDATA);
    }
  }  
}
//////////////////////////// class ClientServerExecutor ////////////////////////
void ClientServerExecutor::InitializeImage(void) {
  Camera* camera = p_scenery->getCamera(0);
  p_image = new Image(camera->getWidth(), camera->getHeight(), 
                      camera->getNumberOfChannels());
  for(unsigned int i = 0; i < camera->getNumberOfChannels(); i++) {
    p_image->setChannelName(i, camera->getChannelName(i));
  }
  p_image->clear();
}
//////////////////////////// class ClientServerExecutor ////////////////////////
void ClientServerExecutor::InitializeBlocks(void) {
  // Destroy previous list of blocks
  EraseBlocks();

  // Redefine the image chunk in the particular case when the must be 
  // automatically computed
  if (m_chunk == -1) {
    m_chunk = p_task_mngr->nb_tasks() / m_nb_openmp_process;
  }

  // Traverse the list of task units, create blocks and add them in the right 
  // place in the list of blocks
  for (unsigned int t = 0; t < p_task_mngr->nb_tasks(); t += m_chunk) {
    // dispatch the blocks among the MPI processes
    if ((t / m_chunk) % m_nb_mpi_process == m_mpi_rank) {
      TaskBlock block;
      // Create the current block
      block.m_blk_size = m_chunk;
      if (t + m_chunk > p_task_mngr->nb_tasks()) 
        block.m_blk_size = p_task_mngr->nb_tasks() - t;
      block.m_blk_orig = t;
      
      // Add to the block list of the right MPI process 
      m_blocks.push_back(block);
    }
  }
}
//////////////////////////// class ClientServerExecutor ////////////////////////
void ClientServerExecutor::PrintBlocks(void) {
  VrtLog::Write("\nList of blocks for process %d: ", m_mpi_rank);
  for (unsigned int b = 0; b < m_blocks.size(); b++) {
    m_blocks[b].Print();
  }
}
//////////////////////////// class ClientServerExecutor ////////////////////////
void ClientServerExecutor::ReceiveRendererData(void) {
  // reception status
	MPI::Status status;
	unsigned char* data=0;
 	unsigned int data_size;

  // datasize must be received
  MPI::COMM_WORLD.Recv(&data_size, 1, MPI_INT, 
                       0,  
                       kSIG_INITSIZE, 
                       status);			

  if (data_size > 0) {
    data = new unsigned char[data_size];
    memset(data, 0, data_size);					

    MPI::COMM_WORLD.Recv(data, data_size, MPI_UNSIGNED_CHAR, 
                         0, 
                         kSIG_INITDATA, 
                         status);

    // init slaves with data
    p_scenery->getRenderer()->InitWithData(*p_scenery, data, data_size);	
  }
}
////////////////////////////// class ClientServerExecutor ////////////////////////
//void ClientServerExecutor::Execute(void) {
//  // Get the first camera point of view (We assume there is only one camera 
//  // in the scene)
//  Camera* camera = p_scenery->getCamera(0);
//
//  // Counter to the number of blocks that has been computed
//  unsigned int b = 0;
//
//  // Private variables for OpenMP : each process has its own value
//  int i, ulx, uly, brx, bry;
//  // Shared variables for OpenMP : the same value for all processes
//  ImageParser parser;
//  //int nbtask = 0;
//
//  // Prepare the Open loop by defining the number of OpenMP processes to be used
//  omp_set_num_threads(m_nb_openmp_process);
//
//  // Execute all the task blocks without any MPI communications
//  while (b < m_blocks.size()) {
//    std::map<std::string, Image*> vec_img;
//
//    // OpenMP Loop
//#   pragma omp parallel for private(i, ulx, uly, brx, bry) schedule(dynamic, 1)
//    for (i = m_blocks[b].m_blk_orig; 
//         i < m_blocks[b].m_blk_orig + m_blocks[b].m_blk_size; i++) {         
//      // Get the current task unit for the current OpenMP process
//      p_task_mngr->GetTaskAt((unsigned int)i).Retrieve((unsigned int&)ulx, 
//                                                       (unsigned int&)uly, 
//                                                       (unsigned int&)brx, 
//                                                       (unsigned int&)bry);
//
//      // Compute this task unit
//      Image* img = new Image(brx - ulx, bry - uly, 
//                             p_image->getNumberOfChannels());
//      camera->local_takeshot(*p_scenery, ulx, brx, uly, bry, *img);    
//
//      // Save
//      char local_output[200];
//      sprintf(local_output, "%u-%u_%u_%u.png", ulx, brx, uly, bry);
//      vec_img.insert( std::pair<std::string, Image*>(std::string(local_output), 
//                                                     img) );
//
//    } // End of the OpenMP loop
//
//    std::map<std::string, Image*>::iterator it;
//    for (it = vec_img.begin(); it != vec_img.end(); it++) {
//      parser.save(*it->second, it->first);
//    }
//
//    b++;
//    std::cout << "[ " << m_mpi_rank << " ] " << b << " / " 
//              << m_blocks.size() << std::endl;
//  } // End of the block loop
//}
//////////////////////////// class ClientServerExecutor ////////////////////////
void ClientServerExecutor::Execute(void) {
  // Get the first camera point of view (We assume there is only one camera 
  // in the scene)
  Camera* camera = p_scenery->getCamera(0);

  // Counter to the number of blocks that has been computed
  unsigned int b = 0;

  // Private variables for OpenMP : each process has its own value
  int i, ulx, uly, brx, bry;
  // Shared variables for OpenMP : the same value for all processes
  ImageParser parser;
  //int nbtask = 0;

  // Prepare the Open loop by defining the number of OpenMP processes to be used
  omp_set_num_threads(m_nb_openmp_process);

  // Execute all the task blocks without any MPI communications
  while (b < m_blocks.size()) {
    std::map<std::string, Image*> vec_img;

    //MPI::COMM_WORLD.Allgather(&m_blocks[b].m_blk_orig, 1, MPI_INT, p_counter, 1, MPI_INT);
    //VrtLog::WriteArray("p_counter after Allgather", p_counter, m_nb_mpi_process);

    // OpenMP Loop
#   pragma omp parallel for private(i, ulx, uly, brx, bry) schedule(dynamic, 1)
    for (i = m_blocks[b].m_blk_orig; 
         i < m_blocks[b].m_blk_orig + m_blocks[b].m_blk_size; i++) {         
      // Get the current task unit for the current OpenMP process
      p_task_mngr->GetTaskAt((unsigned int)i).Retrieve((unsigned int&)ulx, 
                                                       (unsigned int&)uly, 
                                                       (unsigned int&)brx, 
                                                       (unsigned int&)bry);

      // Compute this task unit
      Image* img = new Image(brx - ulx, bry - uly, 
                             p_image->getNumberOfChannels());
      camera->local_takeshot(*p_scenery, ulx, brx, uly, bry, *img);    

      // Save
      char local_output[200];
      sprintf(local_output, "%u-%u_%u_%u.png", ulx, brx, uly, bry);
      vec_img.insert( std::pair<std::string, Image*>(std::string(local_output), 
                                                     img) );

    } // End of the OpenMP loop

    std::map<std::string, Image*>::iterator it;
    for (it = vec_img.begin(); it != vec_img.end(); it++) {
      parser.save(*it->second, it->first);
    }

    b++;
    std::cout << "[ " << m_mpi_rank << " ] " << b << " / " 
              << m_blocks.size() << std::endl;
  } // End of the block loop
}
////////////////////////////////////////////////////////////////////////////////

//  // STEP1 : Execute all the task blocks without any MPI communications
//  int end_process = -1;
//  while (b < m_blocks.size() && end_process == -1) {  
//    int sig = 0;
//    MPI_Status status;
//    MPI_Request request = MPI_REQUEST_NULL;
//
//    // OpenMP Loop
//#   pragma omp parallel for private(i, ulx, uly, brx, bry) schedule(dynamic, 1)
//    for (i = m_blocks[b].m_blk_orig; 
//         i < m_blocks[b].m_blk_orig + m_blocks[b].m_blk_size; i++) {         
//      // Get the current task unit for the current OpenMP process
//      p_task_mngr->GetTaskAt((unsigned int)i).Retrieve((unsigned int&)ulx, 
//                                                       (unsigned int&)uly, 
//                                                       (unsigned int&)brx, 
//                                                       (unsigned int&)bry);
//      // Compute this task unit
//      camera->takeShot(*p_scenery, ulx, brx, uly, bry, *p_image);    
//      m_blocks[b].m_status++;
//    
//    } // End of the OpenMP loop
//    b++;
//    std::cout << "[ " << m_mpi_rank << " ] " << b << " / " << m_blocks.size() << std::endl;
//
//    // Listen to other MPI process (non-blocking)
//    // If another MPI process has finished, then this is the last iteration
//    MPI_Irecv(&end_process, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &request);
//    MPI_Test(&request, &sig, &status);
//    if (end_process != -1) {
//      std::cout << "[ " << m_mpi_rank << " ] Receive value " << end_process << " from process " << status.MPI_SOURCE << std::endl;
//      continue;
//    }
//  } // End of the block loop
//
//  // Send a signal to say the first step of computation is over
//  MPI_Status status;
//  MPI_Request request = MPI_REQUEST_NULL;
//  for (int p = 0; p < m_nb_mpi_process; p++) {
//    if (p != m_mpi_rank) {
//      MPI_Isend(&m_mpi_rank, 1, MPI_INT, p, kSIG_ENDSTEP1, MPI_COMM_WORLD, &request);
//      MPI_Wait(&request, &status); 
//    }
//  }
//
//  char s[200];
//  sprintf(s, "./%d.png", m_mpi_rank);
//  parser.save(*p_image, std::string(s));

