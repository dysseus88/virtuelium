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
#ifndef GUARD_VRT_TASKMANAGERSPIRAL_HPP
#define GUARD_VRT_TASKMANAGERSPIRAL_HPP
//!
//! @file TaskManagerSpiral.hpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details Derived class for task scheduling in parallel computing
//!
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <math.h>

#include <core/taskmanager/TaskManagerBase.hpp>
////////////////////////////////////////////////////////////////////////////////
//! @class TaskManagerSpiral
//! @brief Base class for task scheduling in parallel computing
class TaskManagerSpiral: public TaskManagerBase {
 public:
  //! @enum SpiralDir
  //! @brief Spiral directions
  enum {
    kNONTRIGO = -1,
    kTRIGO = 1
  }; 

 public:
  //! @brief constructor
  TaskManagerSpiral(void);
  //! @brief Destructor
  virtual ~TaskManagerSpiral(void);
  //! @brief Defines all the additional parameters of the derived class
  //! @param idx Position of the parameter
  //! @param value Value of the parameter
  virtual void SetAdditionalParameters(int idx, int value);

 public:
  //! @brief Initialize the list of task
  virtual void CreateTaskList(void);

 private:
  //! @brief Reccursively Fill the list of tasks by following a spiral
  //! @param task_w Column number of the current task
  //! @param task_h, Line number of the current task
  //! @param spiral_map Array where the spiral traversal is followed. Its size 
  //!  must be equal to the global number of tasks
  //! @param dir_vector 2D Vector representing the current direction of the 
  //!  spiral in the map; determine the next task to be traversed
  //! @param task_count Count the node that belongs to the MPI process.
  void RecursiveSpiral(unsigned int task_w, unsigned int task_h, 
                       unsigned int* spiral_map, int* dir_vector,
                       unsigned int task_count);
  //! @brief Return true if the spiral can turn, false, to go straight on
  //! @remark The spiral can virtually go outside the boundaries of the image, 
  //!  and it alwyas can turn when it is outside the image
  //! @param task_w Column number of the current task
  //! @param task_h, Line number of the current task
  //! @param spiral_map Array where the spiral traversal is followed. Its size 
  //!  must be equal to the global number of tasks
  //! @return True, if the spiral can turn, false otherwise
  bool SpiralCanTurn(unsigned int task_w, unsigned int task_h, 
                     unsigned int* spiral_map);

 private:
  //! Orientation of the spiral
  int m_spiral_dir;
}; // class TaskManagerSpiral
////////////////////////////////////////////////////////////////////////////////
#endif // GUARD_VRT_TASKMANAGERSPIRAL_HPP
