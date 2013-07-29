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
#ifndef GUARD_VRT_WARDROUGHNESSFORMULA_HPP
#define GUARD_VRT_WARDROUGHNESSFORMULA_HPP
//!
//! @file WardRoughnessFormula.hpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file defines Ward functions for rough mayerials
//! @remarks This version of virtuelium is MPI-only. please refer to MPI docs!
//!
#include <stdio.h>

#include <common.hpp>
#include <core/VrtLog.hpp>

////////////////////////////////////////////////////////////////////////////////
//! @class WardRoughnessFormula
//! @brief Defines Ward functions for rough mayerials
class WardRoughnessFormula {
 public:
  //! @brief Compute the Ward factor for the surface diffusion
  //! @details Main Ward function
  //! @param h Microfacet normal 
  //! @param nh Cosinus between the k vector of the local basis to the incident 
  //!  point and the microfacet normal
  //! @param ln Cosinus between the k vector of the local basis to the incident 
  //!  point and the incident ray
  //! @param vn Cosinus between the k vector of the local basis to the incident 
  //!  point and the observer ray
  //! @param Ward_mi Ward rugosity factor - direction i
  //! @param Ward_mj Ward rugosity factor - direction j
  //! @return Ward distribution for the given incident point
  //! @remaks We assumed all the input vector have already been normalized and are 
  //! rightly oriented
    static inline Real WardDistribution(
      const Vector& n, 
      const Vector& l, 
      const Vector& v, 
      const Vector& h, 
      const Real& Ward_mi,
      const Real& Ward_mj); 

 public: 
  //! @brief Compute a weighted random ray with a dome random distribution
  //! @param localBasis Local Basis to the incident point
  //! @param weight Weight of the returned ray
  //! @param dir Returned ray
  static inline void getDomeRandomRay(
      const Basis& localBasis, 
      Real& weight, 
      Vector& dir);
  //! @brief Compute a weighted random ray with a Ward random distribution
  //! @param localBasis Local Basis to the incident point
  //! @param view Observer direction
  //! @param Ward_mi Ward rugosity factor - direction i
  //! @param Ward_mj Ward rugosity factor - direction j
  //! @param weight Weight of the returned ray
  //! @param dir Returned ray
  static inline void getWardRandomRay(
      const Basis& localBasis, 
      Vector view, 
      Real Ward_mi,
      Real Ward_mj,
      Real& weight, 
      Vector& dir);  
  //! @brief Get random reflexion direction
  //! @param normal k vector of the local basis to the incident point
  //! @param dir Returned ray
  //! @param Ward_mi Ward rugosity factor - direction i
  //! @param Ward_mj Ward rugosity factor - direction j
  static inline void reflect(
      const Vector& normal, 
      Vector& dir, 
      Real Ward_mi,
      Real Ward_mj);
}; // class WardRoughnessFormula
////////////////////////////////////////////////////////////////////////////////
// DEFINTIONS
////////////////////////////////////////////////////////////////////////////////
inline Real WardRoughnessFormula::WardDistribution(
    const Vector& n, 
    const Vector& l, 
    const Vector& v, 
    const Vector& h, 
    const Real& Ward_mi,
    const Real& Ward_mj) {

  // Apply a small bias to the roughness coefficients to avoid divide-by-zero
  Real fAnisotropicRoughness[2] = { Ward_mi + kEPSILON, Ward_mj + kEPSILON };

  // Define the coordinate frame
  Vector epsilon = Vector( Real(1.0), Real(0.0), Real(0.0) );
  Vector tangent = n.vect(epsilon);
  tangent.normalize();
  Vector bitangent = n.vect(tangent);
  bitangent.normalize();

  // Generate any useful aliases
  Real VdotN = v.dot(n);
  Real LdotN = l.dot(n);
  Real HdotN = h.dot(n);
  Real HdotT = h.dot(tangent);
  Real HdotB = h.dot(bitangent);

  // Evaluate the specular exponent
  Real beta_a  = HdotT / fAnisotropicRoughness[0];
  beta_a       *= beta_a;

  Real beta_b  = HdotB / fAnisotropicRoughness[1];
  beta_b       *= beta_b;

  Real beta = -Real(2.0) * ( ( beta_a + beta_b ) / ( Real(1.0) + HdotN ) );

  // Evaluate the specular denominator
  float s_den  = Real(4.0) * M_PI; 
  s_den       *= fAnisotropicRoughness[0];
  s_den       *= fAnisotropicRoughness[1];
  s_den       *= sqrt( LdotN * VdotN );

  // return Ward's factor
  return exp( beta ) / s_den;

  //// cosinus(ln) or cosinus(vn) very small (NL or NV near to 90°) : return 1
  //if (ln < kEPSILON || vn < kEPSILON) {
  //  return Real(1000.0);
  //}

  //// roughness very small : dirac at NdotH = 1
  //if (Ward_mi < kEPSILON || Ward_mj < kEPSILON) {
  //  if (nh > Real(1.0) - kEPSILON) 
  //    return Real(1000.0);
  //  return kEPSILON;
  //}

  //Real deno = sqrt(ln * vn) * Real(4.0)  * M_PI * Ward_mi * Ward_mj;
  //Real expo = Real(-2.0) * ( ((h[0] * h[0]) / (Ward_mi * Ward_mi))
  //                         + ((h[1] * h[1]) / (Ward_mj * Ward_mj)))
  //                       / (Real(1.0) + nh);

  //Real result = std::exp(expo) / deno;
  //if (result < kEPSILON)
  //  result = Real(kEPSILON);
  //
  ////Real norm = Real(1.0) / (Real(4.0) * M_PI * Ward_m);
  ////result /= norm;

  //return result;
}
////////////////////////////////////////////////////////////////////////////////
inline void WardRoughnessFormula::getDomeRandomRay(
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
inline void WardRoughnessFormula::getWardRandomRay(
    const Basis& localBasis, 
    Vector view, 
    Real Ward_mi, 
    Real Ward_mj, 
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

    //Computing some cosinuses and sinuses
    Real cosLN = localBasis.k.dot(dir);
    Real cosVN = localBasis.k.dot(view);       
    Real cosHN = localBasis.k.dot(microNormal); 

    // compute weight
    weight = WardDistribution(localBasis.k, dir, view, microNormal, 
                              Ward_mi, Ward_mj);

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
  
    //Computing some cosinuses and sinuses
    Real cosLN = localBasis.k.dot(dir);
    Real cosVN = localBasis.k.dot(view);       
    Real cosHN = localBasis.k.dot(microNormal); 

    // compute weight
    weight = WardDistribution(localBasis.k, dir, view, microNormal, 
                              Ward_mi, Ward_mj);
    // add to count
    totalWeight += weight;
    numsamples++;
  } while(rand()/ (Real)RAND_MAX > weight);

  weight = weight * (Real(0.5) / M_PI)/(totalWeight / numsamples);
}
////////////////////////////////////////////////////////////////////////////////
inline void WardRoughnessFormula::reflect(
    const Vector& normal, 
    Vector& dir, 
    Real Ward_mi, 
    Real Ward_mj) {

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

    //Computing some cosinuses and sinuses
    Real cosLN = normal.dot(incident);
    Real cosVN = normal.dot(dir);       
    Real cosHN = normal.dot(microNormal); 

    // compute weight
    weight = WardDistribution(normal, incident, dir, microNormal,
                              Ward_mi, Ward_mj);
  } 
  while(rand()/(float)RAND_MAX > weight);
}
////////////////////////////////////////////////////////////////////////////////
#endif // GUARD_VRT_WARDROUGHNESSFORMULA_HPP
