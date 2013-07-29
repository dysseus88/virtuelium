/*
 *  Copyright 2013 Remi "Programmix" Cerise
 *
 *  This file is part of Virtuelium.
 *
 *  Virtuelium is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License. or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful.
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not. write to the Free Software
 *  Foundation. Inc.. 59 Temple Place - Suite 330. Boston. MA 02111-1307. USA.
 */
#ifndef GUARD_VRT_CIEBASE_HPP
#define GUARD_VRT_CIEBASE_HPP
//!
//! @file CIEBase.hpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @remarks
//! @details This file defines some constants in the colorimetric domain
//! @todo Find better sources than Wikipedia
//!
////////////////////////////////////////////////////////////////////////////////
// Some flags
namespace CIE {
//! @enum Method for chromatic adaptation
typedef enum {
  kCHR_NONE,
  kCHR_XYZ_SCALING,
  kCHR_BRADFORD,
  kCHR_VONKRIES,
  kCHR_CIECAT02
} ChrAdaptMethod;

//! @enum Observer
typedef enum {
  kOBS_NONE,
  kOBS_1931,
  kOBS_1964,
} Observer;

//! @enum Illuminant
typedef enum {
  kILLUM_NONE,
  kILLUM_CUSTOM,
  kILLUM_SPECTRUM,
  kILLUM_A,
  kILLUM_D50,
  kILLUM_D55,
  kILLUM_D65,
  kILLUM_D75,
  kILLUM_E,
  kILLUM_F1,
  kILLUM_F2,
  kILLUM_F3,
  kILLUM_F4,
  kILLUM_F5,
  kILLUM_F6,
  kILLUM_F7,
  kILLUM_F8,
  kILLUM_F9,
  kILLUM_F10,
  kILLUM_F11,
  kILLUM_F12
} Illuminant;
} // namespace CIE
////////////////////////////////////////////////////////////////////////////////
// Some standardized white points
// source: http://en.wikipedia.org/wiki/Standard_illuminant
// xy values have been converted to XYZ using :
//  Y = 1
//  X = xY / y
//  Z = (1-x-y)Y / y
#define CIE31_AX  (1.09847)
#define CIE31_AY  (1.00000)
#define CIE31_AZ  (0.35582)

#define CIE31_D50X  (0.96421)
#define CIE31_D50Y  (1.00000)
#define CIE31_D50Z  (0.82519)

#define CIE31_D55X  (0.95680)
#define CIE31_D55Y  (1.00000)
#define CIE31_D55Z  (0.92148)

#define CIE31_D65X  (0.95043)
#define CIE31_D65Y  (1.00000)
#define CIE31_D65Z  (1.08890)

#define CIE31_D75X  (0.94972)
#define CIE31_D75Y  (1.00000)
#define CIE31_D75Z  (1.22639)

#define CIE31_EX  (1.00000)
#define CIE31_EY  (1.00000)
#define CIE31_EZ  (1.03030)

#define CIE31_F1X  (0.92834)
#define CIE31_F1Y  (1.00000)
#define CIE31_F1Z  (1.03665)

#define CIE31_F2X  (0.99145)
#define CIE31_F2Y  (1.00000)
#define CIE31_F2Z  (0.67316)

#define CIE31_F3X  (1.03753)
#define CIE31_F3Y  (1.00000)
#define CIE31_F3Z  (0.49861)

#define CIE31_F4X  (1.09147)
#define CIE31_F4Y  (1.00000)
#define CIE31_F4Z  (0.38813)

#define CIE31_F5X  (0.90872)
#define CIE31_F5Y  (1.00000)
#define CIE31_F5Z  (0.98723)

#define CIE31_F6X  (0.97309)
#define CIE31_F6Y  (1.00000)
#define CIE31_F6Z  (0.60191)

#define CIE31_F7X  (0.95017)
#define CIE31_F7Y  (1.00000)
#define CIE31_F7Z  (1.08630)

#define CIE31_F8X  (0.96413)
#define CIE31_F8Y  (1.00000)
#define CIE31_F8Z  (0.82333)

#define CIE31_F9X  (1.00365)
#define CIE31_F9Y  (1.00000)
#define CIE31_F9Z  (0.67868)

#define CIE31_F10X  (0.96174)
#define CIE31_F10Y  (1.00000)
#define CIE31_F10Z  (0.81712)

#define CIE31_F11X  (1.00899)
#define CIE31_F11Y  (1.00000)
#define CIE31_F11Z  (0.64262)

#define CIE31_F12X  (1.08046)
#define CIE31_F12Y  (1.00000)
#define CIE31_F12Z  (0.39228)

#define CIE64_AX  (1.11142)
#define CIE64_AY  (1.00000)
#define CIE64_AZ  (0.35200)

#define CIE64_D50X  (0.96721)
#define CIE64_D50Y  (1.00000)
#define CIE64_D50Z  (0.81428)

#define CIE64_D55X  (0.95797)
#define CIE64_D55Y  (1.00000)
#define CIE64_D55Z  (0.90925)

#define CIE64_D65X  (0.94810)
#define CIE64_D65Y  (1.00000)
#define CIE64_D65Z  (1.07305)

#define CIE64_D75X  (0.94417)
#define CIE64_D75Y  (1.00000)
#define CIE64_D75Z  (1.20643)

#define CIE64_EX  (1.00000)
#define CIE64_EY  (1.00000)
#define CIE64_EZ  (1.03030)

#define CIE64_F1X  (0.94791)
#define CIE64_F1Y  (1.00000)
#define CIE64_F1Z  (1.03191)

#define CIE64_F2X  (1.03245)
#define CIE64_F2Y  (1.00000)
#define CIE64_F2Z  (0.68990)

#define CIE64_F3X  (1.08968)
#define CIE64_F3Y  (1.00000)
#define CIE64_F3Z  (0.51965)

#define CIE64_F4X  (1.14961)
#define CIE64_F4Y  (1.00000)
#define CIE64_F4Z  (0.40963)

#define CIE64_F5X  (0.93369)
#define CIE64_F5Y  (1.00000)
#define CIE64_F5Z  (0.98636)

#define CIE64_F6X  (1.02148)
#define CIE64_F6Y  (1.00000)
#define CIE64_F6Z  (0.62074)

#define CIE64_F7X  (0.95780)
#define CIE64_F7Y  (1.00000)
#define CIE64_F7Z  (1.07618)

#define CIE64_F8X  (0.97115)
#define CIE64_F8Y  (1.00000)
#define CIE64_F8Z  (0.81135)

#define CIE64_F9X  (1.02116)
#define CIE64_F9Y  (1.00000)
#define CIE64_F9Z  (0.67826)

#define CIE64_F10X  (0.99001)
#define CIE64_F10Y  (1.00000)
#define CIE64_F10Z  (0.83134)

#define CIE64_F11X  (1.03820)
#define CIE64_F11Y  (1.00000)
#define CIE64_F11Z  (0.65555)

#define CIE64_F12X  (1.11428)
#define CIE64_F12Y  (1.00000)
#define CIE64_F12Z  (0.40353)
////////////////////////////////////////////////////////////////////////////////
#endif // GUARD_VRT_CIEBASE_HPP
