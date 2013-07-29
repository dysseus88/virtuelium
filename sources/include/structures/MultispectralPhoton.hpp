/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _MULTISPECTRALPHOTON_HPP
#define _MULTISPECTRALPHOTON_HPP

#include <core/3DBase.hpp>
#include <core/LightBase.hpp>


class MultispectralPhoton{
public :
  Real radiance[81]; //Index of the photon's wavelenght in the GlobalSpectrum
  Point position;    //Position of the photon
  Vector direction;  //Propagation direction of the photon
  Vector normal;     //DIrection of the normal to the surface
  Real distance;     //Distance from the last intersection of the photon
};

#endif //_MULTISPECTRALPHOTON_HPP
