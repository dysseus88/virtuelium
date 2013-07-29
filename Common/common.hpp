// Copyright (C) 2012 MAS Laboratory (Ecole Centrale Paris)
//
// This file is part of the VirueliumIO library.
//
// The VirtueliumIO library is free software; you can redistribute it and/or 
// modify it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#ifndef GUARD_VIRTUELIUMIO_COMMON_HPP__
#define GUARD_VIRTUELIUMIO_COMMON_HPP__
//!
//! @file common.hpp
//! @author Rémi Cerise <remi.cerise@ecp.fr>
//! @version 1.0.0
//! @date Fri Apr 27 14:19:00 CEST 2012
//! @remarks
//! @details This file contains usefull type definition.
//!
#include <cmath>
////////////////////////////////////////////////////////////////////////////////
#ifdef NO_FLOAT
# define IS_FLOAT_ENABLED 0
#else
# define IS_FLOAT_ENABLED 1
#endif

#ifdef NO_DOUBLE
# define IS_DOUBLE_ENABLED 0
#else
# define IS_DOUBLE_ENABLED 1
#endif

// float by default
#if IS_FLOAT_ENABLED
  typedef float Real;
  static Real kEPSILON = static_cast<Real> ( 1E-6 );
  static const int kPRECISION = 6;
// double only if enable
#elif IS_DOUBLE_ENABLED
  typedef double Real;
  static const Real kEPSILON = static_cast<Real> ( 1E-12 );   
  static const int kPRECISION = 12;
#endif

// PI definition
#ifndef M_PI 
  #define  M_PI 3.14159265359
#endif
static const Real kPI = static_cast<Real>(M_PI);
static const Real k1OVERPI = static_cast<Real>(1.0 / kPI);

// Sample unit of distributive functions
static const unsigned int kSAMPLES = 90;

////////////////////////////////////////////////////////////////////////////////
static Real minimum(const Real& lhs, const Real& rhs) {
  if (lhs > rhs)
    return rhs;
  return lhs;
}
////////////////////////////////////////////////////////////////////////////////
static Real maximum(const Real& lhs, const Real& rhs) {
  if (lhs < rhs)
    return rhs;
  return lhs;
}
////////////////////////////////////////////////////////////////////////////////
// 2D coordinates
typedef struct {
  Real x;
  Real y;
} Real2;

// 3D coordinates
typedef struct {
  Real x;
  Real y;
  Real z;
} Real3;
////////////////////////////////////////////////////////////////////////////////
//! @namespace VirtueliumIO
//! @brief All the classes, variables and functions of the library use this 
//! namespace.
namespace VirtueliumIO {
//! @enum TextureMode
//! @brief Mode of texturing 
//!
//! @details Virtuelium has 3 modes of texturing : 
//!  @arg off : do not repat the texture
//!  @arg repeat : repeat the texture
//!  @arg mirror : repeat the texture with mirror effect
typedef enum {
  kTextureOff = 0,
  kTextureRepeat = 1,
  kTextureMirror = 2,
	} TextureMode;

//! @brief Get the mode of texturing for U mapping coordinate
inline const char* GetTextureModeStr(const TextureMode& mode) {
  switch (mode) {
	  case kTextureOff: {
		  return "off";
    }
	  case kTextureRepeat: {
		  return "repeat";
    }
	  case kTextureMirror: {
		  return "mirror";
    }
	  default: {
		  return "off";
    }
  }
}
//! @enum EnvironmentType
//! @brief Type of environment map used in the scene
//!
//! @details Virtuelium has 3 types of environment : 
//!  @arg Sphere mapping
//!  @arg Cylindric mapping
//!  @arg Cube mapping
typedef enum {
  kSphereMapping = 0,
  kCylindricMapping = 1,
  kCubeMapping = 2, 
} EnvironmentType;
//! @brief Get the type of environment
inline const char* GetTypeStr(const EnvironmentType& type) {
	switch (type) {   
	  case kSphereMapping: {
		  return "Sphere";
    }
	  case kCylindricMapping: {
		  return "Cylindric";
    }
	  case kCubeMapping: {
		  return "Cube";
    }
	  default: {
		  return "Sphere";
    }
  }
}
//! @enum Axis
//! @brief Name of the axis 
//!
//! @details The name of the axis is usefull for rotation geometry node
typedef enum {
  kAxisX = 0,
  kAxisy = 1,
  kAxisz = 2, 
} Axis;
//! @brief Get the type of environment
inline const char* GetAxisStr(const Axis& axis) {
	switch (axis) {   
	  case kAxisX: {
		  return "x";
    }
	  case kAxisy: {
		  return "y";
    }
	  case kAxisz: {
		  return "z";
    }
	  default: {
		  return "x";
    }
  }
}
//! @enum MediumType
//! @brief Type of a medium used in the scene
//!
//! @details A medium decribes the physical properties of an area / surface in the
//!  scene. Every physical interaction is thus modelized by an interaction 
//!  between  two medias. Virtuelium has 4 types of medium:
//!  @arg kMediumReflectance: Reflectance spectrum (lambertian model)
//!  @arg kMediumTransmittance: Transmittance spectrum (lambertian model)
//!  @arg kMediumIndice: Optical indices (Fresnel model)
//!  @arg kMediumPigment: absorption coeficients (Kubelka-Munk model)
//!  This enumeration works as a boolean flag (OR). For example, if a medium is
//!  defined by a reflectance spectrum and optical indices, then its type must 
//!  be: kMediumReflectance | kMediumIndice
typedef enum {
  kMediumReflectance = 1,
  kMediumTransmittance = 2,
  kMediumIndice = 4,
  kMediumPigment = 8,
} MediumType;
} // namespace VirtueliumIO
////////////////////////////////////////////////////////////////////////////////
#endif // GUARD_VIRTUELIUMIO_COMMON_HPP__