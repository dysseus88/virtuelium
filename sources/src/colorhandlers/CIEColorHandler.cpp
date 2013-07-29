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
 */
#include <colorhandlers/CIEColorHandler.hpp>
//!
//! @file CIEColorHandler.cpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file implements the classes declared in CIEColorHandler.hpp 
//!  @arg CIEColorHandler
//! @todo 
//! @remarks 
//!
#include <iostream>
#include <core/LightBase.hpp>

#include <core/VrtLog.hpp>

////////////////////////////////////////////////////////////////////////////////
CIEColorHandler::CIEColorHandler(Spectrum* standard_observer, 
                                 Real* conversion_matrix,
                                 CIE::Illuminant& in_illum_flag, 
                                 Point& in_illum,
                                 CIE::Illuminant& out_illum_flag, 
                                 Point& out_illum,
                                 CIE::ChrAdaptMethod& method_flag)
    : p_observers(standard_observer), 
      p_conversion_matrix(conversion_matrix),
      m_illuminant_in_flag(in_illum_flag),
      m_illuminant_out_flag(out_illum_flag),
      m_chradapt_flag(method_flag),
      p_chradapt_matrix(NULL) {
  
  // Input illuminant
  m_illuminant_in[0] = in_illum[0];
  m_illuminant_in[1] = in_illum[1];
  m_illuminant_in[2] = in_illum[2];

  // Output illuminant
  m_illuminant_out[0] = out_illum[0];
  m_illuminant_out[1] = out_illum[1];
  m_illuminant_out[2] = out_illum[2];

  //VrtLog::WriteArray<Real>("p_conversion_matrix", p_conversion_matrix, 9);

  // Prepare the possible chromatic adaption
  PrepareChradaptMatrix();
}
////////////////////////////////////////////////////////////////////////////////
CIEColorHandler::~CIEColorHandler(void) {
  if(p_chradapt_matrix != NULL) {
    delete[] p_chradapt_matrix;
    p_chradapt_matrix = NULL;
  }
  if(p_conversion_matrix != NULL) {
    delete[] p_conversion_matrix;
    p_conversion_matrix = NULL;
  }
  if(p_observers != NULL) {
    delete[] p_observers;
    p_observers = NULL;
  }
}
////////////////////////////////////////////////////////////////////////////////
void CIEColorHandler::lightDataToRGB(const LightVector& lightdata, 
                                     Pixel& color) {
  //Integrating the spectrum XYZ + N
  Real XYZ_N[4] = {0, 0, 0, 0};

  for (unsigned int wl = 0; wl < GlobalSpectrum::nbWaveLengths(); wl++) {
    // X component
    XYZ_N[0] += lightdata[wl].getRadiance() * p_observers[0][wl];
    // Y component
    XYZ_N[1] += lightdata[wl].getRadiance() * p_observers[1][wl];
    // Z component
    XYZ_N[2] += lightdata[wl].getRadiance() * p_observers[2][wl];
    // N diviser
    XYZ_N[3] += p_observers[1][wl];
  }
  // Use the diviser
  if (XYZ_N[3] != 0) {
    XYZ_N[0] /= XYZ_N[3];
    XYZ_N[1] /= XYZ_N[3];
    XYZ_N[2] /= XYZ_N[3];
  }

  //std::cout << XYZ_N[0] << "  " << XYZ_N[1] << "  " << XYZ_N[2] << std::endl;

  // Do chromatic adaptation if necessary, otherwise just assign
  Real XYZ_out[3] = {0, 0, 0};
  if (p_chradapt_matrix != NULL) {
    //std::cout << "Chormatic adaptation is processing" << std::endl;
    XYZ_out[0] = p_chradapt_matrix[0] * XYZ_N[0]
               + p_chradapt_matrix[1] * XYZ_N[1]
               + p_chradapt_matrix[2] * XYZ_N[2];
    
    XYZ_out[1] = p_chradapt_matrix[3] * XYZ_N[0]
               + p_chradapt_matrix[4] * XYZ_N[1]
               + p_chradapt_matrix[5] * XYZ_N[2];
    
    XYZ_out[2] = p_chradapt_matrix[6] * XYZ_N[0]
               + p_chradapt_matrix[7] * XYZ_N[1]
               + p_chradapt_matrix[8] * XYZ_N[2];
  } else {
     //std::cout << "No chormatic adaptation" << std::endl;
     XYZ_out[0] = XYZ_N[0];
     XYZ_out[1] = XYZ_N[1];
     XYZ_out[2] = XYZ_N[2];
  }

  //std::cout << XYZ_out[0] << "  " << XYZ_out[1] << "  " << XYZ_out[2] << std::endl << std::endl;

  //Converting to RGB
  Real rgb[3];
  rgb[0] = (XYZ_out[0] * p_conversion_matrix[0] 
            + XYZ_out[1] * p_conversion_matrix[1] 
            + XYZ_out[2] * p_conversion_matrix[2]);

  rgb[1] = (XYZ_out[0] * p_conversion_matrix[3] 
            + XYZ_out[1] * p_conversion_matrix[4] 
            + XYZ_out[2] * p_conversion_matrix[5]);
  
  rgb[2] = (XYZ_out[0] * p_conversion_matrix[6] 
            + XYZ_out[1] * p_conversion_matrix[7] 
            + XYZ_out[2] * p_conversion_matrix[8]);

  //Set and return final values
  for(unsigned int k=0; k<3; k++)
    color[k] = (rgb[k]<0) ? 0 : rgb[k];
}
////////////////////////////////////////////////////////////////////////////////
void CIEColorHandler::PrepareChradaptMatrix(void) {
  if(p_chradapt_matrix != NULL) {
    delete[] p_chradapt_matrix;
    p_chradapt_matrix = NULL;
  }
  // We don't have to create anything if:
  //  - No method is selected
  //  - No Output illuminant is slected
  //  - Output illuminant = Input illuminant (except custom ones)
  if (m_chradapt_flag == CIE::kCHR_NONE) {
    //std::cout << "No method for chromatic adaptation !" << std::endl;
    return;
  }
  if (m_illuminant_out_flag == CIE::kILLUM_NONE) {
    //std::cout << "No output illuminant !" << std::endl;
    return;
  }
  if (m_illuminant_out_flag != CIE::kILLUM_CUSTOM
        && m_illuminant_out_flag == m_illuminant_in_flag) {
    //std::cout << "Output illuminant  = input illuminant !" << std::endl;
    return;
  }

  // Step 1: Create matrices for transformation to and from cone response domain
  Real A[9];    // Matrix A: Cone response domaine
  memset(A, 0, 9 * sizeof(Real));
  Real A_1[9];  // Matrix A_1 = Inverse(A)
  memset(A_1, 0, 9 * sizeof(Real));
  switch(m_chradapt_flag) {
    case CIE::kCHR_XYZ_SCALING: 
      PrepareAWithScale(A, A_1);
      break;
    case CIE::kCHR_BRADFORD: 
      PrepareAWithBradford(A, A_1);
      break;
    case CIE::kCHR_VONKRIES: 
      PrepareAWithVonKries(A, A_1);
      break;
    case CIE::kCHR_CIECAT02: 
      PrepareAWithCiecat02(A, A_1);
      break;
    default:
      PrepareAWithCiecat02(A, A_1);
      break;
  }

  // Step 2: Compute the illuminant matrix in Cone Response Domain (CRD)
  // Mathematically, 3 steps are merged into one:
  //  - Convert input and output illum to CRD => in_crd and out_crd
  //  - Divide out_crd by in_crd (component by component) => vec_CRD
  //  - Multiply vec_CRD by 3x3 Identity matrix
  Real CRD[3]; // It is a diagonal matrix
  memset(CRD, 0, 3 * sizeof(Real));
  CRD[0] = (A[0] * m_illuminant_out[0] + A[1] * m_illuminant_out[1] 
              + A[2] * m_illuminant_out[2])
         / (A[0] * m_illuminant_in[0] + A[1] * m_illuminant_in[1] 
              + A[2] * m_illuminant_in[2]);
  CRD[1] = (A[3] * m_illuminant_out[0] + A[4] * m_illuminant_out[1] 
              + A[5] * m_illuminant_out[2])
         / (A[3] * m_illuminant_in[0] + A[4] * m_illuminant_in[1] 
              + A[5] * m_illuminant_in[2]);
  CRD[2] = (A[6] * m_illuminant_out[0] + A[7] * m_illuminant_out[1] 
              + A[8] * m_illuminant_out[2])
         / (A[6] * m_illuminant_in[0] + A[7] * m_illuminant_in[1] 
              + A[8] * m_illuminant_in[2]);

  // Step 3: Compute the chromatic adaptation matrix
  // Mathematically, 2 steps are merged into one:
  //  - Multiply CRD * A = B
  //  - Multiply A_1 * B
  p_chradapt_matrix = new Real[9];

  p_chradapt_matrix[0] = A_1[0] * CRD[0] * A[0] 
                       + A_1[1] * CRD[1] * A[3]
                       + A_1[2] * CRD[2] * A[6];
  p_chradapt_matrix[1] = A_1[0] * CRD[0] * A[1] 
                       + A_1[1] * CRD[1] * A[4]
                       + A_1[2] * CRD[2] * A[7];
  p_chradapt_matrix[2] = A_1[0] * CRD[0] * A[2] 
                       + A_1[1] * CRD[1] * A[5]
                       + A_1[2] * CRD[2] * A[8];

  p_chradapt_matrix[3] = A_1[3] * CRD[0] * A[0] 
                       + A_1[4] * CRD[1] * A[3]
                       + A_1[5] * CRD[2] * A[6];
  p_chradapt_matrix[4] = A_1[3] * CRD[0] * A[1] 
                       + A_1[4] * CRD[1] * A[4]
                       + A_1[5] * CRD[2] * A[7];
  p_chradapt_matrix[5] = A_1[3] * CRD[0] * A[2] 
                       + A_1[4] * CRD[1] * A[5]
                       + A_1[5] * CRD[2] * A[8];

  p_chradapt_matrix[6] = A_1[6] * CRD[0] * A[0] 
                       + A_1[7] * CRD[1] * A[3]
                       + A_1[8] * CRD[2] * A[6];
  p_chradapt_matrix[7] = A_1[6] * CRD[0] * A[1] 
                       + A_1[7] * CRD[1] * A[4]
                       + A_1[8] * CRD[2] * A[7];
  p_chradapt_matrix[8] = A_1[6] * CRD[0] * A[2] 
                       + A_1[7] * CRD[1] * A[5]
                       + A_1[8] * CRD[2] * A[8];
}
////////////////////////////////////////////////////////////////////////////////
void CIEColorHandler::PrepareAWithScale(Real* A, Real* A_1) {
  if (A == NULL || A_1 == NULL)
    return;

  A[0] = Real(1.0); A[4] = Real(1.0); A[8] = Real(1.0);
  A_1[0] = Real(1.0); A_1[4] = Real(1.0); A_1[8] = Real(1.0);
}
////////////////////////////////////////////////////////////////////////////////
void CIEColorHandler::PrepareAWithBradford(Real* A, Real* A_1) {
  if (A == NULL || A_1 == NULL)
    return;

 	A[0] = Real(0.8951000); A[1] = Real(0.2664000); A[2] = Real(-0.1614000);
 	A[3] = Real(-0.7502000); A[4] = Real(1.7135000); A[5] = Real(0.0367000);
 	A[6] = Real(0.0389000); A[7] = Real(-0.0685000); A[8] = Real(1.0296000);

 	A_1[0] = Real(0.9869929); A_1[1] = Real(-0.1470543); A_1[2] = Real(0.1599627);
 	A_1[3] = Real(0.4323053); A_1[4] = Real(0.5183603); A_1[5] = Real(0.0492912);
 	A_1[6] = Real(-0.0085287); A_1[7] = Real(0.0400428); A_1[8] = Real(0.9684867);
}
////////////////////////////////////////////////////////////////////////////////
void CIEColorHandler::PrepareAWithVonKries(Real* A, Real* A_1) {
  if (A == NULL || A_1 == NULL)
    return;

 	A[0] = Real(0.4002400); A[1] = Real(0.7076000); A[2] = Real(-0.0808100);
 	A[3] = Real(-0.2263000); A[4] = Real(1.1653200); A[5] = Real(0.0457000);
 	A[6] = Real(0.0000000); A[7] = Real(0.0000000); A[8] = Real(0.9182200);

 	A_1[0] = Real(1.8599364); A_1[1] = Real(-1.1293816); A_1[2] = Real(0.2198974);
 	A_1[3] = Real(0.3611914); A_1[4] = Real(0.6388125); A_1[5] = Real(-0.0000064);
 	A_1[6] = Real(0.0000000); A_1[7] = Real(0.0000000); A_1[8] = Real(1.0890636);
}
////////////////////////////////////////////////////////////////////////////////
void CIEColorHandler::PrepareAWithCiecat02(Real* A, Real* A_1) {
  if (A == NULL || A_1 == NULL)
    return;

  A[0] = Real(0.73280); A[1] = Real(0.42960); A[2] = Real(-0.16240);
  A[3] = Real(-0.70360); A[4] = Real(1.69750); A[5] = Real(0.00610);
  A[6] = Real(0.00300); A[7] = Real(0.01360); A[8] = Real(0.98340);
  
  A_1[0] = Real(1.096124); A_1[1] = Real(-0.278869); A_1[2] = Real(0.182745);
  A_1[3] = Real(0.454369); A_1[4] = Real(0.473533); A_1[5] = Real(0.072098);
  A_1[6] = Real(-0.009628); A_1[7] = Real(-0.005698); A_1[8] = Real(1.015326);
}
////////////////////////////////////////////////////////////////////////////////
