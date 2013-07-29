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

#include <materials/TwoSidedBRDF.hpp>

/**
 * Constructor
 */
TwoSidedBRDF::TwoSidedBRDF(Material* externalBRDF, Material* internalBRDF)
: Material(externalBRDF->isDiffuse() || internalBRDF->isDiffuse(), externalBRDF->isSpecular() || internalBRDF->isSpecular()), _external(externalBRDF), _internal(internalBRDF)
{
  //Nothing to do more
}

/**
 * Compute the diffuse part of a reflexion. Use the given incident light and
 * place the computed result into the reemitedLight parameter.
 * @param localBasis : the object localBasis at the computation point.
 * @param surfaceCoordinate : the surface coordinate (texture coordinate) of the computation
 *   point on the object.
 * @param incidentLight : the incident light comming from the incident ray.
 * @param reemitedLight : the light reemited into the view direction (result will be placed 
 *   here).
 */
void TwoSidedBRDF::getDiffuseReemited(const Basis& localBasis, const Point2D& surfaceCoordinate, const LightVector& incidentLight, LightVector& reemitedLight)
{
  if(reemitedLight.getRay().v.dot(localBasis.k)<0)
    _external->getDiffuseReemited(localBasis, surfaceCoordinate, incidentLight, reemitedLight);
  else
  {
    Basis b = localBasis;
    b.i.mul(-1.0);
    b.j.mul(-1.0);
    b.k.mul(-1.0);
    _internal->getDiffuseReemited(b, surfaceCoordinate, incidentLight, reemitedLight);
  }
}

/**
 * Compute the specular part of a reflexion. Use the given incident light and 
 * place the computed result into the reemitedLight parameter.
 *
 * Most of the implementations of this methode dont use the geometric information
 * of the reemitedLight. They assume that the given incidentLight is build from
 * the result (backwarded) of getSpecularSubRays().
 *
 * @param localBasis : the object localBasis at the computation point.
 * @param surfaceCoordinate : the surface coordinate (texture coordinate) of the computation
 *   point on the object.
 * @param incidentLight : the incident light comming from the incident ray.
 * @param reemitedLight : the light reemited into the view direction (result will be placed 
 *   here).
 */
void TwoSidedBRDF::getSpecularReemited(const Basis& localBasis, const Point2D& surfaceCoordinate, const LightVector& incidentLight, LightVector& reemitedLight)
{
  if(reemitedLight.getRay().v.dot(localBasis.k)<0)
    _external->getSpecularReemited(localBasis, surfaceCoordinate, incidentLight, reemitedLight);
  else
  {
    Basis b = localBasis;
    b.i.mul(-1.0);
    b.j.mul(-1.0);
    b.k.mul(-1.0);
    _internal->getSpecularReemited(b, surfaceCoordinate, incidentLight, reemitedLight);
  }
}

/**
 * Compute a set of reflected ray for the speculars reflexions. As it is designed
 * for a backward ray-traing it is a set of incoming ray that is realy builded 
 * from the reflexion direction (observation direction).
 * 
 * Non specular material should not override this method. The default implementation
 * dont create any secondary ray as wanted for non specular material.
 *
 * @param localBasis : the local Basis on the object at the computation point.
 * @param surfaceCoordinate : the surface  coordinate (texture coordinate) of the computation 
 *   point on the object.
 * @param reemitedLight : the light data that will be computed by using the rays
 *   this method will compute. 
 * @param subrays : the vector were the secondaries rays will be put.
 */
void TwoSidedBRDF::getSpecularSubRays(const Basis& localBasis, const Point2D& surfaceCoordinate, LightVector& reemitedLight, std::vector<LightVector>& subrays)
{
  if(reemitedLight.getRay().v.dot(localBasis.k)<0)
    _external->getSpecularSubRays(localBasis, surfaceCoordinate, reemitedLight, subrays);
  else
  {
    Basis b = localBasis;
    b.i.mul(-1.0);
    b.j.mul(-1.0);
    b.k.mul(-1.0);
    _internal->getSpecularSubRays(b, surfaceCoordinate, reemitedLight, subrays);
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
bool TwoSidedBRDF::bouncePhoton(const Basis& localBasis, const Point2D& surfaceCoordinate, MultispectralPhoton& photon, bool& specular)
{
  if(photon.direction.dot(localBasis.k)<0)
    return _external->bouncePhoton(localBasis, surfaceCoordinate, photon, specular);

  Basis b = localBasis;
  b.i.mul(-1.0);
  b.j.mul(-1.0);
  b.k.mul(-1.0);
  return _internal->bouncePhoton(b, surfaceCoordinate, photon, specular);
}

/**
 * Compute secondary ray for sampling the diffuse reflexion incoming light.
 *
 * Default implementation use a uniform distribution over the dome if the
 * material was created as diffuse. (See Material()) Otherwise it create no
 * secondary ray.
 *
 * @param localBasis : the local Basis on the object at the computation point.
 * @param surfaceCoordinate : the surface  coordinate (texture coordinate) of the 
 *   computation point on the object.
 * @param reemitedLight : the reemitedLight we want to compute with the ray
 *   we are asking for.
 * @param incomming : the nearest incoming photons.
 * @param nbRays : the number of sample we want, it is indicative.
 * @param subrays : the generated secondaries rays will be placed into this vector.
 * @param weights : the weights of the random distrubution will be placed here.
 */
void TwoSidedBRDF::getRandomDiffuseRay(const Basis& localBasis, const Point2D& surfaceCoordinate, LightVector& reemitedLight, unsigned int nbRays, std::vector<LightVector>& subrays)
{
  if(reemitedLight.getRay().v.dot(localBasis.k)<0)
    _external->getRandomDiffuseRay(localBasis, surfaceCoordinate, reemitedLight, nbRays, subrays);
  else
  {
    Basis b = localBasis;
    b.i.mul(-1.0);
    b.j.mul(-1.0);
    b.k.mul(-1.0);
    _internal->getRandomDiffuseRay(b, surfaceCoordinate, reemitedLight, nbRays, subrays);
  }
}

void TwoSidedBRDF::getDiffuseReemitedFromAmbiant(const Basis& localBasis, const Point2D& surfaceCoordinate, LightVector& reemitedLight, const Spectrum& incident)
{
   if(reemitedLight.getRay().v.dot(localBasis.k)<0)
    _external->getDiffuseReemitedFromAmbiant(localBasis, surfaceCoordinate, reemitedLight, incident);
  else
  {
    Basis b = localBasis;
    b.i.mul(-1.0);
    b.j.mul(-1.0);
    b.k.mul(-1.0);
    _internal->getDiffuseReemitedFromAmbiant(b, surfaceCoordinate, reemitedLight, incident);
  }
}