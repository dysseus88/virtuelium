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
#ifndef GUARD_VRT_BECKMANNROUGHNESSFORMULA_HPP
#define GUARD_VRT_BECKMANNROUGHNESSFORMULA_HPP
//!
//! @file BeckmannRoughnessFormula.hpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file defines beckmann functions for rough mayerials
//! @remarks This version of virtuelium is MPI-only. please refer to MPI docs!
//!
#include <stdio.h>

#include <common.hpp>
#include <core/VrtLog.hpp>

////////////////////////////////////////////////////////////////////////////////
//! @class BeckmannRoughnessFormula
//! @brief Defines beckmann functions for rough mayerials
//! @todo Implement anisotropic roughness
class BeckmannRoughnessFormula {
 public:
  //! @brief Compute the Beckmann factor for the surface diffusion
  //! @details Main beckmann function
  //! @param nh Cosinus between the k vector of the local basis to the incident 
  //!  point and the microfacet normal
  //! @param beck_mij Beckmann rugosity factor
  //! @return Beckmann distribution for the given incident point
  //! @remaks The k vector of the local basis is the normal of the based surface
  //! @remaks beck_mij = 2o/T with:
  //!  @arg 2o: the average level of the surface asperities due to rugosity
  //!  @arg T: Corerelation length between two consecutive asperities.
  static inline Real BeckmannDistribution(
      const Real& nh, 
      const Real& beck_mij); 
  //! @brief Compute the Shadowing/Masking factor
  //! @details The shadow/masking mechansism simulates the fact surface 
  //!  asperities may create micro-shadows around them
  //! @param ln Cosinus between the k vector of the local basis to the incident 
  //!  point and the incident ray
  //! @param vn Cosinus between the k vector of the local basis to the incident 
  //!  point and the observer ray
  //! @param vh Cosinus between the microfacet normal and the observer ray
  //! @param nh Cosinus between the k vector of the local basis to the incident 
  //!  point and the microfacet normal
  //! @return Shadow/masking factor for the given incident point
  //! @remaks The k vector of the local basis is the normal of the based surface
  static inline Real BeckmannShadowMasking(
      const Real& ln, 
      const Real& vn,  
      const Real& vh, 
      const Real& nh); 
 
 public: 
  //! @brief Compute a weighted random ray with a dome random distribution
  //! @param localBasis Local Basis to the incident point
  //! @param weight Weight of the returned ray
  //! @param dir Returned ray
  static inline void getDomeRandomRay(
      const Basis& localBasis, 
      Real& weight, 
      Vector& dir);
  //! @brief Compute a weighted random ray with a beckmann random distribution
  //! @param localBasis Local Basis to the incident point
  //! @param view Observer direction
  //! @param beck_mij Beckmann rugosity factor
  //! @param weight Weight of the returned ray
  //! @param dir Returned ray
  //! @remaks beck_mij = 2o/T with:
  //!  @arg 2o: the average level of the surface asperities due to rugosity
  //!  @arg T: Corerelation length between two consecutive asperities.
  static inline void getBeckmannRandomRay(
      const Basis& localBasis, 
      Vector view, 
      Real beck_mij, 
      Real& weight, 
      Vector& dir);  
  //! @brief Get random reflexion direction
  //! @param normal k vector of the local basis to the incident point
  //! @param dir Returned ray
  //! @param beck_mij Beckmann rugosity factor
  //! @remaks beck_mij = 2o/T with:
  //!  @arg 2o: the average level of the surface asperities due to rugosity
  //!  @arg T: Corerelation length between two consecutive asperities.
  static inline void reflect(
      const Vector& normal, 
      Vector& dir, 
      Real beck_mij);
  
