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
#include <environments/SphericalEnvironment2.hpp>
//!
//! @file SphericalEnvironment2.cpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file implements the classes declared in SphericalEnvironment2.hpp 
//!  @arg SphericalEnvironment2
//! @todo 
//! @remarks 
//!
#include <structures/Texture.hpp>
#include <structures/Spectrum.hpp>
////////////////////////////////////////////////////////////////////////////////
SphericalEnvironment2::SphericalEnvironment2(const char* file, Real envir_power)
  : m_amount(0) {

  // WARNING : simple pointer affectation !
  m_file = file;
  m_amount = envir_power;
}
////////////////////////////////////////////////////////////////////////////////
SphericalEnvironment2::~SphericalEnvironment2(void) { 
}
////////////////////////////////////////////////////////////////////////////////
void SphericalEnvironment2::AddContribution(LightVector& lightdata) {
  if (m_file.empty() == true)
    return;

  LightVector env(lightdata);
  //Vector view = lightdata.getRay().v;
  //Point origin = lightdata.getRay().o;

  //Real x = view.x();
  //Real y = view.y();
  //Real z = view.z();

  //Real m = sqrt ( x*x + y*y + (z+1)*(z+1) );
  //Real u = x / (2 * m) + 0.5f;
  //Real v = y / (2 * m) + 0.5f;
  //
  //Spectrum Spix;
  // p_environment->GetSpectrumValue(u, v, Spix);

  //for(unsigned int i=0; i<lightdata.size(); i++)
  //  env[i].setRadiance(m_amount * Spix[i]);

  lightdata.add(env);
}
////////////////////////////////////////////////////////////////////////////////
