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

#include <materials/RoughLambertianBRDF.hpp>
#include <physics/OrenNayarFormula.hpp>

static const Real oneOverPi = 1.0/M_PI;

/**
 * Constructor
 * spectrum : the normal incident reflection spectrum.
 * ambiant : the spectrum of the ambiant light.
 * power : the power of ambiant light (if negative or null no ambiant).
 */
RoughLambertianBRDF::RoughLambertianBRDF(const Spectrum& spectrum, const Real& roughness)
: Material(true, false), _roughness(roughness)
{
  _spectrum = spectrum;
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
void RoughLambertianBRDF::getDiffuseReemited(const Basis& localBasis, const Point2D& surfaceCoordinate, const LightVector& incidentLight, LightVector& reemitedLight)
{
  Vector view     = reemitedLight.getRay().v;
  Vector incident = incidentLight.getRay().v;

  Real cosOi = -incident.dot(localBasis.k);
  Real cosOv = -view.dot(localBasis.k);

  if(cosOi <= 0)
  {
    reemitedLight.clear();
    return;
  }

  if(cosOi>=1.0)
    cosOi=1.0;
  if(cosOv>=1.0)
    cosOv=1.0;

  Real Oi = std::acos(cosOi);
  Real Ov = std::acos(cosOv);
  Real Pi = std::atan2(incident.dot(localBasis.j), incident.dot(localBasis.i));
  Real Pv = std::atan2(view.dot(localBasis.j), view.dot(localBasis.i));

  Real factor = OrenNayarFormula::orenNayarReflectance(Oi, Pi, Ov, Pv, _roughness);

  for(unsigned int i=0; i<reemitedLight.size(); i++)
    reemitedLight[i].setRadiance(incidentLight[i].getRadiance()*_spectrum[incidentLight[i].getIndex()]*factor);

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
void RoughLambertianBRDF::getRandomDiffuseRay(const Basis& localBasis, const Point2D& surfaceCoordinate, LightVector& reemitedLight, unsigned int nbRays, std::vector<LightVector>& subrays)
{
  Vector normal=localBasis.k;
  if(normal.dot(reemitedLight.getRay().v)>0)
    normal.mul(-1);
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
    subray.setRay(localBasis.o, incident);
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
bool RoughLambertianBRDF::bouncePhoton(const Basis& localBasis, const Point2D& surfaceCoordinate, MultispectralPhoton& photon, bool& specular)
{
  Vector normal = localBasis.k;
  bool inside = normal.dot(photon.direction)>0;

  //Compute photon absorption
  Real mean=0;
  for(unsigned int i=0; i<GlobalSpectrum::nbWaveLengths(); i++)
  {
    photon.radiance[i]*=_spectrum[i];
    mean+=photon.radiance[i];
  }

  //Russian roulette
  if(rand()/(Real)RAND_MAX > mean)
    return false;

  //Normalizing photon energy
  for(unsigned int i=0; i<GlobalSpectrum::nbWaveLengths(); i++)
    photon.radiance[i] /= mean;

  if(inside)
    normal.mul(-1.0);

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

  //Done
  specular=false;
  return true;
}

void RoughLambertianBRDF::getDiffuseReemitedFromAmbiant(const Basis& localBasis, const Point2D& surfaceCoordinate, LightVector& reemitedLight, const Spectrum& incident)
{
  reemitedLight.clear();
  reemitedLight.changeReemitedPolarisationFramework(localBasis.k);
  //// the same in all directions
  //for(unsigned int i=0; i<reemitedLight.size(); i++) {
  //  reemitedLight[i].setRadiance(incident[i] * _spectrum[i] );
  //}

  //reemitedLight.changeReemitedPolarisationFramework(localBasis.k);
}