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
#include <materials/AlloyBRDF.hpp>
//!
//! @file AlloyBRDF.cpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file implements the class declared in AlloyBRDF.hpp:
//!  @arg AlloyBRDF
//! @todo 
//! @remarks 
//!
#include <core/VrtLog.hpp>


#include <materials/BeckmannBRDF.hpp>

#include <physics/BeckmannRoughnessFormula.hpp>
#include <physics/LayeredSystemComputer.hpp>
#include <physics/DielectricFormula.hpp>
////////////////////////////////////////////////////////////////////////////////
AlloyBRDF::AlloyBRDF(std::vector<const Medium*>& media_stack, 
                     Real CoeffCon, 
                     Real roughness)
    : Material(true, false) {

  VrtLog::Write("Entering AlloyBRDF::AlloyBRDF(c1=%f, c2=%f)...", CoeffCon, 1.0 - CoeffCon);

	Spectrum n1 = media_stack[1]->n;
	Spectrum k1 = media_stack[1]->k;
	Spectrum n2 = media_stack[2]->n;
	Spectrum k2 = media_stack[2]->k;
	Spectrum nEff,kEff;

  VrtLog::WriteArray("n1: ", n1.values(), GlobalSpectrum::nbWaveLengths());
  VrtLog::WriteArray("k1: ", k1.values(), GlobalSpectrum::nbWaveLengths());
  VrtLog::WriteArray("n2: ", n2.values(), GlobalSpectrum::nbWaveLengths());
  VrtLog::WriteArray("k2: ", k2.values(), GlobalSpectrum::nbWaveLengths());

	//E1=(n1+i*k1)^2=n1^2-k1^2+2*n1*k1*i
	//E2=(n2+i*k2)^2=n2^2-k2^2+2*n2*k2*i
	//E3=(n3+i*k3)^2=n3^2-k3^2+2*n3*k3*i
	//E3=c1*E1+c2*E2=b+2*a*i where(a>0,b>0)
	//b=c1*n1^2+c2*n2^2-c1*k1^2-c2*k2^2
	//a=n1*k1*c1+n2*k2*c2
  Real coefCon_1 = Real(1.0) - CoeffCon;
	for(unsigned int i = 0; i < GlobalSpectrum::nbWaveLengths(); i++) {
		Real a = n1[i] * k1[i] * CoeffCon + n2[i] * k2[i] * coefCon_1;
		Real b = n1[i] * n1[i] * CoeffCon + coefCon_1 * n2[i] * n2[i]
           - CoeffCon * k1[i] * k1[i] - coefCon_1 * k2[i] * k2[i];
		kEff[i] = sqrt((sqrt(b * b + Real(4.0) * a * a) - b) / Real(2.0));
		if (kEff[i] != 0)
      nEff[i] = a / kEff[i];
	}

  VrtLog::WriteArray("nEff: ", nEff.values(), GlobalSpectrum::nbWaveLengths());
  VrtLog::WriteArray("kEff: ", kEff.values(), GlobalSpectrum::nbWaveLengths());

  Medium MediumEff;
	MediumEff.hasDispersion = media_stack[1]->hasDispersion 
                          & media_stack[2]->hasDispersion;
	MediumEff.hasPhotonDispersion = media_stack[1]->hasPhotonDispersion 
                                & media_stack[1]->hasPhotonDispersion;
	MediumEff.isOpaque = media_stack[1]->isOpaque 
                     & media_stack[2]->isOpaque;

	MediumEff.useFresnelModel = true;
	MediumEff.useKubelkaMunkModel = false;
	MediumEff.useLambertianModel = false;

	MediumEff.k = kEff;
	MediumEff.n = nEff;

	std::vector<const Medium*> layers(2);
	layers[0] = media_stack[0];
	layers[1] = &MediumEff;
 
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
                            cosOi, layers, thickness);
   
    for(unsigned int i = 0; i < GlobalSpectrum::nbWaveLengths(); i++) {
      if(!(R12p[iOi][i] < Real(1.0) + kEPSILON)) {
        VrtLog::Write("(AlloyBRDF::AlloyBRDF) La réflectance 1-2 P de la \
bande %d vaut (%f) pour l'angle d'incidence %f°", 
                      i, R12p[iOi][i], (Oi * 180.0 / M_PI));
      }
      if(!(R12s[iOi][i] < Real(1.0) + kEPSILON)) {
        VrtLog::Write("(AlloyBRDF::AlloyBRDF) La réflectance 1-2 S de la \
bande %d vaut (%f) pour l'angle d'incidence %f°", 
                      i, R12p[iOi][i], (Oi * 180.0 / M_PI));
      }
      if(!(T12p[iOi][i] < Real(1.0) + kEPSILON)) {
        VrtLog::Write("(AlloyBRDF::AlloyBRDF) La transmittance 1-2 P de la \
bande %d vaut (%f) pour l'angle d'incidence %f°", 
                      i, R12p[iOi][i], (Oi * 180.0 / M_PI));
      }
      if(!(T12s[iOi][i] < Real(1.0) + kEPSILON)) {
        VrtLog::Write("(AlloyBRDF::AlloyBRDF) La transmittance 1-2 S de la \
bande %d vaut (%f) pour l'angle d'incidence %f°", 
                      i, R12p[iOi][i], (Oi * 180.0 / M_PI));
      }           
    }	
  }
  p_RPara = R12p;
  p_ROrth = R12s;
  m_roughness  = roughness;
}
////////////////////////////////////////////////////////////////////////////////
AlloyBRDF::~AlloyBRDF(void) {
  //Nothing to do
}
////////////////////////////////////////////////////////////////////////////////
void AlloyBRDF::getDiffuseReemited(const Basis& localBasis,
	                                 const Point2D& surfaceCoordinate, 
                                   const LightVector& incidentLight,
                                   LightVector& reemitedLight) {
  const Vector& incident = incidentLight.getRay().v;
  const Vector& view = reemitedLight.getRay().v;

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
  Real cosVH = -microNormal.dot(view);        // view and micro normal

  //Compute Beckmann and Shadowing&masking coeficients
  Real beckmann = BeckmannRoughnessFormula::BeckmannDistribution(cosHN, 
                                                                 m_roughness);
  Real shadmask = BeckmannRoughnessFormula::BeckmannShadowMasking(cosLN, 
                                                                  cosVN, 
                                                                  cosVH, 
                                                                  cosHN);
  
  //Setting the polarisation framework
  LightVector localIncidentLight(incidentLight);
  localIncidentLight.changeIncidentPolarisationFramework(microNormal);
  localIncidentLight.flip();
  reemitedLight.changeReemitedPolarisationFramework(microNormal);

  if(cosVN <= kEPSILON || cosLN <= kEPSILON) {
    reemitedLight.clear();
    return;
  }

  Real max = 0;
  //Computing reflectances for each wavelength
  for(unsigned int i=0; i<localIncidentLight.size(); i++) {
    Real ROrth, RPara;
    getReflectance(cosLH, localIncidentLight[i].getIndex(), ROrth, RPara);
    reemitedLight[i].applyReflectance(localIncidentLight[i], RPara*beckmann*shadmask, ROrth*beckmann*shadmask);
    if (max < reemitedLight[i].getRadiance())
      max = reemitedLight[i].getRadiance();
  }
  if (max > Real(1.0)) {
    printf("max=%f\n", max);
    for(unsigned int i=0; i<localIncidentLight.size(); i++) {
      reemitedLight[i].setRadiance(reemitedLight[i].getRadiance() / max);
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
void AlloyBRDF::getRandomDiffuseRay(const Basis& localBasis, 
	                                  const Point2D& surfaceCoordinate, 
                                    LightVector& reemitedLight, 
                                    unsigned int nbRays, 
                                    std::vector<LightVector>& subrays) {
  Vector view = reemitedLight.getRay().v;
  view.mul(-1.0);

  if(view.dot(localBasis.k) < 0)
    return;

  for(unsigned int i=0; i<nbRays; i++)  {
    Vector dir;
    Real weight;
    BeckmannRoughnessFormula::getBeckmannRandomRay(localBasis, view, 
                                                   m_roughness, weight, dir);

    LightVector subray;
    subray.setRay(localBasis.o, dir);
    subray.initSpectralData(reemitedLight);
    subray.changeReemitedPolarisationFramework(localBasis.k);
    subray.setWeight(weight);
    subrays.push_back(subray);
  }
}
////////////////////////////////////////////////////////////////////////////////
bool AlloyBRDF::bouncePhoton(const Basis& localBasis,
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
  for(unsigned int i=0; i<GlobalSpectrum::nbWaveLengths(); i++){ 
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
  BeckmannRoughnessFormula::reflect(normal, photon.direction, m_roughness);

  //Done
  specular = false;
  return true;
}
////////////////////////////////////////////////////////////////////////////////
void AlloyBRDF::getReflectance(const Real& cosOi, const int& index, 
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
void AlloyBRDF::getDiffuseReemitedFromAmbiant(const Basis& localBasis,
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
  Real cosVH = -microNormal.dot(view);        // view and micro normal

  //Compute Beckmann and Shadowing&masking coeficients
  Real beckmann = BeckmannRoughnessFormula::BeckmannDistribution(cosHN, 
                                                                 m_roughness);
  Real shadmask = BeckmannRoughnessFormula::BeckmannShadowMasking(cosLN, 
                                                                  cosVN, 
                                                                  cosVH, 
                                                                  cosHN);

  if(cosVN <= kEPSILON)  {
    reemitedLight.clear();
    return;
  }
  
  for(unsigned int wl = 0; wl < reemitedLight.size(); wl++) {
    Real ROrth, RPara;
    getReflectance(cosLH, wl, ROrth, RPara);
    ROrth *= beckmann * shadmask;
    RPara *= beckmann * shadmask;
    reemitedLight[wl].setRadiance(incident[wl] /** cosVN*/ 
                                  * Real(0.5) * (ROrth + RPara));
  }
 reemitedLight.changeReemitedPolarisationFramework(microNormal);
}
////////////////////////////////////////////////////////////////////////////////
