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
#ifndef VRT_GUARD_SPECTRUM_HPP
#define VRT_GUARD_SPECTRUM_HPP
//!
//! @file Spectrum.hpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file defines a data structure for physical spectra
//! @remarks The spectrum class uses inline methods
//!
#include <cstdio>
#include <cstring>

#include <common.hpp>
#include <core/LightBase.hpp>

////////////////////////////////////////////////////////////////////////////////
//! @class Spectrum
//! @brief This class defines a data structure for physical spectra
class Spectrum {
 public:
  //! @brief Default contructor
  inline Spectrum(void);
  //! @brief Copy constructor
  inline Spectrum(const Spectrum& spectrum);
  //! @brief Destructor
  inline ~Spectrum(void);
  
 public:
  //! @brief Set all contained data to 0
  //! @details This functions does not delete the array pointer, but create a
  //!  zero-constant spectrum
  inline void clear(void);
  //! @brief Add a other spectrum to this one
  inline Spectrum& add(const Spectrum& spectrum);
  //! @brief Multiply all the value of the spectrum by a factor
  inline Spectrum& mul(const Real& factor );
  //! @brief Assignment operator
  inline Spectrum& operator=(const Spectrum& spectrum);
  //! @brief Acces operator
  inline const Real& operator[](unsigned int i) const;
  //! @brief Acces operator
  inline Real& operator[](unsigned int i);
  //! @brief Return the spectral value for a given wavelength 
  //! @param wavelength Base wavelength in nanometers
  inline Real getValueForWavelength(const Real& wavelength) const;
  //! @brief Return the index corresponding to a wavelength
  //! @param wavelength Base wavelength in nanometers
  inline unsigned int getIndexOfWavelength(const Real& wavelength) const;
  //! @brief Normalize the spectrum
  inline void normalize(void);
  //! @brief Normalize the spectrum with its value at 560 nm
  //! @details These function ivide all the values of the spectrum by its value
  //!  at 560nm, average wavelength corresponding to the maximum sensitivity of 
  //!  the human eye.
  inline void normalize560(void);
  //! @brief Normalize the power of spectrum
  inline void normalizePower(void);
  //! @brief Create and return the p_str pointer
  //! @details Only if required, the spectrum is written into a string and 
  //!  is returned for print messsages
  inline Real* values(void);

 private :
  //! Spectral values
  Real* p_values;
}; // class Spectrum
////////////////////////////////////////////////////////////////////////////////
inline Spectrum::Spectrum(void) 
    : p_values(NULL) {
  p_values = new Real[GlobalSpectrum::nbWaveLengths()];
}
////////////////////////////////////////////////////////////////////////////////
inline Spectrum::Spectrum(const Spectrum& spectrum) 
    : p_values(NULL) {
  p_values = new Real[GlobalSpectrum::nbWaveLengths()];
  std::memcpy(p_values, spectrum.p_values, 
              GlobalSpectrum::nbWaveLengths() * sizeof(Real));
}
////////////////////////////////////////////////////////////////////////////////
inline Spectrum::~Spectrum(void) {
  if (p_values != NULL) {
    delete[] p_values;
    p_values = NULL;
  }
}
////////////////////////////////////////////////////////////////////////////////
inline void Spectrum::clear(void) {
  for(unsigned int wl = 0; wl < GlobalSpectrum::nbWaveLengths(); wl++)
    p_values[wl] = Real(0.0);
}
////////////////////////////////////////////////////////////////////////////////
inline Spectrum& Spectrum::add(const Spectrum& spectrum) {
  for(unsigned int wl = 0; wl < GlobalSpectrum::nbWaveLengths(); wl++)
    p_values[wl] += spectrum[wl];

  return *this;
}
////////////////////////////////////////////////////////////////////////////////
inline Spectrum& Spectrum::mul(const Real& factor) {
  for(unsigned int wl = 0; wl < GlobalSpectrum::nbWaveLengths(); wl++)
    p_values[wl] *= factor;
  
  return *this;
}
////////////////////////////////////////////////////////////////////////////////
inline Spectrum& Spectrum::operator=(const Spectrum& spectrum) {
  std::memcpy(p_values, spectrum.p_values, 
              GlobalSpectrum::nbWaveLengths() * sizeof(Real));
  
  return *this;
}
////////////////////////////////////////////////////////////////////////////////
inline const Real& Spectrum::operator[](unsigned int i) const {
  return p_values[i];
}
////////////////////////////////////////////////////////////////////////////////
inline Real& Spectrum::operator[](unsigned int i) {
  return p_values[i];
}
////////////////////////////////////////////////////////////////////////////////
inline Real Spectrum::getValueForWavelength(const Real& wavelength) const {
  return p_values[getIndexOfWavelength(wavelength)];
}
////////////////////////////////////////////////////////////////////////////////
inline unsigned int Spectrum::getIndexOfWavelength(
    const Real& wavelength) const {

  unsigned int min = 0;
  unsigned int max = GlobalSpectrum::nbWaveLengths() - 1;
  while(min < max - 1) {
    unsigned int pivot = (min + max) / 2;
    if(wavelength < GlobalSpectrum::getWaveLength(pivot))
      max = pivot;
    else
      min = pivot;
  }
  return min;
}
////////////////////////////////////////////////////////////////////////////////
inline void Spectrum::normalize(void) {
  Real max = 0;
  for(unsigned int wl = 0; wl < GlobalSpectrum::nbWaveLengths(); wl++)
    if(p_values[wl] > max)
      max = p_values[wl];

  for(unsigned int wl = 0; wl < GlobalSpectrum::nbWaveLengths(); wl++)
    p_values[wl] /= max;
}
////////////////////////////////////////////////////////////////////////////////
inline void Spectrum::normalize560(void) {
  Real max = getValueForWavelength(Real(560.0));

  if (max != 0) {
    for(unsigned int wl = 0; wl < GlobalSpectrum::nbWaveLengths(); wl++)
      p_values[wl] /= max;
  }
}
////////////////////////////////////////////////////////////////////////////////
inline void Spectrum::normalizePower(void) {
  Real sum = 0;
  for(unsigned int wl = 0; wl < GlobalSpectrum::nbWaveLengths(); wl++)
    sum += p_values[wl];

  for(unsigned int wl = 0; wl < GlobalSpectrum::nbWaveLengths(); wl++)
    p_values[wl] /= sum;
}
////////////////////////////////////////////////////////////////////////////////
inline Real* Spectrum::values(void) {
  return p_values;
}
////////////////////////////////////////////////////////////////////////////////
#endif // VRT_GUARD_SPECTRUM_HPP

