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
#ifndef GUARD_VRT_TASKMANAGERLINE_HPP
#define GUARD_VRT_TASKMANAGERLINE_HPP
//!
//! @file TaskManagerLine.hpp
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
//! @class TaskManagerLine
//! @brief Base class for task scheduling in parallel computing
class TaskManagerLine: public TaskManagerBase {
 public:
  //! @enum ReadingDir
  //! @brief Reading directions
  enum {
    //! From the top-left corner to the bottom-right corner of the image;
    //! horizontal reading
    kLRTB,
    //! From the bottom-left corner to the top-right corner of the image;
    //! horizontal reading
    kLRBT,
    //! From the top-right corner to the bottom-left corner of the image;
    //! horizontal reading
    kRLTB,
    //! From the bottom-right corner to the top-left corner of the image;
    //! horizontal reading
    kRLBT,
    //! From the top-left corner to the bottom-right corner of the image;
    //! vertical reading
    kTBLR,
    //! From the top-right corner to the bottom-left corner of the image;
    //! vertical reading
    kTBRL,
    //! From the bottom-left corner to the top-right corner of the image;
    //! vertical reading
    kBTLR,
    //! From the bottom-right corner to the top-left corner of the image;
    //! vertical reading
    kBTRL,
    // Snake mode
    kSNAKE
  };

public:
  //! @brief constructor
  TaskManagerLine(void);
  //! @brief Destructor
  virtual ~TaskManagerLine(void);
  //! @brief Defines all the additional parameters of the derived class
  //! @param idx Position of the parameter
  //! @param value Value of the parameter
  virtual void SetAdditionalParameters(int idx, int value);

 public:
  //! @brief Initialize the list of task
  virtual void CreateTaskList(void);

 private:
  //! @brief Initialize the list of task for direction kLRTB
  virtual void CreateTaskList_LRTB(void);
  //! @brief Initialize the list of task for direction kLRBT
  virtual void CreateTaskList_LRBT(void);
  //! @brief Initialize the list of task for direction kRLTB
  virtual void CreateTaskList_RLTB(void);
  //! @brief Initialize the list of task for direction kRLBT
  virtual void CreateTaskList_RLBT(void);
  //! @brief Initialize the list of task for direction kTBLR
  virtual void CreateTaskList_TBLR(void);
  //! @brief Initialize the list of task for direction kTBRL
  virtual void CreateTaskList_TBRL(void);
  //! @brief Initialize the list of task for direction kBTLR
  virtual void CreateTaskList_BTLR(void);
  //! @brief Initialize the list of task for direction kBTRL
  virtual void CreateTaskList_BTRL(void);

 private:
  //! Reading direction; the order determines the priority
  int m_reading_dir;
  //! 1 if the rendering path acts like a continuous snake and is not 
  //! visually cut; 0 otherwise
  int m_snake;
}; // class TaskManagerLine
////////////////////////////////////////////////////////////////////////////////
#endif // GUARD_VRT_TASKMANAGERLINE_HPP
