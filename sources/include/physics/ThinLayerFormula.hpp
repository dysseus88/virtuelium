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

#ifndef _THINLAYERFORMULA_HPP
#define _THINLAYERFORMULA_HPP

#include <core/3DBase.hpp>
#include <core/LightBase.hpp>


#include <physics/DielectricFormula.hpp>

class ThinLayerFormula {
public :
  /**
   * Compute the reflected amplitude of a thin layer.
   * @param cosO2 : the angle to the normal of the refracted ray in the thin layer
   * @param lambda : the wavelength (in nanometer) of the spectral band considered
   * @param thickness : the thickness of the layer in nanometer
   * @param n2 : the real part of the indice of the layer
   * @param r12 : the reflectance of the first interface
   * @param r13 : the reflectance of the second interface
   */
  static inline void thinLayerReflectance(const Real& cosO2, const Real& lambda, const Real& thickness, const Complex& n2, const Complex& r12, const Complex& r23, Complex& r13);

  /**
   * Compute the reflected amplitude of a thin layer system. in the differents arrays, the closest layers from the surface are the first.
   * @param cosOi : the incident angle of the light
   * @param spectralBand : the index of the considered spectral band
   * @param n : an array of nbLayer spectrum that contain the real part of the absolutes indices of the layers
   * @param k : an array of nbLayer spectrum that contain the imaginary (k of n + ik) part of the absolutes indices of the layers
   * @param thickness : an array that contain the thickness (in nanometers) of each layer
   * @param nbLayer : the number of thin layer.
   * @param rs : the the S-polarised computed amplitude reflectance will be placed here
   * @param rp : the the P-polarised computed amplitude reflectance will be placed here
   */
  static inline void thinLayerSystemReflectance(const Real& cosOi, const unsigned int& spectralBand, const Spectrum* n, const Spectrum* k, const Real* thickness, unsigned int nbLayer, Real& rs, Real& rp);

  /**
   * Compute the corresponding matrix of a thin layer.
   * @param cosO2 : the angle to the normal of the refracted ray in the thin layer
   * @param lambda : the wavelength (in nanometer) of the spectral band considered
   * @param thickness : the thickness of the layer in nanometer
   * @param n2 : the complex indice of the layer
   * @param ms : the computed matrix values for the S-polarization will be place here.
   * @param mp : the computed matrix values for the S-polarization will be place here.
   */
  static inline void thinLayerMatrix(const Complex& cosO2, const Real& lambda, const Real& thickness, const Complex& n2, Complex ms[4], Complex mp[4]);

  /**
   * Compute the reflected amplitude of a thin layer system. in the differents arrays, the closest layers from the surface are the first.
   * @param cosOi : the incident angle of the light
   * @param spectralBand : the index of the considered spectral band
   * @param n : an array of nbLayer spectrum that contain the real part of the absolutes indices of the layers
   * @param k : an array of nbLayer spectrum that contain the imaginary (k of n + ik) part of the absolutes indices of the layers
   * @param thickness : an array that contain the thickness (in nanometers) of each layer
   * @param nbLayer : the number of thin layer.
   * @param rs : the the S-polarised computed amplitude reflectance will be placed here
   * @param rp : the the P-polarised computed amplitude reflectance will be placed here
   */
  static inline void thinLayerSystemReflectanceByMatrixComputation(const Real& cosOi, const unsigned int& spectralBand, const Spectrum* n, const Spectrum* k, const Real* thickness, unsigned int nbLayer, Real& rs, Real& rp);
};

/**
 * Compute the reflected amplitude of a thin layer.
 * @param cosO2 : the angle to the normal of the refracted ray in the thin layer
 * @param lambda : the wavelength (in nanometer) of the spectral band considered
 * @param thickness : the thickness of the layer in nanometer
 * @param n2 : the real part of the indice of the layer
 * @param r12 : the reflectance of the first interface
 * @param r13 : the reflectance of the second interface
 */
