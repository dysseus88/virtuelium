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
#ifndef _COLORHANDLER_HPP
#define _COLORHANDLER_HPP

#include <core/LightBase.hpp>

#include <structures/Image.hpp>

/**
 * This interface rules the objects that will convert light data (spectral, 
 * polarised...) into RGB values in order to build the final image.
 * @author Sylvain "Greewi" Dumazet
 */
class ColorHandler{
public :
  /**
   * Virtual destructor.
   */
  virtual inline ~ColorHandler();
  
  /**
   * This function convert a light data into an image color.
   * @param lightdata : the light data to convert
   * @param color : the array that will contain the converted image color.
   */
  virtual void lightDataToRGB(const LightVector& lightdata, Pixel& color)=0;

  /**
   * Return the number of channels of the computable image.
   */
  virtual inline unsigned int getNumberOfChannels();

  /**
   * Return the name of the i-th channel.
   */
  virtual inline std::string getChannelName(unsigned int i);
};

/**
 * Virtual destructor.
 */
inline ColorHandler::~ColorHandler()
{
  //  Nothing to do !
}

/**
 * Return the number of channels of the computable image.
 */
inline unsigned int ColorHandler::getNumberOfChannels()
{
  return 3;
}

/**
 * Return the name of the i-th channel.
 */
inline std::string ColorHandler::getChannelName(unsigned int i)
{
  switch(i)
  {
    case 0 : return "red";
    case 1 : return "green";
    case 2 : return "blue";
  }
  return "";
}

#endif //_COLORHANDLER_HPP
