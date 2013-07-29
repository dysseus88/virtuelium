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

#ifndef _LIGHTVECTOR_HPP
#define _LIGHTVECTOR_HPP

#include <core/3DBase.hpp>
#include <structures/LightData.hpp>
#include <exceptions/Exception.hpp>

class LightVector{
public :
  inline LightVector();
  inline LightVector(const LightVector& original);
  inline ~LightVector();
  inline LightVector& operator=(const LightVector& original);

  /**
   * Initialize the light vector spectral data.
   * wavelength : the indices in the global spectrum corresponding to the
   *   wavelength of this ray. Negative value for multispectral vector (default).
   */
  inline void initSpectralData(int wavelength=-1);

  /**
   * Initialize the light vector spectral data using properties (spectral bands
   * not spectral values) of an other.
   * lightdata : LightVector from wich we take the properties
   */
  inline void initSpectralData(const LightVector& lightdata);

  /**
   * Initialize the light vector geomtrical data using properties of an other.
   * lightdata : LightVector from wich we take the properties
   */
  inline void initGeometricalData(const LightVector& lightdata);

  /**
   * Clear all spectral data.
   */
  inline void clear();

  // Spectral accessors --------------------------------------------------------

  /**
   * Return the number of spectral band stored into this light vector.
   */
  inline unsigned int size() const;

  /**
   * Return the LightData of the i-th spectral band contained into this vector 
   */
  inline LightData& operator[](unsigned int i);

  /**
   * Return the LightData of the i-th spectral band contained into this vector 
   */
  inline const LightData& operator[](unsigned int i) const;

  /**
   * Add the given light data to the current
   */
  inline void add(const LightVector& lightvector);

  /**
   * Multiply the light data by the given factor
   */
  inline void mul(const Real& factor);

  /**
   * Return the weight of this lightray.
   */
  inline Real getWeight() const;

  /**
   * Set the weight of this lightray.
   */
  inline void setWeight(Real weight);

  // Geometric accessors -------------------------------------------------------

  /**
   * Get the propagation ray (parallel to the incident plane) accessor
   */
  inline const Ray getRay() const;

  /**
   * Set the propagation ray (parallel to the incident plane) accessor
   */
  inline void setRay(const Ray& ray);

  /**
   * Set the propagation ray (parallel to the incident plane) accessor
   */
  inline void setRay(const Point& origin, const Vector& direction);

  /**
   * Get the distance form source
   */
  inline Real getDistance() const;

  /**
   * Set the distance form source
   */
  inline void setDistance(Real distance);

  /**
   * Flip the light vector propagation
   */
  inline void flip();

  /**
   * Change the polarisation framework of an incident ray for matching new 
   * incident plan.
   * incident : the incident vector
   * normal : normal of the surface
   */
  inline void changeIncidentPolarisationFramework(const Vector& normal);

  /**
   * Change the polarisation framework of an reemited ray for matching last 
   * incident plan and new ray direction. No projection are done !
   * reemitedDir : the reemited direction vector
   * normal : normal of the surface
   */
  inline void changeReemitedPolarisationFramework(const Vector& normal);

  //inline void normalize(void);

private :
  LightData* _data;
  unsigned int _size;
  Basis _framework; //Framework of this lightdata.
                   // _framework.o & _framework.i : the inverted propagation ray (backward raytracing) is (o, i)
                   // _framework.j : the P-polarization vector (Parallel to the incident plane).
                   // _framework.k : the S-polarization vector (Orthogonal to the incident plane).
  Real _distance;  //Distance from the last emission/reflexion
  Real _weight;    //The weight of the raylight in the computations
};

inline LightVector::LightVector()
{
  _size=0;
  _data=0;
}

inline LightVector::LightVector(const LightVector& original)
{
  if(original._data==0)
  {
    _size=0;
    _data=0;
  }
  else
  {
    _size=original._size;
    _data=new LightData[_size];
    for(unsigned int i=0; i<_size; i++)
      _data[i]=original._data[i];
  }

  _framework = original._framework;
  _distance = original._distance;
  _weight= original._weight;
}