inline void ThinLayerFormula::thinLayerReflectance(const Real& cosO2, const Real& lambda, const Real& thickness, const Complex& n2, const Complex& r12, const Complex& r23, Complex& r13)
{
  Real phi = 4 * thickness * n2.re * cosO2 * M_PI / lambda;
  
  Complex argument = Complex(std::cos(phi), std::sin(phi)) * DielectricFormula::dielectricAbsorption(2*thickness/cosO2, lambda, n2.im/n2.re);

  r13 = r12 + (r23 * (1-r12*r12) * argument) / (1 - r23*r12*argument);

  //  r13 = (r12 + r23*argument) / (1 + r12*r23*argument);
}

/**
 * Compute the reflected amplitude of a thin layer system. in the differents arrays, the closest layers from the surface are the first.
 * @param cosOi : the incident angle of the light
 * @param spectralBand : the index of the considered spectral band
 * @param n : an array of nbLayer spectrum that contain the real part of the absolutes indices of the layers
 * @param k : an array of nbLayer spectrum that contain the imaginary (k of n + ik) part of the absolutes indices of the layers
 * @param thickness : an array that contain the thickness (in nanometers) of each layer
 * @param nbLayer : the number of thin layer.
 * @param rs : the the S-polarised computed amplitude reflectance will be placed here
 * @param rp : the the P-polarised computed amplitude reflectance will be placed here
 */
inline void ThinLayerFormula::thinLayerSystemReflectance(const Real& cosOi, const unsigned int& spectralBand, const Spectrum* n, const Spectrum* k, const Real* thickness, unsigned int nbLayer, Real& rs, Real& rp)
{
  Real lambda = GlobalSpectrum::getWaveLength(spectralBand);
  Complex r12p, r12s;
  Complex r23p(0,0), r23s(0,0), r13p(0,0), r13s(0,0);
  for(int i=(int)nbLayer-2; i>=0; i--)
  {
    //Computing indices
    Real n1 = (i==0)? 1.0 : n[i-1][spectralBand];
    Real n2 = n[i][spectralBand];
    Real n3 = n[i+1][spectralBand];
    Real n12 = n2/n1;
    Real n23 = n3/n2;

    //Computing cosinus
    Real cosO1 = std::sqrt(1 - (1-cosOi*cosOi)/(n1*n1));
    Real cosO2 = std::sqrt(1 - (1-cosOi*cosOi)/(n2*n2));

    //Computing r23
    if(i==(int)nbLayer-2)
    {
      DielectricFormula::complexFresnelReflectance(cosO2, Complex(n23, k[i+1][spectralBand]), r23s, r23p);
    }
    else
    {
      r23s=r13s;
      r23p=r13p;
    }

    //Computing r12
    DielectricFormula::complexFresnelReflectance(cosO1, Complex(n12, k[i][spectralBand]), r12s, r12p);

    //Computing r13
    thinLayerReflectance(cosO2, lambda, thickness[i], Complex(n2, k[i][spectralBand]), r12p, r23p, r13p);
    thinLayerReflectance(cosO2, lambda, thickness[i], Complex(n2, k[i][spectralBand]), r12s, r23s, r13s);
  }

  //Compute the final amplitude of the reflectance
  rs = r13s.abs();
  rp = r13p.abs();
}

/**
 * Compute the corresponding matrix of a thin layer.
 * @param cosO2 : the angle to the normal of the refracted ray in the thin layer
 * @param lambda : the wavelength (in nanometer) of the spectral band considered
 * @param thickness : the thickness of the layer in nanometer
 * @param n2 : the complex indice of the layer
 * @param ms : the computed matrix values for the S-polarization will be place here.
 * @param mp : the computed matrix values for the S-polarization will be place here.
 */
inline void ThinLayerFormula::thinLayerMatrix(const Complex& cosO2, const Real& lambda, const Real& thickness, const Complex& n2, Complex ms[4], Complex mp[4])
{
  Real k = 2*M_PI * thickness / lambda;
  Complex phi = k*n2 * cosO2;

  Complex qs = n2*cosO2;
  Complex qp = n2/cosO2;

  Complex cosphi = phi.cos();
  Complex isinphi = phi.sin()*Complex::i();

  ms[0] = cosphi;
  ms[1] = isinphi/qs;
  ms[2] = isinphi*qs;
  ms[3] = cosphi;  

  mp[0] = cosphi;  
  mp[1] = isinphi/qp;
  mp[2] = isinphi*qp;
  mp[3] = cosphi;  
}

