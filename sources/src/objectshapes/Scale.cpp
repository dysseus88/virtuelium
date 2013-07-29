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
#include <objectshapes/Scale.hpp>
//!
//! @file Scale.cpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file implements the classes declared in Scale.hpp 
//!  @arg Scale
//! @todo 
//! @remarks 
//!
#include <stdio.h>
////////////////////////////////////////////////////////////////////////////////
Scale::Scale(Real xfactor, Real yfactor, Real zfactor, ObjectShape* shape)
    : m_xfactor(Real(1.0)),
      m_yfactor(Real(1.0)),
      m_zfactor(Real(1.0)),
      p_shape(NULL) {
  p_shape = shape;

  if (xfactor == 0)
    m_xfactor = 0;
  else
    m_xfactor /= xfactor;

  if (yfactor == 0)
    m_yfactor = 0;
  else
    m_yfactor /= yfactor;

  if (zfactor == 0)
    m_zfactor = 0;
  else
    m_zfactor /= zfactor;
}
////////////////////////////////////////////////////////////////////////////////
Scale::~Scale() {
  if (p_shape != NULL)
    delete p_shape;
  p_shape = NULL;

}
////////////////////////////////////////////////////////////////////////////////
bool Scale::intersect(const Ray& ray, Real& distance) {
  if (p_shape == NULL)
    return false;

  Ray transRay = ray;
  transform(transRay.o);
  transform(transRay.v);
  return p_shape->intersect(transRay, distance);
}
////////////////////////////////////////////////////////////////////////////////
void Scale::getLocalBasis(const Ray& ray, const Real& distance, 
                          Basis& localBasis, Point2D& surfaceCoordinate) {
  if (p_shape == NULL)
    return;
                            
  Ray transRay = ray;
  transform(transRay.o);
  transform(transRay.v);
  p_shape->getLocalBasis(transRay, distance, localBasis, surfaceCoordinate);
  inverse_transform(localBasis.o);
  inverse_transform(localBasis.i);
  inverse_transform(localBasis.j);
  inverse_transform(localBasis.k);
}
////////////////////////////////////////////////////////////////////////////////
void Scale::getBoundingBox(BoundingBox& boundingBox) {
  if (p_shape == NULL)
    return;

  BoundingBox tmp;
  p_shape->getBoundingBox(tmp);
  Point vertices[8];
  vertices[0][0] = tmp.min[0]; 
  vertices[0][1] = tmp.min[1]; 
  vertices[0][2] = tmp.min[2];

  vertices[1][0] = tmp.min[0]; 
  vertices[1][1] = tmp.min[1]; 
  vertices[1][2] = tmp.max[2];
  
  vertices[2][0] = tmp.min[0]; 
  vertices[2][1] = tmp.max[1]; 
  vertices[2][2] = tmp.min[2];
  
  vertices[3][0] = tmp.min[0];
  vertices[3][1] = tmp.max[1]; 
  vertices[3][2] = tmp.max[2];
  
  vertices[4][0] = tmp.max[0]; 
  vertices[4][1] = tmp.min[1]; 
  vertices[4][2] = tmp.min[2];
  
  vertices[5][0] = tmp.max[0]; 
  vertices[5][1] = tmp.min[1]; 
  vertices[5][2] = tmp.max[2];
  
  vertices[6][0] = tmp.max[0]; 
  vertices[6][1] = tmp.max[1]; 
  vertices[6][2] = tmp.min[2];
  
  vertices[7][0] = tmp.max[0]; 
  vertices[7][1] = tmp.max[1]; 
  vertices[7][2] = tmp.max[2];
  
  boundingBox.min = vertices[0];
  boundingBox.max = vertices[0];  
  for(int i=1; i<8; i++) {
    inverse_transform(vertices[i]);
    boundingBox.updateWith(vertices[i]);
  }
}
////////////////////////////////////////////////////////////////////////////////
void Scale::transform(Vector& v) {
  Vector tmp = v;
  v[0] = tmp[0] * m_xfactor;
  v[1] = tmp[1] * m_yfactor;
  v[2] = tmp[2] * m_zfactor;
}
////////////////////////////////////////////////////////////////////////////////                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
void Scale::transform(Point& p) {
  Point tmp = p;
  p[0] = tmp[0] * m_xfactor;
  p[1] = tmp[1] * m_yfactor;
  p[2] = tmp[2] * m_zfactor;
}
////////////////////////////////////////////////////////////////////////////////
void Scale::inverse_transform(Vector& v) {
  Vector tmp = v;
  if (m_xfactor == 0)
    v[0] = 0;
  else
    v[0] = tmp[0] * m_xfactor;

  if (m_yfactor == 0)
    v[1] = 0;
  else
  v[1] = tmp[1] * m_yfactor;

  if (m_zfactor == 0)
    v[2] = 0;
  else
  v[2] = tmp[2] * m_zfactor;
}
////////////////////////////////////////////////////////////////////////////////                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
void Scale::inverse_transform(Point& p) {
  Point tmp = p;
  if (m_xfactor == 0)
    p[0] = 0;
  else
    p[0] = tmp[0] * m_xfactor;

  if (m_yfactor == 0)
    p[1] = 0;
  else
  p[1] = tmp[1] * m_yfactor;

  if (m_zfactor == 0)
    p[2] = 0;
  else
  p[2] = tmp[2] * m_zfactor;
}