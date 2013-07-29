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
#ifndef _PERSPECTIVECAMERASHAPE_HPP
#define _PERSPECTIVECAMERASHAPE_HPP

#include <camerashapes/CameraShape.hpp>
#include <core/3DBase.hpp>

/**
 * This is an implemntation of the camera shape. This is a perspective camera.
 * It's defined by 4 parameters :
 * - An origine from where all the ray will be cast
 * - An aiming point (it will be in the center of the final image)
 * - An opening angle (field of view angle). It is the vertical angle.
 * - An approximated vector that point to the up direction.
 *
 * @author Sylvain "Greewi" Dumazet
 */
class PerspectiveCameraShape : public CameraShape{
public :
  /**
   * Constructor.
   * @param width : the width of the image to compute
   * @param height : the height of the image to compute
   * @param start :  the starting row of the image to compute
   * @param end : the ending row of the image to compute
   * @param fov : the vertical field of view angle
   * @param origin : the position of the camera. All ray will be casted from this point
   * @param lookat : the aimed point.
   * @param up : a vector that point to the up of this camera. It can be approximated.
   */
  PerspectiveCameraShape(unsigned int width, unsigned int height, const Real& fov, const Point& origin, const Point& lookat, const Vector& up);

  /**
   * Virtual destructor.
   */
  virtual inline ~PerspectiveCameraShape();
  
  /**
   * Build the ray corresponding to the given pixel coordinate.
   * @return false only if there is no ray to cast for the given pixel
   * (some shape dont process a rectangular image)
   * @param x,y : the coordinate of the pixel.
   * @param ray : we will return the ray into this parameter
   */
  virtual bool getRay(unsigned int x, unsigned int y, Ray& ray);
  
private :
  Real _distance;
  Basis _origin;
};

/**
 * Virtual destructor.
 */
inline PerspectiveCameraShape::~PerspectiveCameraShape()
{
  //  Nothing to do !
}

#endif //_PERSPECTIVECAMERASHAPE_HPP
