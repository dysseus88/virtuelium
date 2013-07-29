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

#ifndef _MEDIUM_HPP
#define _MEDIUM_HPP

#include <core/LightBase.hpp>

#include <physics/DielectricFormula.hpp>
#include <structures/MultispectralPhoton.hpp>

class Medium {
public :
  bool isOpaque;
  bool hasDispersion;
  bool hasPhotonDispersion;

  /*
   * Simple lambertian description
   */
  bool useLambertianModel;
  Spectrum r;  //The spectrum of reflexion of this Medium
  Spectrum t;  //The transmission of this Medium

  /*
   * Fresnel description
   */
  bool useFresnelModel;
  Spectrum n;  //The real part of the index of refraction of this medium.
  Spectrum k;  //The imaginary part of the index of refraction of this medium.

  /*
   * Kubeklka-Munk Description
   */
  bool useKubelkaMunkModel;
  Spectrum S;  //The diffusion Kubelka&Munk parameter of this medium
  Spectrum K;  //The absorption Kubelka&Munk parameter of this medium

  /**
   * Default constructor
   */
  inline Medium();

  /**
   * Compute the absorption of a light ray through the medium.
   */
  inline void transportLight(LightVector& light) const;

  /**
   * Compute the absorption of a photon through the medium.
   */
  inline bool transportPhoton( MultispectralPhoton& photon) const;
};

/**
 * Default constructor
 */
inline Medium::Medium()
: isOpaque(false), hasDispersion(false), hasPhotonDispersion(false), useLambertianModel(false), useFresnelModel(false), useKubelkaMunkModel(false)
{
  r.clear();
  t.clear();
  n.clear();
  k.clear();
  S.clear();
  K.clear();
}

/**
 * Compute the absorption of a light ray through the medium.
 */
inline void Medium::transportLight(LightVector& light) const
{
  if(isOpaque)
  {
    light.clear();
    return;
  }

  if(useLambertianModel)
  {
    for(unsigned int i=0; i<light.size(); i++)
      light[i].mul(t[light[i].getIndex()]);
  }

  if(useFresnelModel)
  {
    for(unsigned int i=0; i<light.size(); i++)
    {
      Real a = DielectricFormula::dielectricAbsorption(light.getDistance(), GlobalSpectrum::getWaveLength(light[i].getIndex())*1E-9, k[light[i].getIndex()]);
      light[i].mul(a);
    }
  }
}

/**
 * Compute the absorption of a photon through the medium.
 */
inline bool Medium::transportPhoton(MultispectralPhoton& photon) const
{
  if(isOpaque)
    return false;

  if(useFresnelModel)
  {
    //Computing photon absorption at the reflexion
    Real mean=0;
    for(unsigned int i=0; i<GlobalSpectrum::nbWaveLengths(); i++)
    { 
      Real a = DielectricFormula::dielectricAbsorption(photon.distance, GlobalSpectrum::getWaveLength(i)*1E-9, k[i]);
      photon.radiance[i] *= a;
      mean+=photon.radiance[i];
    }

    //Absorption
    if(rand()/(Real)RAND_MAX > mean)
      return false;
    
    //Normalizing photon energy
    for(unsigned int i=0; i<GlobalSpectrum::nbWaveLengths(); i++)
      photon.radiance[i] /= mean;
 
    return true;
  }
  
  return false;
}

#endif // _MEDIUUM_HPP
