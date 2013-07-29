/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
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

#ifndef _RGBCOLORHANDLER_HPP
#define _RGBCOLORHANDLER_HPP

#include <colorhandlers/ColorHandler.hpp>
#include <core/LightBase.hpp>


/**
 * This class is the "natural" color converter. It convert light spectrum into
 * RGB according a standard observer (CIE-XYZ for example). Next, it convert this
 * tri-stimulus into RGB using a matrix (Most of the time it is a XYZ to RGB
 * with blanc point at D65).
 * @author Sylvain "Greewi" Dumazet
 */
class RGBColorHandler: public ColorHandler{
public :
  /**
   * Constructor
   * @param standardObserver : Standard observer (array of 3 spectra).
   */
  RGBColorHandler(Spectrum* standardObserver, Real gain);

  /**
   * Constructor
   * @param standardObserver : Standard observer (array of 3 spectra).
   */
  RGBColorHandler(Spectrum* standardObserver, Real gain, const Spectrum& white);

  /**
   * Virtual destructor.
   */
  virtual inline ~RGBColorHandler();
  
  /**
   * This function convert a light data into an RGB color.
   * lightdata : the light data to convert
   * color : the array that will contain the converted RGB color.
   */
  virtual void lightDataToRGB(const LightVector& lightdata, Pixel& color);

public :
  Spectrum* _standardObserver;  // Standard observer (array of 3 spectra)
  Real _gain[3];
};

/**
 * Virtual destructor.
 */
inline RGBColorHandler::~RGBColorHandler()
{
  if(_standardObserver!=0)
    delete[] _standardObserver;
}

#endif //_RGBCOLORHANDLER_HPP
