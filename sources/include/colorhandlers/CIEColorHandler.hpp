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
#ifndef GUARD_VRT_CIECOLORHANDLER_HPP
#define GUARD_VRT_CIECOLORHANDLER_HPP
//!
//! @file CIEColorHandler.hpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @remarks
//! @details This file defines the behavior of the CIE colorimetric converter
//!
#include <colorhandlers/ColorHandler.hpp>
#include <colorhandlers/CIEBase.hpp>

#include <core/3DBase.hpp>
#include <core/LightBase.hpp>

////////////////////////////////////////////////////////////////////////////////
//! @class CIEColorHandler
//! @brief Defines the behavior of the CIE colorimetric converter
//! @details This class is the "natural" color converter. It convert light 
//!  spectrum into RGB compontents according a standard observer (CIE-XYZ for 
//!  example). Next, it convert this tri-stimulus into RGB using a matrix (Most 
//!  of the time it is a XYZ to RGB with blanc point at D65).
//! @Remarks The chromatic adaptation is now embedded inside this class
class CIEColorHandler: public ColorHandler {
 public:
  //! @brief Constructor
  //! @param standard_observer Standard observer (array of 3 spectra /x, /y,/z)
  //! @param conversion_matrix 3x3 Matrix for converting from XYZ to the 
  //!  wanted basis
  //! @param in_illum_flag Flag of the input illuminant
  //! @param in_illum Input illuminant
  //! @param out_illum_flag Flag of the output illuminant
  //! @param out_illum Output illuminant
  //! @param method_flag Flag of the method for chromatic adaptation
  CIEColorHandler(Spectrum* standard_observer, 
                  Real* conversion_matrix,
                  CIE::Illuminant& in_illum_flag, Point& in_illum,
                  CIE::Illuminant& out_illum_flag, Point& out_illum,
                  CIE::ChrAdaptMethod& method_flag);
  //! @brief Virtual destructor.
  virtual ~CIEColorHandler(void);
  //! @brief Convert a light data into RGB components
  //! @param lightdata Spectral data to be converted
  //! @param color Returned RGB components
  virtual void lightDataToRGB(const LightVector& lightdata, Pixel& color);

 private:
  //! @brief Precompute the matrix for chromatic adaptation if required
  void PrepareChradaptMatrix(void);
  //! @brief Prepare the cone responde domain matrices for XYZ_SCALING
  void PrepareAWithScale(Real* A, Real* A_1);
  //! @brief Prepare the cone responde domain matrices for BRADFORD
  void PrepareAWithBradford(Real* A, Real* A_1);
  //! @brief Prepare the cone responde domain matrices for VON KIRES
  void PrepareAWithVonKries(Real* A, Real* A_1);
  //! @brief Prepare the cone responde domain matrices for CIECAT02
  void PrepareAWithCiecat02(Real* A, Real* A_1);
  //! @brief Matrix product from

 private:
  //! Observer spectra (array of 3)
  Spectrum* p_observers;  
  //! 3x3 Matrix for converting from XYZ to the wanted basis (example sRGB). 
  Real *p_conversion_matrix; 
  //! Assumed input illuminant flag (must be coherent with the scene)
  CIE::Illuminant m_illuminant_in_flag;
  //! Assumed input illuminant (XYZ white point)
  Point m_illuminant_in;
  //! Assumed output illuminant flag. If present and different from input 
  //! illuminant, then a chromatic adaptation will be done. Obviously it must
  //! be coherent with the XYZ-to-RGB conversion matrix
  CIE::Illuminant m_illuminant_out_flag;
  //! Assumed output illuminant (XYZ white point)
  Point m_illuminant_out;
  //! Method used for chromatic adaptation
  CIE::ChrAdaptMethod m_chradapt_flag;
  //! 3x3 Matrix for chromatic adaptation (not necessary used)
  Real* p_chradapt_matrix; 
}; // class CIEColorHandler
////////////////////////////////////////////////////////////////////////////////
#endif // GUARD_VRT_CIECOLORHANDLER_HPP
