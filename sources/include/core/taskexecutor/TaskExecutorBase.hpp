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
#ifndef GUARD_VRT_TASKEXECUTERBASE_HPP
#define GUARD_VRT_TASKEXECUTERBASE_HPP
//!
//! @file TaskExecutorBase.hpp
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
#include <vector>
////////////////////////////////////////////////////////////////////////////////
//! @see TaskManagerBase
class TaskManagerBase;
////////////////////////////////////////////////////////////////////////////////
//! @see Scenery
class Scenery;
//! @see Image
class Image;
////////////////////////////////////////////////////////////////////////////////
//! @class TaskExecutorBase
//! @brief Base class for task parallel computing
//! @details This the base abstract class. See inherited classed for more 
//!  details.
class TaskExecutorBase {
 public:
  //! @enum RenderingAlgo
  //! @brief Rendering algorithm tha must bu used for the manager
  typedef enum {
    kLINE,
    kSPIRAL
  } RenderingAlgo;  

 public:
  //! @brief constructor
  TaskExecutorBase(void);
  //! @brief Destructor
  virtual ~TaskExecutorBase(void);
  //! @brief Defines all parameters of the base class TaskExecutorBase
  //! @param img_ulx X Coord of the upper-left corner of the area of the image
  //! @param img_uly Y Coord of the upper-left corner of the area of the image
  //! @param img_brx X Coord of the bottom-right corner of the area of the image 
  //! @param img_bry Y Coord of the bottom-right corner of the area of the image
  //! @param tsk_width Width of one task area
  //! @param tsk_height Height of one task area
  //! @param nb_task_refresh Number of tasks to be rendered between two savings 
  //! @param ovrride Override the image
  //! @param chunk Size of a computing bloc (in number of TaskUnit)
  void SetBaseParameters(unsigned int img_ulx, unsigned int img_uly,
                         unsigned int img_brx, unsigned int img_bry,
                         unsigned int tsk_width, unsigned int tsk_height,
                         unsigned int nb_task_refresh, bool ovrride, 
                         int m_chunk);
  //! @brief Defines all the additional parameters of the derived class
  //! @param nb_params Number of additional parameters
  virtual void SetAdditionalParameters(int nb_params, ...) = 0;

 public:
  //! @brief Initialize the executor
  //! @param scenery Scenery to be rendered
  //! @param manager_class Name of the TaskManager class to be instanced
  //! @param nb_params Number of additional parameters for the task manager
  virtual void Initialize(Scenery* scenery, const std::string& manager_class,
                          int nb_params, ...) = 0;
  //! @brief Get the executor to process
  virtual void Execute(void) = 0;

 protected:
  //! @brief Delete p_task_mngr
   inline void EraseTaskManager(void) { 
     if (p_task_mngr != NULL) {
       delete p_task_mngr;
       p_task_mngr = NULL;
     }
   }
  //! @brief Delete p_scenery
   inline void EraseScenery(void) { 
     if (p_scenery != NULL) {
       delete p_scenery;
       p_scenery = NULL;
     }
   }
  //! @brief Delete p_image
   inline void EraseImage(void) { 
     if (p_image != NULL) {
       delete p_image;
       p_image = NULL;
     }
   }

 protected:
  //! X Coord of the upper-left corner of the area of the image to be rendered
  unsigned int m_img_ulx;
  //! Y Coord of the upper-left corner of the area of the image to be rendered
  unsigned int m_img_uly;
  //! Width of the area of the image to be rendered
  unsigned int m_img_width;
  //! Height of the area of the image to be rendered
  unsigned int m_img_height;
  //! Width of one task area
  unsigned int m_tsk_width;
  //! Height of one task area
  unsigned int m_tsk_height;
  //! Associated Task Manager
  TaskManagerBase* p_task_mngr;
  //! Scenery to be rendered
  Scenery* p_scenery;
  //! Number of TaskUnit to be rendered between two saving operations
  unsigned int m_nb_task_refresh;
  //! Image data to be computed
  Image* p_image;
  //! Override mode
  bool b_overwrite;
  //! Size of a computing bloc (in number of TaskUnit)
  //! In Stand-Alone execution, this is the chunk value of openMP scheduling 
  //!  function. In Client-Server execution, this determines how the is 
  //!  decomposed and shared between the different MPI nodes
  int m_chunk;
}; // class TaskExecutorBase
////////////////////////////////////////////////////////////////////////////////
#endif // GUARD_VRT_TASKEXECUTERBASE_HPP
