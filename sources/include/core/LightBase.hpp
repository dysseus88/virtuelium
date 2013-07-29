/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
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
 */

#ifndef _LIGHTBasis_HPP
#define _LIGHTBasis_HPP

#include <vector>
#include <algorithm>

#include <core/3DBase.hpp>

class GlobalSpectrum{
public :
  /**
   * Initialize the global spectrum. This function must be called only one time,
   * before any creation of spectrums !
   * wavelengths : a vector that contain the wave lengths of the wanted spectrum.
   */
  inline static void init(std::vector<Real> wavelengths);

  /**
   * Return the number of wave lengths
   */
  inline static unsigned int nbWaveLengths();

  /**
   * Return the i-th wavelength
   */
  inline static Real getWaveLength(unsigned int i);
  
private :
  static std::vector<Real> _wavelengths;
};

/**
 * Initialize the global spectrum. This function must be called only one time,
 * before any creation of spectrums !
 * wavelengths : a vector that contain the wave lengths of the wanted spectrum.
 */
inline void GlobalSpectrum::init(std::vector<Real> wavelengths)
{
  _wavelengths = wavelengths;
  std::sort(_wavelengths.begin(), _wavelengths.end());
}

/**
 * Return the number of wave lengths
 */
inline unsigned int GlobalSpectrum::nbWaveLengths()
{
  return _wavelengths.size();
}

/**
 * Return the i-th wavelength
 */
inline Real GlobalSpectrum::getWaveLength(unsigned int i)
{
  return _wavelengths[i];
}

#include <structures/Spectrum.hpp>
#include <structures/LightData.hpp>
#include <structures/LightVector.hpp>

#endif //_LIGHTBasis_HPP
