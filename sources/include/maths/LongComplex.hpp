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

#ifndef _LONGCOMPLEX_HPP
#define _LONGCOMPLEX_HPP

#include <common.hpp>

class LongComplex{
public :
  LongReal re;
  LongReal im;

  /**
   * Default constructor
   */
  inline LongComplex();

  /**
   * Constructor
   * @param re : LongReal part of the LongComplex number
   * @param im : imaginary part of the LongComplex number
   */
  inline LongComplex(const LongReal& re, const LongReal& im);

  /**
   * Copy Constructor
   */
  inline LongComplex(const LongComplex& c);

  /**
   * Set the value of the LongComplex number with trigonometric representation
   * @param re : LongReal part of the LongComplex number
   * @param im : imaginary part of the LongComplex number
   */
  inline void setFromTrigo(const LongReal& rho, const LongReal& theta);

  /**
   * Return the conjugate of this LongComplex number
   */
  inline LongComplex conjugate() const;
  
  /**
   * Return the module of this LongComplex number
   */
  inline LongReal abs() const;

  /**
   * Return the square of the module of this LongComplex number
   */
  inline LongReal squareAbs() const;
  
  /**
   * Return the argument of this LongComplex number
   */
  inline LongReal arg() const;

  /**
   * Return the square root of this LongComplex number
   */
  inline LongComplex sqrt() const;

  /**
   * Return the cosinus of this LongComplex number
   */
  inline LongComplex cos() const;

  /**
   * Return the sinus of this LongComplex number
   */
  inline LongComplex sin() const;

  static inline LongComplex i();
};

/**
 * Operators
 */
inline LongComplex operator+(const LongReal& op1, const LongComplex& op2);
inline LongComplex operator+(const LongComplex& op1, const LongComplex& op2);
inline LongComplex operator+(const LongComplex& op1, const LongReal& op2);
inline LongComplex operator-(const LongReal& op1, const LongComplex& op2);
inline LongComplex operator-(const LongComplex& op1, const LongComplex& op2);
inline LongComplex operator-(const LongComplex& op1, const LongReal& op2);
inline LongComplex operator*(const LongReal& op1, const LongComplex& op2);
inline LongComplex operator*(const LongComplex& op1, const LongComplex& op2);
inline LongComplex operator*(const LongComplex& op1, const LongReal& op2);
inline LongComplex operator/(const LongReal& op1, const LongComplex& op2);
inline LongComplex operator/(const LongComplex& op1, const LongComplex& op2);
inline LongComplex operator/(const LongComplex& op1, const LongReal& op2);

/**
 * Default constructor
 */
inline LongComplex::LongComplex()
{
  //Nothing to do
}

/**
 * Constructor
 * @param re : LongReal part of the LongComplex number
 * @param im : imaginary part of the LongComplex number
 */
inline LongComplex::LongComplex(const LongReal& re, const LongReal& im)
: re(re), im(im)
{
  //nothing to do more
}

/**
 * Copy Constructor
 */
inline LongComplex::LongComplex(const LongComplex& c)
: re(c.re), im(c.im)
{
  //nothing to do more
}

/**
 * Set the value of the LongComplex number with trigonometric representation
 * @param re : LongReal part of the LongComplex number
 * @param im : imaginary part of the LongComplex number
 */
inline void LongComplex::setFromTrigo(const LongReal& rho, const LongReal& theta)
{
  re = rho*std::cos(theta);
  im = rho*std::sin(theta);
}

/**
 * Return the conjugate of this LongComplex number
 */
inline LongComplex LongComplex::conjugate() const
{
  return LongComplex(re, -im);
}

/**
 * Return the square of the module of this LongComplex number
 */
inline LongReal LongComplex::squareAbs() const
{
  return re*re + im*im;
}

/**
 * Return the module of this LongComplex number
 */
inline LongReal LongComplex::abs() const
{
  return std::sqrt(squareAbs());
}

/**
 * Return the argument of this LongComplex number
 */
inline LongReal LongComplex::arg() const
{
  return std::atan2(im, re);
}

/**
 * Return the square root of this LongComplex number
 */
inline LongComplex LongComplex::sqrt() const
{
  LongComplex result;
  result.setFromTrigo(std::sqrt(abs()), arg()*0.5);
  return result;
}

/**
 * Return the cosinus of this LongComplex number
 */
inline LongComplex LongComplex::cos() const
{
  return LongComplex(
    std::cos(re) * std::cosh(im),
   -std::sin(re) * std::sinh(im)
  );
}

/**
 * Return the sinus of this LongComplex number
 */
inline LongComplex LongComplex::sin() const
{
  return LongComplex(
    std::sin(re) * std::cosh(im),
   -std::cos(re) * std::sinh(im)
  );
}

/**
 * Operator
 */
inline LongComplex operator+(const LongReal& op1, const LongComplex& op2)
{
  return LongComplex(op1+op2.re, op2.im);
}

inline LongComplex operator+(const LongComplex& op1, const LongComplex& op2)
{
  return LongComplex(op1.re+op2.re, op1.im+op2.im);
}

inline LongComplex operator+(const LongComplex& op1, const LongReal& op2)
{
  return LongComplex(op1.re+op2, op1.im);
}

inline LongComplex operator-(const LongReal& op1, const LongComplex& op2)
{
  return LongComplex(op1-op2.re, op2.im);
}

inline LongComplex operator-(const LongComplex& op1, const LongComplex& op2)
{
  return LongComplex(op1.re-op2.re, op1.im-op2.im);
}

inline LongComplex operator-(const LongComplex& op1, const LongReal& op2)
{
  return LongComplex(op1.re-op2, op1.im);
}

inline LongComplex operator*(const LongReal& op1, const LongComplex& op2)
{
  return LongComplex(op1*op2.re, op1*op2.im);
}

inline LongComplex operator*(const LongComplex& op1, const LongComplex& op2)
{
  return LongComplex(op1.re*op2.re - op1.im*op2.im, op1.re*op2.im + op1.im*op2.re);
}

inline LongComplex operator*(const LongComplex& op1, const LongReal& op2)
{
  return LongComplex(op1.re*op2, op1.im*op2);
}

inline LongComplex operator/(const LongReal& op1, const LongComplex& op2)
{
  return LongComplex(
    (op1*op2.re) / (op2.re*op2.re + op2.im*op2.im),
    (-op1*op2.im) / (op2.re*op2.re + op2.im*op2.im)
  );
}

inline LongComplex operator/(const LongComplex& op1, const LongComplex& op2)
{
  return LongComplex(
    (op1.re*op2.re + op1.im*op2.im) / (op2.re*op2.re + op2.im*op2.im),
    (op1.im*op2.re - op1.re*op2.im) / (op2.re*op2.re + op2.im*op2.im)
  );
}

inline LongComplex operator/(const LongComplex& op1, const LongReal& op2)
{
  return LongComplex(op1.re/op2, op1.im/op2);
}

inline LongComplex LongComplex::i(){
  return LongComplex(0.0, 1.0);
}

#endif //LONGCOMPLEX
