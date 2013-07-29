/*
 *  Copyright 2013 Remi "Programmix" Cerise
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
#ifndef GUARD_VRT_BOUNDINGBOX_HPP
#define GUARD_VRT_BOUNDINGBOX_HPP
//! @file BoundingBox.hpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @remarks
//! @details This file defines a structures for bounding boxes (square) 
//!
#include <common.hpp>
#include <maths/Point.hpp>
#include <maths/Ray.hpp>
////////////////////////////////////////////////////////////////////////////////
//! @class BoundingBox
//! @brief Structures for bounding boxes (square) 
class BoundingBox {
 public:
  //! @brief Default constructor
  inline BoundingBox(void) { };
  //! @brief Constructor 2
  //! @param minx Minimum x position
  //! @param maxx Maximim x position
  //! @param miny Minimum y position
  //! @param maxy Maximim y position
  //! @param minz Minimum z position
  //! @param maxz Maximim z position
  inline BoundingBox(const Real& minx, const Real& maxx, 
                     const Real& miny, const Real& maxy, 
                     const Real& minz, const Real& maxz);
  
 public:
  //! @brief Update this bounding box in order to contain the given one
  //! @param Input bounding box that must be contained inside the current one
  inline void updateWith(const BoundingBox& box);
  //! @brief Update this bounding box in order to contain the given point
  //! @param p Input point that must be contained inside the current one
  inline void updateWith(const Point& p);
  //! @brief Tests if a given ray intersect the current boundign box
  //! @param ray Input ray
  inline bool canIntersect(const Ray& ray) const;

 public:
  //! Point of the box located the closest to world (0,0,0)
  Point min;
  //! Point of the box located the farest to world (0,0,0)
  Point max;
  //! Center of the bounding box int the three dimensions
  Point center;
}; // classBoundingBox
////////////////////////////////////////////////////////////////////////////////
inline BoundingBox::BoundingBox(const Real& minx, const Real& maxx, 
                                const Real& miny, const Real& maxy, 
                                const Real& minz, const Real& maxz) {
  min[0] = minx;
  min[1] = miny;
  min[2] = minz;
  max[0] = maxx;
  max[1] = maxy;
  max[2] = maxz;

  // compute the center of the boundign box
  center[0] = min[0] + Real(0.5) * (max[0] - min[0]);
  center[1] = min[1] + Real(0.5) * (max[1] - min[1]);
  center[2] = min[2] + Real(0.5) * (max[2] - min[2]);
}
////////////////////////////////////////////////////////////////////////////////
inline void BoundingBox::updateWith(const BoundingBox& box) 
{
  if(min[0] > box.min[0]) 
    min[0] = box.min[0];
  if(min[1] > box.min[1]) 
    min[1] = box.min[1];
  if(min[2] > box.min[2]) 
    min[2] = box.min[2];

  if(max[0] < box.max[0]) 
    max[0] = box.max[0];
  if(max[1] < box.max[1]) 
    max[1] = box.max[1];
  if(max[2] < box.max[2]) 
    max[2] = box.max[2];

  // compute the center of the boundign box
  center[0] = min[0] + Real(0.5) * (max[0] - min[0]);
  center[1] = min[1] + Real(0.5) * (max[1] - min[1]);
  center[2] = min[2] + Real(0.5) * (max[2] - min[2]);
}
////////////////////////////////////////////////////////////////////////////////
inline void BoundingBox::updateWith(const Point& p)
{
  if(min[0] > p[0]) 
    min[0] = p[0];
  if(min[1] > p[1]) 
    min[1] = p[1];
  if(min[2] > p[2]) 
    min[2] = p[2];

  if(max[0] < p[0]) 
    max[0] = p[0];
  if(max[1] < p[1]) 
    max[1] = p[1];
  if(max[2] < p[2]) 
    max[2] = p[2];

  // compute the center of the boundign box
  center[0] = min[0] + Real(0.5) * (max[0] - min[0]);
  center[1] = min[1] + Real(0.5) * (max[1] - min[1]);
  center[2] = min[2] + Real(0.5) * (max[2] - min[2]);
}
////////////////////////////////////////////////////////////////////////////////
inline bool BoundingBox::canIntersect(const Ray& ray) const
{
  Real min0 = min[0];
  Real min1 = min[1];
  Real min2 = min[2];

  Real max0 = max[0];
  Real max1 = max[1];
  Real max2 = max[2];
  
  Real o0 = ray.o[0];
  Real o1 = ray.o[1];
  Real o2 = ray.o[2];
  
  Real v0 = ray.v[0];
  Real v1 = ray.v[1];
  Real v2 = ray.v[2];

  //Test trivial cases : start inside the box
  if( o0 >= min0 && o0 <=max0 
      && o1 >= min1 && o1 <=max1 
      && o2 >= min2 && o2 <=max2)
    return true;

  //yz planes
  if(v0 < 0 && o0 >= max0) {
    Real d = (max0 - o0) / v0;
    if( (o1 + d * v1) >= min1 
        && (o1 + d * v1) <= max1 
        && (o2 + d * v2) >= min2 
        && (o2 + d * v2) <= max2 )
      return true;
  } else if(v0 > 0 && o0 <= min0) {
    Real d = (min0 - o0) / v0;
    if( (o1 + d * v1) >= min1 
      && (o1 + d * v1) <= max1 
      && (o2 + d * v2) >= min2 
      && (o2 + d * v2) <= max2 )
      return true;
  }

  //xz planes
  if(v1 < 0 && o1 >= max1) {
    Real d = (max1 - o1) / v1;
    if( (o0 + d * v0) >= min0 
        && (o0 + d * v0) <= max0 
        && (o2 + d * v2) >= min2 
        && (o2 + d * v2) <= max2 )
      return true;
  } else if(v1 > 0 && o1 <= min1) {
    Real d = (min1 - o1) / v1;
    if( (o0 + d * v0) >= min0 
        && (o0 + d * v0) <= max0 
        && (o2 + d * v2) >= min2 
        && (o2 + d * v2) <= max2 )
      return true;
  }

  //xy planes
  if(v2 < 0 && o2 >= max2) {
    Real d = (max2 - o2) / v2;
    if( (o1 + d * v1) >= min1 
        && (o1 + d * v1) <= max1 
        && (o0 + d * v0) >= min0 
        && (o0 + d * v0) <= max0 )
      return true;
  } else if(v2 > 0 && o2 <= min2) {
    Real d = (min2 - o2) / v2;
    if( (o1 + d * v1) >= min1 
      && (o1 + d * v1) <= max1 
      && (o0 + d * v0) >= min0 
      && (o0 + d * v0) <= max0 )
      return true;
  }

  return false;
}
////////////////////////////////////////////////////////////////////////////////
#endif // GUARD_VRT_BOUNDINGBOX_HPP
