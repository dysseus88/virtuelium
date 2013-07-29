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

#ifndef _POINT2D_HPP
#define _POINT2D_HPP

#include <common.hpp>

class Point2D{
public :
  inline Point2D(){coord[0] = 0; coord[1] = 0;}
  inline Point2D(const Real& x, const Real& y){coord[0]=x; coord[1]=y;}
  inline Point2D(const Point2D& p){coord[0]=p.coord[0]; coord[1]=p.coord[1];}

  Real coord[2];

  //Accessor ***************************************************
  inline Real& x(){return coord[0];}
  inline Real x() const {return coord[0];}
  inline Real& y(){return coord[1];}
  inline Real y() const {return coord[1];}
  inline Real& u(){return coord[0];}
  inline Real u() const {return coord[0];}
  inline Real& v(){return coord[1];}
  inline Real v() const {return coord[1];}
  inline Real& operator[](int i){return coord[i];}
  inline Real operator[](int i) const {return coord[i];}
};

#endif
