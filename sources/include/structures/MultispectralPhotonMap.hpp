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
#ifndef _MULTISPECTRALPHOTONMAP_HPP
#define _MULTISPECTRALPHOTONMAP_HPP

#include <core/3DBase.hpp>
#include <structures/MultispectralPhoton.hpp>
#include <structures/KdTree.hpp>
#include <core/Object.hpp>


class MultispectralPhotonMap{
public :
  /**
   * Constructor
   * @param photonPower : the power of a photon (all photon have the same energy in a same photon map)
   */
  inline MultispectralPhotonMap(const Real& photonPower);

  /**
   * Add a photon to the photon map. Note that the map must have not been optimized !
   * @param photon : the photon to add.
   */
  inline void addPhoton(const MultispectralPhoton& photon);

  /**
   * Optimize the photon map to enhance the search speed.
   */
  inline void optimize();

  /**
   * Convert the photon map into an irradiance map.
   * @param radius : the initial radius search for the nearest photon photon search pass.
   * @param nb_poton : the number of nearest pĥoton to take count in the computation.
   */
  inline void convertToIrradianceMap(Real Radius, int nb_photon);


  /**
   * Get the irradiance at the given point.
   * @param origin : the point where we want to know the irradiance.
   * @param irradiance : the computed irradiance will be placed into this spectrum.
   */
  inline void getIrradiance(const Point& origin, Spectrum& irradiance);

  /**
   * Compute the estimated reflected radiance.
   * @param localBasis : the local basis at the computation point.
   * @param surfaceCoordinate : the local surface coordinate at the computation point.
   * @param object : the surface that reflect the light.
   * @param radius : the initial radius search for the nearest photon photon search pass.
   * @param nb_poton : the number of nearest pĥoton to take count in the computation.
   * @param lightdata : the reflected light data to compute.
   */
  inline void getEstimation(const Basis& localBasis, const Point2D& surfaceCoordinate, Object& object, Real radius, int nb_photon, LightVector& lightdata);

  /**
   * Serach the nearest photon from a given point.
   * @param origin : the point near where we are searching photons.
   * @param radius : the the initial radius search for the nearest photon photon search.
   * @param nb_photon : the number of wanted photons.
   * @param photons : found photons will be placed into this vector.
   */
  inline void getNearestPhotons(const Point& origin, Real radius, int nb_photon, std::vector<MultispectralPhoton*>& photons);

  /**
   * Return the number of photon contained into this photon map.
   */
  inline unsigned int getSize();

  /**
   * Export the photon map.
   */
  inline void getData(MultispectralPhoton* photons);

  /**
   * Import the photon map.
   */
  inline void setData(MultispectralPhoton* photons, unsigned int nb_photons);
private :
  Real _photonPower;
  KdTree<MultispectralPhoton> _tree;
};

/**
 * Constructor
 * @param photonPower : the power of a photon (all photon have the same energy in a same photon map)
 */
inline MultispectralPhotonMap::MultispectralPhotonMap(const Real& photonPower)
: _photonPower(photonPower)
{
  //Nothing to do more
}

/**
 * Add a photon to the photon map. Note that the map must have not been optimized !
 * @param photon : the photon to add.
 */
inline void MultispectralPhotonMap::addPhoton(const MultispectralPhoton& photon)
{
  _tree.add(photon);
}

/**
 * Optimize the photon map to enhance the search speed.
 */
inline void MultispectralPhotonMap::optimize()
{
  _tree.optimize();
}

/**
 * Convert the photon map into an irradiance map.
 * @param radius : the initial radius search for the nearest photon photon search pass.
 * @param nb_poton : the number of nearest pĥoton to take count in the computation.
 */
