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

#ifndef _BASIS_HPP
#define _BASIS_HPP

#include <common.hpp>
#include <maths/Vector.hpp>
#include <maths/Point.hpp>

class Basis{
public :
  Point o;
  Vector i;
  Vector j;
  Vector k;
  
  inline void getFromBasis(const Point& inBasis, Point& absolute) const;
  inline void getInBasis(const Point& absolute, Point& inBasis) const;
  inline void getFromBasis(const Vector& inBasis, Vector& absolute) const;
  inline void getInBasis(const Vector& absolute, Vector& inBasis) const;
};


inline void Basis::getFromBasis(const Point& inBasis, Point& absolute) const
{
  absolute[0] = o[0] + inBasis[0]*i[0] + inBasis[1]*j[0] + inBasis[2]*k[0];
  absolute[1] = o[1] + inBasis[0]*i[1] + inBasis[1]*j[1] + inBasis[2]*k[1];
  absolute[2] = o[2] + inBasis[0]*i[2] + inBasis[1]*j[2] + inBasis[2]*k[2];
}

inline void Basis::getInBasis(const Point& absolute, Point& inBasis) const
{
  inBasis[0] = (absolute[0]-o[0])*i[0] + (absolute[1]-o[1])*i[1] + (absolute[2]-o[2])*i[2];
  inBasis[1] = (absolute[0]-o[0])*j[0] + (absolute[1]-o[1])*j[1] + (absolute[2]-o[2])*j[2];
  inBasis[2] = (absolute[0]-o[0])*k[0] + (absolute[1]-o[1])*k[1] + (absolute[2]-o[2])*k[2];
}

inline void Basis::getFromBasis(const Vector& inBasis, Vector& absolute) const
{
  absolute[0] = inBasis[0]*i[0] + inBasis[1]*j[0] + inBasis[2]*k[0];
  absolute[1] = inBasis[0]*i[1] + inBasis[1]*j[1] + inBasis[2]*k[1];
  absolute[2] = inBasis[0]*i[2] + inBasis[1]*j[2] + inBasis[2]*k[2];  
}

inline void Basis::getInBasis(const Vector& absolute, Vector& inBasis) const
{
  inBasis[0] = absolute.dot(i);
  inBasis[1] = absolute.dot(j);
  inBasis[2] = absolute.dot(k);
}

#endif
