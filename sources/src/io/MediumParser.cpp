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

#include <io/MediumParser.hpp>
#include <io/DataParser.hpp>

/**
 * Load a medium from the given file.
 * @param filename : the filename of the of the file that contain the medium
 *   info.
 * @return : the loaded medium.
 */
Medium* MediumParser::loadMedium(const std::string& filename)
{
  Medium* medium = new Medium();

  DataParser parser;
  parser.loadSpectralMaterial(filename, medium->r);
  parser.loadSpectralMaterial(filename, medium->t);
  parser.loadDielectricMaterial(filename, medium->n, medium->k);
  parser.loadPigmentMaterial(filename, medium->K, medium->S);

  medium->isOpaque=false;
  medium->useLambertianModel=false;
  medium->useFresnelModel=false;
  medium->useKubelkaMunkModel=false;

  for(unsigned int i=0; i<GlobalSpectrum::nbWaveLengths(); i++)
  {
    if(medium->r[i] > 0.00001)
    {
      medium->isOpaque=true;
      medium->useLambertianModel=true;
    }
    if(medium->n[i] > 0.00001)
    {
      medium->useFresnelModel=true;
      medium->k[i]*=medium->n[i]; //Transform kappa into k
    }
    if(medium->S[i] > 0.00001)
      medium->useKubelkaMunkModel=true;
  }

  return medium;
}

/**
 * Load a medium from a list of multiples files.
 * @param filenames : a vector that contain the filenames of the files that
 *   describes the medium.
 * @return : the loaded medium.
 */
Medium* MediumParser::loadMedium(const std::vector<std::string>& filenames)
{
  Medium* medium = new Medium();
  medium->isOpaque=false;
  medium->useLambertianModel=false;
  medium->useFresnelModel=false;
  medium->useKubelkaMunkModel=false;

  for(unsigned int i=0; i<filenames.size(); i++)
  {
    Medium* tmp = loadMedium(filenames[i]);

    if(tmp->isOpaque)
      medium->isOpaque=true;
    if(tmp->useLambertianModel)
    {
      medium->r=tmp->r;
      medium->useLambertianModel=true;
    }
    if(tmp->useFresnelModel)
    {
      medium->n=tmp->n;
      medium->k=tmp->k;
      medium->useFresnelModel=true;
    }
    if(tmp->useKubelkaMunkModel)
    {
      medium->K=tmp->K;
      medium->S=tmp->S;
      medium->useKubelkaMunkModel=true;
    }

    delete tmp;
  }
  
  return medium;
}

