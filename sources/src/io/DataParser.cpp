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
 
#include <io/DataParser.hpp>
#include <io/XMLParser.hpp>
#include <exceptions/Exception.hpp>

/**
 * Load a spectral property from a file. The file must be sorted by wavelength.
 * @param filename : the data filename to open.
 * @param propretyName : the name pf the property to load.
 * @param data : this function will place the data loaded into this spectrum.
 */
void DataParser::extractSpectralProperty(const std::string& filename, const std::string& propertyName, Spectrum& data)
{

  data.clear();

  //Parse the XML file and get the tree
  XMLParser parser;
  XMLTree* root = parser.getXMLTree(filename);

  //Verify the validity of the file 
  if(root->getNumberOfChildren()==0)
    throw Exception("(DataParser::extractSpectralProperty) Le fichier " + filename + " ne contient aucune entrée.");
  if(root->getChild(0)->getAttributeValue("wavelength")=="")
    throw Exception("(DataParser::extractSpectralProperty) Le fichier " + filename + " ne contient pas d'entrée spectrale : impossible de trouver les longueurs d'ondes.");
  
  unsigned int child=0;
  unsigned int index=0;
  while(index < GlobalSpectrum::nbWaveLengths())
  {
    Real childw=strtod(root->getChild(child)->getAttributeValue("wavelength").c_str(), NULL);
    Real indexw=GlobalSpectrum::getWaveLength(index) ;

    if(child==root->getNumberOfChildren()-1)
    {
      data[index] = strtod(root->getChild(child)->getAttributeValue(propertyName).c_str(), NULL);
      index++;
    }
    else if(childw<indexw)
      child++;
    else
    {
      if(child==0)
        data[index] = strtod(root->getChild(child)->getAttributeValue(propertyName).c_str(), NULL);
      else
      {
        Real lastcw=strtod(root->getChild(child-1)->getAttributeValue("wavelength").c_str(), NULL);
        Real d1 = strtod(root->getChild(child-1)->getAttributeValue(propertyName).c_str(), NULL);
        Real d2 = strtod(root->getChild(child)->getAttributeValue(propertyName).c_str(), NULL);
        Real f  = (indexw - lastcw)/(childw - lastcw);
        data[index] = d1*(1.0-f) + d2*f;
      }
      index++;
    }
  }

  delete root;
}

/**
 * Load a spectral material from a file. The file must be sorted by wavelength.
 * @param filename : the spectrum filename to open.
 * @param spectrum : this function will place the data loaded into this spectrum.
 */
void DataParser::loadSpectralMaterial(const std::string& filename, Spectrum& spectrum)
{
  extractSpectralProperty(filename, "value", spectrum);
}

/**
 * Load a pigment based material from a file. The file must be sorted by wavelength.
 * @param filename : the spectrum filename to open.
 * @param k : this function will place the k loaded into this spectrum.
 * @param s : this function will place the s loaded into this spectrum.
 */
void DataParser::loadPigmentMaterial(const std::string& filename, Spectrum& k, Spectrum& s)
{
  extractSpectralProperty(filename, "k", k);
  extractSpectralProperty(filename, "s", s);
}

/**
 * Load a set of Fresnel's complexes indices. The file must be sorted by
 * wavelength and composed by two entry for each wavelength. Each entry is a
 * wavelenght followed by the two values (n, k) separated by whitespaces.
 * @param filename : the indices filename to open.
 * @param n : this will contain the parsed real part of the indice (n)
 * @param kappa : this will contain the parsed imaginary part of the indice
 */
void DataParser::loadDielectricMaterial(const std::string& filename, Spectrum& n, Spectrum& kappa)
{
  extractSpectralProperty(filename, "n", n);
  extractSpectralProperty(filename, "kappa", kappa);

  Spectrum k;
  extractSpectralProperty(filename, "k", k);
  for(unsigned int i=0; i<GlobalSpectrum::nbWaveLengths(); i++)
    if(kappa[i]==0)
      kappa[i] = k[i]/n[i];
}

/**
 * Load a pigment based material from a file. The file must be sorted by wavelength.
 * @param filename : the spectrum filename to open.
 * @param a : this function will place the spectral absorption coeficients into this spectrum.
 * @param thickness : this function will return the reference thickness into this parameter
 */
void DataParser::loadAbsorptionFromTransmissionMaterial(const std::string& filename, Spectrum& a)
{
  extractSpectralProperty(filename, "value", a);

  XMLParser parser;
  XMLTree* root = parser.getXMLTree(filename);
  Real thickness = strtod(root->getAttributeValue("thickness").c_str(), NULL);
  delete root;
  
  if(thickness<=0)
    throw Exception("(DataParser::loadTransmissionMaterial) Le fichier " + filename + " ne contient pas un paramètre d'épaisseur valide.");

  for(unsigned int i=0; i<GlobalSpectrum::nbWaveLengths(); i++)
    a[i] = -std::log(a[i])/thickness;
}

/**
 * Load a illuminant from a file. The file must be sorted by wavelength.
 * @param filename : the spectrum filename to open.
 * @param spectrum : this function will place the data loaded into this spectrum.
 */
void DataParser::loadSpectralIlluminant(const std::string& filename, Spectrum& spectrum)
{
  extractSpectralProperty(filename, "value", spectrum);
}

/**
 * Load an observer from a file. The file must be sorted by wavelength.
 * @param filename : the spectrum filename to open.
 * @param observer : this function will place the data loaded into this array.
 */
void DataParser::loadStandardObserver(const std::string& filename, Spectrum observer[3])
{
  extractSpectralProperty(filename, "x", observer[0]);
  extractSpectralProperty(filename, "y", observer[1]);
  extractSpectralProperty(filename, "z", observer[2]);
}

/**
 * Load a list of wavelengths from a file and place them into the given vector.
 * @param filename : the file that contain the list of wavelength.
 * @param wavelengths : loaded wavelengths will be placed here.
 */
void DataParser::loadWaveLenghts(const std::string& filename, std::vector<Real>& wavelengths)
{
  //Parse the XML file and get the tree
  XMLParser parser;
  XMLTree* node = parser.getXMLTree(filename);

  //Verify that the xml file contain a material
  if(node->getMarkup()!="wavelengths")
    throw Exception("(DataParser::loadWaveLenghts) Impossible de trouver la racine (<wavelengths>) du fichier " + filename);
  if(node->getNumberOfChildren()==0)
    throw Exception("(DataParser::loadWaveLenghts) Le fichier " + filename + " ne contient aucune entrée.");

  for(unsigned int i=0; i<node->getNumberOfChildren(); i++)
    wavelengths.push_back(strtod(node->getChild(i)->getAttributeValue("wavelength").c_str(), NULL));

  delete node;
}

