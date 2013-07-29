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
#include <objectshapes/Rotation.hpp>
//!
//! @file Rotation.cpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file implements the classes declared in Rotation.hpp 
//!  @arg Rotation
//! @todo 
//! @remarks 
//!
#include <stdio.h>
////////////////////////////////////////////////////////////////////////////////
Rotation::Rotation(char axis, Real angle, ObjectShape* shape)
    : p_shape(NULL) {
  p_shape = shape;

  world_to_local[0] = world_to_local[1] = world_to_local[2] = 0;

  for(int i = 0; i < 9; i++) {
    m_matrix[i]     = 0.0;
    m_backmatrix[i] = 0.0;
  }
  // nothing to do if p_shape = NULL
  if (shape != NULL) {
    BoundingBox tmp;
    p_shape->getBoundingBox(tmp);

    // world_to_local = vector from the center of the bounding box to (0,0,0)
    Real bb_lengthX = tmp.max[0] - tmp.min[0];
    Real bb_lengthY = tmp.max[1] - tmp.min[1];
    Real bb_lengthZ = tmp.max[2] - tmp.min[2];
    world_to_local[0] = Real(-1.0) * (tmp.min[0] + Real(0.5) * bb_lengthX);
    world_to_local[1] = Real(-1.0) * (tmp.min[1] + Real(0.5) * bb_lengthY);
    world_to_local[2] = Real(-1.0) * (tmp.min[2] + Real(0.5) * bb_lengthZ);

//    switch(axis)
//    {
//    case 0 : //X axis
//      m_matrix[0] = 1.0;
//      m_matrix[4] = cos(angle);
//      m_matrix[5] = -sin(angle);
//      m_matrix[7] = sin(angle);
//      m_matrix[8] = cos(angle);
//      m_backmatrix[0] = 1.0;
//      m_backmatrix[4] = cos(-angle);
//      m_backmatrix[5] = -sin(-angle);
//      m_backmatrix[7] = sin(-angle);
//      m_backmatrix[8] = cos(-angle);
//      break; 
//    case 1 : //Y axis
//      m_matrix[0] = cos(angle);
//      m_matrix[2] = sin(angle);
//      m_matrix[4] = 1.0;
//      m_matrix[6] = -sin(angle);
//      m_matrix[8] = cos(angle);
//      m_backmatrix[0] = cos(-angle);
//      m_backmatrix[2] = sin(-angle);
//      m_backmatrix[4] = 1.0;
//      m_backmatrix[6] = -sin(-angle);
//      m_backmatrix[8] = cos(-angle);    
//      break;
//    default : //Z axis
//      m_matrix[0] = cos(angle);
//      m_matrix[1] = -sin(angle);
//      m_matrix[3] = sin(angle);
//      m_matrix[4] = cos(angle);
//      m_matrix[8] = 1.0;
//      m_backmatrix[0] = cos(-angle);
//      m_backmatrix[1] = -sin(-angle);
//      m_backmatrix[3] = sin(-angle);
//      m_backmatrix[4] = cos(-angle);   
//      m_backmatrix[8] = 1.0;
//    }
//  }
//}
    
    // Generate vector u(ux, uy, uz)
    Vector u(0, 0, 0);

    // This generation depends on the value of parameter 'axis'
    switch(axis)  {
    // X axis
    case 0:
      u[0] = Real(1.0);
      world_to_local[0] = 0;
      break; 
    // Y axis
    case 1:
      u[1] = Real(1.0);
      world_to_local[1] = 0;
      break; 
    // Z axis
    case 2:  
      u[2] = Real(1.0);
      world_to_local[2] = 0;
      break; 
    // default = Z axis
    default:  
      u[2] = Real(1.0);
      world_to_local[2] = 0;
      break; 
    }

    // precompute some usefull value
    Real cos_a = cos(angle);
    Real sin_a = sin(angle);
    Real un_cos_a = Real(1.0) - cos_a;
    Real icos_a = cos(-angle);
    Real isin_a = sin(-angle);
    Real un_icos_a = Real(1.0) - icos_a;

    // General Matrix of Rotation of angle A around axis u(ux, uy, uz)
    // with:
    //   c = cosA
    //   s = sinA
    //   ux_2 = ux*ux
    //   uy_2 = uy*uy
    //   uz_2 = uz*uz
    //
    // ( ux_2 + (1 - ux_2)*c    ux*uy*(1 - c) - uz*s    ux*uz*(1 - c) + uy*s )
    // ( ux*uy*(1 - c) + uz*s   uy_2 + (1 - uy_2)*c     uy*uz*(1 - c) - ux*s ) 
    // ( ux*uz*(1 - c) - uy*s   uy*uz*(1 - c) + ux*s    uz_2 + (1 - uz_2)*c  )
    //
    // matrix angle alpha
    m_matrix[0] = u[0] * u[0] + (Real(1.0) - u[0] * u[0]) * cos_a;
    m_matrix[1] = u[0] * u[1] * un_cos_a - u[2] * sin_a;
    m_matrix[2] = u[0] * u[2] * un_cos_a + u[1] * sin_a;

    m_matrix[3] = u[0] * u[1] * un_cos_a + u[2] * sin_a;
    m_matrix[4] = u[1] * u[1] + (Real(1.0) - u[1] * u[1]) * cos_a;
    m_matrix[5] = u[1] * u[2] * un_cos_a - u[0] * sin_a;

    m_matrix[6] = u[0] * u[2] * un_cos_a - u[1] * sin_a;
    m_matrix[7] = u[1] * u[2] * un_cos_a + u[0] * sin_a;
    m_matrix[8] = u[2] * u[2] + (Real(1.0) - u[2] * u[2]) * cos_a;
    // matrix angle -alpha
    m_backmatrix[0] = u[0] * u[0] + (Real(1.0) - u[0] * u[0]) * icos_a;
    m_backmatrix[1] = u[0] * u[1] * un_icos_a - u[2] * isin_a;
    m_backmatrix[2] = u[0] * u[2] * un_icos_a + u[1] * isin_a;

    m_backmatrix[3] = u[0] * u[1] * un_icos_a + u[2] * isin_a;
    m_backmatrix[4] = u[1] * u[1] + (Real(1.0) - u[1] * u[1]) * icos_a;
    m_backmatrix[5] = u[1] * u[2] * un_icos_a - u[0] * isin_a;

    m_backmatrix[6] = u[0] * u[2] * un_icos_a - u[1] * isin_a;
    m_backmatrix[7] = u[1] * u[2] * un_icos_a + u[0] * isin_a;
    m_backmatrix[8] = u[2] * u[2] + (Real(1.0) - u[2] * u[2]) * icos_a;
  }
}
////////////////////////////////////////////////////////////////////////////////
Rotation::~Rotation() {
  if (p_shape != NULL)
    delete p_shape;
  p_shape = NULL;

}
////////////////////////////////////////////////////////////////////////////////
bool Rotation::intersect(const Ray& ray, Real& distance) {
  if (p_shape == NULL)
    return false;

  Ray transRay = ray;

  to_local(transRay.o);
  to_local(transRay.v);

  transform(transRay.o);
  transform(transRay.v);
  
  to_world(transRay.o);
  to_world(transRay.v);

  return p_shape->intersect(transRay, distance);
}
////////////////////////////////////////////////////////////////////////////////
void Rotation::getLocalBasis(const Ray& ray, const Real& distance, 
                          Basis& localBasis, Point2D& surfaceCoordinate) {
  if (p_shape == NULL)
    return;
                            
  Ray transRay = ray;

  to_local(transRay.o);
  to_local(transRay.v);

  transform(transRay.o);
  transform(transRay.v);

  to_world(transRay.o);
  to_world(transRay.v);

  p_shape->getLocalBasis(transRay, distance, localBasis, surfaceCoordinate);

  to_local(localBasis.o);
  to_local(localBasis.i);
  to_local(localBasis.j);
  to_local(localBasis.k);

  inverse_transform(localBasis.o);
  inverse_transform(localBasis.i);
  inverse_transform(localBasis.j);
  inverse_transform(localBasis.k);

  to_world(localBasis.o);
  to_world(localBasis.i);
  to_world(localBasis.j);
  to_world(localBasis.k);
}
////////////////////////////////////////////////////////////////////////////////
void Rotation::getBoundingBox(BoundingBox& boundingBox) {
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
  for(int i = 1; i < 8; i++) {

    to_local(vertices[i]);
    inverse_transform(vertices[i]);
    to_world(vertices[i]);
    
    boundingBox.updateWith(vertices[i]);
  }
}
////////////////////////////////////////////////////////////////////////////////
void Rotation::transform(Vector& v) {
  Vector tmp = v;
  
  // do rotation
  v[0] = tmp[0] * m_matrix[0] 
       + tmp[1] * m_matrix[3] 
       + tmp[2] * m_matrix[6];
  v[1] = tmp[0] * m_matrix[1] 
       + tmp[1] * m_matrix[4] 
       + tmp[2] * m_matrix[7];
  v[2] = tmp[0] * m_matrix[2] 
       + tmp[1] * m_matrix[5] 
       + tmp[2] * m_matrix[8];
}
////////////////////////////////////////////////////////////////////////////////                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
void Rotation::transform(Point& p) {
  Point tmp = p;

  // do rotation
  p[0] = tmp[0] * m_matrix[0] 
       + tmp[1] * m_matrix[3] 
       + tmp[2] * m_matrix[6];
  p[1] = tmp[0] * m_matrix[1] 
       + tmp[1] * m_matrix[4] 
       + tmp[2] * m_matrix[7];
  p[2] = tmp[0] * m_matrix[2] 
       + tmp[1] * m_matrix[5] 
       + tmp[2] * m_matrix[8];
 }
