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
#ifndef _SPECTRUMCOLORHANDLER_HPP
#define _SPECTRUMCOLORHANDLER_HPP

#include <colorhandlers/ColorHandler.hpp>
#include <core/LightBase.hpp>


class SpectrumColorHandler: public ColorHandler{
public :
  /**
   * Virtual destructor.
   */
  virtual inline ~SpectrumColorHandler();
  
  /**
   * This function convert a light data into an image color.
   * @param lightdata : the light data to convert
   * @param color : the array that will contain the converted image color.
   */
  virtual void lightDataToRGB(const LightVector& lightdata, Pixel& color);

  /**
   * Return the number of channels of the computable image.
   */
  virtual unsigned int getNumberOfChannels();

  /**
   * Return the name of the i-th channel.
   */
  virtual std::string getChannelName(unsigned int i);
};

/**
 * Virtual destructor.
 */
inline SpectrumColorHandler::~SpectrumColorHandler()
{
  //  Nothing to do !
}

#endif //_SPECTRUMCOLORHANDLER_HPP
