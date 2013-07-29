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
#include <objectshapes/Transformation.hpp>
//!
//! @file Transformation.cpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file implements the classes declared in Transformation.hpp 
//!  @arg SquareMatrix
//!  @arg Transformation
//! @todo 
//! @remarks 
//!
////////////////////////////////////////////////////////////////////////////////
SquareMatrix::SquareMatrix(void) 
    : p_value(NULL) { 
  
  p_value = new Real[kSIZE_FULL];
  for (unsigned int i = 0; i < kSIZE_FULL; i++)
    p_value[i] = 0;
}
////////////////////////////////////////////////////////////////////////////////
SquareMatrix::SquareMatrix(const Real& init_value)
    : p_value(NULL) { 

  p_value = new Real[kSIZE_FULL];
  for (unsigned int i = 0; i < kSIZE_FULL; i++)
    p_value[i] = init_value;
}
////////////////////////////////////////////////////////////////////////////////
SquareMatrix::SquareMatrix(const SquareMatrix & m)
    : p_value(NULL) { 

  if (m.p_value != NULL) {
    p_value = new Real[kSIZE_FULL];
    for (unsigned int i = 0; i < kSIZE_FULL; i++)
      p_value[i] = m.p_value[i];
  }
}
////////////////////////////////////////////////////////////////////////////////
SquareMatrix::~SquareMatrix() {
  Erase();
}
////////////////////////////////////////////////////////////////////////////////
SquareMatrix& SquareMatrix::operator=(const SquareMatrix& rhs) {
  if (this != &rhs) {
    Erase();

    if (rhs.p_value != NULL) {
      p_value = new Real[kSIZE_FULL];
      for (unsigned int i = 0; i < kSIZE_FULL; i++)
        p_value[i] = rhs.p_value[i];
    } 
  }
  return *this;
}
////////////////////////////////////////////////////////////////////////////////
SquareMatrix& SquareMatrix::operator+=(const SquareMatrix& rhs) {
 if (p_value == NULL || rhs.p_value == NULL)
   return *this;

 for (unsigned int i = 0; i < kSIZE_FULL; i++)
   p_value[i] += rhs.p_value[i];

  return *this;
}
////////////////////////////////////////////////////////////////////////////////
SquareMatrix& SquareMatrix::operator-=(const SquareMatrix& rhs) {
 if (p_value == NULL || rhs.p_value == NULL)
   return *this;

 for (unsigned int i = 0; i < kSIZE_FULL; i++)
   p_value[i] -= rhs.p_value[i];

  return *this;
}
////////////////////////////////////////////////////////////////////////////////
const SquareMatrix SquareMatrix::operator+(const SquareMatrix& rhs) const {
  SquareMatrix result;
 if (p_value == NULL || rhs.p_value == NULL)
   return result;

  result.p_value = new Real[kSIZE_FULL];
  for (unsigned int i = 0; i < kSIZE_FULL; i++)
    result.p_value[i] = p_value[i] + rhs.p_value[i];

  return result;  
}
////////////////////////////////////////////////////////////////////////////////
const SquareMatrix SquareMatrix::operator-(const SquareMatrix& rhs) const {
  SquareMatrix result;
 if (p_value == NULL || rhs.p_value == NULL)
   return result;

  result.p_value = new Real[kSIZE_FULL];
  for (unsigned int i = 0; i < kSIZE_FULL; i++)
    result.p_value[i] = p_value[i] - rhs.p_value[i];

  return result;  
}
////////////////////////////////////////////////////////////////////////////////
const SquareMatrix SquareMatrix::operator*(const SquareMatrix& rhs) const {
  SquareMatrix result;
  if (p_value == NULL || rhs.p_value == NULL)
    return result;

  result.p_value = new Real[kSIZE_FULL];
  for (unsigned int i = 0; i < kSIZE_LIN; i++) { 
    for (unsigned int j = 0; j < kSIZE_LIN; j++) {

      result.p_value[i * kSIZE_LIN + j] = 0;

      for (unsigned int k = 0; k < kSIZE_LIN; k++) {
        result.p_value[i * kSIZE_LIN + j] += p_value[i * kSIZE_LIN + k]
                                        * rhs.p_value[k * kSIZE_LIN + j];
      }

    }
  }

  return result;  
}
////////////////////////////////////////////////////////////////////////////////
const Point SquareMatrix::Transform(const Point& rhs) const {
  Point result;
  if (p_value == NULL )
    return result;

  // Project the point in 4D space
  Real rhs_4d[4] = { rhs[0], rhs[1], rhs[2], Real(1.0) };
  Real result_4d[4] = { 0, 0, 0, 0 };

  // do the matrix product
  for (unsigned int i = 0; i < kSIZE_LIN; i++) { 
    for (unsigned int k = 0; k < kSIZE_LIN; k++) {
      result_4d[i] += p_value[i * kSIZE_LIN + k] * rhs_4d[k]; 
    }
  }

  // Project back to 3d space
  if (result_4d[3] < kEPSILON) { // && result_4d[3] > -kEPSILON) {
    printf ("\n -- ATTENTION -- The projection of the point (%f, %f, %f) ",
            rhs[0], rhs[1], rhs[2]);
    printf("is  potentially wrong (4th dim = 0)\n\n");
  } else {
    result[0] = result_4d[0] / result_4d[3];
    result[1] = result_4d[1] / result_4d[3];
    result[2] = result_4d[2] / result_4d[3];
  }

  return result;  
}
////////////////////////////////////////////////////////////////////////////////
const Vector SquareMatrix::Transform(const Vector& rhs) const {
  Vector result;
  if (p_value == NULL )
    return result;

  // Project the point in 4D space
  Real rhs_4d[4] = { rhs[0], rhs[1], rhs[2], Real(1.0) };
  Real result_4d[4] = { 0, 0, 0, 0 };

  // do the matrix product
  for (unsigned int i = 0; i < kSIZE_LIN; i++) { 
    for (unsigned int k = 0; k < kSIZE_LIN; k++) {
      result_4d[i] += p_value[i * kSIZE_LIN + k] * rhs_4d[k]; 
    }
  }

  // Project back to 3d space
  if (result_4d[3] == 0) {
    printf ("\n -- ATTENTION -- The projection of the point (%f, %f, %f) ",
            rhs[0], rhs[1], rhs[2]);
    printf("is  potentially wrong (4th dim = 0)\n\n");
  } else {
    result[0] = result_4d[0] / result_4d[3];
    result[1] = result_4d[1] / result_4d[3];
    result[2] = result_4d[2] / result_4d[3];
  }

  return result;  
}
////////////////////////////////////////////////////////////////////////////////
void SquareMatrix::Erase(void) {
  if (p_value != NULL)
    delete[] p_value;
  p_value = NULL;
}
////////////////////////////////////////////////////////////////////////////////
Transformation::Transformation(ObjectShape* embedded) 
    : p_embedded(embedded) {

  //if (p_embedded != NULL) {
  //  BoundingBox embedded_box;
  //  p_embedded->getBoundingBox(embedded_box);

  //  //printf("\nBounding box of embbeded object:\n");
  //  //printf("   min = (%f, %f, %f)\n", embedded_box.min[0],
  //  //                                  embedded_box.min[1],
  //  //                                  embedded_box.min[2]);
  //  //printf("   max = (%f, %f, %f)\n", embedded_box.max[0],
  //  //                                  embedded_box.max[1],
  //  //                                  embedded_box.max[2]);
  //  //printf("   center = (%f, %f, %f)\n", embedded_box.center[0],
  //  //                                     embedded_box.center[1],
  //  //                                     embedded_box.center[2]); 
  //}
}
////////////////////////////////////////////////////////////////////////////////
Transformation::Transformation(ObjectShape* embedded, 
                               const Vector& translation) 
    : p_embedded(embedded) {

  // main matrix
  m_main_transform[0] = Real(1.0);
  m_main_transform[5] = Real(1.0);
  m_main_transform[10] = Real(1.0);
  m_main_transform[15] = Real(1.0);
  m_main_transform[3] = translation[0];
  m_main_transform[7] = translation[1];
  m_main_transform[11] = translation[2];

  // back matrix
  m_back_transform[0] = Real(1.0);
  m_back_transform[5] = Real(1.0);
  m_back_transform[10] = Real(1.0);
  m_back_transform[15] = Real(1.0);
  m_back_transform[3] = -translation[0];
  m_back_transform[7] = -translation[1];
  m_back_transform[11] = -translation[2];
}
////////////////////////////////////////////////////////////////////////////////
Transformation::Transformation(ObjectShape* embedded, 
                               const Vector& u, 
                               const Real& angle)
    : p_embedded(embedded) {
 
  // precompute some usefull value
  Real cos_a = cos(angle);
  Real sin_a = sin(angle);
  Real un_cos_a = Real(1.0) - cos_a;
  Real icos_a = cos(-angle);
  Real isin_a = sin(-angle);
  Real un_icos_a = Real(1.0) - icos_a;

  // main matrix
  m_main_transform[0] = u[0] * u[0] + (Real(1.0) - u[0] * u[0]) * cos_a;
  m_main_transform[1] = u[0] * u[1] * un_cos_a - u[2] * sin_a;
  m_main_transform[2] = u[0] * u[2] * un_cos_a + u[1] * sin_a;

  m_main_transform[4] = u[0] * u[1] * un_cos_a + u[2] * sin_a;
  m_main_transform[5] = u[1] * u[1] + (Real(1.0) - u[1] * u[1]) * cos_a;
  m_main_transform[6] = u[1] * u[2] * un_cos_a - u[0] * sin_a;

  m_main_transform[8] = u[0] * u[2] * un_cos_a - u[1] * sin_a;
  m_main_transform[9] = u[1] * u[2] * un_cos_a + u[0] * sin_a;
  m_main_transform[10] = u[2] * u[2] + (Real(1.0) - u[2] * u[2]) * cos_a;

  m_main_transform[15] = Real(1.0);

  m_main_transform.Print();

  // back matrix
  m_back_transform[0] = u[0] * u[0] + (Real(1.0) - u[0] * u[0]) * icos_a;
  m_back_transform[1] = u[0] * u[1] * un_icos_a - u[2] * isin_a;
  m_back_transform[2] = u[0] * u[2] * un_icos_a + u[1] * isin_a;

  m_back_transform[4] = u[0] * u[1] * un_icos_a + u[2] * isin_a;
  m_back_transform[5] = u[1] * u[1] + (Real(1.0) - u[1] * u[1]) * icos_a;
  m_back_transform[6] = u[1] * u[2] * un_icos_a - u[0] * isin_a;

  m_back_transform[8] = u[0] * u[2] * un_icos_a - u[1] * isin_a;
  m_back_transform[9] = u[1] * u[2] * un_icos_a + u[0] * isin_a;
  m_back_transform[10] = u[2] * u[2] + (Real(1.0) - u[2] * u[2]) * icos_a;

  m_back_transform[15] = Real(1.0);

  m_back_transform.Print();
}
////////////////////////////////////////////////////////////////////////////////
Transformation::~Transformation(void) {
  EraseEmbedded();
}
////////////////////////////////////////////////////////////////////////////////
bool Transformation::intersect(const Ray& ray, Real& distance) {
  if (p_embedded == NULL)
    return false;

  Ray trans_ray = ray;
  trans_ray.o = m_back_transform.Transform(trans_ray.o);
  return p_embedded->intersect(trans_ray, distance);   
}
////////////////////////////////////////////////////////////////////////////////
void Transformation::getLocalBasis(const Ray& ray, const Real& distance, 
                                Basis& local_basis, 
                                Point2D& surface_coordinate) {
  if (p_embedded == NULL)
    return;

  Ray trans_ray = ray;
  trans_ray.o = m_back_transform.Transform(trans_ray.o);

  p_embedded->getLocalBasis(trans_ray, distance, local_basis, 
                            surface_coordinate);

  local_basis.o = m_main_transform.Transform(local_basis.o);
}
////////////////////////////////////////////////////////////////////////////////
void Transformation::getBoundingBox(BoundingBox& bounding_box) {
  if (p_embedded == NULL)
    return;
  
  p_embedded->getBoundingBox(bounding_box);

  bounding_box.min = m_main_transform.Transform(bounding_box.min);
  bounding_box.max = m_main_transform.Transform(bounding_box.max);
  bounding_box.center = m_main_transform.Transform(bounding_box.center);
}
////////////////////////////////////////////////////////////////////////////////
void Transformation::EraseEmbedded(void) {
  if (p_embedded != NULL) 
    delete p_embedded;
  p_embedded = NULL;
}
////////////////////////////////////////////////////////////////////////////////
