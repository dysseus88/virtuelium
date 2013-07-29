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

#ifndef _SIMPLERGBCOLORHANDLER_HPP
#define _SIMPLERGBCOLORHANDLER_HPP

#include <colorhandlers/ColorHandler.hpp>
#include <core/LightBase.hpp>


/**
 * This is the most simple color representation. It juste take the three first
 * spectral bande and put it int the BGR channels.
 * @author Sylvain "Greewi" Dumazet
 */
class SimpleRGBColorHandler: public ColorHandler{
public :
  /**
   * Virtual destructor.
   */
  virtual inline ~SimpleRGBColorHandler();
  
  /**
   * This function convert a light data into an RGB color.
   * @param lightdata : the light data to convert
   * @param color : the array that will contain the converted RGB color.
   */
  virtual void lightDataToRGB(const LightVector& lightdata, Pixel& color);
};

/**
 * Virtual destructor.
 */
inline SimpleRGBColorHandler::~SimpleRGBColorHandler()
{
  //  Nothing to do !
}

#endif //_SIMPLERGBCOLORHANDLER_HPP
