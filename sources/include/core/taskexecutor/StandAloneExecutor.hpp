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
#ifndef GUARD_VRT_STANDALONEEXECUTOR_HPP
#define GUARD_VRT_STANDALONEEXECUTOR_HPP
//!
//! @file StandAloneExecutor.hpp
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
//! @class StandAloneExecutor
//! @brief Class for task parallel computing on a single computer
//! @details Stand-Alone executions assume they are working on a single machine 
//!  and only use openMP processes to run in parallel. The global number of
//!  TaskUnit are shared between the different processes and the variable 
//!  m_chunk (used in openMP scheduling) allows users to control the rendering, 
//!  more precisely: 
//!  @arg m_chunk = 1 => Every process will compute a task located in contigous 
//!   area of the image;
//!  @arg m_cjunk > 1 => Every process  will compute different chunks of the 
//!   images. ideal to quickly have an overview ot the rendering result.
class StandAloneExecutor : public TaskExecutorBase {
 public:
  //! @brief constructor
  StandAloneExecutor(void);
  //! @brief Destructor
  virtual ~StandAloneExecutor(void);
  //! @brief Defines all the additional parameters of the derived class
  //! @param nb_params Number of additional parameters
  //! @rermaks Use the following order for parameters
  //!  @arg 1: Number of OpenMP processes to be used
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

 private:
  //! Number of OpenMP processes to be used 
  int m_nb_openmp_process;
}; // class StandAloneExecutor
////////////////////////////////////////////////////////////////////////////////
#endif // GUARD_VRT_STANDALONEEXECUTOR_HPP
