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

#ifndef _COMPLEX_HPP
#define _COMPLEX_HPP

#include <common.hpp>

class Complex{
public :
  Real re;
  Real im;

  /**
   * Default constructor
   */
  inline Complex();

  /**
   * Constructor
   * @param re : real part of the complex number
   * @param im : imaginary part of the complex number
   */
  inline Complex(const Real& re, const Real& im);

  /**
   * Copy Constructor
   */
  inline Complex(const Complex& c);

  /**
   * Set the value of the complex number with trigonometric representation
   * @param re : real part of the complex number
   * @param im : imaginary part of the complex number
   */
  inline void setFromTrigo(const Real& rho, const Real& theta);

  /**
   * Return the conjugate of this complex number
   */
  inline Complex conjugate() const;
  
  /**
   * Return the module of this complex number
   */
  inline Real abs() const;

  /**
   * Return the square of the module of this complex number
   */
  inline Real squareAbs() const;
  
  /**
   * Return the argument of this complex number
   */
  inline Real arg() const;

  /**
   * Return the square root of this complex number
   */
  inline Complex sqrt() const;

  /**
   * Return the cosinus of this complex number
   */
  inline Complex cos() const;

  /**
   * Return the sinus of this complex number
   */
  inline Complex sin() const;

  static inline Complex i();
};

/**
 * Operators
 */
inline Complex operator+(const Real& op1, const Complex& op2);
inline Complex operator+(const Complex& op1, const Complex& op2);
inline Complex operator+(const Complex& op1, const Real& op2);
inline Complex operator-(const Real& op1, const Complex& op2);
inline Complex operator-(const Complex& op1, const Complex& op2);
inline Complex operator-(const Complex& op1, const Real& op2);
inline Complex operator*(const Real& op1, const Complex& op2);
inline Complex operator*(const Complex& op1, const Complex& op2);
inline Complex operator*(const Complex& op1, const Real& op2);
inline Complex operator/(const Real& op1, const Complex& op2);
inline Complex operator/(const Complex& op1, const Complex& op2);
inline Complex operator/(const Complex& op1, const Real& op2);

/**
 * Default constructor
 */
inline Complex::Complex()
{
  //Nothing to do
}

/**
 * Constructor
 * @param re : real part of the complex number
 * @param im : imaginary part of the complex number
 */
inline Complex::Complex(const Real& re, const Real& im)
: re(re), im(im)
{
  //nothing to do more
}

/**
 * Copy Constructor
 */
inline Complex::Complex(const Complex& c)
: re(c.re), im(c.im)
{
  //nothing to do more
}

/**
 * Set the value of the complex number with trigonometric representation
 * @param re : real part of the complex number
 * @param im : imaginary part of the complex number
 */
inline void Complex::setFromTrigo(const Real& rho, const Real& theta)
{
  re = rho*std::cos(theta);
  im = rho*std::sin(theta);
}

/**
 * Return the conjugate of this complex number
 */
inline Complex Complex::conjugate() const
{
  return Complex(re, -im);
}

/**
 * Return the square of the module of this complex number
 */
inline Real Complex::squareAbs() const
{
  return re*re + im*im;
}

/**
 * Return the module of this complex number
 */
inline Real Complex::abs() const
{
  return std::sqrt(squareAbs());
}

/**
 * Return the argument of this complex number
 */
inline Real Complex::arg() const
{
  return std::atan2(im, re);
}

/**
 * Return the square root of this complex number
 */
inline Complex Complex::sqrt() const
{
  Complex result;
  result.setFromTrigo(std::sqrt(abs()), arg() * Real(0.5));
  return result;
}

/**
 * Return the cosinus of this complex number
 */
inline Complex Complex::cos() const
{
  return Complex(
    std::cos(re) * std::cosh(im),
   -std::sin(re) * std::sinh(im)
  );
}

/**
 * Return the sinus of this complex number
 */
inline Complex Complex::sin() const
{
  return Complex(
    std::sin(re) * std::cosh(im),
   -std::cos(re) * std::sinh(im)
  );
}

/**
 * Operator
 */
inline Complex operator+(const Real& op1, const Complex& op2)
{
  return Complex(op1+op2.re, op2.im);
}

inline Complex operator+(const Complex& op1, const Complex& op2)
{
  return Complex(op1.re+op2.re, op1.im+op2.im);
}

inline Complex operator+(const Complex& op1, const Real& op2)
{
  return Complex(op1.re+op2, op1.im);
}

inline Complex operator-(const Real& op1, const Complex& op2)
{
  return Complex(op1-op2.re, op2.im);
}

inline Complex operator-(const Complex& op1, const Complex& op2)
{
  return Complex(op1.re-op2.re, op1.im-op2.im);
}

inline Complex operator-(const Complex& op1, const Real& op2)
{
  return Complex(op1.re-op2, op1.im);
}

inline Complex operator*(const Real& op1, const Complex& op2)
{
  return Complex(op1*op2.re, op1*op2.im);
}

inline Complex operator*(const Complex& op1, const Complex& op2)
{
  return Complex(op1.re*op2.re - op1.im*op2.im, op1.re*op2.im + op1.im*op2.re);
}

inline Complex operator*(const Complex& op1, const Real& op2)
{
  return Complex(op1.re*op2, op1.im*op2);
}

inline Complex operator/(const Real& op1, const Complex& op2)
{
  return Complex(
    (op1*op2.re) / (op2.re*op2.re + op2.im*op2.im),
    (-op1*op2.im) / (op2.re*op2.re + op2.im*op2.im)
  );
}

inline Complex operator/(const Complex& op1, const Complex& op2)
{
  return Complex(
    (op1.re*op2.re + op1.im*op2.im) / (op2.re*op2.re + op2.im*op2.im),
    (op1.im*op2.re - op1.re*op2.im) / (op2.re*op2.re + op2.im*op2.im)
  );
}

inline Complex operator/(const Complex& op1, const Real& op2)
{
  return Complex(op1.re/op2, op1.im/op2);
}

inline Complex Complex::i(){
  return Complex(0.0, 1.0);
}

#endif
