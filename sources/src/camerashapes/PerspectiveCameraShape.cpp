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
#include <camerashapes/PerspectiveCameraShape.hpp>
 
/**
 * Constructor.
 * You must set the width and height of the image to compute.
 * width : the width of the image to compute
 * height : the height of the image to compute
 */
PerspectiveCameraShape::PerspectiveCameraShape(unsigned int width, unsigned int height, const Real& fov, const Point& origin, const Point& lookat, const Vector& up)
: CameraShape(width, height)
{
  // Computing the camera base
  _origin.o=origin;
  _origin.k=up;
  _origin.i=Vector(origin, lookat);
  _origin.k.vect(_origin.i, _origin.j);
  _origin.i.vect(_origin.j, _origin.k);
  _origin.i.normalize();
  _origin.j.normalize();
  _origin.k.normalize();
  
  // Computing the plane distance
  _distance = 0.5/(tan(fov*0.5*M_PI/180.0));
}

/**
 * Build the ray corresponding to the given pixel coordinate.
 * Return false only if there is no ray to cast for the given pixel
 * (some shape dont process a rectangular image)
 * x,y : the coordinate of the pixel.
 * ray : we will return the ray into this parameter
 */
bool PerspectiveCameraShape::getRay(unsigned int x, unsigned int y, Ray& ray)
{
  Vector localray;
  localray[0] = _distance;
  localray[1] = (getWidth()/2.0 - x)/(Real)getHeight();
  localray[2] = 0.5 - y/(Real)getHeight();

  localray.normalize();

  _origin.getFromBasis(localray, ray.v);
  ray.v.normalize();
  ray.o=_origin.o;
  return true;
}
