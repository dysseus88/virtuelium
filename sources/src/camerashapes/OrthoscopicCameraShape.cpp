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
#include <camerashapes/OrthoscopicCameraShape.hpp>
 
  /**
   * Constructor.
   * @param resx : the width of the image to compute
   * @param resy : the height of the image to compute
   * @param height : the vertical height of the camera in the scenery unit
   * @param origin : the position of the camera. All ray will be casted from this point
   * @param lookat : the aimed point.
   * @param up : a vector that point to the up of this camera. It can be approximated.
   */
OrthoscopicCameraShape::OrthoscopicCameraShape(unsigned int resx, unsigned int resy, const Real& height, const Point& origin, const Point& lookat, const Vector& up)
: CameraShape(resx, resy)
{
  // Computing the camera base
  _origin.o=origin;
  _origin.i=up;
  _origin.k=Vector(origin, lookat);
  _origin.k.vect(_origin.i, _origin.j);
  _origin.j.vect(_origin.k, _origin.i);
  _origin.i.normalize();
  _origin.j.normalize();
  _origin.k.normalize();
  
  // Computing the plane distance
  _rheight = height;
  _rwidth  = height*resx/(Real)resy;
}

/**
 * Build the ray corresponding to the given pixel coordinate.
 * Return false only if there is no ray to cast for the given pixel
 * (some shape dont process a rectangular image)
 * x,y : the coordinate of the pixel.
 * ray : we will return the ray into this parameter
 */
bool OrthoscopicCameraShape::getRay(unsigned int x, unsigned int y, Ray& ray)
{
  Real rx = (x/(Real)getWidth() - 0.5) *_rwidth;
  Real ry = (y/(Real)getHeight() - 0.5) *_rheight;

  ray.v = _origin.k;
  ray.o = _origin.o;
  ray.o[0] += _origin.i[0]*ry +  _origin.j[0]*rx;
  ray.o[1] += _origin.i[1]*ry +  _origin.j[1]*rx;
  ray.o[2] += _origin.i[2]*ry +  _origin.j[2]*rx;

  ray.v.normalize();

  return true;
}
