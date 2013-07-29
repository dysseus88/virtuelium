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

#include<materials/RefractiveBRDF.hpp>
#include <physics/DielectricFormula.hpp>

/**
 * Constructor
 * @param RPara : the P-Polarised Reflectance
 * @param ROrth : the S-Polarised Reflectance
 * @param TPara : the P-Polarised transmittance
 * @param TOrth : the S-Polarised transmittance
 * @param indices : the indices of refraction
 */
RefractiveBRDF::RefractiveBRDF(std::vector<Spectrum> RPara, std::vector<Spectrum> ROrth, std::vector<Spectrum> TPara, std::vector<Spectrum> TOrth, Spectrum indices, bool dispersion, bool photondispersion)
: Material(false, true)
{
  _RPara=RPara;
  _ROrth=ROrth;
  _TPara=TPara;
  _TOrth=TOrth;
  _indices = indices;
  _dispersion=dispersion;
  _photondispersion=photondispersion;

  //Compute the greatest index
  _maxIndex = _indices[0];
  for(unsigned int l=0; l<GlobalSpectrum::nbWaveLengths(); l++)
    if(std::abs(_maxIndex)<std::abs(_indices[l]))
      _maxIndex=_indices[l];
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
void RefractiveBRDF::getSpecularReemited(const Basis& localBasis, const Point2D& surfaceCoordinate, const LightVector& incidentLight, LightVector& reemitedLight)
{
  const Vector& incident = incidentLight.getRay().v;
  const Vector& view = reemitedLight.getRay().v;
  Vector normal=localBasis.k;

  Real cosOi = - normal.dot(incident);
  Real cosOv = - normal.dot(view);

  if(cosOv<=0 || cosOi==0){
    reemitedLight.clear();
    return;
  }

  //Setting the polarisation framework
  LightVector localIncidentLight(incidentLight);
  localIncidentLight.changeIncidentPolarisationFramework(localBasis.k);
  localIncidentLight.flip();
  reemitedLight.changeReemitedPolarisationFramework(localBasis.k);

  //Reflexion
  if(cosOi > 0)
  {
    for(unsigned int i=0; i<localIncidentLight.size(); i++)
    {
      Real ROrth, RPara;
      getReflectance(cosOv, localIncidentLight[i].getIndex(), ROrth, RPara);
      reemitedLight[i].applyReflectance(localIncidentLight[i], RPara, ROrth);  
    }
  }
  //Transmission
  else
  {
    for(unsigned int i=0; i<localIncidentLight.size(); i++)
    {
      Real TOrth, TPara;
      getTransmittance(cosOv, localIncidentLight[i].getIndex(), TOrth, TPara);
      reemitedLight[i].applyReflectance(localIncidentLight[i], TPara, TOrth);  
    }
  }
}

/**
 * Compute the secondary rays for specular reflexion and place them into the subrays
 * vector.
 * localBasis : the local Basis on the object at the computation point.
 * surfaceCoordinate : the surface  coordinate (texture coordinate) of the computation 
 *   point on the object.
 * view : the view ray (from the camera or bounced)
 * subrays : the vector were the secondaries rays will be put.
 */
void RefractiveBRDF::getSpecularSubRays(const Basis& localBasis, const Point2D& surfaceCoordinate, LightVector& reemitedLight, std::vector<LightVector>& subrays)
{
  const Vector& view = reemitedLight.getRay().v;
  Vector normal = localBasis.k;
  Real cosOv = - normal.dot(view);
  if(cosOv<=0)
    return;

  LightVector subray;

  //Reflexion
  Vector reflected = view;
  DielectricFormula::reflect(normal, reflected);
  subray.initSpectralData(reemitedLight);
  subray.setRay(localBasis.o, reflected);
  subray.changeReemitedPolarisationFramework(normal);
  subrays.push_back(subray);

  //Refraction
  //If there is spectral dispersion
  if(_dispersion)
  {
    //Refraction rays
    for(unsigned int i=0; i<reemitedLight.size(); i++)
    {
      Vector refracted = view;
      DielectricFormula::refract(normal, _indices[reemitedLight[i].getIndex()], refracted);
      subray.setRay(localBasis.o, refracted);      
      subray.initSpectralData(reemitedLight[i].getIndex()); 
      subray.changeReemitedPolarisationFramework(normal);
      subrays.push_back(subray);
    }
  }
  //If there is no spectral dispersion
  else
  {
    Vector refracted = view;
    DielectricFormula::refract(normal, _maxIndex, refracted);
    subray.setRay(localBasis.o, refracted);
    subray.initSpectralData(reemitedLight);
    subray.changeReemitedPolarisationFramework(normal);
    subrays.push_back(subray);
  }
}

/**
 * This method compute a random reflexion for the given photon based on an
 * russian roulette for handeling the absorption of the photon.
 *
 * The default implementation absorb every received photon.
 *
 * This method tel also if this bounce is specular or not.
 * 
 * @param localBasis : the local Basis on the object at the computation point.
 * @param surfaceCoordinate : the surface  coordinate (texture coordinate) of the 
 *   computation point on the object.
 * @param photon : the photon to bounce. This parametre will be modified to reflect
 *   the bounce.
 * @param specular : this function will set it to true if this bounce is specular
 *   false otherwise
 *
 * @return true if the photon had been reemited or false if it was absorbed.
 */
bool RefractiveBRDF::bouncePhoton(const Basis& localBasis, const Point2D& surfaceCoordinate, MultispectralPhoton& photon, bool& specular)
{
  //Computing base angles
  Vector normal = localBasis.k;
  Real cosOi = - normal.dot(photon.direction);

  if(cosOi < 0)
    return false;


  //Compute photon absorption
  Real mean=0;
  Real rmean=0;
  Real tmean=0;
  MultispectralPhoton tphoton=photon;
  for(unsigned int i=0; i<GlobalSpectrum::nbWaveLengths(); i++)
  {
    Real ROrth, RPara;
    Real TOrth, TPara;
    getReflectance(cosOi, i, ROrth, RPara);
    getTransmittance(cosOi, i, TOrth, TPara);
    photon.radiance[i]*=(ROrth+RPara)*0.5;
    tphoton.radiance[i]*=(TOrth+TPara)*0.5;
    mean+=photon.radiance[i]+tphoton.radiance[i];
    rmean+=photon.radiance[i];
    tmean+=tphoton.radiance[i];
  }

  //Russian roulette (Absorption)
  if(rand()/(Real)RAND_MAX > mean)
    return false;

  //Reflexion
  if(rand()/(Real)RAND_MAX < rmean/mean)
  {
    //Normalizing photon energy
    for(unsigned int i=0; i<GlobalSpectrum::nbWaveLengths(); i++)
      photon.radiance[i] /= rmean;

    //Compute the reflected ray
    DielectricFormula::reflect(normal, photon.direction);

    //Done
    specular = true;
    return true;
  }
  //Refraction
  else
  {
    unsigned int index = 0;

    //If there is spectral dispersion
    if(_photondispersion)
    {
      //Counting non nul values
      unsigned int nb_of_nnv = 0;
      for(unsigned int i=0; i<GlobalSpectrum::nbWaveLengths(); i++)
        if(tphoton.radiance[i] > kEPSILON)
        {
          index=i;
          nb_of_nnv++;
        }
     
      if(nb_of_nnv==0)
        return false;

      //Selecting a random wavelength
      if(nb_of_nnv>1)
        do{
          index = (int)(GlobalSpectrum::nbWaveLengths()*(rand()/((Real)RAND_MAX+1)));
        }while(rand()/(Real)RAND_MAX <= tphoton.radiance[index]);
      
      //Constructing the photon spectrum
      for(unsigned int i=0; i<GlobalSpectrum::nbWaveLengths(); i++)
        photon.radiance[i] = 0;
      photon.radiance[index] = 1.0;
    }
    //If there is no spectral dispersion
    else
    {
      //Normalizing photon energy
      for(unsigned int i=0; i<GlobalSpectrum::nbWaveLengths(); i++)
        photon.radiance[i] = tphoton.radiance[i]/tmean;
    }

    //Building the refracted ray
    DielectricFormula::refract(normal, _indices[index], photon.direction);

    //Done
    specular = true;
    return true;
  }

  return false;
}

/**
 * Get the reflectance in the reflection direction
 * @param cosOi : the cosinus of the angle between the normal and incident.
 * @param index : the wavelenght index to get
 * @param ROrth : the othogonal polarised computed Reflectance will be placed here
 * @param Rpara : the parallel polarised computed Reflectance will be placed here
 */
void RefractiveBRDF::getReflectance(const Real& cosOi, const int& index, Real& ROrth, Real& RPara)
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

/**
 * Get the transmittance in the reflection direction
 * @param cosOi : the cosinus of the angle between the normal and incident.
 * @param index : the wavelenght index to get
 * @param TOrth : the othogonal polarised computed Transmittance will be placed here
 * @param TPara : the parallel polarised computed Transmittance will be placed here
 */
void RefractiveBRDF::getTransmittance(const Real& cosOi, const int& index, Real& TOrth, Real& TPara)
{
  Real factor = 2.0*std::acos(cosOi)*_TPara.size()/M_PI;
  if(cosOi>=1.0) factor=0.0;
  unsigned int iOi =(int)factor;
  factor -= iOi;

  Real Ts0, Tp0, Ts1, Tp1;
  if(iOi>=_TPara.size()-1){
    Ts0=_TOrth[_TPara.size()-1][index];
    Tp0=_TPara[_TPara.size()-1][index];
    Ts1=_TOrth[_TPara.size()-1][index];
    Tp1=_TPara[_TPara.size()-1][index];
  }
  else{
    Ts0=_TOrth[iOi][index];
    Tp0=_TPara[iOi][index];
    Ts1=_TOrth[iOi+1][index];
    Tp1=_TPara[iOi+1][index];    
  }

  TPara = Tp0*(1-factor) + Tp1*factor;
  TOrth = Ts0*(1-factor) + Ts1*factor;
}

void RefractiveBRDF::getDiffuseReemitedFromAmbiant(const Basis& localBasis, const Point2D& surfaceCoordinate, LightVector& reemitedLight, const Spectrum& incident)
{
  //Real cosOi = Real(1.0);//-incidentLight.getRay().v.dot(localBasis.k);
  //Real cosOv = -reemitedLight.getRay().v.dot(localBasis.k);

  //if(cosOv<=0.0) {
  //  reemitedLight.clear();
  //  return;
  //}
  //
  //for(unsigned int wl=0; wl < reemitedLight.size(); wl++) {
  //  Real ROrth, RPara;
  //  getReflectance(cosOv, wl, ROrth, RPara);
  //  reemitedLight[wl].setRadiance(incident[wl] /** cosOv*/ * 0.5 * (RPara + ROrth) );
  //}
  reemitedLight.changeReemitedPolarisationFramework(localBasis.k);
}
