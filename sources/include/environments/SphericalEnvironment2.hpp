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
#ifndef GUARD_VRT_SPHERICALENVIRONMENT2_HPP
#define GUARD_VRT_SPHERICALENVIRONMENT2_HPP
//! @file SphericalEnvironment2.hpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @remarks
//! @details This file defines a sphere mapping
#include <environments/Environment.hpp>
#include <string>
////////////////////////////////////////////////////////////////////////////////
//! @class SphericalEnvironment2
//! @brief Defines the spherical environment mapping
class SphericalEnvironment2: public Environment { 
 public:
  //! @brief Constructor
  //! @param file Tetxure file
  //! @param envir_power Power of the SphericalEnvironment
  SphericalEnvironment2(const char* file, Real envir_power);
  //! @brief Destructor
  virtual ~SphericalEnvironment2(void);
 
 public:
  //! @brief Add Contribution
  //! @param lightdata Light going to inifinty and that must be modified
  void AddContribution(LightVector& lightdata);

 private:
  //! Environment map
  std::string m_file;
  //! Environment power
  Real m_amount;
}; // class SphericalEnvironment
////////////////////////////////////////////////////////////////////////////////
#endif //GUARD_VRT_SPHERICALENVIRONMENT2_HPP
