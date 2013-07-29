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
 
#ifndef _DATAPARSER_HPP
#define _DATAPARSER_HPP

#include <core/LightBase.hpp>

#include <string>
#include <vector>

/**
 * This class contain miscellaneous methodes to parse data files like materials
 * files.
 * @author Sylavin "Greewi" Dumazet
 */
class DataParser{
public :

  /**
   * Load a spectral property from a file. The file must be sorted by wavelength.
   * @param filename : the data filename to open.
   * @param propretyName : the name pf the property to load.
   * @param data : this function will place the data loaded into this spectrum.
   */
  void extractSpectralProperty(const std::string& filename, const std::string& propertyName, Spectrum& data);

  /**
   * Load a spectral material from a file. The file must be sorted by wavelength.
   * @param filename : the spectrum filename to open.
   * @param spectrum : this function will place the data loaded into this spectrum.
   */
  void loadSpectralMaterial(const std::string& filename, Spectrum& spectrum);

  /**
   * Load a pigment based material from a file. The file must be sorted by wavelength.
   * @param filename : the spectrum filename to open.
   * @param k : this function will place the k loaded into this spectrum.
   * @param s : this function will place the s loaded into this spectrum.
   */
  void loadPigmentMaterial(const std::string& filename, Spectrum& k, Spectrum& s);

  /**
   * Load a pigment based material from a file. The file must be sorted by wavelength.
   * @param filename : the spectrum filename to open.
   * @param t : this function will place the transmition loaded into this spectrum.
   * @param thickness : this function will return the reference thickness into this parameter
   */
  void loadAbsorptionFromTransmissionMaterial(const std::string& filename, Spectrum& t);

  /**
   * Load a set of Fresnel's complexes indices. The file must be sorted by
   * wavelength and composed by two entry for each wavelength. Each entry is a
   * wavelenght followed by the two values (n, k) separated by whitespaces.
   * @param filename : the indices filename to open.
   * @param n : this will contain the parsed real part of the indice (n)
   * @param k : this will contain the parsed imaginary part of the indice (kappa)
   */
  void loadDielectricMaterial(const std::string& filename, Spectrum& n, Spectrum& k);

  /**
   * Load a illuminant from a file. The file must be sorted by wavelength.
   * @param filename : the spectrum filename to open.
   * @param spectrum : this function will place the data loaded into this spectrum.
   */
  void loadSpectralIlluminant(const std::string& filename, Spectrum& spectrum);

  /**
   * Load an observer from a file. The file must be sorted by wavelength.
   * @param filename : the spectrum filename to open.
   * @param observer : this function will place the data loaded into this array.
   */
  void loadStandardObserver(const std::string& filename, Spectrum observer[3]);

  /**
   * Load a list of wavelengths from a file and place them into the given vector.
   * @param filename : the file that contain the list of wavelength.
   * @param wavelengths : loaded wavelengths will be placed here.
   */
  void loadWaveLenghts(const std::string& filename, std::vector<Real>& wavelengths);


};


#endif //_DATAPARSER_HPP
