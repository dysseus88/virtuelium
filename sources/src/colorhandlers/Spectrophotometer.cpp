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

#include <stdio.h>
#include <mpi.h>

#include <colorhandlers/Spectrophotometer.hpp>
#include <core/LightBase.hpp>


/**
 * Constructor
 * @param standardObserver : Standard observer (array of 3 spectra).
 * @param convertionMatrix : 3x3 Matrix for converting from XYZ to the wanted basis.
 * @param resultFilename : the filename where the result spectrum will be stored
 * @param nbSample : set the number of sample to integrate
 */
Spectrophotometer::Spectrophotometer(const std::string& resultFilename, unsigned int nbSample, ColorHandler* colorHandler)
:_colorHandler(colorHandler), _resultFilename(resultFilename), _nbSample(nbSample), _samples(0)
{
  _result.clear();  
  //if(pthread_mutex_init(&_mutex, NULL)) {
  //  printf("\n -- ATTENTION --  ");
  //  printf("(Spectrophotometer::Spectrophotometer) Echec de l'initialisation ");
  //  printf("du verrou d'exclusion mutuel.\n");
  //}
}

/**
 * Virtual destructor.
 */
inline Spectrophotometer::~Spectrophotometer()
{
  //Save spectral data
  FILE *out = NULL;
  out = fopen(_resultFilename.c_str(), "w");
  if (out == NULL ) {
    printf("\n -- ATTENTION --  ");
    printf("(Spectrophotometer::Spectrophotometer) Echec de l'écriture ");
    printf("du fichier de sortie du spectrophotomètre virtuel.\n");
  } else {
    fprintf(out, "Spectrum simulated by Virtuelium's spectrophotometer.\n");
    fprintf(out, "Wavelength\tValue\n");
    for(unsigned int i=0; i<GlobalSpectrum::nbWaveLengths(); i++) {
      fprintf(out, "%f\t%f\n", 
        GlobalSpectrum::getWaveLength(i), 
        (_result[i]/(Real)_samples));
    }
    fclose(out);
  }

  //Free data
  if(_colorHandler!=0)
    delete _colorHandler;

  //if(pthread_mutex_destroy(&_mutex)) {
  //  printf("\n -- ATTENTION --  ");
  //  printf("(Spectrophotometer::Spectrophotometer) Echec de l'initialisation ");
  //  printf("du verrou d'exclusion mutuel.\n");
  //}
}

/**
 * This function convert a light data into an RGB color.
 * lightdata : the light data to convert
 * color : the array that will contain the converted RGB color.
 */
void Spectrophotometer::lightDataToRGB(const LightVector& lightdata, Pixel& color)
{
  if(_colorHandler!=0)
    _colorHandler->lightDataToRGB(lightdata, color);

  //if(pthread_mutex_lock(&_mutex)) {
  //  printf("\n -- ATTENTION --  ");
  //  printf("(Spectrophotometer::Spectrophotometer) Echec de l'initialisation ");
  //  printf("du verrou d'exclusion mutuel.\n");
  //}
  _samples++;
  for(unsigned int i=0; i<GlobalSpectrum::nbWaveLengths(); i++)
    _result[i] += lightdata[i].getRadiance()/(Real)_nbSample;

  //if(pthread_mutex_unlock(&_mutex)) {
  //  printf("\n -- ATTENTION --  ");
  //  printf("(Spectrophotometer::Spectrophotometer) Echec de l'initialisation ");
  //  printf("du verrou d'exclusion mutuel.\n");
  //}
}

/**
 * Return the number of channels of the computable image.
 */
unsigned int Spectrophotometer::getNumberOfChannels()
{
  return _colorHandler-> getNumberOfChannels();
}

/**
 * Return the name of the i-th channel.
 */
std::string Spectrophotometer::getChannelName(unsigned int i)
{
  return _colorHandler->getChannelName(i);
}

