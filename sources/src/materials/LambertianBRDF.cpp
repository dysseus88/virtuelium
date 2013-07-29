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

#include <materials/LambertianBRDF.hpp>

static const Real oneOverPi = 1.0/M_PI;

/**
 * Constructor
 * @param reflectance : the reflectance spectrum
 * @param transmittance : the transmittance spectrum
 */
LambertianBRDF::LambertianBRDF(const Spectrum& reflectance, const Spectrum& transmittance, bool isOpaque)
: Material(true, false), _isOpaque(isOpaque)
{
  _r = reflectance;
  _t = transmittance;
}

/**
 * Compute the diffusely reemited light data and place the result into reemited.
 * localBasis : the object localBasis at the computation point.
 * surfaceCoordinate : the surface coordinate (texture coordinate) of the computation
 *   point on the object.
 * incident : the incident ray (light ray)
 * view : the view ray (from the camera or bounced)
 * incidentLight : the incident light comming from the incident ray.
 * reemitedLight : the light reemited into the view direction (result will be placed 
 *   here).
 */
void LambertianBRDF::getDiffuseReemited(const Basis& localBasis, const Point2D& surfaceCoordinate, const LightVector& incidentLight, LightVector& reemitedLight)
{
  Real cosOi = -incidentLight.getRay().v.dot(localBasis.k);
  Real cosOv = -reemitedLight.getRay().v.dot(localBasis.k);

  if(cosOv<=0 || (_isOpaque && cosOi<=0))
  {
    reemitedLight.clear();
    return;
  }
  
  //Reflectance
  if(cosOi > 0)
  {
    for(unsigned int i=0; i<reemitedLight.size(); i++)
      reemitedLight[i].setRadiance(incidentLight[i].getRadiance()*oneOverPi*cosOi*_r[incidentLight[i].getIndex()]);
  }
  //Transmittance
  else
  {
    for(unsigned int i=0; i<reemitedLight.size(); i++)
      reemitedLight[i].setRadiance(-incidentLight[i].getRadiance()*oneOverPi*cosOi*_t[incidentLight[i].getIndex()]);
  }
  reemitedLight.changeReemitedPolarisationFramework(localBasis.k);
}

/**
 * Compute the secondary rays for diffuse reflexion and place them into the subrays
 * vector.
 * localBasis : the local base on the object at the computation point.
 * surfaceCoordinate : the surface  coordinate (texture coordinate) of the computation 
 *   point on the object.
 * view : the view ray (from the camera or bounced)
 * nbRays : the number of wanted ray. It is an indicative information.
 * subrays : the vector were the secondaries rays will be put.
 * weights : the weights corresponding to the distribution
 */
void LambertianBRDF::getRandomDiffuseRay(const Basis& localBasis, const Point2D& surfaceCoordinate, LightVector& reemitedLight, unsigned int nbRays, std::vector<LightVector>& subrays)
{
  Vector normal=localBasis.k;
  if(normal.dot(reemitedLight.getRay().v)>0)
    return;

  if(!_isOpaque)
    nbRays = (nbRays+1)/2;

  //Reflexion 
  generateRandomeDiffuseRay(normal, localBasis.o, nbRays, reemitedLight, subrays);

  if(_isOpaque)
    return;

  //Transmission 
  normal.mul(-1.0);
  generateRandomeDiffuseRay(normal, localBasis.o, nbRays, reemitedLight, subrays);
}

void LambertianBRDF::generateRandomeDiffuseRay(const Vector& normal, const Point& origin, unsigned int nbRays, LightVector& reemitedLight, std::vector<LightVector>& subrays)
{
  for(unsigned int i=0; i<nbRays; i++)
  {
    Vector incident;
    Real norm2;
    Real cosOi;
    do{
      incident[0]=rand()*2.0/RAND_MAX - 1.0;
      incident[1]=rand()*2.0/RAND_MAX - 1.0;
      incident[2]=rand()*2.0/RAND_MAX - 1.0;
      norm2=incident.square();
      incident.normalize();
      cosOi=incident.dot(normal);
    }while(norm2>cosOi*cosOi || cosOi<=0.0);

    LightVector subray;
    subray.setRay(origin, incident);
    subray.changeReemitedPolarisationFramework(normal);
    subray.initSpectralData(reemitedLight);
    subray.setWeight(1.0/M_PI);
    subrays.push_back(subray);
  }
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
bool LambertianBRDF::bouncePhoton(const Basis& localBasis, const Point2D& surfaceCoordinate, MultispectralPhoton& photon, bool& specular)
{
  Vector normal=localBasis.k;
  if(normal.dot(photon.direction)>0)
    return false;

  //Compute photon absorption
  Real mean=0;
  Real rmean=0;
  Real tmean=0;
  MultispectralPhoton tphoton=photon;
  for(unsigned int i=0; i<GlobalSpectrum::nbWaveLengths(); i++)
  {
    photon.radiance[i]*=_r[i];
    tphoton.radiance[i]*=_t[i];
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
    generatePhoton(normal, photon, rmean);
  }
  //Transmission
  else
  {
    photon = tphoton;
    normal.mul(-1.0);
    generatePhoton(normal, photon, tmean);
  }

  //Done
  specular=false;
  return true;
}

void LambertianBRDF::generatePhoton(const Vector& normal, MultispectralPhoton& photon, Real mean)
{
  //Normalizing photon energy
  for(unsigned int i=0; i<GlobalSpectrum::nbWaveLengths(); i++)
    photon.radiance[i] /= mean;

  Real norm2;
  Real cosOi;
  do{
    photon.direction[0]=rand()*2.0/RAND_MAX - 1.0;
    photon.direction[1]=rand()*2.0/RAND_MAX - 1.0;
    photon.direction[2]=rand()*2.0/RAND_MAX - 1.0;
    norm2=photon.direction.square();
    photon.direction.normalize();
    cosOi = photon.direction.dot(normal);
  }while(norm2>cosOi*cosOi || cosOi<=0);
}

void LambertianBRDF::getDiffuseReemitedFromAmbiant(const Basis& localBasis, const Point2D& surfaceCoordinate, LightVector& reemitedLight, const Spectrum& incident)
{
  Real cosOi = Real(1.0);//-incidentLight.getRay().v.dot(localBasis.k);
  Real cosOv = -reemitedLight.getRay().v.dot(localBasis.k);

  if(cosOv <= 0 || (_isOpaque && cosOi<=0))
  {
    reemitedLight.clear();
    return;
  }
  
  for(unsigned int wl=0; wl < reemitedLight.size(); wl++)
    reemitedLight[wl].setRadiance(incident[wl] * oneOverPi /** cosOv*/ * _r[wl]);

  reemitedLight.changeReemitedPolarisationFramework(localBasis.k);
}
