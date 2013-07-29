/*
*  Copyright 2012 Sylvain "Greewi" Dumazet, Remi Cerise
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
*
*/

#ifndef _SPECTROPHOTOMETER_HPP
#define _SPECTROPHOTOMETER_HPP

#include <string>
//#include <pthread.h>

#include <core/LightBase.hpp>
#include <colorhandlers/ColorHandler.hpp>

/**
* @class	Spectrophotometer
* @author	Sylvain "Greewi" Dumazet 
* @date	2008 
* @version	1.0 
*
* This converter can measure and save the integrated spectrum of the calculated image.
* To really save the image, it is possible to encapsulate another color handler as a child of this one.
*/
class Spectrophotometer: public ColorHandler{
public :
	/**
	* @author	Sylvain "Greewi" Dumazet 
	* @date	2008 
	* @version	1.0 
	*
	* Standard constructor which initializes the handler (without white point)
	*
	* @param[in]	resultFilename	Output file name
	* @param[in]	nbSample		The number of sample to integrate
	* @param[in]	colorHandler	Child color handler
	*/
	Spectrophotometer(const std::string& resultFilename, unsigned int nbSample=1, ColorHandler* colorHandler=0);

	/**
	* @author	Sylvain "Greewi" Dumazet 
	* @date	2008 
	* @version	1.0 
	*
	* Virtual destructor.
	*/
	virtual ~Spectrophotometer();

	/**
	* @brief	Light data to RGB
	* @author	Sylvain "Greewi" Dumazet 
	* @date		2008 
	* @version	1.0 
	*
	* This function convert a light data into an image color.
	*
	* @param[in]  lightdata		The light data to convert.
	* @param[out] color			The array that will contain the converted image color.
	*/
	virtual void lightDataToRGB(const LightVector& lightdata, Pixel& color);

	/**
	* @brief	Number of channels
	* @author	Sylvain "Greewi" Dumazet 
	* @date		2008 
	* @version	1.0 
	*
	* Get the number of channels of the computable image.\n
	*
	* @return	The number of channels for the child color handler.
	*/
	virtual unsigned int getNumberOfChannels();

	/**
	* @brief	Name of a channel
	* @author	Sylvain "Greewi" Dumazet 
	* @date		2008 
	* @version	1.0 
	*
	* Get the name of the i-th channel of the computable image.\n
	*
	* @param[in] i		Index of the channel 
	* @return		The name of the channel for the child color handler.
	*/
	virtual std::string getChannelName(unsigned int i);

public :
	///
	/// Child color handler
	///
	ColorHandler* _colorHandler;
	///
	/// Output file name
	///
	std::string _resultFilename;
	///
	/// The number of sample to integrate
	///
	unsigned int _nbSample;
	///
	/// result Spectrum
	///
	Spectrum _result;
	///
	/// The number of sample to integrate
	///
	unsigned int _samples;
	
	/////
	///// count the number of samples (used during integration)
	/////
	//pthread_mutex_t _mutex;     
};

#endif //_SPECTROPHOTOMETER_HPP
