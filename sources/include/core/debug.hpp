/*
*  Copyright 2012 Sylvain "Greewi" Dumazet, Remi Cerise
*
*  This file is part of Virtuelium.
*
*  Virtuelium is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 3 of the License, or
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
*
*/
#ifndef _DEBUG_HPP
#define _DEBUG_HPP

#include <stdio.h>
#include <iostream>
#include <cmath>

#ifdef _WIN32
	// Windows
	#define isnan_local(x)	_isnan(x)
	#define isinf_local(x)	(!_finite(x))
#else
	// Linux
	#define isnan_local(x)	std::isnan(x)
	#define isinf_local(x)	std::isinf(x)
#endif

#include "maths/Complex.hpp"

inline void DEBUGPRINTREAL(Real R, const char* name)
{
  printf("\n-- DEBUG -- %s : %f\n", name, R);
}

inline void DEBUGPRINTCOMPLEX(Complex C, const char* name)
{
  printf("\n-- DEBUG -- %s : %f + i%f\n", name, C.re, C.im);
}

inline bool DEBUGISWRONG(Complex C)
{
	if( isnan_local(C.re) || isinf_local(C.re) || isnan_local(C.im) || isinf_local(C.im))
    return true;
  return false;
}

inline bool DEBUGPRINT(Complex C, const char* name)
{
  if(isnan_local(C.re) || isinf_local(C.re) || isnan_local(C.im) || isinf_local(C.im))
  {
    DEBUGPRINTCOMPLEX(C, name);
    return true;
  }
  return false;
}

#endif //_DEBUG_HPP