////////////////////////////////////////////////////////////////////////////////
void Rotation::inverse_transform(Vector& v) {
  Vector tmp = v;

  // do rotation
  v[0] = tmp[0] * m_backmatrix[0] 
       + tmp[1] * m_backmatrix[3] 
       + tmp[2] * m_backmatrix[6];
  v[1] = tmp[0] * m_backmatrix[1] 
       + tmp[1] * m_backmatrix[4] 
       + tmp[2] * m_backmatrix[7];
  v[2] = tmp[0] * m_backmatrix[2] 
       + tmp[1] * m_backmatrix[5] 
       + tmp[2] * m_backmatrix[8];
}
////////////////////////////////////////////////////////////////////////////////                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
void Rotation::inverse_transform(Point& p) {
  Point tmp = p;

  // do rotation
  p[0] = tmp[0] * m_backmatrix[0] 
       + tmp[1] * m_backmatrix[3] 
       + tmp[2] * m_backmatrix[6];
  p[1] = tmp[0] * m_backmatrix[1] 
       + tmp[1] * m_backmatrix[4] 
       + tmp[2] * m_backmatrix[7];
  p[2] = tmp[0] * m_backmatrix[2] 
       + tmp[1] * m_backmatrix[5] 
       + tmp[2] * m_backmatrix[8];
 }
////////////////////////////////////////////////////////////////////////////////                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
void Rotation::to_local(Vector& v) {
  v[0] += world_to_local[0];
  v[1] += world_to_local[1];
  v[2] += world_to_local[2];
}
void Rotation::to_local(Point& p) {
  p[0] += world_to_local[0];
  p[1] += world_to_local[1];
  p[2] += world_to_local[2];
}
void Rotation::to_world(Vector& v) {
  v[0] -= world_to_local[0];
  v[1] -= world_to_local[1];
  v[2] -= world_to_local[2];
}
void Rotation::to_world(Point& p) {
  p[0] -= world_to_local[0];
  p[1] -= world_to_local[1];
  p[2] -= world_to_local[2];
}