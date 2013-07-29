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

#ifndef _LinearPolarizerColorHandler_HPP
#define _LinearPolarizerColorHandler_HPP

#include <colorhandlers/ColorHandler.hpp>
#include <core/LightBase.hpp>


/**
 * This class show the polarisation of the light at the last bounce according to
 * incident plane.
 * @author Sylvain "Greewi" Dumazet
 */
class LinearPolarizerColorHandler: public ColorHandler{
public :
  /**
   * Constructor
   */
  LinearPolarizerColorHandler(const Vector& up, ColorHandler* colorHandler);

  /**
   * Virtual destructor.
   */
  virtual inline ~LinearPolarizerColorHandler();
  
  /**
   * This function convert a light data into an RGB color.
   * @param lightdata : the light data to convert
   * @param color : the array that will contain the converted RGB color.
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

private :
  Vector _up;
  ColorHandler* _colorHandler;
};

/**
 * Virtual destructor.
 */
inline LinearPolarizerColorHandler::~LinearPolarizerColorHandler()
{
  delete _colorHandler;
}

#endif //_LinearPolarizerColorHandler_HPP
