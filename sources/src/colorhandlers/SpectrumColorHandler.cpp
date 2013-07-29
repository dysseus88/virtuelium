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

#include <colorhandlers/SpectrumColorHandler.hpp>
#include <sstream>

/**
 * This function convert a light data into an RGB color.
 * lightdata : the light data to convert
 * color : the array that will contain the converted RGB color.
 */
void SpectrumColorHandler::lightDataToRGB(const LightVector& lightdata, Pixel& color)
{
  for(unsigned int i = 0; i<GlobalSpectrum::nbWaveLengths(); i++)
    color[i]=lightdata[i].getRadiance();
}

/**
 * Return the number of channels of the computable image.
 */
unsigned int SpectrumColorHandler::getNumberOfChannels()
{
  return GlobalSpectrum::nbWaveLengths();
}

/**
 * Return the name of the i-th channel.
 */
std::string SpectrumColorHandler::getChannelName(unsigned int i)
{
  std::stringstream oss;
  oss << GlobalSpectrum::getWaveLength(i) << "nm";
  return oss.str();
}

