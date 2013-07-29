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

#include <materials/MappedBRDF.hpp>

/**
 * Constructor
 * @param materials : the materials that compose the map
 * @param maps : the maps of each materials
 */
MappedBRDF::MappedBRDF(std::vector<Material*> materials, std::vector<Image*> maps)
: Material(hasDiffuseMaterial(materials), hasSpecularMaterial(materials)), _materials(materials), _maps(maps)
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
void MappedBRDF::getDiffuseReemited(const Basis& localBasis, const Point2D& surfaceCoordinate, const LightVector& incidentLight, LightVector& reemitedLight)
{
  LightVector tmp;
  reemitedLight.clear();

  for(unsigned int i=0; i<_materials.size(); i++)
  {
    Real factor = getFactor(i, surfaceCoordinate);
    if(factor<=0.0001)
      continue;

    tmp.initSpectralData(reemitedLight);
    tmp.initGeometricalData(reemitedLight);
    _materials[i]->getDiffuseReemited(localBasis, surfaceCoordinate, incidentLight, tmp);
    tmp.mul(factor);
    reemitedLight.add(tmp);
  }
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
void MappedBRDF::getSpecularReemited(const Basis& localBasis, const Point2D& surfaceCoordinate, const LightVector& incidentLight, LightVector& reemitedLight)
{
  LightVector tmp;
  reemitedLight.clear();

  for(unsigned int i=0; i<_materials.size(); i++)
  {
    Real factor = getFactor(i, surfaceCoordinate);
    if(factor<=0.0001)
      continue;

    tmp.initSpectralData(reemitedLight);
    tmp.initGeometricalData(reemitedLight);
    _materials[i]->getSpecularReemited(localBasis, surfaceCoordinate, incidentLight, tmp);
    tmp.mul(factor);
    reemitedLight.add(tmp);
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
void MappedBRDF::getSpecularSubRays(const Basis& localBasis, const Point2D& surfaceCoordinate, LightVector& reemitedLight, std::vector<LightVector>& subrays)
{
  for(unsigned int i=0; i<_materials.size(); i++){
    Real factor = getFactor(i, surfaceCoordinate);
    if(factor<=0.0001)
      continue;
    _materials[i]->getSpecularSubRays(localBasis, surfaceCoordinate, reemitedLight, subrays);
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
bool MappedBRDF::bouncePhoton(const Basis& localBasis, const Point2D& surfaceCoordinate, MultispectralPhoton& photon, bool& specular)
{
  Real factorsum=0;
  for(unsigned int i=0; i<_materials.size(); i++)
    factorsum+=getFactor(i, surfaceCoordinate);

  for(unsigned int i=0; i<_materials.size(); i++)
  {
    Real factor = getFactor(i, surfaceCoordinate);
    if(factor<=0.0001)
      continue;

    if(rand() < factor*RAND_MAX/factorsum)
    {
      if(_materials[i]->bouncePhoton(localBasis, surfaceCoordinate, photon, specular))
        return true;
    }
    factorsum-=factor;
  }

  return false;
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
void MappedBRDF::getRandomDiffuseRay(const Basis& localBasis, const Point2D& surfaceCoordinate, LightVector& reemitedLight, unsigned int nbRays, std::vector<LightVector>& subrays)
{
  int nbDiffuseMat=0;
  for(unsigned int i=0; i<_materials.size(); i++)
    if(_materials[i]->isDiffuse())
      nbDiffuseMat++;

  for(unsigned int i=0; i<_materials.size(); i++)
    _materials[i]->getRandomDiffuseRay(localBasis, surfaceCoordinate, reemitedLight, 1+nbRays/nbDiffuseMat, subrays);
}

/**
 * Return the factor of the wanted material at the given point
 */
Real MappedBRDF::getFactor(unsigned int materialIndex, const Point2D& surfaceCoordinate)
{
  Real x = surfaceCoordinate[0]*_maps[materialIndex]->getWidth();
  Real y = surfaceCoordinate[1]*_maps[materialIndex]->getHeight();

  return _maps[materialIndex]->getPixel(x, y)[0];
}

/**
 * Return true if one of the materials of the given vector is diffuse.
 */
bool MappedBRDF::hasDiffuseMaterial(const std::vector<Material*>& materials)
{
  for(unsigned int i=0; i<materials.size(); i++)
    if(materials[i]->isDiffuse())
      return true;
  return false;
}

/**
 * Return true if one of the materials of the given vector is specular.
 */
bool MappedBRDF::hasSpecularMaterial(const std::vector<Material*>& materials)
{
  for(unsigned int i=0; i<materials.size(); i++)
    if(materials[i]->isSpecular())
      return true;
  return false;  
}

void MappedBRDF::getDiffuseReemitedFromAmbiant(const Basis& localBasis, const Point2D& surfaceCoordinate, LightVector& reemitedLight, const Spectrum& incident)
{
  LightVector tmp;
  reemitedLight.clear();

  for(unsigned int i=0; i<_materials.size(); i++)
  {
    Real factor = getFactor(i, surfaceCoordinate);
    if(factor<=0.0001)
      continue;

    tmp.initSpectralData(reemitedLight);
    tmp.initGeometricalData(reemitedLight);
    _materials[i]->getDiffuseReemitedFromAmbiant(localBasis, surfaceCoordinate, tmp, incident);
    tmp.mul(factor);
    reemitedLight.add(tmp);
  }
  reemitedLight.changeReemitedPolarisationFramework(localBasis.k);
}