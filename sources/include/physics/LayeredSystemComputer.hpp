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

#ifndef _LAYEREDSYSTEMCOMPUTER_HPP
#define _LAYEREDSYSTEMCOMPUTER_HPP

#include <vector>
#include <core/LightBase.hpp>

#include <structures/Medium.hpp>

class LayeredSystemComputer{
public :

  /**
   * Return true if there is a layer that is opaque.
   */
  static bool isLayerSystemOpaque(std::vector<const Medium*> media);

  /**
   * Compute the diffuse reflectance of a layered system.
   * 
   * @param reflectance : the computed reflectance will be placed into this spectrum.
   * @param media : the medium of every layer. Note that the first medium is the external medium.
   * @param thickness : the thickness (in meters) of each layer (note that the first one is not used).
   */
  static void computeDiffuseLayerSystemReflectance(Spectrum& reflectance, Spectrum& transmittance, std::vector<const Medium*> media, std::vector<Real> thickness);

  /**
   * Compute the specular reflectance of a layered system.
   * 
   * @param Rs : the computed reflectance for the S-polarization will be placed into this spectrum.
   * @param Rp : the computed reflectance for the P-polarization will be placed into this spectrum.
   * @param Ts : the computed transmittance for the S-polarization will be placed into this spectrum.
   * @param Tp : the computed transmittance for the P-polarization will be placed into this spectrum.
   * @param cosOi : the cosinus of the incident angle.
   * @param media : the medium of every layer. Note that the first medium is the external medium.
   * @param thickness : the thickness (in meters) of each layer (note that the first one is not used).
   */
  static void computeSpecularLayerSystem(Spectrum& Rs, Spectrum& Rp, Spectrum& Ts, Spectrum& Tp, Real cosOi, std::vector<const Medium*> media, std::vector<Real> thickness);

private :
  /**
   * Compute the diffuse reflectance of a layered system.
   * 
   * @param reflectance : the computed reflectance will be placed into this spectrum.
   * @param media : the medium of every layer. Note that the first medium is the external medium.
   * @param thickness : the thickness (in meters) of each layer (note that the first one is not used).
   */
  static void computeDiffuseReflectance(Spectrum& reflectance, Spectrum& transmittance, std::vector<const Medium*> media, std::vector<Real> thickness, unsigned int currentMedium, bool computeFresnel);

  /**
   * Compute the specular amplitude reflectance of a layered system.
   * 
   * @param rs : the computed reflectance for the S-polarization will be placed into this parameter.
   * @param rp : the computed reflectance for the P-polarization will be placed into this parameter.
   * @param ts : the computed transmittance for the S-polarization will be placed into this parameter.
   * @param tp : the computed transmittance for the P-polarization will be placed into this parameter.
   * @param wi : the index of the considered wavelength.
   * @param wl : the wavelength.
   * @param cosOi : the cosinus of the incident angle.
   * @param media : the medium of every layer. Note that the first medium is the external medium.
   * @param thickness : the thickness (in meters) of each layer (note that the first one is not used).
   */
  static void computeSpecularUncoherent(Real& rs, Real& rp, Real& ts, Real& tp, unsigned int wi, Real wl, Real cosOi, std::vector<const Medium*> media, std::vector<Real> thickness, unsigned int currentMedium);

  /**
   * Compute the specular amplitude reflectance of a layered system.
   * 
   * @param Rs : the computed reflectance for the S-polarization will be placed into this parameter.
   * @param Rp : the computed reflectance for the P-polarization will be placed into this parameter.
   * @param Ts : the computed transmittance for the S-polarization will be placed into this parameter.
   * @param Tp : the computed transmittance for the P-polarization will be placed into this parameter.
   * @param wi : the index of the considered wavelength.
   * @param wl : the wavelength.
   * @param cosOi : the cosinus of the incident angle.
   * @param media : the medium of every layer. Note that the first medium is the external medium.
   * @param thickness : the thickness (in meters) of each layer (note that the first one is not used).
   */
  static void computeSpecularCoherent(Real& Rs, Real& Rp, Real& Ts, Real& Tp, unsigned int wi, Real wl, Real cosOi, std::vector<const Medium*> media, std::vector<Real> thickness);
};

#endif //_LAYEREDSYSTEMCOMPUTER_HPP
