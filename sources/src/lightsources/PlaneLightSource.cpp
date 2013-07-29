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

#include <lightsources/PlaneLightSource.hpp>

/**
 * Constructor
 * spectrum : the emited spectrum
 * power : the power of this source (multiplier factor)
 * origin : the origin of emisson
 */
PlaneLightSource::PlaneLightSource(const Spectrum& spectrum, const Real& power, const Point& o, const Vector& normal, const Vector& u, const Vector& v, unsigned int nbSamples)
: _spectrum(spectrum), _lightPower(power), _nbSamples(nbSamples)
{
  _spectrum.normalizePower();
  _basis.o = o;
  _basis.k = normal;
  _basis.i = u;
  _basis.j = v;
  _powerFactor=_lightPower;
  _area=u.norm()*v.norm();
}

/**
 * Place incidents light data casted form this source to the given.
 * receiver : the point where we need to have the incidents lights rays.
 * incidents : incidents light data will be placed into this vector.
 */
void PlaneLightSource::getIncidentLight(const Point& receiver, const LightVector& reemited, std::vector<LightVector>& incidents)
{
  for(unsigned int i=0; i<_nbSamples; i++)
  {
    LightVector lightdata;

    //Generate the origin
    Point origin;
    Real x = rand()/(Real)RAND_MAX;
    Real y = rand()/(Real)RAND_MAX;
    origin[0] = _basis.o[0] + x*_basis.i[0] + y*_basis.j[0];
    origin[1] = _basis.o[1] + x*_basis.i[1] + y*_basis.j[1];
    origin[2] = _basis.o[2] + x*_basis.i[2] + y*_basis.j[2];

    //Generate the direction
    Vector direction(origin, receiver);
    Real distance = direction.norm();
    direction.mul(1.0/distance);

    if(_basis.k.dot(direction)<0)
      return;

    lightdata.setRay(receiver, direction);
    lightdata.setDistance(distance);
    
    //Building the spectral part of this light data
    lightdata.initSpectralData(reemited);
    Real power= _basis.k.dot(direction) * _powerFactor/(lightdata.getDistance()*lightdata.getDistance()*_nbSamples*M_PI);
    for(unsigned int i=0; i<lightdata.size() ;i++)
      lightdata[i].setRadiance(_spectrum[lightdata[i].getIndex()]*power);

    //Initialize the polarisation framework
    if(direction[2]<0.999 && direction[2]>-0.999)
      lightdata.changeReemitedPolarisationFramework(Vector(0.0, 0.0, 1.0));
    else
      lightdata.changeReemitedPolarisationFramework(Vector(1.0, 0.0, 0.0));

    incidents.push_back(lightdata);
  }
}

/**
 * Return the power of this source (the sum of all wavelenght)
 */
Real PlaneLightSource::getPower()
{
  return _lightPower;
}

/**
 * Compute the emitted light for direct source view.
 * @param localBasis : the local basis on the surface of the source.
 * @param surfaceCoordinate : the surface coordinate on the source surface.
 * @param emitted : the light data to compute.
 */
void PlaneLightSource::getEmittedLight(const Basis& localBasis, const Point2D& surfaceCoordinate, LightVector& emitted)
{
  Real factor = _lightPower/_area;
  for(unsigned int i=0; i<emitted.size(); i++)
    emitted[i].setRadiance(_spectrum[emitted[i].getIndex()]*factor);
  emitted.changeReemitedPolarisationFramework(localBasis.k);
}

/**
 * Generate a random photon according this source spectral and spatial
 * distribution.
 * photon : the photon to generate;
 */
void PlaneLightSource::getRandomPhoton(MultispectralPhoton& photon)
{
  //Position
  Real x = rand()/(Real)RAND_MAX;
  Real y = rand()/(Real)RAND_MAX;
  photon.position[0] = _basis.o[0] + x * _basis.i[0] + y*_basis.j[0];
  photon.position[1] = _basis.o[1] + x * _basis.i[1] + y*_basis.j[1];
  photon.position[2] = _basis.o[2] + x * _basis.i[2] + y*_basis.j[2];

  //Direction
  Real norm2;
  Real cosOi;
  do{
    photon.direction[0]=rand()*2.0/RAND_MAX - 1.0;
    photon.direction[1]=rand()*2.0/RAND_MAX - 1.0;
    photon.direction[2]=rand()*2.0/RAND_MAX - 1.0;
    norm2=photon.direction.square();
    photon.direction.normalize();
    cosOi = _basis.k.dot(photon.direction);
  }while(norm2>cosOi*cosOi || cosOi<=0);

  //Building spectral part of the photon
  Real mean=0;
  for(unsigned int i=0; i<GlobalSpectrum::nbWaveLengths(); i++)
    mean+=_spectrum[i];
  for(unsigned int i=0; i<GlobalSpectrum::nbWaveLengths(); i++)
    photon.radiance[i]=_spectrum[i]/mean;
}
