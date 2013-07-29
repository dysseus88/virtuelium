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
#ifndef GUARD_VRT_TRANSFORMATION_HPP
#define GUARD_VRT_TRANSFORMATION_HPP
//!
//! @file Transformation.hpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @remarks
//! @details This file defines transformations in 3D spaces based on quaternion 
//!  matrices
//!
#include <stdio.h>

#include <core/3DBase.hpp>
#include <objectshapes/ObjectShape.hpp>
////////////////////////////////////////////////////////////////////////////////
//! @class SquareMatrix
//! @brief Simple SquareMatrix class
class SquareMatrix {
 public:
  //! Linear size ofthe 4d matrices
  static const unsigned int kSIZE_LIN = 4;
  //! Full size of the 4d matrices
  static const unsigned int kSIZE_FULL = 16;

 public :
  //! @brief Constructor 1
  SquareMatrix(void);
  //! @brief Constructor 2
  SquareMatrix(const Real& init_value);
  //! @brief Copy constructor
  SquareMatrix(const SquareMatrix & m);
  //! @brief Destructor
  virtual ~SquareMatrix(void);

 public:
  //! @brief Overloading of operator '[]' 1
  //! @param i Index of the component to get
  //! @remarks No checking is made on i before attempting to access the 
  //!  component
  inline Real& operator[](int i) { return p_value[i]; }
  //! @brief Overloading of operator '[]' 1
  //! @param i Index of the component to get
  //! @remarks No checking is made on i before attempting to access the 
  //!  component
  inline Real operator[](int i) const { return p_value[i]; }

 public:
  //! @brief Overloading of binary operator '='
  //! @param rhs SquareMatrix to be copied
  SquareMatrix& operator=(const SquareMatrix& rhs);
  //! @brief Overloading of binary operator '+='
  //! @param rhs SquareMatrix to be added
  SquareMatrix& operator+=(const SquareMatrix& rhs);
  //! @brief Overloading of binary operator '-='
  //! @param rhs SquareMatrix to be substracted
  SquareMatrix& operator-=(const SquareMatrix& rhs);
  //! @brief Overloading of binary operator '+'
  //! @param v SquareMatrix to be added
  const SquareMatrix operator+(const SquareMatrix& rhs) const; 
  //! @brief Overloading of binary operator '-'
  //! @param v SquareMatrix to be substracted
  const SquareMatrix operator-(const SquareMatrix& rhs) const; 
  //! @brief Overloading of binary operator '*' 
  //! @param rhs second SquareMatrix
  const SquareMatrix operator*(const SquareMatrix& rhs) const; 

 public:
  //! @brief Matrix product with a point
  //! @param rhs second SquareMatrix
  //! @return rhs Transformed point
  const Point Transform(const Point& rhs) const;
  //! @brief Matrix product with a vector
  //! @param rhs second SquareMatrix
  //! @return rhs Transformed vector
  const Vector Transform(const Vector& rhs) const;

 public:
  //! @brief Print the SquareMatrix
  inline void Print(void);
  //! @brief Clear the SquareMatrix pointer
  void Erase(void);

 public:
   //! Array of values
   Real* p_value;

}; // class SquareMatrix
////////////////////////////////////////////////////////////////////////////////
inline void SquareMatrix::Print(void) {
  if (p_value == NULL ) {
    printf("\nVOID\n");
  } else {
    for (unsigned int i = 0; i < kSIZE_LIN; i++) {
      for (unsigned int j = 0; j < kSIZE_LIN; j++) {
        printf("%f  ", p_value[i * kSIZE_LIN + j]);
      }
      printf("\n");
    }
    printf("\n");
  }
}
////////////////////////////////////////////////////////////////////////////////
//! @class Transformation
//! @brief Transformations in 3D spaces based on quaternion 
class Transformation : public ObjectShape {
 public:
  //! @brief Constructor 1
  //! @param embedded Embedded material
  Transformation(ObjectShape* embedded);
  //! @brief Constructor Translation
  //! @param embedded Embedded material
  //! @param translation Translation vector
  Transformation(ObjectShape* embedded, const Vector& translation);
  //! @brief Constructor Translation
  //! @param embedded Embedded material
  //! @param u Axis of rotation
  //! @param angle Angle of rotation
  Transformation(ObjectShape* embedded, const Vector& u, const Real& angle);
  //! @brief Destructor
  virtual ~Transformation(void);
 
 public: 
  //! @brief tests if a ray hits the transformed object 
  //! @param ray Input ray
  //! @param distance Output length from the origin of the ray to the hit point
  //! @return True if an intersection has been detected
  virtual bool intersect(const Ray& ray, Real& distance);
  //! @brief Get the local basis of the transformed object hit by a ray
  //! @param ray Input ray
  //! @param distance Input length from the origin of the ray to the hit point
  //! @param local_basis Output local basis Oijk (O: Hit point; k: normal to 
  //!  the hit surface)
  //! @param surface_coordinate Output surface cordinate that can be used for 
  //!  mapping
  virtual void getLocalBasis(const Ray& ray, const Real& distance, 
                             Basis& local_basis, Point2D& surface_coordinate);
  //! @brief Get the bounding box of the transformed object
  //! @param bounding_box Output bounding box 
  virtual void getBoundingBox(BoundingBox& bounding_box);
  
 private:
  //! @brief Erase embedded shape
  void EraseEmbedded(void);

 private:
  //! Main transformation matrix
  SquareMatrix m_main_transform;
  //! Inversed transformation matrix
  SquareMatrix m_back_transform;
  //! Embedded shape to be transformed
  ObjectShape* p_embedded; 
}; // class Transformation
////////////////////////////////////////////////////////////////////////////////
#endif // GUARD_VRT_TRANSFORMATION_HPP
