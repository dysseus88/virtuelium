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
#include <materials/MetalB.hpp>
//!
//! @file MetalB.cpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file implements the class declared in MetalB.hpp:
//!  @arg MetalB
//! @todo 
//! @remarks 
//!
#include <core/VrtLog.hpp>

#include <physics/BeckmannRoughnessFormula.hpp>
#include <physics/LayeredSystemComputer.hpp>
#include <physics/DielectricFormula.hpp>

#include <physics/BeckmannRoughnessFormula.hpp>
////////////////////////////////////////////////////////////////////////////////
MetalB::MetalB(std::vector<const Medium*>& media_stack, 
                     Real beck_mi, Real beck_mj)
    : Material(true, false) {

  VrtLog::Write("Entering MetalB::MetalB...");

  m_mi = beck_mi;
  m_mj = beck_mj;

  std::vector<Real> thickness(2);
	thickness[0] = 0.0;
	thickness[1] = 0.0;
  //Compute specular reflectance and transmittance
  std::vector<Spectrum> R12p(kSAMPLES), R12s(kSAMPLES);
  std::vector<Spectrum> T12p(kSAMPLES), T12s(kSAMPLES);
  for(unsigned int iOi = 0; iOi < kSAMPLES; iOi++) {
    Real Oi = iOi * M_PI * 0.5 / (Real)kSAMPLES;
    Real cosOi = std::cos(Oi);
    LayeredSystemComputer::computeSpecularLayerSystem(
                            R12s[iOi], R12p[iOi], T12s[iOi], T12p[iOi], 
                            cosOi, media_stack, thickness);
   
    for(unsigned int i = 0; i < GlobalSpectrum::nbWaveLengths(); i++) {
      if(!(R12p[iOi][i] < Real(1.0) + kEPSILON)) {
        VrtLog::Write("(MetalB::MetalB) La réflectance 1-2 P de la \
bande %d vaut (%f) pour l'angle d'incidence %f°", 
                      i, R12p[iOi][i], (Oi * 180.0 / M_PI));
      }
      if(!(R12s[iOi][i] < Real(1.0) + kEPSILON)) {
        VrtLog::Write("(MetalB::MetalB) La réflectance 1-2 S de la \
bande %d vaut (%f) pour l'angle d'incidence %f°", 
                      i, R12p[iOi][i], (Oi * 180.0 / M_PI));
      }
      if(!(T12p[iOi][i] < Real(1.0) + kEPSILON)) {
        VrtLog::Write("(MetalB::MetalB) La transmittance 1-2 P de la \
bande %d vaut (%f) pour l'angle d'incidence %f°", 
                      i, R12p[iOi][i], (Oi * 180.0 / M_PI));
      }
      if(!(T12s[iOi][i] < Real(1.0) + kEPSILON)) {
        VrtLog::Write("(MetalB::MetalB) La transmittance 1-2 S de la \
bande %d vaut (%f) pour l'angle d'incidence %f°", 
                      i, R12p[iOi][i], (Oi * 180.0 / M_PI));
      }           
    }	
  }
  p_RPara = R12p;
  p_ROrth = R12s;
}
////////////////////////////////////////////////////////////////////////////////
MetalB::~MetalB(void) {
  //Nothing to do
}
////////////////////////////////////////////////////////////////////////////////
void MetalB::getSpecularReemited(const Basis& localBasis, 
                                    const Point2D& surfaceCoordinate, 
                                    const LightVector& incidentLight, 
                                    LightVector& reemitedLight) {
  //VrtLog::Write("Entering MetalB::getSpecularReemited...");

  //const Vector& incident = incidentLight.getRay().v;
  //Vector normal=localBasis.k;

  ////Oi : Incident angle with normal
  //Real cosOi = - normal.dot(incident);

  //if(cosOi <= 0) {
  //  for(unsigned int i=0; i<incidentLight.size(); i++)
  //    reemitedLight[i].applyReflectance(incidentLight[i], 1, 1);
  //  return;
  //}

  ////Setting the polarisation framework
  //LightVector localIncidentLight(incidentLight);
  //localIncidentLight.changeIncidentPolarisationFramework(localBasis.k);
  //localIncidentLight.flip();
  //reemitedLight.changeReemitedPolarisationFramework(localBasis.k);

  //for(unsigned int i=0; i<localIncidentLight.size(); i++) {
  //  Real ROrth, RPara;
  //  getReflectance(cosOi, localIncidentLight[i].getIndex(), ROrth, RPara);
  //  reemitedLight[i].applyReflectance(localIncidentLight[i], RPara, ROrth);  
  //}
}
////////////////////////////////////////////////////////////////////////////////
void MetalB::getSpecularSubRays(const Basis& localBasis, 
                                   const Point2D& surfaceCoordinate, 
                                   LightVector& reemitedLight, 
                                   std::vector<LightVector>& subrays) {
  //VrtLog::Write("Entering MetalB::getSpecularSubRays...");
  const Vector& view = reemitedLight.getRay().v;
  Vector normal = localBasis.k;
  LightVector subray;

  //Reflexion
  Vector reflected = view;
  DielectricFormula::reflect(normal, reflected);
  subray.initSpectralData(reemitedLight);
  subray.setRay(localBasis.o, reflected);
  subray.changeReemitedPolarisationFramework(normal);
  subrays.push_back(subray);
}
////////////////////////////////////////////////////////////////////////////////
void MetalB::getDiffuseReemited(const Basis& localBasis,
	                                 const Point2D& surfaceCoordinate, 
                                   const LightVector& incidentLight,
                                   LightVector& reemitedLight) {
  //VrtLog::Write("Entering MetalB::getDiffuseReemited...");

  Vector incident = incidentLight.getRay().v;
  Vector view = reemitedLight.getRay().v;

  //Computing the micro normal
  Vector microNormal;
  microNormal.setsum(incident, view);
  microNormal.mul(Real(-1.0));
  microNormal.normalize();

  //Computing some cosinuses and sinuses
  Real cosLN = -localBasis.k.dot(incident);   // incident and normal
  Real cosVN = -localBasis.k.dot(view);       // view and normal 
  Real cosHN = localBasis.k.dot(microNormal); // micro normal and normal
  Real cosLH = -microNormal.dot(incident);    // incident and micro normal
  Real cosVH = -microNormal.dot(view);    // incident and micro normal

  //Compute Beckmann and Shadowing&masking coeficients
  Real D = BeckmannRoughnessFormula::BeckmannDistribution(cosHN, m_mi);
  Real G = BeckmannRoughnessFormula::BeckmannShadowMasking(cosLN, cosVN, 
                                                           cosVH, cosHN);
    
  //VrtLog::Write("m = (%f, %f), beckmann = %f", m_mi, m_mj, D);
  
  //Setting the polarisation framework
  LightVector localIncidentLight(incidentLight);
  localIncidentLight.changeIncidentPolarisationFramework(microNormal);
  localIncidentLight.flip();
  reemitedLight.changeReemitedPolarisationFramework(microNormal);

  if(cosVN <= kEPSILON || cosLN <= kEPSILON) {
    reemitedLight.clear();
    return;
  }

  //Computing reflectances for each wavelength
  for(unsigned int i=0; i<localIncidentLight.size(); i++) {
    Real ROrth, RPara;
    getReflectance(cosLH, localIncidentLight[i].getIndex(), ROrth, RPara);
    ROrth *= D * G;
    RPara *= D * G;

    reemitedLight[i].applyReflectance(localIncidentLight[i], RPara, ROrth);
  }
}
////////////////////////////////////////////////////////////////////////////////
void MetalB::getRandomDiffuseRay(const Basis& localBasis, 
	                                  const Point2D& surfaceCoordinate, 
                                    LightVector& reemitedLight, 
                                    unsigned int nbRays, 
                                    std::vector<LightVector>& subrays) {
  //VrtLog::Write("Entering MetalB::getRandomDiffuseRay...");
  Vector view = reemitedLight.getRay().v;
  view.mul(-1.0);

  if(view.dot(localBasis.k) < 0)
    return;

  for(unsigned int i=0; i<nbRays; i++)  {
    Vector dir;
    Real weight;
    BeckmannRoughnessFormula::getBeckmannRandomRay(localBasis, view, 
                                                   m_mi, weight, dir);
    //VrtLog::Write("weight = %f", weight);

    LightVector subray;
    subray.setRay(localBasis.o, dir);
    subray.initSpectralData(reemitedLight);
    subray.changeReemitedPolarisationFramework(localBasis.k);
    subray.setWeight(weight);

    subrays.push_back(subray);
  }
}
////////////////////////////////////////////////////////////////////////////////
bool MetalB::bouncePhoton(const Basis& localBasis,
	                           const Point2D& surfaceCoordinate, 
                             MultispectralPhoton& photon, bool& specular) {
  //Computing base angles
  Vector normal = localBasis.k;
  Real cosOi = - normal.dot(photon.direction);

  //Don't bounce from the interrior
  if(cosOi < 0)
    return false;

  //Computing photon absorption at the reflexion
  Real mean=0;
  Real ROrth, RPara;
  for(unsigned int i=0; i<GlobalSpectrum::nbWaveLengths(); i++) { 
    getReflectance(cosOi, i, ROrth, RPara);
    photon.radiance[i] *= (ROrth + RPara) * Real(0.5);
    mean += photon.radiance[i];
  }

  //Reflexion
  if(rand() / (Real)RAND_MAX > mean)
    return false;

  //Normalizing photon energy
  for(unsigned int i=0; i<GlobalSpectrum::nbWaveLengths(); i++)
    photon.radiance[i] /= mean;

  //Compute the reflected direction
  BeckmannRoughnessFormula::reflect(normal, photon.direction, m_mi);

  //Done
  specular = false;
  return true;
}
////////////////////////////////////////////////////////////////////////////////
void MetalB::getReflectance(const Real& cosOi, const int& index, 
	                             Real& ROrth, Real& RPara) {
  
  Real factor = Real(2.0) * std::acos(cosOi) * p_RPara.size() / M_PI;
  if(cosOi > Real(1.0)) 
    factor = 0;
  unsigned int iOi =(int)factor;
  factor -= iOi;

  Real Rs0, Rp0, Rs1, Rp1;
  if(iOi >= p_RPara.size()-1) {
    Rs0 = p_ROrth[p_RPara.size() - 1][index];
    Rp0 = p_RPara[p_RPara.size() - 1][index];
    Rs1 = p_ROrth[p_RPara.size() - 1][index];
    Rp1 = p_RPara[p_RPara.size() - 1][index];
  } else {
    Rs0 = p_ROrth[iOi][index];
    Rp0 = p_RPara[iOi][index];
    Rs1 = p_ROrth[iOi+1][index];
    Rp1 = p_RPara[iOi+1][index];    
  }

  RPara = Rp0 * (Real(1.0) - factor) + Rp1 * factor;
  ROrth = Rs0 * (Real(1.0) - factor) + Rs1 * factor;
}
////////////////////////////////////////////////////////////////////////////////
void MetalB::getDiffuseReemitedFromAmbiant(const Basis& localBasis,
	                                            const Point2D& surfaceCoordinate, 
                                              LightVector& reemitedLight, 
                                              const Spectrum& incident) {
  const Vector& light = localBasis.k;
  const Vector& view = reemitedLight.getRay().v;

  //Computing the micro normal
  Vector microNormal;
  microNormal.setsum(light, view);
  microNormal.mul(Real(-1.0));
  microNormal.normalize();

  //Computing some cosinuses and sinuses
  Real cosLN = -localBasis.k.dot(light);   // light and normal
  Real cosVN = -localBasis.k.dot(view);       // view and normal 
  Real cosHN = localBasis.k.dot(microNormal); // micro normal and normal
  Real cosLH = -microNormal.dot(light);    // light and micro normal
  Real cosVH = -microNormal.dot(view);    // incident and micro normal

  //Compute Beckmann and Shadowing&masking coeficients
  Real D = BeckmannRoughnessFormula::BeckmannDistribution(cosHN, m_mi);
  Real G = BeckmannRoughnessFormula::BeckmannShadowMasking(cosLN, cosVN, 
                                                           cosVH, cosHN);

  if(cosVN <= kEPSILON)  {
    reemitedLight.clear();
    return;
  }
  
  for(unsigned int wl = 0; wl < reemitedLight.size(); wl++) {
    Real ROrth, RPara;
    getReflectance(cosLH, wl, ROrth, RPara);
    ROrth *= D * G;
    RPara *= D * G;
    reemitedLight[wl].setRadiance(incident[wl] /** cosVN*/ 
                                  * Real(0.5) * (ROrth + RPara));
  }
 reemitedLight.changeReemitedPolarisationFramework(microNormal);
}
////////////////////////////////////////////////////////////////////////////////
