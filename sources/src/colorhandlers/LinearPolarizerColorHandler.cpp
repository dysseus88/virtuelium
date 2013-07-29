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

#include <colorhandlers/LinearPolarizerColorHandler.hpp>
#include <core/LightBase.hpp>


/**
 * Constructor
 * sourceReference : Spectrum of the source of reference.
 * standardObserver : Standard observer (array of 3 spectra).
 * convertionMatrix : 3x3 Matrix for converting from XYZ to the wanted basis.
 */
LinearPolarizerColorHandler::LinearPolarizerColorHandler(const Vector& up, ColorHandler* colorHandler)
: _up(up), _colorHandler(colorHandler)
{
  //nothing to do more
}

/**
 * This function convert a light data into an RGB color.
 * lightdata : the light data to convert
 * color : the array that will contain the converted RGB color.
 */
void LinearPolarizerColorHandler::lightDataToRGB(const LightVector& lightdata, Pixel& color)
{
  LightVector workingRay = lightdata;
  workingRay.changeIncidentPolarisationFramework(_up);

  for(unsigned int i=0; i<workingRay.size(); i++)
    workingRay[i].applyLinearFilter(0.0);

  _colorHandler->lightDataToRGB(workingRay, color);
}

/**
 * Return the number of channels of the computable image.
 */
unsigned int LinearPolarizerColorHandler::getNumberOfChannels()
{
  return _colorHandler-> getNumberOfChannels();
}

/**
 * Return the name of the i-th channel.
 */
std::string LinearPolarizerColorHandler::getChannelName(unsigned int i)
{
  return _colorHandler->getChannelName(i);
}

