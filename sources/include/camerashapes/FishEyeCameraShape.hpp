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
#ifndef _FISHEYECAMERASHAPE_HPP
#define _FISHEYECAMERASHAPE_HPP

#include <camerashapes/CameraShape.hpp>
#include <core/3DBase.hpp>

/**
 * This is an implemntation of a fisheye shape.
 * It's defined by 1 parameter : The origine point of the camera.
 *
 * @author Sylvain "Greewi" Dumazet
 */
class FishEyeCameraShape : public CameraShape{
public :
  /**
   * Constructor.
   * @param width : the width of the image to compute
   * @param height : the height of the image to compute
   * @param start :  the starting row of the image to compute
   * @param end : the ending row of the image to compute
   * @param origin : the position of the camera. All ray will be casted from this point
   */
  FishEyeCameraShape(unsigned int width, unsigned int height, const Point& origin);

  /**
   * Virtual destructor.
   */
  virtual inline ~FishEyeCameraShape();
  
  /**
   * Build the ray corresponding to the given pixel coordinate.
   * @return false only if there is no ray to cast for the given pixel
   * (some shape dont process a rectangular image)
   * @param x,y : the coordinate of the pixel.
   * @param ray : we will return the ray into this parameter
   */
  virtual bool getRay(unsigned int x, unsigned int y, Ray& ray);
  
private :
  Point _origin;
};

/**
 * Virtual destructor.
 */
inline FishEyeCameraShape::~FishEyeCameraShape()
{
  //  Nothing to do !
}

#endif //_FISHEYECAMERASHAPE_HPP
