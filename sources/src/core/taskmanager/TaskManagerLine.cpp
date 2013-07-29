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
#include <core/taskmanager/TaskManagerLine.hpp>
//!
//! @file TaskManagerLine.cpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file implements classs declared in TaskManagerLine.hpp 
//!  @arg TaskManagerLine
//!
#include <core/VrtLog.hpp>
////////////////////////////// class TaskManagerLine ///////////////////////////
TaskManagerLine::TaskManagerLine(void) {
  m_reading_dir = kLRTB;
  m_snake = 0;
}
////////////////////////////// class TaskManagerLine ///////////////////////////
TaskManagerLine::~TaskManagerLine(void) {}
////////////////////////////// class TaskManagerLine ///////////////////////////
void TaskManagerLine::SetAdditionalParameters(int idx, int value) {
  // First parameter
  if (idx == 0) {
    m_reading_dir = value;
  
  // Second parameter
  } else if (idx == 1) {
    m_snake = value;

  // Bad number of parameters
  } else {
   throw Exception("(TaskManagerLine::SetAdditionalParameters) Nombre \
incorrect de paramètres");
  }
}
////////////////////////////// class TaskManagerLine ///////////////////////////
void TaskManagerLine::CreateTaskList(void) {
  // Create the task array
  EraseTasks();
  p_tasks = new TaskUnit[m_nb_tasks];

  switch (m_reading_dir) {
  case kLRTB:
    CreateTaskList_LRTB();
    break;

  case kLRBT:
    CreateTaskList_LRBT();
    break;

  case kRLTB:
    CreateTaskList_RLTB();
    break;

  case kRLBT:
    CreateTaskList_RLBT();
    break;

  case kTBLR:
    CreateTaskList_TBLR();
    break;

  case kTBRL:
    CreateTaskList_TBRL();
    break;

  case kBTLR:
    CreateTaskList_BTLR();
    break;

  case kBTRL:
    CreateTaskList_BTRL();
    break;

  default:
    CreateTaskList_LRTB();
    break;
  }
}
////////////////////////////// class TaskManagerLine ///////////////////////////
void TaskManagerLine::CreateTaskList_LRTB(void) {
  unsigned int task_count = 0;

  for (unsigned int h = 0; h < m_nb_tasks_per_height; h++) {
    // Standard reading direction for the row
    if (h % 2 == 0 || m_snake == false) {
      for (unsigned int w = 0; w < m_nb_tasks_per_width; w++) {
        // Create area
        p_tasks[task_count] = CreateAreaFromTask(h * m_nb_tasks_per_width + w);    
        // Increment
        task_count++;
      }
    // Inverse reading direction for the row
    } else { 
      for (int ww = m_nb_tasks_per_width - 1; ww >= 0; ww--) {
        unsigned int w = static_cast<unsigned int>(ww);
        // Create area
        p_tasks[task_count] = CreateAreaFromTask(h * m_nb_tasks_per_width + w);    
        // Increment
        task_count++;
      }
    }
  }
}
////////////////////////////// class TaskManagerLine ///////////////////////////
void TaskManagerLine::CreateTaskList_LRBT(void) {
  unsigned int task_count = 0;

  for (int hh = m_nb_tasks_per_height - 1; hh >= 0; hh--) {
    unsigned int h = static_cast<unsigned int>(hh);
    // Standard reading direction for the row
    if (h % 2 == 0 || m_snake == false) {
      for (int ww = m_nb_tasks_per_width - 1; ww >= 0; ww--) {
        unsigned int w = static_cast<unsigned int>(ww);
        // Create area
        p_tasks[task_count] = CreateAreaFromTask(h * m_nb_tasks_per_width + w);    
        // Increment
        task_count++;
      }
    // Inverse reading direction for the row
    } else { 
      for (unsigned int w = 0; w < m_nb_tasks_per_width; w++) {
        // Create area
        p_tasks[task_count] = CreateAreaFromTask(h * m_nb_tasks_per_width + w);    
        // Increment
        task_count++;
      }
    }
  }
}
////////////////////////////// class TaskManagerLine ///////////////////////////
void TaskManagerLine::CreateTaskList_RLTB(void) {
  unsigned int task_count = 0;

  for (unsigned int h = 0; h < m_nb_tasks_per_height; h++) {
    // Standard reading direction for the row
    if (h % 2 == 0 || m_snake == false) {
      for (int ww = m_nb_tasks_per_width - 1; ww >= 0; ww--) {
        unsigned int w = static_cast<unsigned int>(ww);
        // Create area
        p_tasks[task_count] = CreateAreaFromTask(h * m_nb_tasks_per_width + w);    
        // Increment
        task_count++;
      }
    // Inverse reading direction for the row
    } else { 
      for (unsigned int w = 0; w < m_nb_tasks_per_width; w++) {
        // Create area
        p_tasks[task_count] = CreateAreaFromTask(h * m_nb_tasks_per_width + w);    
        // Increment
        task_count++;
      }
    }
  }
}
////////////////////////////// class TaskManagerLine ///////////////////////////
void TaskManagerLine::CreateTaskList_RLBT(void) {
  unsigned int task_count = 0;

  for (int hh = m_nb_tasks_per_height - 1; hh >= 0; hh--) {
    unsigned int h = static_cast<unsigned int>(hh);
    // Standard reading direction for the row
    if (h % 2 == 0 || m_snake == false) {
      for (unsigned int w = 0; w < m_nb_tasks_per_width; w++) {
        // Create area
        p_tasks[task_count] = CreateAreaFromTask(h * m_nb_tasks_per_width + w);    
        // Increment
        task_count++;
      }
    // Inverse reading direction for the row
    } else { 
      for (int ww = m_nb_tasks_per_width - 1; ww >= 0; ww--) {
        unsigned int w = static_cast<unsigned int>(ww);
        // Create area
        p_tasks[task_count] = CreateAreaFromTask(h * m_nb_tasks_per_width + w);    
        // Increment
        task_count++;
      }
    }
  }
}

