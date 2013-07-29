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
#ifndef _RENDERERDATAPARSER_HPP
#define _RENDERERDATAPARSER_HPP

#include <renderers/Renderer.hpp>
#include <exceptions/Exception.hpp>
#include <fstream>

class RendererDataParser{
public :
  inline void load(Scenery& scenery, Renderer& renderer, const std::string& filename) const;
  inline void save(Renderer& renderer, const std::string& filename) const;
};


inline void RendererDataParser::load(Scenery& scenery, Renderer& renderer, const std::string& filename) const
{
  //open the file
  std::fstream file(filename.c_str(), std::fstream::in);
  if(!file.is_open)
    throw Exception("(RendererDataParser::load) Impossible de charger les données du moteur de rendu.");
  
  //Load the size of the data
  unsigned int datasize;
  file.read((char*)&datasize, sizeof(unsigned int));
  
  //Load the data
  unsigned char* data = 0;
  if(datasize>0){
    data = new unsigned char[datasize];
    file.read((char*)data, datasize);
  }

  //Initialize the renderer
  renderer.initWithData(scenery, data, datasize);

  //Close the file and free memory
  file.close();
  delete[] data;
}

inline void RendererDataParser::save(Renderer& renderer, const std::string& filename) const
{
  //Open the file
  std::fstream file(filename.c_str(), std::fstream::out);
  if(!file.is_open)
    throw Exception("(RendererDataParser::load) Impossible de charger les données du moteur de rendu.");

  //Get the data from the renderer
  unsigned int datasize;
  unsigned char* data = 0;
  renderer.exportData(&data, &datasize);

  //Save the data
  file.write((char*)&datasize, sizeof(unsigned int));
  if(datasize>0)
    file.write((char*)data, datasize);

  //Close the file and free memory
  file.close();
  delete[] data;
}

#endif //_RENDERERDATAPARSER_HPP
