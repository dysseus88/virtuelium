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
#include <environments/SphericalEnvironment.hpp>
//!
//! @file SphericalEnvironment.cpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file implements the classes declared in SphericalEnvironment.hpp 
//!  @arg SphericalEnvironment
//! @todo 
//! @remarks 
//!
#include <structures/Texture.hpp>
#include <structures/Spectrum.hpp>
////////////////////////////////////////////////////////////////////////////////
SphericalEnvironment::SphericalEnvironment(Texture* envir, Real envir_power)
    : p_environment(NULL), m_amount(0) {

  // WARNING : simple pointer affectation !
  p_environment = envir;
  m_amount = envir_power;
}
////////////////////////////////////////////////////////////////////////////////
SphericalEnvironment::~SphericalEnvironment(void) { 
 if ( p_environment != NULL)
  delete p_environment;
 p_environment = NULL;
}
////////////////////////////////////////////////////////////////////////////////
void SphericalEnvironment::AddContribution(LightVector& lightdata) {
  if (p_environment == NULL)
    return;

 // LightVector env(lightdata);
 // Vector view = lightdata.getRay().v;
 // Point origin = lightdata.getRay().o;

	//Real x = view.x();
	//Real y = view.y();
	//Real z = view.z();

	//Real len = sqrt(x * x + y * y + (z+1) * (z+1));
 // Real u, v;

 // u = (x / (Real(2.0) * len)) + Real(0.5);
 // z = (y / (Real(2.0) * len)) + Real(0.5);

 //// if (x == Real(0.0) && y == Real(0.0)) {
 ////   u = Real(0.0);
 //// } else {
 ////   u = Real(1.0) - (atan2(x, y) / M_PI); //* Real(0.5);
 //// }

 //// z /= len;
 //// v = acos(z) / M_PI;

 // Spectrum Spix;
 // p_environment->GetSpectrumValue(u, v, Spix);

 // for(unsigned int i=0; i<lightdata.size(); i++)
 //   env[i].setRadiance(m_amount * Spix[i]);

 // lightdata.add(env);
  
////////////////////////////////////////////////////////////////////////////////

  LightVector env(lightdata);
  Vector view = lightdata.getRay().v;
  Point origin = lightdata.getRay().o;

  Real x = view.x();
  Real y = view.y();
  Real z = view.z();

  Real m = sqrt ( x*x + y*y + (z+1)*(z+1) );
  Real u = x / (2 * m) + 0.5f;
  Real v = y / (2 * m) + 0.5f;
  
  Spectrum Spix;
  p_environment->GetSpectrumValue(u, v, Spix);

  for(unsigned int i=0; i<lightdata.size(); i++)
    env[i].setRadiance(m_amount * Spix[i]);

  lightdata.add(env);
  
////////////////////////////////////////////////////////////////////////////////

  //Real x = lightdata.getRay().v.x();
  //Real y = lightdata.getRay().v.y();
  //Real z = lightdata.getRay().v.z();
  //Real len = sqrt(x * x + y * y + z * z);
  //Real u, v;
  //
  //if (len > Real(0.0)) {
  //  if (x == Real(0.0) && y == Real(0.0)) {
  //    u = Real(0.0);
  //  } else {
  //    u = (Real(1.0) + atan2(x, y) / M_PI) * Real(0.5);
  //  }

  //  z /= len;
  //  v = acos(z) / M_PI;

  //  Spectrum Spix;
  //  p_environment->GetSpectrumValue(u, v, Spix);

  //  for(unsigned int i=0; i<lightdata.size(); i++)
  //    env[i].setRadiance(m_amount * Spix[i]);

  //  lightdata.add(env);
  //}
}
////////////////////////////////////////////////////////////////////////////////
