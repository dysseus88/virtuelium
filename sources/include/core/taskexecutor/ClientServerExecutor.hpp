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
#ifndef GUARD_VRT_CLIENTSERVEREXECUTOR_HPP
#define GUARD_VRT_CLIENTSERVEREXECUTOR_HPP
//!
//! @file ClientServerExecutor.hpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details Base class for task scheduling in parallel computing
//!
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <math.h>

#include <core/taskexecutor/TaskExecutorBase.hpp>
#include <core/taskmanager/TaskManagerBase.hpp>
#include <core/taskmanager/TaskManagerLine.hpp>
#include <core/taskmanager/TaskManagerSpiral.hpp>
////////////////////////////////////////////////////////////////////////////////
//! @see Scenery
class Scenery;
////////////////////////////////////////////////////////////////////////////////
//! @class TaskBlock
//! @brief A task block is a set of contiguous task units
class TaskBlock {
 public:
  //! @brief Constructor
  TaskBlock(void);
  //! @brief Destructor
  ~TaskBlock(void); 
  //! @brief Copy constructor
  TaskBlock(const TaskBlock& src);
  //! @brief Assignement operator
  TaskBlock& operator=(const TaskBlock& src);
  
 public:
  //! @brief Print the task block
  void Print(void);
  //! @brief Get the ith task unit of the block
  //! @param local index of the task unit in the block
  //! @return Global index of the task unit
  inline unsigned int GetTaskUnit(unsigned int idx) { return m_blk_orig + idx; }
  //! @brief Increment the status
  inline void Increment(void) { m_status++; }
  //! @check if a block is finishes or not
  inline bool isFinished(void) { return (m_status >= m_blk_size); }

 public:
  //! Inf. Boundary of the block
  unsigned int m_blk_orig;
  //! Number of blocks (size of the block)
  unsigned int m_blk_size;
  //! Number of computed task units in the block
  unsigned int m_status;
}; // clss TaskBlock
////////////////////////////////////////////////////////////////////////////////
//! @class ClientServerExecutor
//! @brief Class for task parallel computing on a CPU cluster
//! @details Client-Server executions assume they are working on a set of 
//!  computers on a local area network. They are based on an hybrid 
//!  (MPI + OpenMP) parallelization mechanism. Only one MPI process must be 
//!  executed on each node. They manage network communications between client 
//!  nodes (workers) and a server node (master) for distributed computing. 
//!  Then, each MPI node shares its work between its different openMP 
//!  processses (shared computing). This hybrid approach has been implemented in 
//!  order to minimize memory allocations. The server works as a block manager.
class ClientServerExecutor : public TaskExecutorBase {
 public:
  //! @enum MPI communication flags 
  enum {
    kSIG_INITSIZE, 
    kSIG_INITDATA,
    kSIG_ENDGATHER
  };
 public:
  //! @brief constructor
  ClientServerExecutor(void);
  //! @brief Destructor
  virtual ~ClientServerExecutor(void);
  //! @brief Defines all the additional parameters of the derived class
  //! @param nb_params Number of additional parameters
  //! @rermaks Use the following order for parameters
  //!  @arg 1: Identifier of the MPI process
  //!  @arg 2: Number of MPI processes to be used
  //!  @arg 3: Number of OpenMP processes to be used
  virtual void SetAdditionalParameters(int nb_params, ...);

 public:
  //! @brief Initialize the executor
  //! @param scenery Scenery to be rendered
  //! @param manager_class Name of the TaskManager class to be instanced
  //! @param nb_params Number of additional parameters for the task manager
  virtual void Initialize(Scenery* scenery, const std::string& manager_class,
                          int nb_params, ...);
  //! @brief Get the executor to process
  virtual void Execute(void);

 public:
  //! @brief Initialize the the list of task blocks
  void InitializeBlocks(void);
  //! @brief Print the list of task blocks
  void PrintBlocks(void);
  //! @brief Delete p_blocks
  inline void EraseBlocks(void) {
    m_blocks.clear();
  }

 private:
  //! @brief Initialize the server with a task manager
  //! @param manager_class Name of the TaskManager class to be instanced
  //! @param manager_params List of additional params
  void CreateTaskManager(const std::string& manager_class,
                         const std::vector<int>& manager_params);
  //! @brief Initialize the image
  void InitializeImage(void);

 private:
  //! @brief [SERVER] Send renderer data to clients
  void SendRendererData(void);
 
 private:
  //! @brief [CLIENT] Receive renderer data from the server
  void ReceiveRendererData(void);

 private:
  //! Identifier of the MPI process. The server has a rank equal to 0
  int m_mpi_rank;
  //! Number of MPI processes to be used
  int m_nb_mpi_process;
  //! Number of OpenMP processes to be used 
  int m_nb_openmp_process;
  //! List of task blocks associated to each MPI clients
  std::vector<TaskBlock> m_blocks;

  int* p_counter;

// private:
  //! 

////! Fragmented image: each node will produce parts of the image in separate
  ////!  files
  //bool b_fragment;
}; // class ClientServerExecutor
////////////////////////////////////////////////////////////////////////////////
#endif // GUARD_VRT_CLIENTSERVEREXECUTOR_HPP

