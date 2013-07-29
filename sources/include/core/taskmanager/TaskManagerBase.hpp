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
#ifndef GUARD_VRT_TASKMANAGERBASE_HPP
#define GUARD_VRT_TASKMANAGERBASE_HPP
//!
//! @file TaskManagerBase.hpp
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
////////////////////////////////////////////////////////////////////////////////
//! @class TaskUnit
//! @brief Area of the image that is rendered by one MPI (or openMP) process
//! @details The image to be rendered is decomposed into smaller regions. Each 
//!  area is then identified by the upper-left point and the bottom-right point.
//! @remarks The origin of the image is assumed to be at the upper-left corner 
//!  but depending on the output format, this is not always true, so this kind 
//!  of images will be computed with a wrong orientation
class TaskUnit {
 public:
  //! @brief Constructor
  TaskUnit(void);
  //! @brief Destructor
  ~TaskUnit(void);
  //! @brief Copy constructor
  TaskUnit(const TaskUnit& src);
  //! @brief Assignement operator
  TaskUnit& operator=(const TaskUnit& src);
  
 public:
  //! @brief Attach an sub-area of the image to the task unit
  //! @param ulx X-coordinate of the upper-left corner
  //! @param uly Y-coordinate of the upper-left corner
  //! @param brx X-coordinate of the bottom-right corner
  //! @param bry Y-coordinate of the bottom-right corner
  void Define(unsigned int ulx, unsigned int uly, 
              unsigned int brx, unsigned int bry);
  //! @brief Attach an sub-area of the image to the task unit
  //! @param ulx X-coordinate of the upper-left corner
  //! @param uly Y-coordinate of the upper-left corner
  //! @param brx X-coordinate of the bottom-right corner
  //! @param bry Y-coordinate of the bottom-right corner
  void Retrieve(unsigned int& ulx, unsigned int& uly, 
                unsigned int& brx, unsigned int& bry);
  //! @brief Print the task unit
  void Print(void);
  
 private:
  //! The upper-left corner of the task unit is the point with minimal 
  //! coordinates x
  unsigned int m_ulx;
  //! The upper-left corner of the task unit is the point with minimal 
  //! coordinates y
  unsigned int m_uly;
  //! The bottom-right corner of the task unit is the point with maximal
  //! coordinates x
  unsigned int m_brx;
  //! The bottom-right corner of the task unit is the point with maximal
  //! coordinates y
  unsigned int m_bry;
}; // class TaskUnit
////////////////////////////////////////////////////////////////////////////////
//! @class TaskManagerBase
//! @brief Base class for task scheduling in parallel computing
class TaskManagerBase {
 public:
  //! @brief constructor
  TaskManagerBase(void);
  //! @brief Destructor
  virtual ~TaskManagerBase(void);
  //! @brief Defines all parameters of the base class TaskManagerBase
  //! @param img_ulx X Coord of the upper-left corner of the area of the image
  //! @param img_uly Y Coord of the upper-left corner of the area of the image
  //! @param img_width X Width of the area of the image to be rendered
  //! @param img_height Y Height of the area of the image to be rendered
  //! @param tsk_width Width of one task unit
  //! @param tsk_height Height of one task unit
  void SetBaseParameters(unsigned int img_ulx, unsigned int img_uly,
                         unsigned int img_width, unsigned int img_height,
                         unsigned int tsk_width, unsigned int tsk_height);
  //! @brief Defines all the additional parameters of the derived class
  //! @param idx Position of the parameter
  //! @param value Value of the parameter
  virtual void SetAdditionalParameters(int idx, int value) = 0;

 public:
  //! @brief Initialize the list of task
   virtual void CreateTaskList(void) = 0;
  //! @brief Print the list of task in the log file
  void PrintTaskList(void);
  //! @brief Acces to m_nb_tasks
  //! @return Size of the list of tasks
  inline const unsigned int nb_tasks(void) const { return m_nb_tasks; }
  //! @brief Acces to the task at position idx in the list
  //! @param idx Position of the task to be retrieved
  //! @return A task is an instance of TaskUnit
  inline TaskUnit& GetTaskAt(unsigned int idx) const { return p_tasks[idx]; }
  //! @brief Create Get the image area matching the current node
  //! @param task_idx Position of the current task
  //! @return Bounding box defining the image area
  TaskUnit CreateAreaFromTask(unsigned int task_idx);

 protected:
  //! @brief Delete the array of TaskUnit
   inline void EraseTasks(void) { 
     if (p_tasks != NULL) {
       delete [] p_tasks;
       p_tasks = NULL;
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
  //! Width of one task unit
  unsigned int m_tsk_width;
  //! Height of one task unit
  unsigned int m_tsk_height;
  //! List of tasks for the MPI process
  TaskUnit* p_tasks;
  //! Number of tasks for this MPI process
  unsigned int m_nb_tasks;
  //! Number of task unit in one row
  unsigned int m_nb_tasks_per_width;
  //! Number of task unit in one column
  unsigned int m_nb_tasks_per_height;
}; // class TaskManagerBase
////////////////////////////////////////////////////////////////////////////////
#endif // GUARD_VRT_TASKMANAGERBASE_HPP
