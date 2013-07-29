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

#include <physics/LayeredSystemComputer.hpp>
#include <physics/DielectricFormula.hpp>
#include <physics/KubelkaMunkFormula.hpp>
#include <physics/ThinLayerSystem.hpp>

#include <core/debug.hpp>

/**
 * Return true if there is a layer that is opaque.
 */
bool LayeredSystemComputer::isLayerSystemOpaque(std::vector<const Medium*> media)
{
  for(unsigned int i=0; i<media.size(); i++)
    if(media[i]->isOpaque)
      return true;

  return false;
}

/**
 * Compute the diffuse reflectance of a layered system.
 * 
 * @param reflectance : the computed reflectance will be placed into this spectrum.
 * @param media : the medium of every layer. Note that the first medium is the external medium.
 * @param thickness : the thickness (in meters) of each layer (note that the first one is not used).
 */
void LayeredSystemComputer::computeDiffuseLayerSystemReflectance(Spectrum& reflectance, Spectrum& transmittance, std::vector<const Medium*> media, std::vector<Real> thickness)
{
  computeDiffuseReflectance(reflectance, transmittance, media, thickness, 1, false);

  if(isLayerSystemOpaque(media))
    transmittance.clear();
}

/**
 * Compute the diffuse reflectance of a layered system.
 * 
 * @param reflectance : the computed reflectance will be placed into this spectrum.
 * @param media : the medium of every layer. Note that the first medium is the external medium.
 * @param thickness : the thickness (in meters) of each layer (note that the first one is not used).
 */
void LayeredSystemComputer::computeDiffuseReflectance(Spectrum& reflectance, Spectrum& transmittance, std::vector<const Medium*> media, std::vector<Real> thickness, unsigned int currentMedium, bool computeFresnel)
{
  const Medium* outerMedium = media[currentMedium-1];
  const Medium* layerMedium = media[currentMedium];
  Real layerThickness = thickness[currentMedium];
  
  //If the current medium is opaque
  if(layerMedium->isOpaque)
  {
    reflectance.clear();
    transmittance.clear();
  }
  //If the current medium is transparent and is the last
  else
  {
    if(currentMedium<media.size()-1)
      computeDiffuseReflectance(reflectance, transmittance, media, thickness, currentMedium+1, true);
    else
    {
      reflectance.clear();
      for(unsigned int i=0; i<GlobalSpectrum::nbWaveLengths(); i++)
        transmittance[i]=1.0;
    }
  }  

  // If the layer is lambertian
  if(layerMedium->useLambertianModel)
  {
    for(unsigned int i=0; i<GlobalSpectrum::nbWaveLengths(); i++)
    {
      Real R12 = layerMedium->r[i];
      Real T12 = layerMedium->t[i];
      Real R23 = reflectance[i];
      Real T23 = transmittance[i];

      reflectance[i] = R12 + (T12*T12*R23)/(1 - R12*R23);
      transmittance[i] = (T12*T23)/(1 - R12*R23);
    }
  }

  // If the layer is describe by the Kubelka&Munk theory
  else if(layerMedium->useKubelkaMunkModel)
  {
    for(unsigned int i=0; i<GlobalSpectrum::nbWaveLengths(); i++)
      reflectance[i] = KubelkaMunkFormula::kublekaMunkReflectance(layerMedium->K[i], layerMedium->S[i], layerThickness, reflectance[i]);

    //TODO Transmittance !
  }
  
  // If the medium is a dielectric
  else if(layerMedium->useFresnelModel && outerMedium->useFresnelModel && computeFresnel)
  {
    for(unsigned int i=0; i<GlobalSpectrum::nbWaveLengths(); i++)
    {
      Complex n1(outerMedium->n[i], outerMedium->k[i]);
      Complex n2(layerMedium->n[i], layerMedium->k[i]);

      Complex n12 = n2/n1;
      Complex n21 = n1/n2;
      Real R12 = DielectricFormula::integratedDielectricReflectance(n12);
      Real R21 = DielectricFormula::integratedDielectricReflectance(n21);
      Real T12 = 1-R12;
      Real T21 = 1-R21;
      Real R23 = reflectance[i];
      Real T23 = transmittance[i];

      //TODO Absorption !

      reflectance[i] = R12 + (T12*T21*R23) / (1 - R21*R23);
      transmittance[i] = (T12*T23)/(1 - R12*R23);
    }
  }
}

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
void LayeredSystemComputer::computeSpecularLayerSystem(Spectrum& Rs, Spectrum& Rp, Spectrum& Ts, Spectrum& Tp, Real cosOi, std::vector<const Medium*> media, std::vector<Real> thickness)
{
  bool coherent = false;
  for(unsigned int i=1; i<thickness.size()-1;i++)
    if(thickness[i] < 4E-9*GlobalSpectrum::getWaveLength(GlobalSpectrum::nbWaveLengths()-1))
      coherent = true;

  for(unsigned int wi=0; wi<GlobalSpectrum::nbWaveLengths(); wi++)
  {
    Real wl = GlobalSpectrum::getWaveLength(wi)*1E-9;
    if(coherent)
    {
      Real rs, rp, ts, tp;
      computeSpecularCoherent(rs, rp, ts, tp, wi, wl, cosOi, media, thickness);
      Rs[wi] = rs*rs;
      Rp[wi] = rp*rp;
      Ts[wi] = ts*ts;
      Tp[wi] = tp*tp;
    }
    else
    {
      computeSpecularUncoherent(Rs[wi], Rp[wi], Ts[wi], Tp[wi], wi, wl, cosOi, media, thickness, 1);
    }
  }
}


