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

#include <colorhandlers/RGBColorHandler.hpp>
#include <core/LightBase.hpp>


/**
 * Constructor
 * sourceReference : Spectrum of the source of reference.
 * standardObserver : Standard observer (array of 3 spectra).
 * convertionMatrix : 3x3 Matrix for converting from XYZ to the wanted basis.
 */
RGBColorHandler::RGBColorHandler(Spectrum* standardObserver, Real gain)
:_standardObserver(standardObserver)
{
  _gain[0]=gain;
  _gain[1]=gain;
  _gain[2]=gain;
}


/**
 * Constructor
 * @param standardObserver : Standard observer (array of 3 spectra).
 */
RGBColorHandler::RGBColorHandler(Spectrum* standardObserver, Real gain, const Spectrum& white)
:_standardObserver(standardObserver)
{
  for(int k=0; k<3; k++)
    _gain[k]=0;

  for(unsigned int i = 0; i<GlobalSpectrum::nbWaveLengths(); i++)
    for(int k=0; k<3; k++)
      _gain[k]+= white[i] * _standardObserver[k][i];  
  
  for(int k=0; k<3; k++)
    _gain[k] = gain/_gain[k];
}

/**
 * This function convert a light data into an RGB color.
 * lightdata : the light data to convert
 * color : the array that will contain the converted RGB color.
 */
void RGBColorHandler::lightDataToRGB(const LightVector& lightdata, Pixel& color)
{
  //Integrating the specter
  Real rgb[3]={0, 0, 0};
  for(unsigned int i = 0; i<GlobalSpectrum::nbWaveLengths(); i++)  
    for(int k=0; k<3; k++)
      rgb[k]+= lightdata[i].getRadiance() * _standardObserver[k][i];
  for(int k=0; k<3; k++)
    rgb[k] *= _gain[k]/(Real)GlobalSpectrum::nbWaveLengths();
  
  //Set and return final values
  for(unsigned int k=0; k<3; k++)
    color[k] = (rgb[k]<0)?0 : rgb[k];
}
