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
#include <core/taskexecutor/TaskExecutorBase.hpp>
//!
//! @file TaskExecutorBase.cpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file implements classs declared in TaskExecutorBase.hpp 
//!  @arg TaskExecutorBase
//!
#include <core/VrtLog.hpp>

////////////////////////////// class TaskExecutorBase //////////////////////////
TaskExecutorBase::TaskExecutorBase(void)
    : m_img_ulx(0),
      m_img_uly(0),
      m_img_width(0),
      m_img_height(0),
      m_tsk_width(0),
      m_tsk_height(0),
      p_task_mngr(NULL),
      p_scenery(NULL),
      m_nb_task_refresh(5),
      p_image(NULL),
      b_overwrite(false),
      m_chunk(1) {}
////////////////////////////// class TaskExecutorBase //////////////////////////
TaskExecutorBase::~TaskExecutorBase(void) {
  EraseTaskManager();
  //EraseScenery(); // Not deleted here but in the main class Virtuelium
  EraseImage();
}
////////////////////////////// class TaskExecutorBase //////////////////////////
void TaskExecutorBase::SetBaseParameters(unsigned int img_ulx, 
                                         unsigned int img_uly,
                                         unsigned int img_brx, 
                                         unsigned int img_bry,
                                         unsigned int tsk_width, 
                                         unsigned int tsk_height,
                                         unsigned int nb_task_refresh,
                                         bool ovrride, 
                                         int chunk) {
  m_img_ulx = img_ulx;
  m_img_uly = img_uly;
  m_img_width = img_brx - img_ulx;
  m_img_height = img_bry - img_uly;
  m_tsk_width = tsk_width;
  m_tsk_height = tsk_height;
  m_nb_task_refresh = nb_task_refresh;
  b_overwrite = ovrride;
  m_chunk = chunk;
}
////////////////////////////////////////////////////////////////////////////////


