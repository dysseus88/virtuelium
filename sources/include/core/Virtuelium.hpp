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
#ifndef GUARD_VRT_VIRTUELIUM_HPP
#define GUARD_VRT_VIRTUELIUM_HPP
//!
//! @file Virtuelium.hpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file is the main file of the program
//!
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include <signal.h>
#include <mpi.h>
#include <vector>

#include <exceptions/Exception.hpp>

#include <structures/HashMap.hpp>
#include <structures/HashFunctors.hpp>

#include <io/GlobalSceneryParser.hpp>

#include <raytracers/MPIServer.hpp>
#include <raytracers/MPIClients.hpp>
////////////////////////////////////////////////////////////////////////////////
//! @see TaskExecutorBase
class TaskExecutorBase;
////////////////////////////////////////////////////////////////////////////////
//! @class Virtuelium
//! @brief This class is responsible of launching all the processes
//! @desc After reading the parameter list, Virtuelium is executed in several
//!  step:
//!  @arg #1 The scenery file (XML) is parsed. At this time, we have sort 
//!   the geometrical primitives in an acceletation tree, have instanced 
//!   material and light objects, have created renderer and camera objects 
//!   etc...
//!  @arg #2 The rendering engine is initialized. In simple ray tracers 
//!   nothing is done. In Photon Mapping, photon maps are built
//!  @arg #3 The MPI threads are initialized by exchanging previous data
//!  @arg #4 The ray tracing algorithm is processed and the image is built
class Virtuelium {
 public:
  //! @brief Constructor
  Virtuelium(int argc, char* argv[]);
  //! @brief Destructor
  ~Virtuelium(void);
  
 public:
   //! @brief Access to m_mpi_rank
   const inline int mpi_rank(void) const { return m_mpi_rank; }
   //! @brief Access to m_nb_mpi_procs
   const inline int nb_mpi_procs(void) const { return m_nb_mpi_procs; }
   //! @brief Access to m_nb_omp_procs
   const inline int nb_omp_procs(void) const { return m_nb_omp_procs; }
   //! @brief Access to m_xmin
   const inline int xmin(void) const { return m_xmin; }
   //! @brief Access to m_xmax
   const inline int xmax(void) const { return m_xmax; }
   //! @brief Access to m_ymin
   const inline int ymin(void) const { return m_ymin; }
   //! @brief Access to m_ymax
   const inline int ymax(void) const { return m_ymax; }
   //! @brief Access to b_debug
   const inline bool debug(void) const { return b_debug; }
   //! @brief Access to b_brdf
   const inline bool brdf(void) const { return b_brdf; }
   //! @brief Access to m_brdf_step
   const inline int brdf_step(void) const { return m_brdf_step; }
   //! @brief Access to b_overwrite
   const inline bool is_overwrite(void) const { return b_overwrite; }
   //! @brief Access to b_fragment
   const inline bool fragment(void) const { return b_fragment; }
   //! @brief Access to m_scenery_filename
   const inline char* scenery_filename(void) const { 
    return m_scenery_filename.c_str();
   }
   //! @brief Access to p_scenery
   const inline Scenery* scenery(void) const { return p_scenery; }
   //! @brief Access to p_exec
   const inline TaskExecutorBase* exec_task(void) const { return p_exec; }
   //! @brief Access to m_algorithm
   const inline char* algorithm(void) const { return m_algorithm.c_str(); }
   //! @brief Access to m_algo_params
   const inline int algo_params(unsigned int i) const { 
     return m_algo_params[i]; 
   }

 public:
  //! @brief Read the argument list of the program and extract needed parameters
  //! @param argc Argument count
  //! @param argc Argument list
  //! @param parameters Hashmap that will contain all the loaded parameters
  void ReadParameters(int argc, char* argv[]);
  //! @brief Initialize the scenery
  void CheckDebugMode(void); 
  //! @brief Initialize the scenery
  void InitializeScenery(void); 
  //! @brief Initialize the renderer engine
  void InitializeRenderer(void); 
  //! @brief Initialize the task executor
  void InitializeExecutor(void); 
  //! @brief Launch the client and server with m_rank
  void Execute(void); 
  //! @brief Trace a BRDF with regular angle steps
  void TraceBRDF(const int& step);

 private:
  //! @brief Parse the area string and allocate a bounding box data
  //! @details The area string is formatted as followed : xmin:ymin:xmax:ymax
  //! @param area_str bounding box area in string format
  //! @param xmin X-coordinate of the upper left corner
  //! @param ymin Y-coordinate of the upper left corner
  //! @param xmax X-coordinate of the bottom right corner
  //! @param ymax Y-coordinate of the bottom right corner
  //! @return True if parsing has been ok
  bool ParseAreaParam(const char* area_str, 
                      int& xmin, int& ymin, int& xmax, int& ymax);
  //! @brief Parse algorithm
  //! @param str_line value the command line argument --line
  //! @param str_line_snake value the command line argument --line-snake
  //! @param spiral_trigo value the command line argument --spiral-trigo
  //! @param spiral_inverse value the command line argument --spiral-inverse
  void ParseAlgoParam(const std::string& str_line,
                       const std::string& str_line_snake,
                       bool spiral_trigo, bool spiral_inverse);
  //! @brief Save the binary data of the rendering init to m_save_init_file
  //! @param data Binary data
  //! @param data_size Size of binary data
  void SaveInit(unsigned char* data, unsigned int data_size);
  //! @brief Load the binary data of the rendering init to m_save_init_file
  //! @param data Binary data
  //! @param data_size Size of binary data
  void LoadInit(unsigned char** data, unsigned int* data_size);

 private:
  //! Process ID in MPI world
  int m_mpi_rank;
  //! Number of MPI process
  int m_nb_mpi_procs;
  //! Number of openMP process 
  int m_nb_omp_procs;
  //! Bounding box area delimiters
  int m_xmin, m_ymin, m_xmax, m_ymax;
  //! Size of a task unit
  int m_tsk_w, m_tsk_h;
  //! Debug mode
  bool b_debug;
  //! BRDF creator mode
  bool b_brdf;
  //! BRDF sampling step
  int m_brdf_step;
  //! Override mode
  bool b_overwrite;
  //! Fragmented image: each node will produce parts of the image in separate
  //!  files
  bool b_fragment;
  //! Scenery filename
  std::string m_scenery_filename;
  //! Scenery objects
  Scenery* p_scenery;
  //! TaskExecutor
  TaskExecutorBase* p_exec;
  //! Algorithm name
  std::string m_algorithm;
  //! Algorithm parameter
  std::vector<int> m_algo_params;
  //! Number of TaskUnit to be rendered between two saving operations
  unsigned int m_nb_task_refresh;
  //! Size of a computing bloc (in number of TaskUnit)
  //! In Stand-Alone execution, this is the chunk value of openMP scheduling 
  //!  function. In Client-Server execution, this determines how the is 
  //!  decomposed and shared between the different MPI nodes;
  //!  If m_chunk = -1, then the decompisition is automatically set to give a 
  //!  quick overview of the rendering result
  int m_chunk;
  //! Binary file for saving rendering init maps
  std::string  m_save_init_file;
  //! Binary file for loading rendering init maps
  std::string m_load_init_file;
}; // clas Virtuelium
////////////////////////////////////////////////////////////////////////////////
#endif // GUARD_VRT_VIRTUELIUM_HPP