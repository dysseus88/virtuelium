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
#include <camerashapes/PolarCameraShape.hpp>
 
/**
 * Constructor.
 * You must set the width and height of the image to compute.
 * width : the width of the image to compute
 * height : the height of the image to compute
 */
PolarCameraShape::PolarCameraShape(unsigned int width, unsigned int height, const Point& origin)
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
bool PolarCameraShape::getRay(unsigned int i, unsigned int j, Ray& ray)
{
  Real phi = 2 * M_PI * (i/(Real)getWidth() - 0.5);
  Real theta = M_PI * (j/(Real)getHeight());

  ray.o = _origin;

  ray.v[0] = cos(phi)*sin(theta);
  ray.v[1] = sin(phi)*sin(theta);
  ray.v[2] = cos(theta);
  ray.v.normalize();
  return true;
}
