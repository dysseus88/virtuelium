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

#include <materials/DepolarizedBRDF.hpp>

/**
 * Constructor
 * material : the material to depolarise (reflected light will not be polarised).
 */
DepolarizedBRDF::DepolarizedBRDF(Material* material)
: Material(material->isDiffuse(), material->isSpecular()), _material(material)
{
  //Nothing to do more
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
void DepolarizedBRDF::getDiffuseReemited(const Basis& localBasis, const Point2D& surfaceCoordinate, const LightVector& incidentLight, LightVector& reemitedLight)
{
  _material->getDiffuseReemited(localBasis, surfaceCoordinate, incidentLight, reemitedLight);
  for(unsigned int i=0; i<reemitedLight.size(); i++)
    reemitedLight[i].setRadiance(reemitedLight[i].getRadiance());
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
void DepolarizedBRDF::getSpecularReemited(const Basis& localBasis, const Point2D& surfaceCoordinate, const LightVector& incidentLight, LightVector& reemitedLight)
{
  _material->getSpecularReemited(localBasis, surfaceCoordinate, incidentLight, reemitedLight);
  for(unsigned int i=0; i<reemitedLight.size(); i++)
    reemitedLight[i].setRadiance(reemitedLight[i].getRadiance());
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
void DepolarizedBRDF::getSpecularSubRays(const Basis& localBasis, const Point2D& surfaceCoordinate, LightVector& reemitedLight, std::vector<LightVector>& subrays)
{
  _material->getSpecularSubRays(localBasis, surfaceCoordinate, reemitedLight, subrays);
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
bool DepolarizedBRDF::bouncePhoton(const Basis& localBasis, const Point2D& surfaceCoordinate, MultispectralPhoton& photon, bool& specular)
{
  return _material->bouncePhoton(localBasis, surfaceCoordinate, photon, specular);
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
void DepolarizedBRDF::getRandomDiffuseRay(const Basis& localBasis, const Point2D& surfaceCoordinate, LightVector& reemitedLight, unsigned int nbRays, std::vector<LightVector>& subrays)
{
  _material->getRandomDiffuseRay(localBasis, surfaceCoordinate, reemitedLight, nbRays, subrays);
}

void DepolarizedBRDF::getDiffuseReemitedFromAmbiant(const Basis& localBasis, const Point2D& surfaceCoordinate, LightVector& reemitedLight, const Spectrum& incident)
{
  _material->getDiffuseReemitedFromAmbiant(localBasis, surfaceCoordinate, reemitedLight, incident);
  for(unsigned int i=0; i<reemitedLight.size(); i++)
    reemitedLight[i].setRadiance(reemitedLight[i].getRadiance());

  reemitedLight.changeReemitedPolarisationFramework(localBasis.k);
}
