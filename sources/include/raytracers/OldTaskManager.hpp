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

#ifndef _TASKMANAGER_HPP
#define _TASKMANAGER_HPP

#include <string>
#include <list>

#include <core/Scenery.hpp>
//#include "structures/TCPIPSocket.hpp"
#include <structures/Image.hpp>

class TaskManager{
public :
  /**
   * Constructor
   * @param scenery : the scenery to render
   */
  TaskManager(Scenery* scenery, unsigned int xmin, unsigned int xmax, unsigned int ymin, unsigned int ymax); 

  /**
   * Destructor
   */
  ~TaskManager();

  /**
   * Commit a band and add it to the final image.
   * @return : true if the format is good. False it the commited band is not in
   * the desired format.
   */  
  bool commitBand(float* data, int camera, int line, int size);

  /**
   * Return the next band to compute.
   * @param camera : the camera number of the band will be returned in this
   *   parameter.
   * @param camera : the line number of the band will be returned in this 
   *   parameter.
   * @param camera : the size of the band will be returned in this parameter.
   * @return : true if there is an other band to render.
   */
  bool nextBand(int& camera, int& line, int& size);

  /**
   * Return true if all images where computed.
   * @return : true if all images where computed.
   */
  bool workFinished();

  /**
   * Export the data of the renderer.
   * @param data : data will point on an array that will contain the data
   * @param datasize : the length of the data array
   */
  void exportRendererData(unsigned char** data, unsigned int* datasize);

private :
  Scenery* _scenery;
  int _currentCamera;
  std::list<unsigned int> _currentLines;
  Image* _currentImage;
 
  int _sizeOfBand;

  unsigned int _xmin;
  unsigned int _xmax;
  unsigned int _ymin;
  unsigned int _ymax;

  /**
   * Initialize the computation for the given camera.
   * @param camera : the camera number to compute.
   */
  void initCamera(int camera);

  /**
   * Update the image with the given band.
   * @param band : the data of the band to commit
   * @param line : the line to update
   */
  void updateImage(float* band, int line);
};


#endif //_TASKMANAGER_HPP
