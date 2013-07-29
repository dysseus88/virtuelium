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

#ifndef _VECTOR_HPP
#define _VECTOR_HPP

#include <common.hpp>

class Point;
class Basis;

class Vector{
public :  
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
  inline Vector(){};
  inline Vector(const Real& x, const Real& y, const Real& z);
  inline Vector(const Real coord[3]);
  inline Vector(const Point& from, const Point& to);
  
  // Operators *************************************************
  //! @brief Overloading unary operator '-'
  inline Vector operator-(void);

  /**
   * Set the value of this vector as the sum of the two other vectors
   */
  inline void setsum(const Vector& vector1, const Vector& vector2);

  /**
   * Multiply this vector by the given factor.
   */
  inline void mul(const Real& factor);
 
  /**
   * Return the scalar product beetwen the current vector and an other vector.
   */
  inline Real dot(const Vector& vector) const;

  /**
   * Return the vectorial product beetwen the current vector and an other vector.
   */
  inline Vector vect(const Vector& vector) const;

  /**
   * Compute the vectorial product beetwen the current vector and an other vector
   * and place it into the result variable.
   */
  inline void vect(const Vector& vector, Vector& result) const;
  
  /**
   * Return the square of the norm of this vector
   */
  inline Real square() const;
  
  /**
   * return the norm of this vector
   */
  inline Real norm() const;

  /**
   * Normalise the vector
   */
  inline void normalize();

  /**
   * Project this point into the given basis.
   */
  inline void projectIntoBasis(const Basis& b);

  /**
   * Unproject this point from the given basis.
   */
  inline void unprojectFromBasis(const Basis& b);   

private :
  Real coord[3];
};

#include <maths/Point.hpp>
#include <maths/Basis.hpp>

inline Vector::Vector(const Real& x, const Real& y, const Real& z)
{
  coord[0]=x;
  coord[1]=y;
  coord[2]=z;
}

inline Vector::Vector(const Real coord[3])
{
  this->coord[0]=coord[0];
  this->coord[1]=coord[1];
  this->coord[2]=coord[2];
}

/**
 * Constructor of the vector that go from a point to an other
 * from : the starting point
 * to : the ending point
 */
inline Vector::Vector(const Point& from, const Point& to)
{
  for(int i=0; i<3; i++)
    coord[i]=to[i]-from[i];
}


inline Vector Vector::operator-(void) {
  Vector result;
  result.coord[0] = static_cast<Real>(-1.0 * coord[0]);
  result.coord[1] = static_cast<Real>(-1.0 * coord[1]);
  result.coord[2] = static_cast<Real>(-1.0 * coord[2]);
  return result;
}

/**
 * Set the value of this vector as the sum of the two other vectors
 */
inline void Vector::setsum(const Vector& vector1, const Vector& vector2)
{
  coord[0] = vector1[0] + vector2[0];
  coord[1] = vector1[1] + vector2[1];
  coord[2] = vector1[2] + vector2[2];
}

/**
 * Multiply this vector by the given factor.
 */
inline void Vector::mul(const Real& factor)
{
  coord[0] *= factor;
  coord[1] *= factor;
  coord[2] *= factor;
}

/**
 * Return the ccalar product beetwen the current vector and an other vector.
 */
inline Real Vector::dot(const Vector& vector) const
{
  return coord[0]*vector[0] + coord[1]*vector[1] + coord[2]*vector[2];
}

/**
 * Return the vectorial product beetwen the current vector and an other vector.
 */
inline Vector Vector::vect(const Vector& vector) const
{
  Vector result;
  vect(vector, result);
  return result;
}

/**
 * Compute the vectorial product beetwen the current vector and an other vector
 * and place it into the result variable.
 */
inline void Vector::vect(const Vector& vector, Vector& result) const
{
  result[0] = coord[1]*vector[2] - coord[2]*vector[1];
  result[1] = coord[2]*vector[0] - coord[0]*vector[2];
  result[2] = coord[0]*vector[1] - coord[1]*vector[0];
}

/**
 * Return the square of the norm of this vector
 */
inline Real Vector::square() const
{
  return coord[0]*coord[0] + coord[1]*coord[1] + coord[2]*coord[2];
}
  
/**
 * return the norm of this vector
 */
inline Real Vector::norm() const
{
  return std::sqrt(coord[0]*coord[0] + coord[1]*coord[1] + coord[2]*coord[2]);  
}

/**
 * Normalise the vector
 */
inline void Vector::normalize()
{
  Real scale = Real(1.0)/norm();
  for(int i=0; i<3; i++)
    coord[i]*=scale;
}

/**
 * Project this point into the given basis.
 */
inline void Vector::projectIntoBasis(const Basis& b)
{
  Real newcoord[3];
  newcoord[0] = this->dot(b.i);
  newcoord[1] = this->dot(b.j);
  newcoord[2] = this->dot(b.k);
  coord[0]=newcoord[0];
  coord[1]=newcoord[1];
  coord[2]=newcoord[2];
}

/**
 * Unproject this point from the given basis.
 */
inline void Vector::unprojectFromBasis(const Basis& b)
{
  Real newcoord[3];
  newcoord[0] = coord[0]*b.i[0] + coord[1]*b.j[0] + coord[2]*b.k[0];
  newcoord[1] = coord[0]*b.i[1] + coord[1]*b.j[1] + coord[2]*b.k[1];
  newcoord[2] = coord[0]*b.i[2] + coord[1]*b.j[2] + coord[2]*b.k[2];
  coord[0]=newcoord[0];
  coord[1]=newcoord[1];
  coord[2]=newcoord[2];
}

#endif
