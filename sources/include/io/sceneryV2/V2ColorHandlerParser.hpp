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
#ifndef GUARD_VRT_V2COLORHANDLERPARSER_HPP
#define GUARD_VRT_V2COLORHANDLERPARSER_HPP
//!
//! @file V2ColorHandlerParser.hpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @remarks
//! @details This file describes how a <colorhandler> node is parsing from XML
//!
#include <io/XMLTree.hpp>
#include <io/Parser.hpp>

#include <colorhandlers/ColorHandler.hpp>
#include <colorhandlers/CIEBase.hpp>
////////////////////////////////////////////////////////////////////////////////
//! @class V2ColorHandlerParser
//! @brief Describes how a <colorhandler> node is parsing from XML
class V2ColorHandlerParser : public Parser {
 public:
  //! @brief Create the color handler by using the informations of the node
  //! @param node XML node to use for the creation
  //! @return ColorHandler object ready to be used for color conversion
  ColorHandler* create(XMLTree* node);

 private:
  //! @brief Create a Simple-RGB color handler object
  //! @return ColorHandler object ready to be used for color conversion
  ColorHandler* createSimpleRGBColorHandler(void);
  //! @brief Create a Spectrum color handler object
  //! @return ColorHandler object ready to be used for color conversion
  ColorHandler* createSpectrumColorHandler(void);
  //! @brief Create a Polarization color handler object
  //! @param node XML node to use for the creation
  //! @return ColorHandler object ready to be used for color conversion
  ColorHandler* createPolarizationColorHandler(XMLTree* node);
  //! @brief Create a CIE color handler object
  //! @param node XML node to use for the creation
  //! @return ColorHandler object ready to be used for color conversion
  ColorHandler* createCIE(XMLTree* node);
  ////! @brief Create a CIEChromaADapt color handler object
  ////! @param node XML node to use for the creation
  ////! @return ColorHandler object ready to be used for color conversion
  //ColorHandler* createCIEChromaAdapter(XMLTree* node);
  //! @brief Create a RGB color handler object
  //! @param node XML node to use for the creation
  //! @return ColorHandler object ready to be used for color conversion
  ColorHandler* createRGB(XMLTree* node);
  //! @brief Create a Spectrophotomer color handler object
  //! @param node XML node to use for the creation
  //! @return ColorHandler object ready to be used for color conversion
  ColorHandler* createSpectrophotometer(XMLTree* node);
  //! @brief Create a Linear-Polarizer color handler object
  //! @param node XML node to use for the creation
  //! @return ColorHandler object ready to be used for color conversion
  ColorHandler* createLinearPolarizer(XMLTree* node);

 private:
  //! @brief Retrieve CIE observer info from the node
  //! @param node XML node to use for the creation
  //! @param filename Output file to be read
  //! @param flag Output flag used to create the object
  void GetObserverInfo(XMLTree* node, std::string& filename, 
                       CIE::Observer& flag);
  //! @brief Retrieve CIE illuminant info (in) from the node
  //! @param node XML node to use for the creation
  //! @param obs reference observer used
  //! @param XYZ_illum XYZ Components of the illuminant
  //! @param illum_file File containing the spectral illuminant
  //! @param flag Output flag used to create the object
  void GetInputIllumInfo(XMLTree* node, const CIE::Observer& obs,
                         Point& XYZ_illum, std::string& illum_file,
                         CIE::Illuminant& flag);
  //! @brief Retrieve CIE illuminant info (out) from the node
  //! @param node XML node to use for the creation
  //! @param XYZ_illum XYZ Components of the illuminant
  //! @param illum_file File containing the spectral illuminant
  //! @param flag Output flag used to create the object
  void GetOutputIllumInfo(XMLTree* node, const CIE::Observer& obs,
                          Point& XYZ_illum, std::string& illum_file,
                          CIE::Illuminant& flag);
  //! @brief Retrieve the model for chromatic adaptation
  //! @param node XML node to use for the creation
  //! @param flag Output flag used to create the object
  void GetChrAdaptInfo(XMLTree* node, CIE::ChrAdaptMethod& flag);

  //! @brief Convert a spectral illuminant to an XYZ component
  //! @param standard_observer Standard observer (array of 3 spectra)
  //! @param illum Given spectral illuminant
  //! @param white Return XYZ white point
  void GetWhitePointFromIlluminant(Spectrum* observer, Spectrum& illum, 
                                   Point& white);
}; // class V2ColorHandlerParser
////////////////////////////////////////////////////////////////////////////////
#endif //GUARD_VRT_V2COLORHANDLERPARSER_HPP
