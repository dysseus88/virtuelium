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
 
#ifndef _IMAGE_HPP
#define _IMAGE_HPP

#include <string>
#include <vector>
#include <exceptions/Exception.hpp>
#include <common.hpp>
#include <maths/Point2D.hpp>
#include <cstring>


/**
 * A floating point multispectral image object.
 */
class Pixel{
public :
  inline Pixel(void);
  /**
   * Constructor (values are not initialised)
   * @param nbChannels : number of channels of this pixel
   */
  inline Pixel(unsigned int nbChannels);

  /**
   * Constructor
   * @param nbChannels : number of channels of this pixel
   * @param data : a raw data used to initialize the Pixel
   */
  inline Pixel(unsigned int nbChannels, const float* data);

  /**
   * Copy constructor
   * @param pixel : the pixel to copy.
   */
  inline Pixel(const Pixel& pixel);

  /**
   * Destructor
   */
  inline ~Pixel();

  /**
   * Affectation operator
   * @param pixel : the pixel to return
   * @return a reference on the current pixel
   */
  inline Pixel& operator=(const Pixel& pixel);

  /**
   * Return the number of channel of the pixel.
   * @return : the number of channel of the pixel.
   */  
  inline unsigned int numberOfChannel() const;

  /**
   * Return the raw data of this pixel
   * @return the raw data of this pixel
   */
  inline float* getRawData();

  /**
   * Return the raw data of this pixel
   * @return the raw data of this pixel
   */
  inline const float* getRawData() const;

  /**
   * Return the data of the k-th channel
   * @param k : the number of the wanted channel
   * @return : the data of the k-th channel
   */
  inline float& operator[](unsigned int k);

  /**
   * Return the data of the k-th channel
   * @param k : the number of the wanted channel
   * @return : the data of the k-th channel
   */
  inline const float& operator[](unsigned int k) const;

private :
  float* _data;
  unsigned int _nbChannels;
};

class Image{
public :
  /**
   * Constructor of image without initilization of the pixels colors.
   * (use clear to set all pixel to 0)
   */
  Image(unsigned int width, unsigned int height, unsigned int nbChannels);

  /**
   * Constructor
   */
  Image(unsigned int width, unsigned int height, unsigned int nbChannels, float* raster);

  /**
   * Destructor
   */
  ~Image();
  
  /**
   * Set all pixel to black (0, 0, 0)
   */
  void clear();
  
  /**
   * Return the width of the image.
   */
  unsigned int getWidth() const;

  /**
   * Return the height of the image.
   */
  unsigned int getHeight() const;

  /**
   * Return the number of channels of this image.
   */
  unsigned int getNumberOfChannels() const;

  /**
   * Set the name of a channel
   */
  void setChannelName(unsigned int channel, std::string name);

  /**
   * Get the name of a channel
   */
  std::string getChannelName(unsigned int channel);

  /**
   * Return a pointer on the raster of this image. May be null if the image is null.
   */
  float* getRaster();

  /**
   * Extract a pixel of the image.
   * Out of image pixel are black.
   * x,y : coordinate of the wanted pixel.
   * return : the array where will be stored the color of the pixel.
   */
  Pixel getPixel(int x, int y) const;

  /**
   * Put the color of the pixel into the pixel array. Use linear interpolation.
   * Out of image pixel are black.
   * x,y : coordinate of the wanted pixel.
   */
  Pixel getInterpolatedPixel(Real x, Real y) const;
 
  /**
   * Set the color of one pixel of the image.
   * x,y : coordinate of the pixel to set.
   * pixel : the array that containt the color to affect to the pixel.
   */
  void setPixel(int x, int y, const Pixel& pixel);

  //! @details Normalize each pixel of an image so that the sum of values on 
  //!  each channels belongs to [0 1]
  void normalize(void);

/*Renvoie le couple des valeurs de niveaux de gris minimales et maximales... 
  Cela présuppose que l'image est en niveaux de gris, c'est-à-dire que R=V=B et 
  donc que pixel[0] représente la valeur du niveau de gris*/  
  Point2D getMinAndMaxGreyLevel() const;

private :
  unsigned int _width;
  unsigned int _height;
  unsigned int _nbChannels;
  float* _map;
  std::vector<std::string> _channelsNames;
};

inline Pixel::Pixel(void)
    : _nbChannels(0),
      _data(NULL) { }

/**
 * Constructor (values are not initialised)
 * @param nbChannels : number of channels of this pixel
 */
inline Pixel::Pixel(unsigned int nbChannels)
: _nbChannels(nbChannels)
{
  _data = new float [nbChannels];
  for (unsigned int i; i < nbChannels; i++)
    _data [i] = 0.f;
}

/**
 * Constructor
 * @param nbChannels : number of channels of this pixel
 * @param data : a raw data used to initialize the Pixel
 */
inline Pixel::Pixel(unsigned int nbChannels, const float* data)
: _nbChannels(nbChannels)
{
  _data = new float [nbChannels];
  memcpy(_data, data, nbChannels*sizeof(float));
}

/**
 * Copy constructor
 * @param pixel : the pixel to copy.
 */
inline Pixel::Pixel(const Pixel& pixel)
: _nbChannels(pixel._nbChannels)
{
  _data = new float [_nbChannels];
  memcpy(_data, pixel._data, _nbChannels*sizeof(float));
}

/**
 * Destructor
 */
inline Pixel::~Pixel()
{
  delete[] _data;
}

/**
 * Affectation operator
 * @param pixel : the pixel to return
 * @return a reference on the current pixel
 */
inline Pixel& Pixel::operator=(const Pixel& pixel)
{
  delete[] _data;
  _nbChannels = pixel._nbChannels;
  _data = new float [_nbChannels];
  memcpy(_data, pixel._data, _nbChannels*sizeof(float)); 

  return *this;
}

/**
 * Return the number of channel of the pixel.
 * @return : the number of channel of the pixel.
 */  
inline unsigned int Pixel::numberOfChannel() const
{
  return _nbChannels;
}

/**
 * Return the raw data of this pixel
 * @return the raw data of this pixel
 */
inline float* Pixel::getRawData()
{
  return _data;
}

/**
 * Return the raw data of this pixel
 * @return the raw data of this pixel
 */
inline const float* Pixel::getRawData() const
{
  return _data;
}

/**
 * Return the data of the k-th channel
 * @param k : the number of the wanted channel
 * @return : the data of the k-th channel
 */
inline float& Pixel::operator[](unsigned int k)
{
  return _data[k];
}

/**
 * Return the data of the k-th channel
 * @param k : the number of the wanted channel
 * @return : the data of the k-th channel
 */
inline const float& Pixel::operator[](unsigned int k) const
{
  return _data[k];  
}

#endif //_RGBIMAGE_HPP
