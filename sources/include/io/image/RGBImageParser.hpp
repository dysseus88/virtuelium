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
#ifndef _RGBIMAGEPARSER_HPP
#define _RGBIMAGEPARSER_HPP

#include <string>
#include <structures/Image.hpp>

/**
 * This is an image parser. It load a raster image from a file an create an 
 * Image object. It can also save an Image into a file.
 *
 * @author Sylvain "Greewi" Dumazet
 */
class RGBImageParser{
public :
  /**
   * Constructor
   */
  RGBImageParser();

  /**
   * Load an image from a file. The file type will be determined by the
   * extension.
   * @param filename : the filename of the file that contain the image
   */
  Image* load(std::string filename);

  /**
   * Save an image to a file. The file type will be determined by the
   * extension.
   * @param image : the Image to save
   * @param filename : the filename of the file to save
   */
  void save(Image& image, std::string filename);
private :
  static bool _init;
};

#endif //_IMAGEPARSER_HPP