////////////////////////////// class TaskManagerLine ///////////////////////////
void TaskManagerLine::CreateTaskList_TBLR(void) {
  unsigned int task_count = 0;

  for (unsigned int w = 0; w < m_nb_tasks_per_width; w++) {
    // Standard reading direction for the column
    if (w % 2 == 0 || m_snake == false) {
      for (unsigned int h = 0; h < m_nb_tasks_per_height; h++) {
        // Create area
        p_tasks[task_count] = CreateAreaFromTask(h * m_nb_tasks_per_width + w);    
        // Increment
        task_count++;
      }
    // Inverse reading direction for the column
    } else { 
      for (int hh = m_nb_tasks_per_height - 1; hh >= 0; hh--) {
        unsigned int h = static_cast<unsigned int>(hh);
        // Create area
        p_tasks[task_count] = CreateAreaFromTask(h * m_nb_tasks_per_width + w);    
        // Increment
        task_count++;
      }
    }
  }
}
////////////////////////////// class TaskManagerLine ///////////////////////////
void TaskManagerLine::CreateTaskList_TBRL(void) {
  unsigned int task_count = 0;

  for (int ww = m_nb_tasks_per_width - 1; ww >= 0; ww--) {
    unsigned int w = static_cast<unsigned int>(ww);
    // Standard reading direction for the column
    if (w % 2 == 0 || m_snake == false) {
      for (int hh = m_nb_tasks_per_height - 1; hh >= 0; hh--) {
        unsigned int h = static_cast<unsigned int>(hh);
        // Create area
        p_tasks[task_count] = CreateAreaFromTask(h * m_nb_tasks_per_width + w);    
        // Increment
        task_count++;
      }
    // Inverse reading direction for the column
    } else { 
      for (unsigned int h = 0; h < m_nb_tasks_per_height; h++) {
        // Create area
        p_tasks[task_count] = CreateAreaFromTask(h * m_nb_tasks_per_width + w);    
        // Increment
        task_count++;
      }
    }
  }
}
////////////////////////////// class TaskManagerLine ///////////////////////////
void TaskManagerLine::CreateTaskList_BTLR(void) {
  unsigned int task_count = 0;

  for (unsigned int w = 0; w < m_nb_tasks_per_width; w++) {
    // Standard reading direction for the column
    if (w % 2 == 0 || m_snake == false) {
      for (int hh = m_nb_tasks_per_height - 1; hh >= 0; hh--) {
        unsigned int h = static_cast<unsigned int>(hh);
        // Create area
        p_tasks[task_count] = CreateAreaFromTask(h * m_nb_tasks_per_width + w);    
        // Increment
        task_count++;
      }
    // Inverse reading direction for the column
    } else { 
      for (unsigned int h = 0; h < m_nb_tasks_per_height; h++) {
        // Create area
        p_tasks[task_count] = CreateAreaFromTask(h * m_nb_tasks_per_width + w);    
        // Increment
        task_count++;
      }
    }
  }
}
////////////////////////////// class TaskManagerLine ///////////////////////////
void TaskManagerLine::CreateTaskList_BTRL(void) {
  unsigned int task_count = 0;

  for (int ww = m_nb_tasks_per_width - 1; ww >= 0; ww--) {
    unsigned int w = static_cast<unsigned int>(ww);
    // Standard reading direction for the column
    if (w % 2 == 0 || m_snake == false) {
      for (unsigned int h = 0; h < m_nb_tasks_per_height; h++) {
        // Create area
        p_tasks[task_count] = CreateAreaFromTask(h * m_nb_tasks_per_width + w);    
        // Increment
        task_count++;
      }
    // Inverse reading direction for the column
    } else { 
      for (int hh = m_nb_tasks_per_height - 1; hh >= 0; hh--) {
        unsigned int h = static_cast<unsigned int>(hh);
        // Create area
        p_tasks[task_count] = CreateAreaFromTask(h * m_nb_tasks_per_width + w);    
        // Increment
        task_count++;
      }
    }
  }
}
////////////////////////////////////////////////////////////////////////////////