inline LightVector::~LightVector()
{
  if(_data!=0)
    delete[] _data;
}

inline LightVector& LightVector::operator=(const LightVector& original)
{
  //Delete old data
  if(_data!=0)
    delete[] _data;
  
  //If original is empty
  if(original._data==0)
  {
    _size=0;
    _data=0;
  }
  //If original is not empty create the array and copy the data
  else
  {
    _size=original._size;
    _data=new LightData[_size];
    for(unsigned int i=0; i<_size; i++)
      _data[i]=original._data[i];
  }

  //Copy the geometrical data
  _framework = original._framework;
  _distance = original._distance;
  _weight= original._weight;

  return *this; 
}


/**
 * Initialize the light vector.
 * wavelength : the indices in the global spectrum corresponding to the
 *   wavelength of this ray. Negative value for multispectral vector (default).
 */
inline void LightVector::initSpectralData(int wavelength)
{
  //Delete old data
  if(_data!=0)
    delete[] _data;

  if(wavelength>=0)
  {
    _size=1;
    _data = new LightData[_size];
    _data[0].setIndex(wavelength);
    _data[0].setRadiance(0.0);
  }
  else
  {
    _size=GlobalSpectrum::nbWaveLengths();
    _data = new LightData[_size];

    for(unsigned int i=0; i<_size; i++)
    {
      _data[i].setIndex(i);
      _data[i].setRadiance(0.0);
    }
  }
}

/**
 * Initialize the light vector spectral data using properties (spectral bands
 * not spectral values) of an other.
 * lightdata : LightVector from wich we take the properties
 */
inline void LightVector::initSpectralData(const LightVector& original)
{
  //Delete old data
  if(_data!=0)
    delete[] _data;

  //If original is empty
  if(original._data==0)
  {
    _size=0;
    _data=0;
  }
  //If original is not empty create the array and init the data
  else
  {
    _size=original._size;
    _data=new LightData[_size];
    for(unsigned int i=0; i<_size; i++)
    {
      _data[i].setIndex(original[i].getIndex());
      _data[i].setRadiance(0.0);
    }
  }
}

/**
 * Initialize the light vector geomtrical data using properties of an other.
 * lightdata : LightVector from wich we take the properties
 */
inline void LightVector::initGeometricalData(const LightVector& lightdata)
{
  
  _framework = lightdata._framework;
}

/**
 * Clear all spectral data.
 */
inline void LightVector::clear()
{
  for(unsigned int i=0; i<_size; i++)
    _data[i].setRadiance(0.0);
}

/**
 * Add the given light data to the current
 */
inline void LightVector::add(const LightVector& lightvector)
{
  //Place the current light vector into the same framework
  changeIncidentPolarisationFramework(lightvector._framework.j);

  //Handle pathologic case
  if(lightvector._data==0)
    return;
  if(_data==0)
    initSpectralData(lightvector);

  //Create a new data array
  LightData* data = new LightData[_size + lightvector._size];

  //Merge the two original arrays into the new
  unsigned int i=0;
  unsigned int j=0;
  unsigned int k=0;
  while(i<_size || j<lightvector._size)
  {
    if(i>=_size)
    {
      data[k] = lightvector._data[j];
      j++;k++;
    }
    else if(j>=lightvector._size)
    {
      data[k] = _data[i];
      i++;k++;
    }
    else if(_data[i].getIndex() > lightvector._data[j].getIndex())
    {
      data[k] = lightvector._data[j];
      j++;k++;
    }
    else if(_data[i].getIndex() < lightvector._data[j].getIndex())
    {
      data[k] = _data[i];
      i++;k++;
    }
    else

    {
      data[k] = _data[i]+lightvector._data[j];
      i++; j++; k++;
    }
  }

  //Delete the old array and replace it with the new one
  delete[] _data;
  _data=data;
  _size=k;
}

