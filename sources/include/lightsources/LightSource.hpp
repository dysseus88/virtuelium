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
#ifndef _LIGHTSOURCE_HPP
#define _LIGHTSOURCE_HPP

#include <core/3DBase.hpp>
#include <core/LightBase.hpp>

#include <vector>
#include <structures/MultispectralPhoton.hpp>

/**
 * This interface define the lights sources. A light source a tree goals :
 * - give the spectral luminance incoming at given point
 * - give the total light power to cast (usefull for photon mapping)
 * - cast random photons (for photon mapping)
 *
 * @author Sylvain "Greewi" Dumazet
 */
class LightSource{
public :

  /**
   * Virtual destructor
   */
  inline virtual ~LightSource();

  /**
   * Place incidents light data casted form this source into the given vector.
   * @param receiver : the point where we need to have the incidents lights rays.
   * @param reemited : a light data as reference for spectral data (generally the
   *   one that we want to compute for the interaction that will receive the incidents)
   * @param incidents : incidents light data will be placed into this vector.
   */
  virtual void getIncidentLight(const Point& receiver, const LightVector& reemited, std::vector<LightVector>& incidents)=0;

  /**
   * Compute the emitted light for direct source view.
   * @param localBasis : the local basis on the surface of the source.
   * @param surfaceCoordinate : the surface coordinate on the source surface.
   * @param emitted : the light data to compute.
   */
  virtual void getEmittedLight(const Basis& localBasis, const Point2D& surfaceCoordinate, LightVector& emitted)=0;

  /**
   * Return the power of this source (the sum of all wavelenght)
   * @return the power of the light source
   */
  virtual Real getPower()=0;

  /**
   * Generate a random photon according this source spectral and spatial
   * distribution.
   * @param photon : the photon to generate;
   */
  virtual void getRandomPhoton(MultispectralPhoton& photon)=0;
};

/**
 * Virtual destructor.
 */
inline LightSource::~LightSource()
{
  //Nothing to do !
}
  
#endif //_LIGHTSOURCE_HPP
