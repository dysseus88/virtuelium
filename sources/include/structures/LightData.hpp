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

#ifndef _LIGHTDATA_HPP
#define _LIGHTDATA_HPP

#include <core/3DBase.hpp>

class LightData{
public :

  inline LightData();

  /**
   * Set the global spectrum index corresponding to the wavelength of this light
   * data.
   * index : global spectrum index of the wavelenght of this light data
   */
  inline void setIndex(int index);

  /**
   * Get the global spectrum index corresponding to the wavelength of this light
   * data.
   */
  inline int getIndex() const;

  /**
   * Return the radiance of this light data
   */
  inline Real getRadiance() const;

  /**
   * Set the radiance of this light data
   */
  inline void setRadiance(Real radiance);

  /**
   * Return the P polarised radiance of this light data
   */
  inline Real getPPolarisedRadiance() const;

  /**
   * Return the S polarised radiance of this light data
   */
  inline Real getSPolarisedRadiance() const;

  /**
   * Add two LightData and return the result
   */
  inline LightData operator+(const LightData& operand2) const;

  /**
   * Multiply the value of this light data by the given factor
   */
  inline void mul(const Real& factor);

  /**
   * Set the current light data with appling the given reflectance factor to the
   * given lightdata.
   * source : the light data source one wich we will apply the reflectances
   * rPara : the parallel (P) polarised reflectance component
   * rOrth : the orthogonal (S) polarised reflectance component
   */
  inline void applyReflectance(const LightData& source, const Real& rPara, const Real& rOrth, Real sup_radiance = -1.0, Real alpha = 0.5);

  /**
   * Rotate the ellipse (in order to reflect a framework change for example).
   * angleToRotate : the angle (in radiant) to rotate.
   */
  inline void rotate(const Real& angleToRotate);

  /**
   * Apply the effect of a linear polarization filter to this light data.
   * angle : the tilt angle of the polarizer.
   */
  inline void applyLinearFilter(const Real& angle);

  /**
   * Flip the lightdata as the propagation direciton were mirrored.
   */
  inline void flip();

private :
  int _index;
  Real _radiance; //Total radiance in this light data
  Real _linear0;  //Energy linearly polarised with a 0° angle from reference
  Real _linear45; //Energy lineraly polarised with a 45° angle from reference
  Real _circular; //Energy circularly polarised
};

inline LightData::LightData()
{
  _index=-100000000;
  _radiance=0.0;
  _linear0=0.0;
  _linear45=0.0;
  _circular=0.0;
}


/**
 * Set the global spectrum index corresponding to the wavelength of this light
 * data.
 * index : global spectrum index of the wavelenght of this light data
 */
inline void LightData::setIndex(int index)
{
  _index=index;
}

/**
 * Get the global spectrum index corresponding to the wavelength of this light
 * data.
 */
inline int LightData::getIndex() const
{
  return _index;
}

/**
 * Return the radiance of this light data
 */
inline Real LightData::getRadiance() const
{
  return _radiance;
}

/**
 * Set the radiance of this light data
 */
inline void LightData::setRadiance(Real radiance)
{
  _radiance=radiance;
  _linear0=0;
  _linear45=0;
  _circular=0;
}

/**
 * Return the P polarised radiance of this light data
 */
inline Real LightData::getPPolarisedRadiance() const
{
  if(_linear0 <0)
    return -_linear0;
  return 0;
}

/**
 * Return the S polarised radiance of this light data
 */
inline Real LightData::getSPolarisedRadiance() const
{
  if(_linear0 >0)
    return _linear0;
  return 0;
}

/**
 * Add two LightData and return the result
 */
inline LightData LightData::operator+(const LightData& operand2) const
{
  LightData result;
  result._index=_index;
  result._radiance = _radiance + operand2._radiance;
  result._linear0 = _linear0 + operand2._linear0;
  result._linear45 = _linear45 + operand2._linear45;
  result._circular = _circular + operand2._circular;
  return result;
}

/**
 * Multiply the value of this light data by the given factor
 */
inline void LightData::mul(const Real& factor)
{
  _radiance *= factor;
  _linear0  *= factor;
  _linear45 *= factor;
  _circular *= factor;
}

/**
 * Set the current light data with appling the given reflectance factor to the
 * given lightdata.
 * source : the light data source one wich we will apply the reflectances
 * rPara : the parallel (P) polarised reflectance component
 * rOrth : the orthogonal (S) polarised reflectance component
 */
 
 inline void LightData::applyReflectance(const LightData& source, const Real& rPara, const Real& rOrth, Real sup_radiance, Real alpha)
{
	//According to : Wilkie in Combined Rendering of Polarization and Fluorescence Effects
	Real radiance = source._radiance;
	Real linear0 = source._linear0;
	Real linear45 = source._linear45;
	Real circular = source._circular;

	Real A = (rOrth+rPara)*0.5;
	Real B = (rOrth-rPara)*0.5;

	if(sup_radiance == -1)
	{
		_radiance = radiance*A + linear0*B;
		_linear0  = radiance*B + linear0*A;
		_linear45 = linear45;
		_circular = circular;
	}
	else
	{
		//_radiance = (radiance*A + linear0*B + sup_radiance)/2.0;
		_radiance = (radiance*A + linear0*B)*(1.0-alpha) + (sup_radiance)*alpha;
		_linear0  = radiance*B + linear0*A;
		_linear45 = linear45;
		_circular = circular;
	}
 }

/**
 * Apply the effect of a linear polarization filter to this light data.
 * angle : the tilt angle of the polarizer.
 */
inline void LightData::applyLinearFilter(const Real& angle)
{
  //According to : Wilkie in Combined Rendering of Polarization and Fluorescence Effects
  Real cos2 = std::cos(2.0*angle);
  Real sin2 = std::sin(2.0*angle);

  Real radiance = _radiance;
  Real linear0  = _linear0;
  Real linear45  = _linear45;
  
  _radiance = 0.5*(radiance + cos2*linear0 + sin2*linear45);
  _linear0  = 0.5*(cos2*radiance + cos2*cos2*linear0 + sin2*cos2*linear45);
  _linear45 = 0.5*(sin2*radiance + cos2*sin2*linear0 + sin2*sin2*linear45);
  _circular = 0;
}

/**
 * Rotate the ellipse (in order to reflect a framework change for example).
 * angleToRotate : the angle (in radiant) to rotate.
 */
inline void LightData::rotate(const Real& angleToRotate)
{
  //According to : Wilkie in Combined Rendering of Polarization and Fluorescence Effects
  Real cos2A = cos(2.0*angleToRotate);
  Real sin2A = sin(2.0*angleToRotate);
  Real linear0 = _linear0;
  Real linear45 = _linear45;
  
  _linear0  =  linear0*cos2A + linear45*sin2A;
  _linear45 = -linear0*sin2A + linear45*cos2A;
}

/**
 * Flip the lightdata as the propagation direciton were mirrored.
 */
inline void LightData::flip()
{
  _linear45 *= -1.0;
  _circular *= -1.0;
}

#endif //_LIGHTDATA_HPP
