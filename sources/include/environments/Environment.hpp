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
#ifndef GUARD_VRT_ENVIRONMENT_HPP
#define GUARD_VRT_ENVIRONMENT_HPP
//! @file Environment.hpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @remarks
//! @details This file defines the base environment classs
#include <stdio.h>

#include <core/3DBase.hpp>
#include <core/LightBase.hpp>

////////////////////////////////////////////////////////////////////////////////
//! @class Environment
//! @brief Defines the  base environment mapping
//! @details This is the base class of a polymorphic class architectures
class Environment { 
 public:
  //! @brief Constructor
  Environment(void);
  //! @brief Destructor
  virtual ~Environment(void);
 
 public:
  //! @brief Add Contribution
  //! @param lightdata Light going to inifinty and that must be modified
  virtual void AddContribution(LightVector& lightdata) = 0;

 protected : 
}; // class Environment
////////////////////////////////////////////////////////////////////////////////
#endif //GUARD_VRT_ENVIRONMENT_HPP