 public:
  //! @brief Get the diffuse reflection factor
  //! @details Only used by RoughvarnishedLambertianBRDF
  //! @param ln Cosinus between the k vector of the local basis to the incident 
  //!  point and the incident ray
  //! @param beck_mij Beckmann rugosity factor
  //! @param fr Base factors in thin layer model
  //! @return Reflection factor
  //! @remaks The k vector of the local basis is the normal of the based surface
  //! @remaks beck_mij = 2o/T with:
  //!  @arg 2o: the average level of the surface asperities due to rugosity
  //!  @arg T: Corerelation length between two consecutive asperities.
  static inline Real getDiffuseReflectionFactor(
      const Real& ln, 
      const Real& beck_mij, 
      const std::vector<Real>& Fr);
  //! @brief Get the diffuse transmission factor
  //! @details Only used by RoughvarnishedLambertianBRDF
  //! @param ln Cosinus between the k vector of the local basis to the incident 
  //!  point and the incident ray
  //! @param beck_mij Beckmann rugosity factor
  //! @param ft Base factors in thin layer model
  //! @param ni 
  //! @param no
  //! @return Transmission factor
  //! @remaks The k vector of the local basis is the normal of the based surface
  //! @remaks beck_mij = 2o/T with:
  //!  @arg 2o: the average level of the surface asperities due to rugosity
  //!  @arg T: Corerelation length between two consecutive asperities.
  static inline Real getDiffuseTransmissionFactor(
      const Real ln, 
      const Real& beck_mij, 
      const std::vector<Real>& ft, 
      const Real& ni, 
      const Real& no);
}; // class BeckmannRoughnessFormula
////////////////////////////////////////////////////////////////////////////////
// DEFINTIONS
////////////////////////////////////////////////////////////////////////////////
inline Real BeckmannRoughnessFormula::BeckmannDistribution(
    const Real& nh, 
    const Real& Beck_mij) {
  
  // nh very small (NH near to 90°) : return 0  
  if (nh < kEPSILON) {
    return kEPSILON;
  } 

  // roughness very small : dirac at nh = 1
  if (Beck_mij < kEPSILON) {
    if (nh > Real(1.0) - kEPSILON) 
      return Real(1.0);
    return kEPSILON;
  }
  
  Real Beck_m = Beck_mij * Beck_mij;
  Real nh_2 = nh * nh;
  Real arg = (nh_2 - Real(1.0)) / (nh_2 * Beck_m);
    
  Real result = std::exp(arg) / (Real(4.0) * M_PI * Beck_m * nh_2 * nh_2);
  if (result < kEPSILON)
    result = Real(kEPSILON);
  
  //Real norm = Real(1.0) / (Real(4.0) * M_PI * Beck_m);
  //result /= norm;

  return result;
}
////////////////////////////////////////////////////////////////////////////////
inline Real BeckmannRoughnessFormula::BeckmannShadowMasking(
    const Real& ln, 
    const Real& vn,  
    const Real& vh, 
    const Real& nh) {
  
 Real mG1 = ((Real(2.0) * nh) / vh) * minimum(vn, ln); 
 mG1 /= (ln * vn);
 return fabs(minimum(Real(1.0), mG1));
}
////////////////////////////////////////////////////////////////////////////////
inline void BeckmannRoughnessFormula::getDomeRandomRay(
    const Basis& localBasis, 
    Real& weight, 
    Vector& dir) {

  Real norm2;
  do {
    dir[0] = rand() * Real(2.0) / RAND_MAX - Real(1.0);
    dir[1] = rand() * Real(2.0) / RAND_MAX - Real(1.0);
    dir[2] = rand() * Real(2.0) / RAND_MAX - Real(1.0);
    norm2=dir.square();
  } while(norm2 > Real(1.0) || dir.dot(localBasis.k) <= Real(0.0));
  dir.normalize();
  weight = Real(0.5)  / M_PI;
}
////////////////////////////////////////////////////////////////////////////////
inline void BeckmannRoughnessFormula::getBeckmannRandomRay(
    const Basis& localBasis, 
    Vector view, 
    Real beck_mij, 
    Real& weight, 
    Vector& dir) {

  Real totalWeight = 0;
  int numsamples = 0;

  for(int i = 0; i < 100; i++) {
    // get dir in the positive hemisphere and weight = 1/2PI
    getDomeRandomRay(localBasis, weight, dir);
    // micronormal
    Vector microNormal;
    microNormal.setsum(dir, view);
    microNormal.normalize();
    // compute weight
    weight = BeckmannDistribution(localBasis.k.dot(microNormal), beck_mij);
    // add to count
    totalWeight+=weight;
    numsamples++;    
  }
  
  do{
    // get dir in the positive hemisphere and weight = 1/2PI
    getDomeRandomRay(localBasis, weight, dir);
    // micronormal
    Vector microNormal;
    microNormal.setsum(dir, view);
    microNormal.normalize();
    // compute weight
    weight = BeckmannDistribution(localBasis.k.dot(microNormal), beck_mij);
    
    // add to count
    totalWeight+=weight;
    numsamples++;
  } while(rand()/ (Real)RAND_MAX > weight);

  weight = weight * (0.5 / M_PI)/(totalWeight / numsamples);
}
////////////////////////////////////////////////////////////////////////////////
inline void BeckmannRoughnessFormula::reflect(
    const Vector& normal, 
    Vector& dir, 
    Real beck_mij) {

  Vector incident = dir;
  incident.mul(-1.0);

  Real weight;
  do {
    // Get a random ray on a uniform distribution
    Real norm2;
    do {
    dir[0] = rand() * Real(2.0) / RAND_MAX - Real(1.0);
    dir[1] = rand() * Real(2.0) / RAND_MAX - Real(1.0);
    dir[2] = rand() * Real(2.0) / RAND_MAX - Real(1.0);
      norm2 = dir.square();
    } while(norm2 > Real(1.0) || dir.dot(normal) <= Real(0.0));
    
    dir.normalize();
    // micronormal
    Vector microNormal;
    microNormal.setsum(dir, incident);
    microNormal.normalize();
    // compute weight
    weight = BeckmannDistribution(normal.dot(microNormal), beck_mij);
  } 
  while(rand()/(float)RAND_MAX > weight);
}
////////////////////////////////////////////////////////////////////////////////
inline Real BeckmannRoughnessFormula::getDiffuseReflectionFactor(
    const Real& cosOi, 
    const Real& beck_mij, 
    const std::vector<Real>& Fr) {
  unsigned int samples = Fr.size();

  Vector n(0.0, 0.0, 1.0);
  Vector wi(std::sqrt(1.0 - cosOi*cosOi), 0.0, cosOi);

  Real sum=0;
  for(unsigned int i=0; i<samples; i++)
    for(unsigned int j=0; j<samples; j++)  {
      Real Oo = 0.5*M_PI*i/(Real)samples;
      Real Po = 2.0*M_PI*j/(Real)samples;

      Vector wo(std::sin(Oo)*std::cos(Po), std::sin(Oo)*std::sin(Po), std::cos(Oo));
      Vector h(wi[0]+wo[0], wi[1]+wo[1], wi[2]+wo[2]);
      h.normalize();

      Real cosIN = wi[2];     // incident and normal
      Real cosON = wo[2];     // view and normal 
      Real cosHN = h[2];      // micro normal and normal
      Real cosIH = h.dot(wi); // incident and micro normal
      Real cosOH = h.dot(wo); // view and micro normal
     
      Real beckmann = BeckmannDistribution(cosHN, beck_mij);
      Real shadmask = BeckmannShadowMasking(cosIN, cosON, cosOH, cosHN);
      
      Real Oi = std::acos(cosIH);
      int index = (int)(2.0*samples*Oi/M_PI);
      if(index < 0) index=0;
      if(index >= (int)samples) index=samples-1;
      sum += beckmann*shadmask*Fr[index]*std::sin(Oo);
    }
  
  return 2.0*M_PI*sum/(Real)(samples*samples);
}
////////////////////////////////////////////////////////////////////////////////
inline Real BeckmannRoughnessFormula::getDiffuseTransmissionFactor(
    const Real cosOi, 
    const Real& beck_mij, 
    const std::vector<Real>& Ft, 
    const Real& ni, 
    const Real& no) {
  unsigned int samples = Ft.size();

  Vector n(0.0, 0.0, 1.0);
  Vector wi(std::sqrt(1.0 - cosOi*cosOi), 0.0, cosOi);

  Real sum=0;
  for(unsigned int i=0; i<samples; i++)
    for(unsigned int j=0; j<samples; j++)
    {
      Real Oo = 0.5*M_PI*i/(Real)samples;
      Real Po = 2.0*M_PI*j/(Real)samples;

      Vector wo(std::sin(Oo)*std::cos(Po), std::sin(Oo)*std::sin(Po), -std::cos(Oo));
      Vector h(-(ni*wi[0]+no*wo[0]), -(ni*wi[1]+no*wo[1]), -(ni*wi[2]+no*wo[2]));
      h.normalize();

      if(h[2]<0)
        h.mul(-1.0);

      Real cosIN = std::abs(wi[2]);     // incident and normal
      Real cosON = std::abs(wo[2]);     // view and normal 
      Real cosHN = std::abs(h[2]);      // micro normal and normal
      Real cosIH = std::abs(h.dot(wi)); // incident and micro normal
      Real cosOH = std::abs(h.dot(wo)); // view and micro normal
     
      Real beckmann = BeckmannDistribution(cosHN, beck_mij);
      Real shadmask = BeckmannShadowMasking(cosIN, cosON, cosOH, cosHN);
      
      Real Oi = std::acos(cosIH);
      int index = (int)(2.0*samples*Oi/M_PI);
      if(index < 0) index=0;
      if(index >= (int)samples) index=samples-1;

      sum += ((cosIH*cosOH)/(cosIN*cosON)) * (no*no *(Ft[index])*beckmann*shadmask)
           / ((ni*cosIH - no*cosOH)*(ni*cosIH - no*cosOH));
    } 
  return 2.0 * M_PI * sum / (Real)(samples * samples);
}
////////////////////////////////////////////////////////////////////////////////
#endif // _BECKMANNROUGHNESSFORMULA_HPP
