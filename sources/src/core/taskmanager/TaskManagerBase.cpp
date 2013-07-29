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
#include <core/taskmanager/TaskManagerBase.hpp>
//!
//! @file TaskManagerBase.cpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file implements classs declared in TaskManagerBase.hpp 
//!  @arg TaskUnit
//!  @arg TaskManagerBase
//!
#include <core/VrtLog.hpp>
////////////////////////////// class TaskUnit ////////////////////////////////
TaskUnit::TaskUnit(void) 
    : m_ulx(0), m_uly(0),
      m_brx(0), m_bry(0) {}
////////////////////////////// class TaskUnit ////////////////////////////////
TaskUnit::~TaskUnit(void) {}
////////////////////////////// class TaskUnit ////////////////////////////////
TaskUnit::TaskUnit(const TaskUnit& src) {
  m_ulx = src.m_ulx;
  m_uly = src.m_uly;
  m_brx = src.m_brx;
  m_bry = src.m_bry;
}
////////////////////////////// class TaskUnit ////////////////////////////////
TaskUnit& TaskUnit::operator=(const TaskUnit& src) {
  if (this != &src) {
    m_ulx = src.m_ulx;
    m_uly = src.m_uly;
    m_brx = src.m_brx;
    m_bry = src.m_bry;
  }
  return *this;
}
////////////////////////////// class TaskUnit ////////////////////////////////
void TaskUnit::Define(unsigned int ulx, unsigned int uly, 
                      unsigned int brx, unsigned int bry) {
  m_ulx = ulx;
  m_uly = uly;
  m_brx = brx;
  m_bry = bry; 
}
////////////////////////////// class TaskUnit ////////////////////////////////
void TaskUnit::Retrieve(unsigned int& ulx, unsigned int& uly, 
                        unsigned int& brx, unsigned int& bry) {
  ulx = m_ulx;
  uly = m_uly;
  brx = m_brx;
  bry = m_bry;
}
////////////////////////////// class TaskUnit ////////////////////////////////
void TaskUnit::Print(void) {
  VrtLog::Write(" -- Task Unit: Top-left (%u, %u); Bottom-right (%u, %u)", 
                m_ulx, m_uly, m_brx, m_bry);
}
////////////////////////////// class TaskManagerBase ///////////////////////////
TaskManagerBase::TaskManagerBase(void)
    : m_img_ulx(0),
      m_img_uly(0),
      m_img_width(0),
      m_img_height(0),
      m_tsk_width(0),
      m_tsk_height(0),
      p_tasks(NULL),
      m_nb_tasks(0) {}
////////////////////////////// class TaskManagerBase ///////////////////////////
TaskManagerBase::~TaskManagerBase(void) {
  EraseTasks();
}
////////////////////////////// class TaskManagerBase ///////////////////////////
void TaskManagerBase::SetBaseParameters(unsigned int img_ulx, 
                                        unsigned int img_uly,
                                        unsigned int img_width, 
                                        unsigned int img_height,
                                        unsigned int tsk_width, 
                                        unsigned int tsk_height) {
  m_img_ulx = img_ulx;
  m_img_uly = img_uly;
  m_img_width = img_width;
  m_img_height = img_height;
  m_tsk_width = tsk_width;
  m_tsk_height = tsk_height;

  // Compute the number of tasks per width
  m_nb_tasks_per_width = m_img_width / m_tsk_width;
  if (m_img_width % m_tsk_width != 0)
     m_nb_tasks_per_width += 1;

  // Compute the number of tasks per height
  m_nb_tasks_per_height = m_img_height / m_tsk_height;      
  if (m_img_height % m_tsk_height != 0)
      m_nb_tasks_per_height += 1;

  // Compute the total number of tasks
  m_nb_tasks = m_nb_tasks_per_width * m_nb_tasks_per_height;
}
////////////////////////////// class TaskManagerBase ///////////////////////////
void TaskManagerBase::PrintTaskList(void) {
  VrtLog::Write("Number of tasks: %u", m_nb_tasks); 
  if (p_tasks == NULL)
    return;
  VrtLog::Write("List of tasks:\n");
  for (unsigned int i = 0; i < m_nb_tasks; i++)
    p_tasks[i].Print();
}
////////////////////////////// class TaskManagerBase ///////////////////////////
TaskUnit TaskManagerBase::CreateAreaFromTask(unsigned int task_idx) {
  // Compute the upper-left corner of the area task
  unsigned int ulx = m_img_ulx 
                   + m_tsk_width * (task_idx % m_nb_tasks_per_width);
  unsigned int uly = m_img_uly
                   + m_tsk_height * (task_idx / m_nb_tasks_per_width);
  
  // Deduce the bottom-right corner
  unsigned int brx = ulx + m_tsk_width - 1;
  if (brx > m_img_ulx + m_img_width)
    brx = m_img_ulx + m_img_width;
  unsigned int bry = uly + m_tsk_height - 1;
  if (bry > m_img_uly + m_img_height)
    bry = m_img_uly + m_img_height;

  // Return a new instance of AreaTask
  TaskUnit result;
  result.Define(ulx, uly, brx, bry);
  return result;
}
////////////////////////////////////////////////////////////////////////////////


