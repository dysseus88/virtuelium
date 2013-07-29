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

#ifndef _DIELECTRICFORMULA_HPP
#define _DIELECTRICFORMULA_HPP

#include <common.hpp>
#include <core/3DBase.hpp>
#include <maths/Complex.hpp>

class DielectricFormula {
public :
  /**
   * Compute the dielectric amplitude reflectance in the reflexion direction with the
   * fresnel formula.
   * @param cosOi : the cosinus of the angle between the normal and incident.
   * @param n : the complex index of the medium
   * @param ROrth : the S-polarised computed Reflectance will be placed here
   * @param Rpara : the P-polarised computed Reflectance will be placed here
   */
  static inline void complexFresnelReflectance(const Real& cosOi, const Complex& n, Complex& Rorth, Complex& Rpara);

  /**
   * Compute the dielectric amplitude reflectance in the reflexion direction with the
   * fresnel formula.
   * @param cosOi : the cosinus of the angle between the normal and incident.
   * @param n : the complex index of the medium
   * @param ROrth : the S-polarised computed Reflectance will be placed here
   * @param Rpara : the P-polarised computed Reflectance will be placed here
   */
  static inline void fresnelReflectance(const Real& cosOi, const Complex& n, Real& Rorth, Real& Rpara);

  /**
   * Compute the attenuation factor and return it.
   * @param distance : the length of the way throught the object
   * @param k : the imaginary part of the index divided by n : N = n(1 + ik)
   * @return the computed absorption factor
   */
  static inline Real dielectricAbsorption(const Real& distance, const Real& wavelength, const Real& k);

  /**
   * Compute the integrated reflectance on a dielectric.
   * @param n : the complex index of the medium
   */
  static inline Real integratedDielectricReflectance(const Complex& n);

  /**
   * Reflect the given vector.
   * @param normal : the local normal of the surface.
   * @param ray : the ray to reflect, this parameter will be changed.
   */
  static inline void reflect(const Vector& normal, Vector& ray);

  /**
   * Refract the given vector.
   * @param normal : the local normal of the surface.
   * @param index : the relative index of refaction (n2/n1).
   * @param ray : the ray to reflect, this parameter will be changed.
   * @return false if the ray can not be refracted (total reflexion)
   */
  static inline bool refract(const Vector& normal, Real index, Vector& ray);
};

/**
 * Compute the dielectric amplitude reflectance in the reflexion direction with the
 * fresnel formula.
 * @param cosOi : the cosinus of the angle between the normal and incident.
 * @param n : the complex index of the medium
 * @param ROrth : the S-polarised computed Reflectance will be placed here
 * @param Rpara : the P-polarised computed Reflectance will be placed here
 */
inline void DielectricFormula::complexFresnelReflectance(const Real& cosOi, const Complex& n1, Complex& rOrth, Complex& rPara)
{
  Real cos2Oi = cosOi*cosOi;
  Real sin2Oi = 1 - cos2Oi;
  
  Complex n2 = n1*n1;

  Complex gamma = (n2 - sin2Oi).sqrt() ;

  rPara = (n2*cosOi - gamma)/(n2*cosOi + gamma);
  rOrth = (cosOi - gamma)/(cosOi + gamma);
}

/**
 * Compute the dielectric amplitude reflectance in the reflexion direction with the
 * fresnel formula.
 * @param cosOi : the cosinus of the angle between the normal and incident.
 * @param n : the complex index of the medium
 * @param ROrth : the S-polarised computed Reflectance will be placed here
 * @param Rpara : the P-polarised computed Reflectance will be placed here
 */
inline void DielectricFormula::fresnelReflectance(const Real& cosOi, const Complex& n1, Real& Rorth, Real& Rpara)
{
  Complex rPara, rOrth;
  complexFresnelReflectance(cosOi, n1, rOrth, rPara);
  Rpara = (rPara).abs();
  Rorth = (rOrth).abs();
  
  if (Rpara > Real(1.0))
    Rpara = Real(1.0);
    
  if (Rorth > Real(1.0))
    Rorth = Real(1.0);
}

/**
 * Compute the attenuation factor and return it.
 * @param distance : the length of the way throught the object
 * @param k : the imaginary part of the index divided by n : N = n(1 + ik)
 * @return the computed absorption factor
 */
inline Real DielectricFormula::dielectricAbsorption(const Real& distance, const Real& wavelength, const Real& k)
{
  Real a = (4 * M_PI * k) / (wavelength);
  return std::exp(- a * distance);
}

/**
 * Compute the integrated reflectance on a dielectric.
 * @param n : the complex index of the medium
 */
inline Real DielectricFormula::integratedDielectricReflectance(const Complex& n)
{
  const unsigned int samples=45;

  Real R=0;  
  for(unsigned int i=0; i<samples; i++)
  {
    Real Oi = i*0.5*M_PI/(Real)samples;
    Real cosOi = std::cos(Oi);
    Real rs, rp;
    fresnelReflectance(cosOi, n, rs, rp);
    R += std::sin(2*Oi)*(rs*rs+rp*rp)*0.5;
  }
  R /=(Real)samples;

  return R;
}

/**
 * Reflect the given vector.
 * @param normal : the local normal of the surface.
 * @param ray : the ray to reflect, this parameter will be changed.
 */
inline void DielectricFormula::reflect(const Vector& normal, Vector& ray)
{
  Real cosOi = -normal.dot(ray);
  Real zcoef = 2.0 * cosOi;
  ray[0] += normal[0]*zcoef;
  ray[1] += normal[1]*zcoef;
  ray[2] += normal[2]*zcoef;
}

/**
 * Refract the given vector.
 * @param normal : the local normal of the surface.
 * @param index : the relative index of refaction (n2/n1).
 * @param ray : the ray to reflect, this parameter will be changed.
 * @return false if the ray can not be refracted (total reflexion)
 */
inline bool DielectricFormula::refract(const Vector& normal, Real n, Vector& ray)
{
  Real cosOi = -normal.dot(ray);
  if(cosOi<0)
  {
    Vector norm = normal;
    norm.mul(-1.0);
    return refract(norm, 1.0/n, ray);
  }

  Real n2 = n*n;
  Real sin2Oi = 1 - cosOi*cosOi;  

  //Total reflection
  if(sin2Oi/n2 > 1.0)
    return false;

  Real cosOv = std::sqrt(1 - sin2Oi/n2);
  
  ray[0] = (ray[0] + normal[0]*cosOi)/n - normal[0]*cosOv;
  ray[1] = (ray[1] + normal[1]*cosOi)/n - normal[1]*cosOv;
  ray[2] = (ray[2] + normal[2]*cosOi)/n - normal[2]*cosOv;
  ray.normalize();
  return true;
}


#endif// _DIELECTRICFORMULA_HPP

