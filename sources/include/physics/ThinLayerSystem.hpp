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

#ifndef _THINLAYERSYSTEM_HPP
#define _THINLAYERSYSTEM_HPP

#include <core/3DBase.hpp>
#include <core/LightBase.hpp>

#include <iostream>

class ThinLayerSystem{
public:
  /**
   * Set the outside indices (set imaginary part to 0).
   * @param n : the real part of the indices.
   */
  inline void setOutsideIndices(const Spectrum& n);

  /**
   * Set the outside indices
   * @param n : the real part of the indices.
   * @param ka: the imaginary part of the indices.
   */
  inline void setOutsideIndices(const Spectrum& n, const Spectrum& k);

  /**
   * Set the substrate indices (set imaginary part to 0).
   * @param n : the real part of the indices.
   */
  inline void setSubstrateIndices(const Spectrum& n);

  /**
   * Set the substrate indices
   * @param n : the real part of the indices.
   * @param k : the imaginary part of the indices.
   */
  inline void setSubstrateIndices(const Spectrum& n, const Spectrum& k);

  /**
   * Add a layer to the system. The layer will be placed under the last one
   * (Close to the substrate).
   * @param n : the real part of the indices.
   * @param k : the imaginary part of the indices.
   * @param thickness : the thickness of the layer (must be in meters).
   */
  inline void addLayer(const Spectrum& n, const Spectrum& k, const Real& thickness);

  /**
   * Compute the reflectance and the transmittance for a given incidence.
   * @param cosOi : the cosinus of the incidence angle.
   * @param Rs : the S-Polarised reflectance will be placed here.
   * @param Rp : the P-Polarised reflectance will be placed here.
   * @param Ts : the S-Polarised transmittance will be placed here.
   * @param Tp : the P-Polarised transmittance will be placed here.
   */
  inline void computeReflectanceTransmittance(const Real cosOi, const Real& lambda, unsigned int ilambda,Real& Rs, Real& Rp, Real& Ts, Real& Tp);

private:
  class Layer{
  public :
    Real thickness;
    Spectrum nj;
    Spectrum kj;

    /**
     * Compute the corresponding matrix of a thin layer.
     * @param cosO2 : the angle to the normal of the refracted ray in the thin layer
     * @param lambda : the wavelength (in meters) of the spectral band considered
     * @param ilambda : the global spectrum index of the spectral band considered
     * @param ms : the computed matrix values for the S-polarization will be place here.
     * @param mp : the computed matrix values for the P-polarization will be place here.
     */
    inline void computeAbelesMatrix(const Complex& cosOj, const Real& lambda, unsigned int ilambda, Complex ms[4], Complex mp[4]);
  };

  std::vector<Layer> _layers;
  Spectrum _na;
  Spectrum _ka;
  Spectrum _ns;
  Spectrum _ks;

  /**
   * Compute the Abeles matrix for the layer system.
   * @param cosOi : the cosinus of the incidence angle.
   * @param lambda : the wavelength (in meters) of the spectral band considered
   * @param ilambda : the global spectrum index of the spectral band considered
   * @param ms : the computed matrix values for the S-polarization will be place here.
   * @param mp : the computed matrix values for the P-polarization will be place here.
   */
  inline void computeAbelesMatrix(const Real cosOi, const Real& lambda, unsigned int ilambda,Complex ms[4], Complex mp[4]);
};

/**
 * Set the outside indices (set imaginary part to 0).
 * @param n : the real part of the indices.
 */
inline void ThinLayerSystem::setOutsideIndices(const Spectrum& n)
{
  _na=n;
  _ka.clear();
}

/**
 * Set the outside indices
 * @param n : the real part of the indices.
 * @param ka: the imaginary part of the indices.
 */
inline void ThinLayerSystem::setOutsideIndices(const Spectrum& n, const Spectrum& k)
{
  _na=n;
  _ka=k;
}

/**
 * Set the substrate indices (set imaginary part to 0).
 * @param n : the real part of the indices.
 */
inline void ThinLayerSystem::setSubstrateIndices(const Spectrum& n)
{
  _ns=n;
  _ks.clear();
}

/**
 * Set the substrate indices
 * @param n : the real part of the indices.
 * @param k : the imaginary part of the indices.
 */
inline void ThinLayerSystem::setSubstrateIndices(const Spectrum& n, const Spectrum& k)
{
  _ns=n;
  _ks=k;
}

/**
 * Add a layer to the system. The layer will be placed under the last one
 * (Close to the substrate).
 * @param n : the real part of the indices.
 * @param k : the imaginary part of the indices.
 * @param thickness : the thickness of the layer (must be in meters).
 */
inline void ThinLayerSystem::addLayer(const Spectrum& n, const Spectrum& k, const Real& thickness)
{
  Layer l;
  l.nj=n;
  l.kj=k;
  l.thickness=thickness;
  _layers.push_back(l);
}

/**
 * Compute the reflectance and the transmittance for a given incidence.
 * @param cosOi : the cosinus of the incidence angle.
 * @param Rs : the S-Polarised reflectance will be placed here.
 * @param Rp : the P-Polarised reflectance will be placed here.
 * @param Ts : the S-Polarised transmittance will be placed here.
 * @param Tp : the P-Polarised transmittance will be placed here.
 */
