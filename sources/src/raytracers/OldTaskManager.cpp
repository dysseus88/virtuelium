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

#include <iostream>

#include "raytracers/OldTaskManager.hpp"
#include "io/image/ImageParser.hpp"

#include "exceptions/Exception.hpp"

/**
* Constructor
* @param scenery : the scenery to render
*/
TaskManager::TaskManager(Scenery* scenery, unsigned int xmin, unsigned int xmax, unsigned int ymin, unsigned int ymax)
	: _scenery(scenery), _currentImage(0), 
	_xmin(xmin), _xmax(xmax), _ymin(ymin), _ymax(ymax)
{
	initCamera(0);
}

/**
* Destructor
*/
TaskManager::~TaskManager()
{
	if(_currentImage!=0)
		delete _currentImage;
}

/**
* Commit a band and add it to the final image.
* @return : true if the format of the band is the one that is wanted.
*/  
bool TaskManager::commitBand(float* data, int camera, int line, int size)
{
	//If this is an old band, dismiss it.
	if(camera!=_currentCamera)
		return true;

	//Verify the size parameter and the line number
	if(_sizeOfBand!=size || line<0 || line>=(int)_currentImage->getHeight())
		return false;

	//Update image
	updateImage(data, line);

	//If the image is finished, save it and process the next image
	if(_currentLines.empty())
	{
		//Save the image
		ImageParser parser;
		parser.save(*_currentImage, _scenery->getCamera(_currentCamera)->getOutputFilename());

		//Start the processing of the next camera
		initCamera(_currentCamera+1);
	}

	return true;
}

/**
* Return the next band to compute.
* @param camera : the camera number of the band will be returned in this
*   parameter.
* @param camera : the line number of the band will be returned in this 
*   parameter.
* @param camera : the size of the band will be returned in this parameter.
* @return : true if there is an other band to render.
*/
bool TaskManager::nextBand(int& camera, int& line, int& size)
{
	if(workFinished())
		return false;

	//Get the camera number
	camera = _currentCamera;

	//Get the next line number to compute
	line = _currentLines.front();
	_currentLines.pop_front();
	_currentLines.push_back(line);

	//Get the size of a band
	size = _sizeOfBand;

	return true;
}

/**
* Return true if all images where computed.
* @return : true if all images where computed.
*/
bool TaskManager::workFinished()
{
	return _currentCamera >= (int)_scenery->getNbCamera();
}

/**
* Initialize the computation for the given camera.
* @param camera : the camera number to compute.
*/
void TaskManager::initCamera(int camera_number)
{
	if(_currentImage!=0)
		delete _currentImage;
	_currentImage = 0;
	_currentLines.clear();

	//Set the current camera
	_currentCamera = camera_number;
	if(workFinished())
		return;

	Camera* camera = _scenery->getCamera(_currentCamera);

	//Initialize the list of lines to compute
	//for(unsigned int i=0; i<camera->getHeight(); i++)
	for(unsigned int i=_ymin; i<=_ymax; i++)
		_currentLines.push_back(i);

	//Initialize the image to compute
	//_currentImage = new Image(camera->getWidth(), camera->getHeight(), camera->getNumberOfChannels());
	//for(unsigned int i=0; i<camera->getNumberOfChannels(); i++)
	//	_currentImage->setChannelName(i, camera->getChannelName(i));
	//_currentImage->clear();
	ImageParser parser;
	try
	{
		_currentImage = parser.load( _scenery->getCamera(_currentCamera)->getOutputFilename() );
	}
	catch(Exception exc)
	{
		_currentImage = new Image(camera->getWidth(), camera->getHeight(), camera->getNumberOfChannels());
		
		for(unsigned int i=0; i<camera->getNumberOfChannels(); i++)
			_currentImage->setChannelName(i, camera->getChannelName(i));

		_currentImage->clear();
	}


	//Initialize the size of a band
	_sizeOfBand = camera->getWidth()*camera->getNumberOfChannels();
}

/**
* Update the image with the given band.
* @param band : the data of the band to commit
* @param line : the line to update
*/
void TaskManager::updateImage(float* band, int line)
{
	//Update the line data in the image
	//float* addressOfLine = (_currentImage->getRaster() + _sizeOfBand*line);
	//memcpy(addressOfLine, band, _sizeOfBand*sizeof(float));

	size_t startColumn = _xmin * _scenery->getCamera(_currentCamera)->getNumberOfChannels();
	size_t columnSize = (_xmax - _xmin) * _scenery->getCamera(_currentCamera)->getNumberOfChannels();

	float* addressOfLine = (_currentImage->getRaster() + _sizeOfBand*line + startColumn);
	float* adressOfBand = band;//(band + startColumn);
	
	memcpy(addressOfLine, adressOfBand, columnSize*sizeof(float));

	//Remove the line from the 
	for(std::list<unsigned int>::iterator i = _currentLines.begin(); i != _currentLines.end(); i++)
		if((int)*i == line)
		{
			_currentLines.erase(i);
			break;
		}

		//Print progress
		//std::cout << "\rCamera " << _currentCamera << " : " << 100 - (100*_currentLines.size())/_currentImage->getHeight() << "%        ";
		std::cout << "\rCamera " << _currentCamera << " : " << 100 - (100*_currentLines.size())/(_ymax - _ymin) << "%        ";
		std::cout.flush();

		//Update image
		if(_currentLines.size()  % (1+_currentImage->getHeight()/100) == 0)
		{
			ImageParser parser;
			parser.save(*_currentImage, _scenery->getCamera(_currentCamera)->getOutputFilename());
		}
}

/**
* Export the data of the renderer.
* @param data : data will point on an array that will contain the data
* @param datasize : the length of the data array
*/
void TaskManager::exportRendererData(unsigned char** data, unsigned int* datasize)
{
	_scenery->getRenderer()->ExportData(data, datasize);
}

