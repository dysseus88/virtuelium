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

#include <materials/RegularBRDF.hpp>
#include <physics/DielectricFormula.hpp>

/**
 * Constructor
 * spectrum : the normal incident reflection spectrum.
 */
RegularBRDF::RegularBRDF(std::vector<Spectrum> RPara, std::vector<Spectrum> ROrth)
: Material(false, true)
{
  _RPara = RPara;
  _ROrth = ROrth;
}

/**
 * Compute the specularly reemited light data and place the result into reemited.
 * localBasis : the object localBasis at the computation point.
 * surfaceCoordinate : the surface coordinate (texture coordinate) of the computation
 *   point on the object.
 * incident : the incident ray (light ray)
 * view : the view ray (from the camera or bounced)
 * incidentLight : the incident light comming from the incident ray.
 * reemitedLight : the light reemited into the view direction (result will be placed 
 *   here).
 */
void RegularBRDF::getSpecularReemited(const Basis& localBasis, const Point2D& surfaceCoordinate, const LightVector& incidentLight, LightVector& reemitedLight)
{
  const Vector& incident = incidentLight.getRay().v;
  Vector normal=localBasis.k;

  Real cosOi = - normal.dot(incident);              //Oi : Incident angle with normal

  if(cosOi<=0){
    for(unsigned int i=0; i<incidentLight.size(); i++)
      reemitedLight[i].applyReflectance(incidentLight[i], 1, 1);
    return;
  }

  //Setting the polarisation framework
  LightVector localIncidentLight(incidentLight);
  localIncidentLight.changeIncidentPolarisationFramework(localBasis.k);
  localIncidentLight.flip();
  reemitedLight.changeReemitedPolarisationFramework(localBasis.k);

  for(unsigned int i=0; i<localIncidentLight.size(); i++)
  {
    Real ROrth, RPara;
    getReflectance(cosOi, localIncidentLight[i].getIndex(), ROrth, RPara);
    reemitedLight[i].applyReflectance(localIncidentLight[i], RPara, ROrth);  
  }
}

/**
 * Compute the secondary rays for specular reflexion and place them into the subrays
 * vector.
 * localBasis : the local base on the object at the computation point.
 * surfaceCoordinate : the surface  coordinate (texture coordinate) of the computation 
 *   point on the object.
 * view : the view ray (from the camera or bounced)
 * subrays : the vector were the secondaries rays will be put.
 */
void RegularBRDF::getSpecularSubRays(const Basis& localBasis, const Point2D& surfaceCoordinate, LightVector& reemitedLight, std::vector<LightVector>& subrays)
{
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

/**
 * Compute the bounce of the given photon and modify it. Use the russian 
 * roulette to return if it bounce (true) or if it's absorbed. Tel also if 
 * this bounce is specular or not.
 * localBasis : the local base on the object at the computation point.
 * surfaceCoordinate : the surface  coordinate (texture coordinate) of the 
 *   computation point on the object.
 * photon : the photon to bounce. This parametre will be modified to reflect
 *   the bounce.
 * specular : this function will set it to true if this bounce is specular
 */
bool RegularBRDF::bouncePhoton(const Basis& localBasis, const Point2D& surfaceCoordinate, MultispectralPhoton& photon, bool& specular)
{
  //Computing base angles
  Vector normal = localBasis.k;
  Real cosOi = - normal.dot(photon.direction);

  //Don't bounce from the interrior
  if(cosOi < 0)
    return false;

  //Computing photon absorption at the reflexion
  Real mean=0;
  Real ROrth, RPara;
  for(unsigned int i=0; i<GlobalSpectrum::nbWaveLengths(); i++)
  { 
    getReflectance(cosOi, i, ROrth, RPara);
    photon.radiance[i]   *=(ROrth+RPara)*0.5;
    mean+=photon.radiance[i];
  }

  //Reflexion
  if(rand()/(Real)RAND_MAX > mean)
    return false;

  //Normalizing photon energy
  for(unsigned int i=0; i<GlobalSpectrum::nbWaveLengths(); i++)
    photon.radiance[i] /= mean;

  //Compute the reflected ray
  DielectricFormula::reflect(normal, photon.direction);

  //Done
  specular = true;
  return true;
}

/**
 * Get the reflectance in the reflection direction
 * @param cosOi : the cosinus of the angle between the normal and incident.
 * @param index : the wavelenght index to get
 * @param ROrth : the othogonal polarised computed Reflectance will be placed here
 * @param Rpara : the parallel polarised computed Reflectance will be placed here
 */
void RegularBRDF::getReflectance(const Real& cosOi, const int& index, Real& ROrth, Real& RPara)
{
  Real factor = 2.0*std::acos(cosOi)*_RPara.size()/M_PI;
  if(cosOi>=1.0) factor=0.0;
  unsigned int iOi =(int)factor;
  factor -= iOi;

  Real Rs0, Rp0, Rs1, Rp1;
  if(iOi>=_RPara.size()-1){
    Rs0=_ROrth[_RPara.size()-1][index];
    Rp0=_RPara[_RPara.size()-1][index];
    Rs1=_ROrth[_RPara.size()-1][index];
    Rp1=_RPara[_RPara.size()-1][index];
  }
  else{
    Rs0=_ROrth[iOi][index];
    Rp0=_RPara[iOi][index];
    Rs1=_ROrth[iOi+1][index];
    Rp1=_RPara[iOi+1][index];    
  }

  RPara = Rp0*(1-factor) + Rp1*factor;
  ROrth = Rs0*(1-factor) + Rs1*factor;
}

void RegularBRDF::getDiffuseReemitedFromAmbiant(const Basis& localBasis, const Point2D& surfaceCoordinate, LightVector& reemitedLight, const Spectrum& incident)
{
  Real cosOi = Real(1.0);//-incidentLight.getRay().v.dot(localBasis.k);
  Real cosOv = -reemitedLight.getRay().v.dot(localBasis.k);

  if(cosOv<=0.0 || cosOi<=0)
  {
    reemitedLight.clear();
    return;
  }
  
  for(unsigned int wl=0; wl < reemitedLight.size(); wl++) {
    Real ROrth, RPara;
    getReflectance(cosOi, wl, ROrth, RPara);
    reemitedLight[wl].setRadiance(incident[wl] /** cosOv*/ * 0.5 * (ROrth + RPara));
  }
  reemitedLight.changeReemitedPolarisationFramework(localBasis.k);
}
