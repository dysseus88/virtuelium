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
#ifndef _CAMERASHAPE_HPP
#define _CAMERASHAPE_HPP

#include <core/3DBase.hpp>

/**
 * This abstract class defined the camera interface. A camera shape is an object
 * that will define the ray to cast and recompose the final image. It is the
 * object that do the correspondance between the casted rays and pixels.
 * @author Sylvain "Greewi" Dumazet
 */
class CameraShape{
public :
  /**
   * Constructor.
   * You must set the width and height of the image to compute.
   * @param width : the width of the image to compute
   * @param height : the height of the image to compute
   */
  inline CameraShape(unsigned int width, unsigned int height);

  /**
   * Virtual destructor.
   */
  virtual inline ~CameraShape();
  
  /**
   * Build the ray corresponding to the given pixel coordinate.
   * @return false only if there is no ray to cast for the given pixel
   * (some shape dont process a rectangular image)
   * @param x,y : the coordinate of the pixel.
   * @param ray : we will return the ray into this parameter
   */
  virtual bool getRay(unsigned int x, unsigned int y, Ray& ray)=0;
  
  /**
   * Return the width of the computable image.
   * @return the width of the computable image.
   */
  inline unsigned int getWidth();
  
  /**
   * Return the height of the computable image.
   * @return the height of the computable image.
   */
  inline unsigned int getHeight();

private :
  unsigned int _width;  
  unsigned int _height;
};

/**
 * Constructor.
 * You must set the width and height of the image to compute.
 * @param width : the width of the image to compute
 * @param height : the height of the image to compute
 * @param start :  the starting row of the image to compute
 * @param end : the ending row of the image to compute
 */
inline CameraShape::CameraShape(unsigned int width, unsigned int height)
: _width(width), _height(height)
{
  // Nothing to do more.
}

/**
 * Virtual destructor.
 */
inline CameraShape::~CameraShape()
{
  //  Nothing to do !
}

/**
 * Return the width of the computable image.
 */
inline unsigned int CameraShape::getWidth()
{
  return _width;
}
  
/**
 * Return the height of the computable image.
 */
inline unsigned int CameraShape::getHeight()
{
  return _height;
}
  
#endif //_CAMERASHAPE_HPP
