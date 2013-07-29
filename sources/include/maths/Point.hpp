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

#ifndef _POINT_HPP
#define _POINT_HPP

#include <common.hpp>

class Vector;
class Ray;
class Basis;

class Point{
public :
  Real coord[3];

  //Accessor ***************************************************
  inline Real& x(){return coord[0];}
  inline Real x() const {return coord[0];}
  inline Real& y(){return coord[1];}
  inline Real y() const {return coord[1];}
  inline Real& z(){return coord[2];}
  inline Real z() const {return coord[2];}
  inline Real& operator[](int i){return coord[i];}
  inline Real operator[](int i) const {return coord[i];}

  // Constructors **********************************************
  inline Point(){}
  inline Point(const Real& x, const Real& y, const Real& z);
  inline Point(const Ray& ray, const Real& distance);
  // Operators *************************************************
  
  /**
   * Translate the point with the given vector
   */
  inline void translate(const Vector& vector);
  
  /**
   * Project this point into the given basis.
   */
  inline void projectIntoBasis(const Basis& b);

  /**
   * Unproject this point from the given basis.
   */
  inline void unprojectFromBasis(const Basis& b);
};

#include <maths/Vector.hpp>
#include <maths/Ray.hpp>
#include <maths/Basis.hpp>

inline Point::Point(const Real& x, const Real& y, const Real& z)
{
  coord[0]=x;
  coord[1]=y;
  coord[2]=z;
}

inline Point::Point(const Ray& ray, const Real& distance)
{
  coord[0]=ray.o[0] + ray.v[0]*distance;
  coord[1]=ray.o[1] + ray.v[1]*distance;
  coord[2]=ray.o[2] + ray.v[2]*distance;
}

/**
 * Translate the point with the given vector
 */
inline void Point::translate(const Vector& vector)
{
  coord[0]+=vector[0];
  coord[1]+=vector[1];
  coord[2]+=vector[2];
}

/**
 * Project this point into the given basis.
 */
inline void Point::projectIntoBasis(const Basis& b)
{
  Real newcoord[3];
  newcoord[0] = (coord[0]-b.o[0])*b.i[0] + (coord[1]-b.o[1])*b.i[1] + (coord[2]-b.o[2])*b.i[2];
  newcoord[1] = (coord[0]-b.o[0])*b.j[0] + (coord[1]-b.o[1])*b.j[1] + (coord[2]-b.o[2])*b.j[2];
  newcoord[2] = (coord[0]-b.o[0])*b.k[0] + (coord[1]-b.o[1])*b.k[1] + (coord[2]-b.o[2])*b.k[2];
  coord[0]=newcoord[0];
  coord[1]=newcoord[1];
  coord[2]=newcoord[2];
}

/**
 * Unproject this point from the given basis.
 */
inline void Point::unprojectFromBasis(const Basis& b)
{
  Real newcoord[3];
  newcoord[0] = b.o[0] + coord[0]*b.i[0] + coord[1]*b.j[0] + coord[2]*b.k[0];
  newcoord[1] = b.o[1] + coord[0]*b.i[1] + coord[1]*b.j[1] + coord[2]*b.k[1];
  newcoord[2] = b.o[2] + coord[0]*b.i[2] + coord[1]*b.j[2] + coord[2]*b.k[2];
  coord[0]=newcoord[0];
  coord[1]=newcoord[1];
  coord[2]=newcoord[2];
}

#endif