inline void ThinLayerSystem::computeReflectanceTransmittance(const Real cosOi, const Real& lambda, unsigned int ilambda,Real& Rs, Real& Rp, Real& Ts, Real& Tp)
{
  //Getting the indices of air and substrate
  Complex na(_na[ilambda], 0.0);
  Complex ns(_ns[ilambda], -_ks[ilambda]);

  // Compute cosOs (Substrate)
  Complex cosOs = (1 - (na/ns)*(na/ns)*(1 - cosOi*cosOi)).sqrt();

  //Compute qj's
  Complex qas = na*cosOi;
  Complex qap = na/cosOi;
  Complex qss = ns*cosOs;
  Complex qsp = ns/cosOs;

  //Computing the Abeles Matrix
  Complex ms[4], mp[4];
  computeAbelesMatrix(cosOi, lambda, ilambda, ms, mp);
  
  //Compute amplitude reflectance and tranmittance
  Complex rs = (qas*ms[0] - qss*ms[3] + qas*qss*ms[1] - ms[2])
                 / (qas*ms[0] + qss*ms[3] + qas*qss*ms[1] + ms[2]);
  Complex rp = (qap*mp[0] - qsp*mp[3] + qap*qsp*mp[1] - mp[2])
                 / (qap*mp[0] + qsp*mp[3] + qap*qsp*mp[1] + mp[2]);
  Complex ts = (2*qas) / (qas*ms[0] + qss*ms[3] + qas*qss*ms[1] + ms[2]);
  Complex tp = (2*qap) / (qap*mp[0] + qsp*mp[3] + qap*qsp*mp[1] + mp[2]);

  //Compute final reflectance and transmittance
  Rs = rs.squareAbs();
  Rp = rp.squareAbs();
  Ts = (qss.re/qas.re)*ts.squareAbs();
  Tp = (qsp.re/qap.re)*tp.squareAbs();
}

/**
 * Compute the Abeles matrix for the layer system.
 * @param cosOi : the cosinus of the incidence angle.
 * @param lambda : the wavelength (in meters) of the spectral band considered
 * @param ilambda : the global spectrum index of the spectral band considered
 * @param ms : the computed matrix values for the S-polarization will be place here.
 * @param mp : the computed matrix values for the P-polarization will be place here.
 */
inline void ThinLayerSystem::computeAbelesMatrix(const Real cosOi, const Real& lambda, unsigned int ilambda,Complex ms[4], Complex mp[4])
{
  const Complex zero(0.0, 0.0);
  const Complex one (1.0, 0.0);

  //Building the Abeles Matrix
  ms[0]=one; ms[1]=zero; ms[2]=zero; ms[3]=one; //Set to identity
  mp[0]=one; mp[1]=zero; mp[2]=zero; mp[3]=one; //Set to identity
  
  for(unsigned int i=0; i<_layers.size(); i++)
  {
    Complex mjs[4];
    Complex mjp[4];

    // Compute cosOj
    Complex na = Complex(_na[ilambda], 0.0);
    Complex nj = Complex(_layers[i].nj[ilambda], -_layers[i].kj[ilambda]);
    Complex cosOj = (1 - (na/nj)*(na/nj)*(1 - cosOi*cosOi)).sqrt();

    //Get the matrix of the current layer
    _layers[i].computeAbelesMatrix(cosOj, lambda, ilambda, mjs, mjp);

    //Compute the matrix of the system
    Complex mts[4]={ms[0],ms[1],ms[2],ms[3]};
    Complex mtp[4]={mp[0],mp[1],mp[2],mp[3]};
    
    ms[0] = mjs[0]*mts[0] + mjs[1]*mts[2];
    ms[1] = mjs[0]*mts[1] + mjs[1]*mts[3];
    ms[2] = mjs[2]*mts[0] + mjs[3]*mts[2];
    ms[3] = mjs[2]*mts[1] + mjs[3]*mts[3];

    mp[0] = mjp[0]*mtp[0] + mjp[1]*mtp[2];
    mp[1] = mjp[0]*mtp[1] + mjp[1]*mtp[3];
    mp[2] = mjp[2]*mtp[0] + mjp[3]*mtp[2];
    mp[3] = mjp[2]*mtp[1] + mjp[3]*mtp[3];
  }
}


/**
 * Compute the corresponding matrix of a thin layer.
 * @param cosOj : the angle to the normal of the refracted ray in the thin layer
 * @param lambda : the wavelength (in meters) of the spectral band considered
 * @param ilambda : the global spectrum index of the spectral band considered
 * @param ms : the computed matrix values for the S-polarization will be place here.
 * @param mp : the computed matrix values for the P-polarization will be place here.
 */
inline void ThinLayerSystem::Layer::computeAbelesMatrix(const Complex& cosOj, const Real& lambda, unsigned int ilambda, Complex ms[4], Complex mp[4])
{
  Complex n(nj[ilambda], -kj[ilambda]);

  Real k = 2*M_PI / lambda;
  Complex pj = k * n * cosOj * thickness;

  Complex qjs = n * cosOj;
  Complex qjp = n / cosOj;

  Complex cospj = pj.cos();
  Complex sinpj = pj.sin();

  ms[0] = cospj;
  ms[1] = Complex::i()*sinpj/qjs;
  ms[2] = Complex::i()*sinpj*qjs;
  ms[3] = cospj;  

  mp[0] = cospj;  
  mp[1] = Complex::i()*sinpj/qjp;
  mp[2] = Complex::i()*sinpj*qjp;
  mp[3] = cospj;
}

#endif //_THINLAYERSYSTEM_HPP