/**
 * Multiply the light data by the given factor
 */
inline void LightVector::mul(const Real& factor)
{
  for(unsigned int i=0; i<_size; i++)
    _data[i].mul(factor);
}

/**
 * Return the number of spectral band stored into this light vector.
 */
inline unsigned int LightVector::size() const
{
  return _size;
}

/**
 * Return the LightData of the i-th spectral band contained into this vector 
 */
inline LightData& LightVector::operator[](unsigned int i)
{
  return _data[i];
}

/**
 * Return the LightData of the i-th spectral band contained into this vector 
 */
inline const LightData& LightVector::operator[](unsigned int i) const
{
  return _data[i];
}

/**
 * Return the weight of this lightray.
 */
inline Real LightVector::getWeight() const
{
  return _weight;
}

/**
 * Set the weight of this lightray.
 */
inline void LightVector::setWeight(Real weight)
{
  _weight=weight;
}

inline const Ray LightVector::getRay() const
{
  Ray ray;
  ray.o = _framework.o;
  ray.v = _framework.i;
  return ray;
}

inline void LightVector::setRay(const Ray& ray)
{
  _framework.o = ray.o;
  _framework.i = ray.v;  
}

inline void LightVector::setRay(const Point& origin, const Vector& direction)
{
  _framework.o = origin;
  _framework.i = direction;  
}

/**
 * Get the distance form source
 */
inline Real LightVector::getDistance() const
{
  return _distance;
}

/**
 * Set the distance form source
 */
inline void LightVector::setDistance(Real distance)
{
  _distance=distance;
}


/**
 * Change the polarisation framework of an incident ray for matching new 
 * incident plan.
 * incident : the incident vector
 * normal : normal of the surface
 */
inline void LightVector::changeIncidentPolarisationFramework(const Vector& normal)
{
  Vector& _pp = _framework.j;
  Vector& _ps = _framework.k;
  Vector pp, ps;

  //Building the new polarisation framework
  if(_framework.i.dot(normal)<0.999 && _framework.i.dot(normal)>-0.999 )
    _framework.i.vect(normal, ps);
  else
    _framework.i.vect(Vector(normal[1],normal[2],normal[0]), ps);
  ps.vect(_framework.i, pp);
  pp.normalize();
  ps.normalize();

  Real angleToRotate = atan2(_ps.dot(pp), _pp.dot(pp));
  for(unsigned int i=0; i<_size; i++)
    _data[i].rotate(angleToRotate);

  //Affect the new vectors
  _pp=pp;
  _ps=ps;
}

/**
 * Change the polarisation framework of an reemited ray for matching last 
 * incident plan and new ray direction. No projection are done !
 * reemitedDir : the reemited direction vector
 * normal : normal of the surface
 */
inline void LightVector::changeReemitedPolarisationFramework(const Vector& normal)
{
  Vector& _pp = _framework.j;
  Vector& _ps = _framework.k;
  Vector pp, ps;

  //Building the new polarisation framework
  if(_framework.i.dot(normal)<0.999 && _framework.i.dot(normal)>-0.999 )
    _framework.i.vect(normal, ps);
  else
    _framework.i.vect(Vector(normal[1],normal[2],normal[0]), ps);
  ps.vect(_framework.i, pp);
  pp.normalize();
  ps.normalize();

  _pp=pp;
  _ps=ps;
}

/**
 * Flip the light vector propagation
 */
inline void LightVector::flip()
{
  _framework.i.mul(-1.0);
  _framework.k.mul(-1.0);
  for(unsigned int i=0; i<_size; i++)
    _data[i].flip();
}

//inline void LightVector::normalize(void) {
//  Real max = 0;
//  for (unsigned int i = 0; i < _size; i++) {
//    if (max < _data[i].getRadiance())
//      max = _data[i].getRadiance();
//  }
//  if (max == 0)
//    return;
//
//  mul(Real(1.0 / max));
//}

#endif //_LIGHTVECTOR_HPP
