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
#include <camerashapes/FishEyeCameraShape.hpp>
 
/**
 * Constructor.
 * You must set the width and height of the image to compute.
 * width : the width of the image to compute
 * height : the height of the image to compute
 */
FishEyeCameraShape::FishEyeCameraShape(unsigned int width, unsigned int height, const Point& origin)
: CameraShape(width, height)
{
  // Computing the camera base
  _origin=origin;
}

/**
 * Build the ray corresponding to the given pixel coordinate.
 * Return false only if there is no ray to cast for the given pixel
 * (some shape dont process a rectangular image)
 * x,y : the coordinate of the pixel.
 * ray : we will return the ray into this parameter
 */
bool FishEyeCameraShape::getRay(unsigned int i, unsigned int j, Ray& ray)
{
  unsigned int maxdim = getHeight();
  if(getWidth()>maxdim)
    maxdim=getWidth();

  Real x = i - getWidth()*0.5;
  Real y = j - getHeight()*0.5;
  Real dist = std::sqrt((Real)(x*x + y*y)) / (Real)maxdim * 2.0;
  if(dist>1.0)
    return false;

  ray.o = _origin;

  ray.v[1] = sin(M_PI*dist)*x/(Real)maxdim;
  ray.v[2] = -sin(M_PI*dist)*y/(Real)maxdim;
  ray.v[0] = cos(M_PI*dist);
  ray.v.normalize();
  return true;
}