inline void MultispectralPhotonMap::convertToIrradianceMap(Real radius, int nb_photon)
{
  // Get the photon array 
  unsigned int size = _tree.getSize();
  MultispectralPhoton* irradiancesValues = new MultispectralPhoton[size];
  _tree.getData(irradiancesValues);
  
  // Compute the irradiance for each photon
  std::vector<MultispectralPhoton*> photons(50);
  for(unsigned int i=0; i<size; i++)
  {
    // Initialize data
    photons.clear();
    for(unsigned int l=0; l<GlobalSpectrum::nbWaveLengths(); l++)
      irradiancesValues[i].radiance[l]=0;   

    // Get the irradiant photons
    Real r2=_tree.getNearestNeighbor(irradiancesValues[i].position, nb_photon, radius, irradiancesValues[i].normal, photons);
    for(unsigned int p=0; p<photons.size(); p++)
      for(unsigned int l=0; l<GlobalSpectrum::nbWaveLengths(); l++)
        irradiancesValues[i].radiance[l]+=photons[p]->radiance[l];
    
    // Compute the irradiance
    if(r2>0)
    {
      const Real invarea = 1.0/(M_PI*r2);
      const Real photonPower = _photonPower*invarea;
      for(unsigned int l=0; l<GlobalSpectrum::nbWaveLengths(); l++)
        irradiancesValues[i].radiance[l]*=photonPower;   
    }
    else
    {
      for(unsigned int l=0; l<GlobalSpectrum::nbWaveLengths(); l++)
        irradiancesValues[i].radiance[l]=0.0;   
    }
  }
  
  //Set the irradiance data
  _tree.setData(irradiancesValues, size);
  
  //Free memory
  delete[] irradiancesValues;
}

/**
 * Get the irradiance at the given point.
 * @param origin : the point where we want to know the irradiance.
 * @param irradiance : the computed irradiance will be placed into this spectrum.
 */
inline void MultispectralPhotonMap::getIrradiance(const Point& origin, Spectrum& irradiance)
{
  //Get the nearest photon
  MultispectralPhoton* irradianceValue =  _tree.getNearestElement(origin);

  if(irradianceValue==NULL)
  {
    irradiance.clear();
    return;
  }

  //Set the irradiance
  for(unsigned int l=0; l<GlobalSpectrum::nbWaveLengths(); l++)
    irradiance[l] = irradianceValue->radiance[l];
}

/**
 * Compute the estimated reflected radiance.
 * @param localBasis : the local basis at the computation point.
 * @param surfaceCoordinate : the local surface coordinate at the computation point.
 * @param object : the surface that reflect the light.
 * @param radius : the initial radius search for the nearest photon photon search pass.
 * @param nb_poton : the number of nearest pĥoton to take count in the computation.
 * @param lightdata : the reflected light data to compute.
 */
inline void MultispectralPhotonMap::getEstimation(const Basis& localBasis, const Point2D& surfaceCoordinate, Object& object, Real radius, int nb_photon, LightVector& lightdata)
{
  lightdata.clear();

  //Get the nearest photons
  std::vector<MultispectralPhoton*> photons;
  Real r2 = _tree.getNearestNeighbor(localBasis.o, nb_photon, radius, localBasis.k, photons);
  if(r2==0.0)
    return;

  Real invarea = 1.0/(M_PI*r2);
  Real photonPower = _photonPower*invarea;

  LightVector incident;
  LightVector reemited;
  reemited.initGeometricalData(lightdata);
  for(unsigned int i=0; i<photons.size(); i++)
  {
    Real weight = photonPower/std::abs(photons[i]->direction.dot(localBasis.k));

    reemited.initSpectralData(lightdata);
    incident.initSpectralData(lightdata);
    incident.changeReemitedPolarisationFramework(localBasis.k);
    for(unsigned int k=0; k<lightdata.size(); k++)
      incident[k].setRadiance(weight*photons[i]->radiance[lightdata[k].getIndex()]);
    incident.setRay(photons[i]->position, photons[i]->direction);

    object.getDiffuseReemited(localBasis, surfaceCoordinate, incident, reemited);
    lightdata.add(reemited);
  }
  lightdata.changeReemitedPolarisationFramework(localBasis.k);
}

/**
 * Serach the nearest photon from a given point.
 * @param origin : the point near where we are searching photons.
 * @param radius : the the initial radius search for the nearest photon photon search.
 * @param nb_photon : the number of wanted photons.
 * @param photons : found photons will be placed into this vector.
 */
inline void MultispectralPhotonMap::getNearestPhotons(const Point& origin, Real radius, int nb_photon, std::vector<MultispectralPhoton*>& photons)
{
  _tree.getNearestNeighbor(origin, nb_photon, radius , photons);
}

/**
 * Export the photon map.
 */
inline void MultispectralPhotonMap::getData(MultispectralPhoton* photons)
{
  _tree.getData(photons);
}

/**
 * Import the photon map.
 */
inline void MultispectralPhotonMap::setData(MultispectralPhoton* photons, unsigned int nbPhotons)
{
  _tree.setData(photons, nbPhotons);
}

/**
 * Return the number of photon contained into this photon map.
 */
inline unsigned int MultispectralPhotonMap::getSize()
{
  return _tree.getSize();
}

#endif //_MULTISPECTRALPHOTONMAP_HPP