//! @brief Compute the specular amplitude reflectance of a layered system
//! @todo Add influence of the scattering from pigments
//! @param rs Computed reflectance for the S-polarization will be placed into this parameter.
//! @param rp Computed reflectance for the P-polarization will be placed into this parameter.
//! @param ts Computed transmittance for the S-polarization will be placed into this parameter.
//! @param tp Computed transmittance for the P-polarization will be placed into this parameter.
//! @param wi Index of the considered wavelength.
//! @param wl Wavelength.
//! @param cosOi Cosinus of the incident angle.
//! @param media Medium of every layer. Note that the first medium is the external medium.
//! @param thickness Thickness (in meters) of each layer (note that the first one is not used).
void LayeredSystemComputer::computeSpecularUncoherent(
    Real& rs, Real& rp, Real& ts, Real& tp, unsigned int wi, Real wl, 
    Real cosOi, std::vector<const Medium*> media, std::vector<Real> thickness, 
    unsigned int currentMedium) {
  // Get the index
  Real n1 = media[currentMedium-1]->n[wi];
  Real k1 = media[currentMedium-1]->k[wi];
  Real n2 = media[currentMedium]->n[wi];
  Real k2 = media[currentMedium]->k[wi];
  Real n21 = n1 / n2;

  // Complex
  Complex n1_ik1(n1, k1);
  Complex n2_ik2(n2, k2);
  Complex relative_index = n2_ik2 / n1_ik1;

  //Compute R12
  Real r12s, r12p, t12s, t12p;
  DielectricFormula::fresnelReflectance(cosOi, relative_index, r12s, r12p);
  
  r12s *= r12s;
  if (r12s < Real(1.0))
    t12s = Real(1.0) - r12s;
  else
    t12s = 0;
    
  r12p *= r12p;
  if (r12p < Real(1.0))
    t12p = Real(1.0) - r12p;
  else
    t12p = 0;

  //If the layer is Opaque (or if we have a total reflexion) we've done it ! 
  if(media[currentMedium]->isOpaque 
      || currentMedium == media.size() - 1 
      || !media[currentMedium + 1]->useFresnelModel 
      || n21 * n21 * (Real(1.0) - cosOi * cosOi) >= Real(1.0)) {
    rs = std::sqrt(r12s);
    rp = std::sqrt(r12p);
    ts = std::sqrt(t12s);
    tp = std::sqrt(t12p);  
    // our case ends here because currentMedium == media.size() - 1
    return;
  }

  //Compute the refracted ray cosinus
  Real cosOv = std::sqrt(Real(1.0) - n21 * n21 * (Real(1.0) - cosOi * cosOi));
  
  //Get the substrate reflectance
  computeSpecularUncoherent(rs, rp, ts, tp, wi, wl, cosOv, media, thickness, 
                            currentMedium + 1);
  rs *= rs;
  rp *= rp;
  ts *= ts;
  tp *= tp;

  //Compute the absorption
  Real a = Real(1.0);
  if(k2 > 0) {
    a = DielectricFormula::dielectricAbsorption(thickness[currentMedium]/cosOv, wl, k2);
    // Add influence of the scattering from pigments here
  }
  //Compute the final reflectance
  Real t12s2 = t12s * t12s;
  Real t12p2 = t12p * t12p;
  ts = std::sqrt((t12s * ts * a) / (Real(1.0) - r12s * rs * a * a));
  tp = std::sqrt((t12p * tp * a) / (Real(1.0) - r12p * rp * a * a));
  rs = std::sqrt(r12s + (t12s2 * rs * a * a) / (Real(1.0) - r12s * rs * a * a));
  rp = std::sqrt(r12p + (t12p2 * rp * a * a) / (Real(1.0) - r12p * rp * a * a));
}

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
void LayeredSystemComputer::computeSpecularCoherent(Real& Rs, Real& Rp, Real& Ts, Real& Tp, unsigned int wi, Real wl, Real cosOi, std::vector<const Medium*> media, std::vector<Real> thickness)
{
  // Find the substrate layer
  unsigned int s=1;
  while((s<media.size()-1) && (media[s+1]->useFresnelModel) && !(media[s]->isOpaque))
    s++;

  // Build the thin layer system
  ThinLayerSystem layersystem;  
  layersystem.setOutsideIndices(media[0]->n, media[0]->k);
  layersystem.setSubstrateIndices(media[s]->n, media[s]->k);
  for(unsigned int i=1; i<s; i++)
    layersystem.addLayer(media[i]->n, media[i]->k, thickness[i]);

  layersystem.computeReflectanceTransmittance(cosOi, wl, wi, Rs, Rp, Ts, Tp);
}