/**
 * Compute the reflected amplitude of a thin layer system. in the differents arrays, the closest layers from the surface are the first.
 * @param cosOi : the incident angle of the light
 * @param spectralBand : the index of the considered spectral band
 * @param n : an array of nbLayer spectrum that contain the real part of the absolutes indices of the layers
 * @param k : an array of nbLayer spectrum that contain the imaginary (k of n + ik) part of the absolutes indices of the layers
 * @param thickness : an array that contain the thickness (in nanometers) of each layer
 * @param nbLayer : the number of thin layer.
 * @param rs : the the S-polarised computed amplitude reflectance will be placed here
 * @param rp : the the P-polarised computed amplitude reflectance will be placed here
 */
inline void ThinLayerFormula::thinLayerSystemReflectanceByMatrixComputation(const Real& cosOi, const unsigned int& spectralBand, const Spectrum* n, const Spectrum* k, const Real* thickness, unsigned int nbLayer, Real& rs, Real& rp)
{
  Real lambda = GlobalSpectrum::getWaveLength(spectralBand);
  Complex ms[4]={Complex(1.0, 0.0), Complex(0.0, 0.0), Complex(0.0, 0.0), Complex(1.0, 0.0)}; //Initalized as identity
  Complex mp[4]={Complex(1.0, 0.0), Complex(0.0, 0.0), Complex(0.0, 0.0), Complex(1.0, 0.0)}; //Initalized as identity
  Complex mjs[4];
  Complex mjp[4];

  for(unsigned int i=0; i<nbLayer-1; i++)
  {
    //Computing cosO2
    Complex n2(n[i][spectralBand], -n[i][spectralBand]*k[i][spectralBand]);
    Complex cosO2 = (1 - (1-cosOi*cosOi)/(n2*n2));

    //Compute the i-th layer matrix
    thinLayerMatrix(cosO2, lambda, thickness[i], Complex(n[i][spectralBand], k[i][spectralBand]), mjs, mjp);

    //Add this layer to the overall computation
    Complex m11 = mjs[0]*ms[0] + mjs[1]*ms[2];
    Complex m12 = mjs[0]*ms[1] + mjs[1]*ms[3];
    Complex m21 = mjs[2]*ms[0] + mjs[3]*ms[2];
    Complex m22 = mjs[2]*ms[1] + mjs[3]*ms[3];
    ms[0]=m11;ms[1]=m12;ms[2]=m21;ms[3]=m22;

    m11 = mjp[0]*mp[0] + mjp[1]*mp[2];
    m12 = mjp[0]*mp[1] + mjp[1]*mp[3];
    m21 = mjp[2]*mp[0] + mjp[3]*mp[2];
    m22 = mjp[2]*mp[1] + mjp[3]*mp[3];
    mp[0]=m11;mp[1]=m12;mp[2]=m21;mp[3]=m22;
  }

  //Computing cosOs
  Real cosOs = std::sqrt(1 - (1-cosOi*cosOi)/(n[nbLayer-1][spectralBand]*n[nbLayer-1][spectralBand]));

  //Compute the final amplitude of the reflectance
  Real qas = 1.0*cosOi;
  Real qap = 1.0/cosOi;
  Real qss = 1.0*cosOs;
  Real qsp = 1.0/cosOs;

  Complex rcs = (qas*ms[0] - qss*ms[3] + qas*qss*ms[1] - ms[2])
              / (qas*ms[0] + qss*ms[3] + qas*qss*ms[1] + ms[2]);

  Complex rcp = (qap*mp[0] - qsp*mp[3] + qap*qsp*mp[1] - mp[2])
              / (qap*mp[0] + qsp*mp[3] + qap*qsp*mp[1] + mp[2]);

  rs = rcs.abs();
  rp = rcp.abs();
}

#endif// _THINLAYERFORMULA_HPP

