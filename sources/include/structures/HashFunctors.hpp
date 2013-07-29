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
 
#ifndef _HASHFUNCTORS_HPP
#define _HASHFUNCTORS_HPP

/**
 * This file contain some common hash functors for the HashMap.
 * - int           -> IntHashFunctor
 * - unsigned int  -> UnsignedIntHashFunctor
 * - char          -> CharHashFunctor
 * - float         -> FloatHashFunctor
 * - double        -> DoubleHashFunctor
 * - char *        -> CStringHashFunctor
 * - std::string   -> StringHashFunctor
 */

#include <string>

/**
 * Functor for int type
 */
class IntHashFunctor{
  public : unsigned int operator()(const int& i) const
  {
    return (unsigned int)i;
  }
};

/**
 * Functor for unsigned int type
 */
class UnsignedIntHashFunctor{
  public : unsigned int operator()(const unsigned int& i) const
  {
    return i;
  }
};

/**
 * Functor for unsigned char type
 */
class CharHashFunctor{
  public : unsigned int operator()(const char& i) const
  {
    return (unsigned int)i;
  }
};

/**
 * Functor for unsigned float type
 */
class FloatHashFunctor{
  public : unsigned int operator()(const float& i) const
  {
    return *((unsigned int*) &i);
  }
};

/**
 * Functor for unsigned double type
 */
class DoubleHashFunctor{
  public : unsigned int operator()(const float& i) const
  {
    return *((unsigned int*) &i);
  }
};

/**
 * Functor for char* type
 */
class CStringHashFunctor{
  public : unsigned int operator()(const char*& str) const
  {
    unsigned int index=0;
    for(int i=0; i<5 && str[i]!='\0'; i++)
      index = (index<<5) + str[i];
    return index;
  }
};

/**
 * Functor for std::string type.
 */
class StringHashFunctor{
  public : unsigned int operator()(const std::string& str) const
  {
    unsigned int index=0;
    for(int i=0; i<5 && str[i]!='\0'; i++)
      index = (index<<5) + str[i];
    return index;
  }
};

#endif //_HASHFUNCTORS_HPP

