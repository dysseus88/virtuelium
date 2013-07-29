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

#include <colorhandlers/PolarisationColorHandler.hpp>
#include <core/LightBase.hpp>


/**
 * Constructor
 * sourceReference : Spectrum of the source of reference.
 * standardObserver : Standard observer (array of 3 spectra).
 * convertionMatrix : 3x3 Matrix for converting from XYZ to the wanted basis.
 */
PolarisationColorHandler::PolarisationColorHandler(const Vector& up)
: _up(up)
{
  //nothing to do more
}

/**
 * This function convert a light data into an RGB color.
 * lightdata : the light data to convert
 * color : the array that will contain the converted RGB color.
 */
void PolarisationColorHandler::lightDataToRGB(const LightVector& lightdata, Pixel& color)
{
  LightVector workingRay = lightdata;

  workingRay.changeIncidentPolarisationFramework(_up);

  //Black for no polarisation
  Real rad=0;
  Real pp=0;
  Real ps=0;
  for(unsigned int i=0; i<GlobalSpectrum::nbWaveLengths(); i++)
  {
    pp += workingRay[i].getPPolarisedRadiance();
    ps += workingRay[i].getSPolarisedRadiance();
    rad+= workingRay[i].getRadiance();
  }
  
  color[0] = pp/rad; //Red for vertical polarisation
  color[1] = ps/rad; //Green for horizontal polarisation
}

/**
 * Return the number of channels of the computable image.
 */
unsigned int PolarisationColorHandler::getNumberOfChannels()
{
  return 2;
}

/**
 * Return the name of the i-th channel.
 */
std::string PolarisationColorHandler::getChannelName(unsigned int i)
{
  switch(i){
  case 0 : return "v_pol";
  case 1 : return "h_pol";
  }
  return "";
}

