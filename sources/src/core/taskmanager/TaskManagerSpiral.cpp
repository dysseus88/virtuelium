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
#include <core/taskmanager/TaskManagerSpiral.hpp>
//!
//! @file TaskManagerSpiral.cpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file implements classs declared in TaskManagerSpiral.hpp 
//!  @arg TaskManagerSpiral
//!
#include <core/VrtLog.hpp>

////////////////////////////// class TaskManagerSpiral ///////////////////////////
TaskManagerSpiral::TaskManagerSpiral(void)
    : m_spiral_dir(kTRIGO) {}
////////////////////////////// class TaskManagerSpiral ///////////////////////////
TaskManagerSpiral::~TaskManagerSpiral(void) {}
////////////////////////////// class TaskManagerSpiral ///////////////////////////
void TaskManagerSpiral::SetAdditionalParameters(int idx, int value) { 
  // First parameter
  if (idx == 0) {
    m_spiral_dir = value;
  
  // Bad number of parameters
  } else {
   throw Exception("(TaskManagerSpiral::SetAdditionalParameters) Nombre \
incorrect de paramètres");
  }
}
////////////////////////////// class TaskManagerSpiral ///////////////////////////
void TaskManagerSpiral::CreateTaskList(void) {
  // Create the task array
  EraseTasks();
  p_tasks = new TaskUnit[m_nb_tasks];
   
  // Create the spiral map 
  unsigned int* spiral_map = new unsigned int[m_nb_tasks];
  for (unsigned int i = 0; i < m_nb_tasks; i++)
    spiral_map[i] = m_img_width * m_img_height;

  // Look for the starting column
  int pivot_w = m_nb_tasks_per_width / 2;
  if (m_nb_tasks_per_width % 2 == 0 )
    pivot_w = m_nb_tasks_per_width / 2 - 1;

  // Look for starting row
  int pivot_h = pivot_h = m_nb_tasks_per_height / 2;
  if (m_nb_tasks_per_height % 2 == 0 )
    pivot_h = m_nb_tasks_per_height / 2 - 1;

  // Start direction is up for trigonometric rotation (will always try to turn 
  // left) or down in the other case (will always try to turn right)
  int dir_vector[2] = {0, m_spiral_dir};

  // Start the traversal
  RecursiveSpiral(pivot_w, pivot_h, spiral_map, dir_vector, 0);

  // The spiral map is not necessery anymore
  if (spiral_map != NULL) {
    delete [] spiral_map;
    spiral_map = NULL;
  }
}
////////////////////////////// class TaskManagerSpiral ///////////////////////////
void TaskManagerSpiral::RecursiveSpiral(unsigned int task_w, 
                                        unsigned int task_h, 
                                        unsigned int* spiral_map, 
                                        int* dir_vector,
                                        unsigned int task_count) {
  // The current task can treated only if the current position of the spiral is 
  // inside the image
  if (task_w >= 0 && task_w < m_nb_tasks_per_width 
   && task_h >= 0 && task_h < m_nb_tasks_per_height) {
    
    unsigned int task_idx = task_h * m_nb_tasks_per_width + task_w;
    // Marks the task
    spiral_map[task_idx] = task_count;     
    // Create area
    p_tasks[task_count] = CreateAreaFromTask(task_idx);    
    // Increment
    task_count++;
  }
    
  // The recursive spiral will end if all the tasks have been marked...
  if (task_count >=  m_nb_tasks)
    return;

  // If the spiral continue, then it always tries to turn left (trigo) or 
  // right (non-trigo). Otherwise, it keeps its previous direction
  if (SpiralCanTurn(task_w + (m_spiral_dir * dir_vector[1]), 
                    task_h + (-m_spiral_dir * dir_vector[0]),
                    spiral_map) == true) {
    // Change direction
    int tmp = dir_vector[0];
    dir_vector[0] = m_spiral_dir * dir_vector[1];
    dir_vector[1] = -m_spiral_dir * tmp;
  }

  /// Next step
  RecursiveSpiral(task_w + dir_vector[0], task_h + dir_vector[1],
                  spiral_map, dir_vector, task_count);
}
////////////////////////////// class TaskManagerSpiral ///////////////////////////
bool TaskManagerSpiral::SpiralCanTurn(unsigned int task_w, 
                                      unsigned int task_h,
                                      unsigned int* spiral_map) {

  // If the spiral is currently outside the image, then it can always turn
  // This is a particular case
  if (task_w < 0 || task_w >= m_nb_tasks_per_width 
      || task_h < 0 || task_h >= m_nb_tasks_per_height)
    return true;
  // The spiral cannot turn to a task that has already been traversed
  // A task is not traversed when its value in the spiral map is inferior to
  // m_img_width * m_img_height (seems to be Ok in every case)
  unsigned int task_idx = task_h * m_nb_tasks_per_width + task_w;
  if (spiral_map[task_idx] != m_img_width * m_img_height) 
    return false;
  // Otherwise it is ok for turning
  return true;
}
////////////////////////////////////////////////////////////////////////////////


