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
#include <io/sceneryV2/V2ColorHandlerParser.hpp>
//!
//! @file V2ColorHandlerParser.cpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file implements the class declared in V2ColorHandlerParser.hpp 
//!  @arg V2ColorHandlerParser
//! @todo 
//! @remarks 
//!
#include <string>

#include <core/3DBase.hpp>

#include <exceptions/Exception.hpp>

#include <io/DataParser.hpp>
#include <io/MathParser.hpp>

#include <colorhandlers/SimpleRGBColorHandler.hpp>
#include <colorhandlers/SpectrumColorHandler.hpp>
#include <colorhandlers/CIEColorHandler.hpp>
#include <colorhandlers/RGBColorHandler.hpp>
#include <colorhandlers/PolarisationColorHandler.hpp>
#include <colorhandlers/Spectrophotometer.hpp>
#include <colorhandlers/LinearPolarizerColorHandler.hpp>
////////////////////// class V2ColorHandlerParser //////////////////////////////
ColorHandler* V2ColorHandlerParser::create(XMLTree* node) {
  std::string type = node->getAttributeValue("type");
  
  if(type=="SimpleRGB")
    return createSimpleRGBColorHandler();
  else if(type=="Spectrum")
    return createSpectrumColorHandler();
  else if(type=="Polarisation")
    return createPolarizationColorHandler(node);
  else if(type=="CIE")
    return createCIE(node);
  //else if(type=="CIE2")
  //  return createCIEChromaAdapter(node);
  else if(type=="RGB")
    return createRGB(node);
  else if(type=="LinearPolarizer")
    return createLinearPolarizer(node);
  else if(type=="Spectrophotometer")
    return createSpectrophotometer(node);

  throw Exception("(V2ObjectShapeParser::create) Type de <colorhandler> " + type + " inconnu.");
}
////////////////////// class V2ColorHandlerParser //////////////////////////////
ColorHandler* V2ColorHandlerParser::createSimpleRGBColorHandler(void) {
  return new SimpleRGBColorHandler();
}
////////////////////// class V2ColorHandlerParser //////////////////////////////
ColorHandler* V2ColorHandlerParser::createSpectrumColorHandler(void) {
  return new SpectrumColorHandler();
}
////////////////////// class V2ColorHandlerParser //////////////////////////////
ColorHandler* V2ColorHandlerParser::createPolarizationColorHandler(
    XMLTree* node) {
  Vector up;
  up[0] = getRealValue(node, "upx", 0.0);
  up[1] = getRealValue(node, "upy", 0.0);
  up[2] = getRealValue(node, "upz", 1.0);
  return new PolarisationColorHandler(up);
}
////////////////////// class V2ColorHandlerParser //////////////////////////////
ColorHandler* V2ColorHandlerParser::createCIE(XMLTree* node) {
  DataParser dataparser;
  MathParser mathparser;

  Spectrum* observer = NULL;
  Real* conversion_matrix = NULL;

  // Reference observer
  std::string observer_file;
  CIE::Observer observer_flag = CIE::kOBS_NONE;
  GetObserverInfo(node, observer_file, observer_flag);
  if (observer_flag != CIE::kOBS_NONE) {
    observer = new Spectrum[3];
    dataparser.loadStandardObserver(observer_file, observer);
  } else {
    throw Exception("(V2ObjectShapeParser::createCIE) Aucun obsevateur de \
référence n'a été défini.");
  }

  // XYZ-to-RGB conversion matrix
  // This matrix embbeds a white point that must be the same as the input 
  // illuminant if no chromatic adaption are done or the output illuminant 
  // othserwise
  conversion_matrix = mathparser.loadMatrix(
                          3, 3, node->getAttributeValue("rgbProfile"));

  // Input illuminant (must be coherent with the scene lighting)
  Point input_illum(0, 0, 0);
  CIE::Illuminant input_illum_flag = CIE::kILLUM_NONE;
  std::string input_illum_file;
  GetInputIllumInfo(node, observer_flag, input_illum, input_illum_file, 
                    input_illum_flag);
  // Input illumiant is a file with spectral definition, convert to XYZ
  if (input_illum_flag == CIE::kILLUM_SPECTRUM) {
    Spectrum spectral_illum;
    dataparser.loadSpectralIlluminant(input_illum_file, spectral_illum);
    GetWhitePointFromIlluminant(observer, spectral_illum, input_illum); 
  // No input, then D65 
  } else if (input_illum_flag == CIE::kILLUM_NONE) {
    if (observer_flag == CIE::kOBS_1931) {
      input_illum_flag = CIE::kILLUM_D65;
      input_illum[0] = CIE31_D65X;
      input_illum[1] = CIE31_D65Y;
      input_illum[2] = CIE31_D65Z;
    } else if (observer_flag == CIE::kOBS_1964) {
      input_illum_flag = CIE::kILLUM_D65;
      input_illum[0] = CIE64_D65X;
      input_illum[1] = CIE64_D65Y;
      input_illum[2] = CIE64_D65Z;
    }
  }

  // Output illuminant (must be coherent with the white point used by the 
  // XYZ-to-RGB conversion matrix)
  // if not present, then no chromatic adaptation will be done
  Point output_illum(0, 0, 0);
  CIE::Illuminant output_illum_flag = CIE::kILLUM_NONE;
  std::string output_illum_file;
  GetOutputIllumInfo(node, observer_flag, output_illum, output_illum_file,
                     output_illum_flag);
  // Output illumiant is a file with spectral definition, convert to XYZ
  if (output_illum_flag == CIE::kILLUM_SPECTRUM) {
    Spectrum spectral_illum;
    dataparser.loadSpectralIlluminant(output_illum_file, spectral_illum);
    GetWhitePointFromIlluminant(observer, spectral_illum, output_illum); 
  }
  
  // Method for chromatic adaption
  CIE::ChrAdaptMethod method_flag = CIE::kCHR_NONE;
  GetChrAdaptInfo(node, method_flag);
  if (method_flag == CIE::kCHR_NONE) {
    // no method, then CIECAT02
    method_flag = CIE::kCHR_CIECAT02;
  }

  //printf("\n\n---- input: %d, output: %d, method: %d ---- \n\n", 
  //       input_illum_flag, output_illum_flag, method_flag);

  return new CIEColorHandler(observer, conversion_matrix,
                             input_illum_flag, input_illum,
                             output_illum_flag, output_illum,
                             method_flag);
}
////////////////////// class V2ColorHandlerParser //////////////////////////////
ColorHandler* V2ColorHandlerParser::createRGB(XMLTree* node) {
  DataParser dataparser;

  if(node->getAttributeValue("white") == "") {
    Spectrum* standardObserver = new Spectrum[3];
    dataparser.loadStandardObserver(node->getAttributeValue("refobserver"), 
                                    standardObserver);
    Real gain = strtod(node->getAttributeValue("gain").c_str(), NULL);
    return new RGBColorHandler(standardObserver, gain);
  
  } else {
    Spectrum* standardObserver = new Spectrum[3];
    Spectrum white;
    dataparser.loadStandardObserver(node->getAttributeValue("refobserver"), 
                                    standardObserver);
    dataparser.loadSpectralIlluminant(node->getAttributeValue("white"), white);
    Real gain = strtod(node->getAttributeValue("gain").c_str(), NULL);
    return new RGBColorHandler(standardObserver, gain, white);
  }
}
////////////////////// class V2ColorHandlerParser //////////////////////////////
ColorHandler* V2ColorHandlerParser::createSpectrophotometer(XMLTree* node) {
  ColorHandler* colorHandler = NULL;

  if(node->getNumberOfChildren() == 1)
    colorHandler = create(node->getChild(0));

  unsigned int nbSample = getIntegerValue(node, "samples", 1);
  
  return new Spectrophotometer(node->getAttributeValue("resultfile"), 
                               nbSample, colorHandler);
}
////////////////////// class V2ColorHandlerParser //////////////////////////////
ColorHandler* V2ColorHandlerParser::createLinearPolarizer(XMLTree* node) {
  Vector up;
  up[0] = getRealValue(node, "upx", 0.0);
  up[1] = getRealValue(node, "upy", 0.0);
  up[2] = getRealValue(node, "upz", 1.0);

  ColorHandler* colorHandler = V2ColorHandlerParser::create(node->getChild(0));

  return new LinearPolarizerColorHandler(up, colorHandler);
}
////////////////////// class V2ColorHandlerParser //////////////////////////////
void V2ColorHandlerParser::GetObserverInfo(XMLTree* node, 
                                           std::string& filename, 
                                           CIE::Observer& flag) {
  // Standard observer 
  if (node->getAttributeValue("refObserver") != "") {
    // Use standard observer
    filename = node->getAttributeValue("refObserver");
    if (filename == "CIE_1931") {
      flag = CIE::kOBS_1931;
      filename = "data/colorimetry/observers/observers_CIE1931.xml";
    } else if (filename == "CIE_1964") {
      flag = CIE::kOBS_1964;
      filename = "data/colorimetry/observers/observers_CIE1964.xml";
    }
  } 
}
////////////////////// class V2ColorHandlerParser //////////////////////////////
void V2ColorHandlerParser::GetInputIllumInfo(XMLTree* node, 
                                             const CIE::Observer& obs,
                                             Point& XYZ_illum, 
                                             std::string& illum_file, 
                                             CIE::Illuminant& flag) {
  XYZ_illum[0] = XYZ_illum[1] = XYZ_illum[2] = 0;
  // Use custom XYZ values
  if ( (node->getAttributeValue("illumX") != "")
      || (node->getAttributeValue("illumY") != "")
      || (node->getAttributeValue("illumZ") != "")) {
    flag = CIE::kILLUM_CUSTOM;
    XYZ_illum[0] = getRealValue(node, "illumX", 0.0);
    XYZ_illum[1] = getRealValue(node, "illumY", 0.0);
    XYZ_illum[2] = getRealValue(node, "illumZ", 0.0);

  // Use custom spectral values
  } else if (node->getAttributeValue("illum") != "") {
    flag = CIE::kILLUM_SPECTRUM;
    illum_file = node->getAttributeValue("illum");

  // Use standard illuminant
  } else if (node->getAttributeValue("refIllum") != "") {
    std::string ref = node->getAttributeValue("refIllum");
    // Illuminant A
    if (ref == "A" && obs == CIE::kOBS_1931) {
      flag = CIE::kILLUM_A;
      XYZ_illum[0] = Real(CIE31_AX);
      XYZ_illum[1] = Real(CIE31_AY);
      XYZ_illum[2] = Real(CIE31_AZ);
    } else if (ref == "A" && obs == CIE::kOBS_1964) {
      flag = CIE::kILLUM_A;
      XYZ_illum[0] = Real(CIE64_AX);
      XYZ_illum[1] = Real(CIE64_AY);
      XYZ_illum[2] = Real(CIE64_AZ);

    // Illuminant D50
    } else if (ref == "D50" && obs == CIE::kOBS_1931) {
      flag = CIE::kILLUM_D50;
      XYZ_illum[0] = Real(CIE31_D50X);
      XYZ_illum[1] = Real(CIE31_D50Y);
      XYZ_illum[2] = Real(CIE31_D50Z);
    } else if (ref == "D50" && obs == CIE::kOBS_1964) {
      flag = CIE::kILLUM_D50;
      XYZ_illum[0] = Real(CIE64_D50X);
      XYZ_illum[1] = Real(CIE64_D50Y);
      XYZ_illum[2] = Real(CIE64_D50Z);

    // Illuminant D55
    } else if (ref == "D55" && obs == CIE::kOBS_1931) {
      flag = CIE::kILLUM_D55;
      XYZ_illum[0] = Real(CIE31_D55X);
      XYZ_illum[1] = Real(CIE31_D55Y);
      XYZ_illum[2] = Real(CIE31_D55Z);
    } else if (ref == "D55" && obs == CIE::kOBS_1964) {
      flag = CIE::kILLUM_D55;
      XYZ_illum[0] = Real(CIE64_D55X);
      XYZ_illum[1] = Real(CIE64_D55Y);
      XYZ_illum[2] = Real(CIE64_D55Z);

    // Illuminant D65
    } else if (ref == "D65" && obs == CIE::kOBS_1931) {
      flag = CIE::kILLUM_D65;
      XYZ_illum[0] = Real(CIE31_D65X);
      XYZ_illum[1] = Real(CIE31_D65Y);
      XYZ_illum[2] = Real(CIE31_D65Z);
    } else if (ref == "D65" && obs == CIE::kOBS_1964) {
      flag = CIE::kILLUM_D65;
      XYZ_illum[0] = Real(CIE64_D65X);
      XYZ_illum[1] = Real(CIE64_D65Y);
      XYZ_illum[2] = Real(CIE64_D65Z);

    // Illuminant D75
    } else if (ref == "D75" && obs == CIE::kOBS_1931) {
      flag = CIE::kILLUM_D75;
      XYZ_illum[0] = Real(CIE31_D75X);
      XYZ_illum[1] = Real(CIE31_D75Y);
      XYZ_illum[2] = Real(CIE31_D75Z);
    } else if (ref == "D75" && obs == CIE::kOBS_1964) {
      flag = CIE::kILLUM_D75;
      XYZ_illum[0] = Real(CIE64_D75X);
      XYZ_illum[1] = Real(CIE64_D75Y);
      XYZ_illum[2] = Real(CIE64_D75Z);

    // Illuminant E
    } else if (ref == "E" && obs == CIE::kOBS_1931) {
      flag = CIE::kILLUM_E;
      XYZ_illum[0] = Real(CIE31_EX);
      XYZ_illum[1] = Real(CIE31_EY);
      XYZ_illum[2] = Real(CIE31_EZ);
    } else if (ref == "E" && obs == CIE::kOBS_1964) {
      flag = CIE::kILLUM_E;
      XYZ_illum[0] = Real(CIE64_EX);
      XYZ_illum[1] = Real(CIE64_EY);
      XYZ_illum[2] = Real(CIE64_EZ);

    // Illuminant F1
    } else if (ref == "F1" && obs == CIE::kOBS_1931) {
      flag = CIE::kILLUM_F1;
      XYZ_illum[0] = Real(CIE31_F1X);
      XYZ_illum[1] = Real(CIE31_F1Y);
      XYZ_illum[2] = Real(CIE31_F1Z);
    } else if (ref == "F1" && obs == CIE::kOBS_1964) {
      flag = CIE::kILLUM_F1;
      XYZ_illum[0] = Real(CIE64_F1X);
      XYZ_illum[1] = Real(CIE64_F1Y);
      XYZ_illum[2] = Real(CIE64_F1Z);

    // Illuminant F2
    } else if (ref == "F2" && obs == CIE::kOBS_1931) {
      flag = CIE::kILLUM_F2;
      XYZ_illum[0] = Real(CIE31_F2X);
      XYZ_illum[1] = Real(CIE31_F2Y);
      XYZ_illum[2] = Real(CIE31_F2Z);
    } else if (ref == "F2" && obs == CIE::kOBS_1964) {
      flag = CIE::kILLUM_F2;
      XYZ_illum[0] = Real(CIE64_F2X);
      XYZ_illum[1] = Real(CIE64_F2Y);
      XYZ_illum[2] = Real(CIE64_F2Z);

    // Illuminant F3
    } else if (ref == "F3" && obs == CIE::kOBS_1931) {
      flag = CIE::kILLUM_F3;
      XYZ_illum[0] = Real(CIE31_F3X);
      XYZ_illum[1] = Real(CIE31_F3Y);
      XYZ_illum[2] = Real(CIE31_F3Z);
    } else if (ref == "F3" && obs == CIE::kOBS_1964) {
      flag = CIE::kILLUM_F3;
      XYZ_illum[0] = Real(CIE64_F3X);
      XYZ_illum[1] = Real(CIE64_F3Y);
      XYZ_illum[2] = Real(CIE64_F3Z);

    // Illuminant F4
    } else if (ref == "F4" && obs == CIE::kOBS_1931) {
      flag = CIE::kILLUM_F4;
      XYZ_illum[0] = Real(CIE31_F4X);
      XYZ_illum[1] = Real(CIE31_F4Y);
      XYZ_illum[2] = Real(CIE31_F4Z);
    } else if (ref == "F4" && obs == CIE::kOBS_1964) {
      flag = CIE::kILLUM_F4;
      XYZ_illum[0] = Real(CIE64_F4X);
      XYZ_illum[1] = Real(CIE64_F4Y);
      XYZ_illum[2] = Real(CIE64_F4Z);

    // Illuminant F5
    } else if (ref == "F5" && obs == CIE::kOBS_1931) {
      flag = CIE::kILLUM_F5;
      XYZ_illum[0] = Real(CIE31_F5X);
      XYZ_illum[1] = Real(CIE31_F5Y);
      XYZ_illum[2] = Real(CIE31_F5Z);
    } else if (ref == "F5" && obs == CIE::kOBS_1964) {
      flag = CIE::kILLUM_F5;
      XYZ_illum[0] = Real(CIE64_F5X);
      XYZ_illum[1] = Real(CIE64_F5Y);
      XYZ_illum[2] = Real(CIE64_F5Z);

    // Illuminant F6
    } else if (ref == "F6" && obs == CIE::kOBS_1931) {
      flag = CIE::kILLUM_F6;
      XYZ_illum[0] = Real(CIE31_F6X);
      XYZ_illum[1] = Real(CIE31_F6Y);
      XYZ_illum[2] = Real(CIE31_F6Z);
    } else if (ref == "F6" && obs == CIE::kOBS_1964) {
      flag = CIE::kILLUM_F6;
      XYZ_illum[0] = Real(CIE64_F6X);
      XYZ_illum[1] = Real(CIE64_F6Y);
      XYZ_illum[2] = Real(CIE64_F6Z);

    // Illuminant F7
    } else if (ref == "F7" && obs == CIE::kOBS_1931) {
      flag = CIE::kILLUM_F7;
      XYZ_illum[0] = Real(CIE31_F7X);
      XYZ_illum[1] = Real(CIE31_F7Y);
      XYZ_illum[2] = Real(CIE31_F7Z);
    } else if (ref == "F7" && obs == CIE::kOBS_1964) {
      flag = CIE::kILLUM_F7;
      XYZ_illum[0] = Real(CIE64_F7X);
      XYZ_illum[1] = Real(CIE64_F7Y);
      XYZ_illum[2] = Real(CIE64_F7Z);

    // Illuminant F8
    } else if (ref == "F8" && obs == CIE::kOBS_1931) {
      flag = CIE::kILLUM_F8;
      XYZ_illum[0] = Real(CIE31_F8X);
      XYZ_illum[1] = Real(CIE31_F8Y);
      XYZ_illum[2] = Real(CIE31_F8Z);
    } else if (ref == "F8" && obs == CIE::kOBS_1964) {
      flag = CIE::kILLUM_F8;
      XYZ_illum[0] = Real(CIE64_F8X);
      XYZ_illum[1] = Real(CIE64_F8Y);
      XYZ_illum[2] = Real(CIE64_F8Z);

    // Illuminant F9
    } else if (ref == "F9" && obs == CIE::kOBS_1931) {
      flag = CIE::kILLUM_F9;
      XYZ_illum[0] = Real(CIE31_F9X);
      XYZ_illum[1] = Real(CIE31_F9Y);
      XYZ_illum[2] = Real(CIE31_F9Z);
    } else if (ref == "F9" && obs == CIE::kOBS_1964) {
      flag = CIE::kILLUM_F9;
      XYZ_illum[0] = Real(CIE64_F9X);
      XYZ_illum[1] = Real(CIE64_F9Y);
      XYZ_illum[2] = Real(CIE64_F9Z);

    // Illuminant F10
    } else if (ref == "F10" && obs == CIE::kOBS_1931) {
      flag = CIE::kILLUM_F10;
      XYZ_illum[0] = Real(CIE31_F10X);
      XYZ_illum[1] = Real(CIE31_F10Y);
      XYZ_illum[2] = Real(CIE31_F10Z);
    } else if (ref == "F10" && obs == CIE::kOBS_1964) {
      flag = CIE::kILLUM_F10;
      XYZ_illum[0] = Real(CIE64_F10X);
      XYZ_illum[1] = Real(CIE64_F10Y);
      XYZ_illum[2] = Real(CIE64_F10Z);

    // Illuminant F11
    } else if (ref == "F11" && obs == CIE::kOBS_1931) {
      flag = CIE::kILLUM_F11;
      XYZ_illum[0] = Real(CIE31_F11X);
      XYZ_illum[1] = Real(CIE31_F11Y);
      XYZ_illum[2] = Real(CIE31_F11Z);
    } else if (ref == "F11" && obs == CIE::kOBS_1964) {
      flag = CIE::kILLUM_F11;
      XYZ_illum[0] = Real(CIE64_F11X);
      XYZ_illum[1] = Real(CIE64_F11Y);
      XYZ_illum[2] = Real(CIE64_F11Z);

    // Illuminant F12
    } else if (ref == "F12" && obs == CIE::kOBS_1931) {
      flag = CIE::kILLUM_F12;
      XYZ_illum[0] = Real(CIE31_F12X);
      XYZ_illum[1] = Real(CIE31_F12Y);
      XYZ_illum[2] = Real(CIE31_F12Z);
    } else if (ref == "F12" && obs == CIE::kOBS_1964) {
      flag = CIE::kILLUM_F12;
      XYZ_illum[0] = Real(CIE64_F12X);
      XYZ_illum[1] = Real(CIE64_F12Y);
      XYZ_illum[2] = Real(CIE64_F12Z);
    }
  }
}
////////////////////// class V2ColorHandlerParser //////////////////////////////
void V2ColorHandlerParser::GetOutputIllumInfo(XMLTree* node, 
                                              const CIE::Observer& obs,
                                              Point& XYZ_illum, 
                                              std::string& illum_file, 
                                              CIE::Illuminant& flag) {
  XYZ_illum[0] = XYZ_illum[1] = XYZ_illum[2] = 0;
  // Use custom XYZ values
  if ( (node->getAttributeValue("illumOutX") != "")
      || (node->getAttributeValue("illumOutY") != "")
      || (node->getAttributeValue("illumOutZ") != "")) {
    flag = CIE::kILLUM_CUSTOM;
    XYZ_illum[0] = getRealValue(node, "illumOutX", 0.0);
    XYZ_illum[1] = getRealValue(node, "illumOutY", 0.0);
    XYZ_illum[2] = getRealValue(node, "illumOutZ", 0.0);
  
  // Use custom spectral values
  } else if (node->getAttributeValue("illumOut") != "") {
    flag = CIE::kILLUM_SPECTRUM;
    illum_file = node->getAttributeValue("illumOut");

  // Use standard illuminant
  } else if (node->getAttributeValue("refIllumOut") != "") {
    std::string ref = node->getAttributeValue("refIllumOut");
    // Illuminant A
    if (ref == "A" && obs == CIE::kOBS_1931) {
      flag = CIE::kILLUM_A;
      XYZ_illum[0] = Real(CIE31_AX);
      XYZ_illum[1] = Real(CIE31_AY);
      XYZ_illum[2] = Real(CIE31_AZ);
    } else if (ref == "A" && obs == CIE::kOBS_1964) {
      flag = CIE::kILLUM_A;
      XYZ_illum[0] = Real(CIE64_AX);
      XYZ_illum[1] = Real(CIE64_AY);
      XYZ_illum[2] = Real(CIE64_AZ);

    // Illuminant D50
    } else if (ref == "D50" && obs == CIE::kOBS_1931) {
      flag = CIE::kILLUM_D50;
      XYZ_illum[0] = Real(CIE31_D50X);
      XYZ_illum[1] = Real(CIE31_D50Y);
      XYZ_illum[2] = Real(CIE31_D50Z);
    } else if (ref == "D50" && obs == CIE::kOBS_1964) {
      flag = CIE::kILLUM_D50;
      XYZ_illum[0] = Real(CIE64_D50X);
      XYZ_illum[1] = Real(CIE64_D50Y);
      XYZ_illum[2] = Real(CIE64_D50Z);

    // Illuminant D55
    } else if (ref == "D55" && obs == CIE::kOBS_1931) {
      flag = CIE::kILLUM_D55;
      XYZ_illum[0] = Real(CIE31_D55X);
      XYZ_illum[1] = Real(CIE31_D55Y);
      XYZ_illum[2] = Real(CIE31_D55Z);
    } else if (ref == "D55" && obs == CIE::kOBS_1964) {
      flag = CIE::kILLUM_D55;
      XYZ_illum[0] = Real(CIE64_D55X);
      XYZ_illum[1] = Real(CIE64_D55Y);
      XYZ_illum[2] = Real(CIE64_D55Z);

    // Illuminant D65
    } else if (ref == "D65" && obs == CIE::kOBS_1931) {
      flag = CIE::kILLUM_D65;
      XYZ_illum[0] = Real(CIE31_D65X);
      XYZ_illum[1] = Real(CIE31_D65Y);
      XYZ_illum[2] = Real(CIE31_D65Z);
    } else if (ref == "D65" && obs == CIE::kOBS_1964) {
      flag = CIE::kILLUM_D65;
      XYZ_illum[0] = Real(CIE64_D65X);
      XYZ_illum[1] = Real(CIE64_D65Y);
      XYZ_illum[2] = Real(CIE64_D65Z);

    // Illuminant D75
    } else if (ref == "D75" && obs == CIE::kOBS_1931) {
      flag = CIE::kILLUM_D75;
      XYZ_illum[0] = Real(CIE31_D75X);
      XYZ_illum[1] = Real(CIE31_D75Y);
      XYZ_illum[2] = Real(CIE31_D75Z);
    } else if (ref == "D75" && obs == CIE::kOBS_1964) {
      flag = CIE::kILLUM_D75;
      XYZ_illum[0] = Real(CIE64_D75X);
      XYZ_illum[1] = Real(CIE64_D75Y);
      XYZ_illum[2] = Real(CIE64_D75Z);

    // Illuminant E
    } else if (ref == "E" && obs == CIE::kOBS_1931) {
      flag = CIE::kILLUM_E;
      XYZ_illum[0] = Real(CIE31_EX);
      XYZ_illum[1] = Real(CIE31_EY);
      XYZ_illum[2] = Real(CIE31_EZ);
    } else if (ref == "E" && obs == CIE::kOBS_1964) {
      flag = CIE::kILLUM_E;
      XYZ_illum[0] = Real(CIE64_EX);
      XYZ_illum[1] = Real(CIE64_EY);
      XYZ_illum[2] = Real(CIE64_EZ);

    // Illuminant F1
    } else if (ref == "F1" && obs == CIE::kOBS_1931) {
      flag = CIE::kILLUM_F1;
      XYZ_illum[0] = Real(CIE31_F1X);
      XYZ_illum[1] = Real(CIE31_F1Y);
      XYZ_illum[2] = Real(CIE31_F1Z);
    } else if (ref == "F1" && obs == CIE::kOBS_1964) {
      flag = CIE::kILLUM_F1;
      XYZ_illum[0] = Real(CIE64_F1X);
      XYZ_illum[1] = Real(CIE64_F1Y);
      XYZ_illum[2] = Real(CIE64_F1Z);

    // Illuminant F2
    } else if (ref == "F2" && obs == CIE::kOBS_1931) {
      flag = CIE::kILLUM_F2;
      XYZ_illum[0] = Real(CIE31_F2X);
      XYZ_illum[1] = Real(CIE31_F2Y);
      XYZ_illum[2] = Real(CIE31_F2Z);
    } else if (ref == "F2" && obs == CIE::kOBS_1964) {
      flag = CIE::kILLUM_F2;
      XYZ_illum[0] = Real(CIE64_F2X);
      XYZ_illum[1] = Real(CIE64_F2Y);
      XYZ_illum[2] = Real(CIE64_F2Z);

    // Illuminant F3
    } else if (ref == "F3" && obs == CIE::kOBS_1931) {
      flag = CIE::kILLUM_F3;
      XYZ_illum[0] = Real(CIE31_F3X);
      XYZ_illum[1] = Real(CIE31_F3Y);
      XYZ_illum[2] = Real(CIE31_F3Z);
    } else if (ref == "F3" && obs == CIE::kOBS_1964) {
      flag = CIE::kILLUM_F3;
      XYZ_illum[0] = Real(CIE64_F3X);
      XYZ_illum[1] = Real(CIE64_F3Y);
      XYZ_illum[2] = Real(CIE64_F3Z);

    // Illuminant F4
    } else if (ref == "F4" && obs == CIE::kOBS_1931) {
      flag = CIE::kILLUM_F4;
      XYZ_illum[0] = Real(CIE31_F4X);
      XYZ_illum[1] = Real(CIE31_F4Y);
      XYZ_illum[2] = Real(CIE31_F4Z);
    } else if (ref == "F4" && obs == CIE::kOBS_1964) {
      flag = CIE::kILLUM_F4;
      XYZ_illum[0] = Real(CIE64_F4X);
      XYZ_illum[1] = Real(CIE64_F4Y);
      XYZ_illum[2] = Real(CIE64_F4Z);

    // Illuminant F5
    } else if (ref == "F5" && obs == CIE::kOBS_1931) {
      flag = CIE::kILLUM_F5;
      XYZ_illum[0] = Real(CIE31_F5X);
      XYZ_illum[1] = Real(CIE31_F5Y);
      XYZ_illum[2] = Real(CIE31_F5Z);
    } else if (ref == "F5" && obs == CIE::kOBS_1964) {
      flag = CIE::kILLUM_F5;
      XYZ_illum[0] = Real(CIE64_F5X);
      XYZ_illum[1] = Real(CIE64_F5Y);
      XYZ_illum[2] = Real(CIE64_F5Z);

    // Illuminant F6
    } else if (ref == "F6" && obs == CIE::kOBS_1931) {
      flag = CIE::kILLUM_F6;
      XYZ_illum[0] = Real(CIE31_F6X);
      XYZ_illum[1] = Real(CIE31_F6Y);
      XYZ_illum[2] = Real(CIE31_F6Z);
    } else if (ref == "F6" && obs == CIE::kOBS_1964) {
      flag = CIE::kILLUM_F6;
      XYZ_illum[0] = Real(CIE64_F6X);
      XYZ_illum[1] = Real(CIE64_F6Y);
      XYZ_illum[2] = Real(CIE64_F6Z);

    // Illuminant F7
    } else if (ref == "F7" && obs == CIE::kOBS_1931) {
      flag = CIE::kILLUM_F7;
      XYZ_illum[0] = Real(CIE31_F7X);
      XYZ_illum[1] = Real(CIE31_F7Y);
      XYZ_illum[2] = Real(CIE31_F7Z);
    } else if (ref == "F7" && obs == CIE::kOBS_1964) {
      flag = CIE::kILLUM_F7;
      XYZ_illum[0] = Real(CIE64_F7X);
      XYZ_illum[1] = Real(CIE64_F7Y);
      XYZ_illum[2] = Real(CIE64_F7Z);

    // Illuminant F8
    } else if (ref == "F8" && obs == CIE::kOBS_1931) {
      flag = CIE::kILLUM_F8;
      XYZ_illum[0] = Real(CIE31_F8X);
      XYZ_illum[1] = Real(CIE31_F8Y);
      XYZ_illum[2] = Real(CIE31_F8Z);
    } else if (ref == "F8" && obs == CIE::kOBS_1964) {
      flag = CIE::kILLUM_F8;
      XYZ_illum[0] = Real(CIE64_F8X);
      XYZ_illum[1] = Real(CIE64_F8Y);
      XYZ_illum[2] = Real(CIE64_F8Z);

    // Illuminant F9
    } else if (ref == "F9" && obs == CIE::kOBS_1931) {
      flag = CIE::kILLUM_F9;
      XYZ_illum[0] = Real(CIE31_F9X);
      XYZ_illum[1] = Real(CIE31_F9Y);
      XYZ_illum[2] = Real(CIE31_F9Z);
    } else if (ref == "F9" && obs == CIE::kOBS_1964) {
      flag = CIE::kILLUM_F9;
      XYZ_illum[0] = Real(CIE64_F9X);
      XYZ_illum[1] = Real(CIE64_F9Y);
      XYZ_illum[2] = Real(CIE64_F9Z);

    // Illuminant F10
    } else if (ref == "F10" && obs == CIE::kOBS_1931) {
      flag = CIE::kILLUM_F10;
      XYZ_illum[0] = Real(CIE31_F10X);
      XYZ_illum[1] = Real(CIE31_F10Y);
      XYZ_illum[2] = Real(CIE31_F10Z);
    } else if (ref == "F10" && obs == CIE::kOBS_1964) {
      flag = CIE::kILLUM_F10;
      XYZ_illum[0] = Real(CIE64_F10X);
      XYZ_illum[1] = Real(CIE64_F10Y);
      XYZ_illum[2] = Real(CIE64_F10Z);

    // Illuminant F11
    } else if (ref == "F11" && obs == CIE::kOBS_1931) {
      flag = CIE::kILLUM_F11;
      XYZ_illum[0] = Real(CIE31_F11X);
      XYZ_illum[1] = Real(CIE31_F11Y);
      XYZ_illum[2] = Real(CIE31_F11Z);
    } else if (ref == "F11" && obs == CIE::kOBS_1964) {
      flag = CIE::kILLUM_F11;
      XYZ_illum[0] = Real(CIE64_F11X);
      XYZ_illum[1] = Real(CIE64_F11Y);
      XYZ_illum[2] = Real(CIE64_F11Z);

    // Illuminant F12
    } else if (ref == "F12" && obs == CIE::kOBS_1931) {
      flag = CIE::kILLUM_F12;
      XYZ_illum[0] = Real(CIE31_F12X);
      XYZ_illum[1] = Real(CIE31_F12Y);
      XYZ_illum[2] = Real(CIE31_F12Z);
    } else if (ref == "F12" && obs == CIE::kOBS_1964) {
      flag = CIE::kILLUM_F12;
      XYZ_illum[0] = Real(CIE64_F12X);
      XYZ_illum[1] = Real(CIE64_F12Y);
      XYZ_illum[2] = Real(CIE64_F12Z);
    }
  }
}
////////////////////// class V2ColorHandlerParser //////////////////////////////
void V2ColorHandlerParser::GetChrAdaptInfo(XMLTree* node, 
                                           CIE::ChrAdaptMethod& flag) {
  // Method for chromatic adaption 
  if (node->getAttributeValue("method") != "") {
    std::string method = node->getAttributeValue("method");
    if (method == "Scale") {
      flag = CIE::kCHR_XYZ_SCALING;
    } else if (method == "Bradford") {
      flag = CIE::kCHR_BRADFORD;
    } else if (method == "VonKries") {
      flag = CIE::kCHR_VONKRIES;
    } else if (method == "CIECAT02") {
      flag = CIE::kCHR_CIECAT02;
    }
  }   
}
////////////////////// class V2ColorHandlerParser //////////////////////////////
void V2ColorHandlerParser::GetWhitePointFromIlluminant(Spectrum* observer, 
                                                       Spectrum& illum, 
                                                       Point& white) {
  white[0] = white[1] = white[2] = 0;
  for (unsigned int wl = 0; wl < GlobalSpectrum::nbWaveLengths(); wl++) {
    // X component
    white[0] += observer[0][wl] * illum[wl];
    // Y component
    white[1] += observer[1][wl] * illum[wl];
    // Z component
    white[2] += observer[2][wl] * illum[wl];
  }

  // Normalization
  if (white[1] != 0) {
    white[0] /= white[1];
    white[2] /= white[1];
    white[1] = Real(1.0);
  }
}
////////////////////////////////////////////////////////////////////////////////