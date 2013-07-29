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
 
#include <structures/Image.hpp>

/**
 * Constructor of image without initilization of the pixels colors.
 * (use clear to set all pixel to 0)
 */
Image::Image(unsigned int width, unsigned int height, unsigned int nbChannels)
: _width(width), _height(height), _nbChannels(nbChannels), _channelsNames(nbChannels)
{
  _map = new float[_width*_height*_nbChannels];
}

/**
 * Constructor of image without initilization of the pixels colors.
 * (use clear to set all pixel to 0)
 */
Image::Image(unsigned int width, unsigned int height, unsigned int nbChannels, float* raster)
: _width(width), _height(height), _nbChannels(nbChannels), _map(raster), _channelsNames(nbChannels)
{
  if(_map==0)
    _map = new float[_width*_height*_nbChannels];
}

/**
 * Destructor
 */
Image::~Image()
{
  delete[] _map;
}

/**
 * Set all pixel to black (0, 0, 0)
 */
void Image::clear()
{
  memset(_map, 0, _width*_height*_nbChannels*sizeof(float));
}

/**
 * Return the width of the image.
 */
unsigned int Image::getWidth() const
{
  return _width;
}

/**
 * Return the height of the image.
 */
unsigned int Image::getHeight() const
{
  return _height;
}

/**
 * Return the number of channels of this image.
 */
unsigned int Image::getNumberOfChannels() const
{
  return _nbChannels;
}

/**
 * Set the name of a channel
 */
void Image::setChannelName(unsigned int channel, std::string name)
{
  _channelsNames[channel]=name;
}

/**
 * Get the name of a channel
 */
std::string Image::getChannelName(unsigned int channel)
{
  return _channelsNames[channel];  
}

/**
 * Return a pointer on the raster of this image. May be null if the image is null.
 */
float* Image::getRaster()
{
  return _map;
}

/**
 * Put the color of the pixel into the pixel array.
 * Out of image pixel are black.
 * x,y : coordinate of the wanted pixel.
 * return : the extracted pixel.
 */
Pixel Image::getPixel(int x, int y) const
{
  if(x<0) x=0;
  if(y<0) y=0;
  if((unsigned int)x>=_width) x=_width-1;
  if((unsigned int)y>=_height) y=_height-1;
  float* data = &_map[((y*_width) + x)*_nbChannels];
  return Pixel(_nbChannels, data);
}

/**
 * Set the color of one pixel of the image.
 * x,y : coordinate of the pixel to set.
 * pixel : the array that containt the color to affect to the pixel.
 */
void Image::setPixel(int x, int y, const Pixel& pixel)
{
  if(x<0) x=0;
  if(y<0) y=0;
  if((unsigned int)x>=_width) x=_width-1;
  if((unsigned int)y>=_height) y=_height-1;
  memcpy(&_map[((y*_width) + x)*_nbChannels], pixel.getRawData(), _nbChannels*sizeof(float));
}

/**
 * Put the color of the pixel into the pixel array. Use linear interpolation.
 * Out of image pixel are black.
 * x,y : coordinate of the wanted pixel.
 */
Pixel Image::getInterpolatedPixel(Real x, Real y) const
{
  Pixel pixel(_nbChannels);

  int x1 = (int) x;
  int x2 = x1 + 1 ;
  int y1 = (int) y;
  int y2 = y1 + 1;

  Real xFactor1 = 1.0 - x + x1;
  Real xFactor2 = 1.0 - xFactor1;
  Real yFactor1 = 1.0 - y + y1;
  Real yFactor2 = 1.0 - yFactor1;

  Pixel tmp = getPixel(x1, y1);
  for(unsigned int i=0; i<_nbChannels; i++)
    pixel[i]=tmp[i]*xFactor1*yFactor1;

  tmp = getPixel(x1, y2);
  for(unsigned int i=0; i<_nbChannels; i++)
    pixel[i]+=tmp[i]*xFactor1*yFactor2;

  tmp = getPixel(x2, y1);
  for(unsigned int i=0; i<_nbChannels; i++)
    pixel[i]+=tmp[i]*xFactor2*yFactor1;

  tmp = getPixel(x2, y2);
  for(unsigned int i=0; i<_nbChannels; i++)
    pixel[i]+=tmp[i]*xFactor2*yFactor2;

  return pixel;
}

/*Renvoie le couple des valeurs de niveaux de gris minimales et maximales... 
  Cela présuppose que l'image est en niveaux de gris, c'est-à-dire que R=V=B et 
  donc que pixel[0] représente la valeur du niveau de gris*/  
Point2D Image::getMinAndMaxGreyLevel() const {
	float min(getPixel(0,0)[0]),max(getPixel(0,0)[0]);
	Point2D result;
	float tmp;
	for (unsigned int x=0;x<_width;x++) {
		for (unsigned int y=0;y<_height;y++) {
			tmp=getPixel(x,y)[0];
			if (tmp>max) {max=tmp;}
			else if (tmp<min) {min=tmp;}
		}
	}
	result.coord[0]=min;
	result.coord[1]=max;
	return result;
}

void Image::normalize(void) {
  if (_map == NULL)
    return;

  float* pixel_data = new float[_nbChannels];

  for (unsigned int i = 0; i < _width * _height; i += _nbChannels) {
    float sum_channels = 0.f;
    for (unsigned int c = 0; c <_nbChannels; c++) {
      pixel_data[c] = _map[i + c];
      sum_channels += _map[i + c];
    }
    if (sum_channels != 0) {
      for (unsigned int c = 0; c <_nbChannels; c++) {
         _map[i + c] = pixel_data[c] / sum_channels;
      }
    }
  }

  if (pixel_data != NULL)
    delete[] pixel_data;
  pixel_data = NULL;
}