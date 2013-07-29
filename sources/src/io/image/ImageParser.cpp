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

#include <io/image/ImageParser.hpp>
#include <io/image/RGBImageParser.hpp>
#include <io/image/MHDRImageParser.hpp>
#include <exceptions/Exception.hpp>
#include <iostream>

/**
 * Load an image from a file. The file type will be determined by the
 * extension.
 * @param filename : the filename of the file that contain the image
 */
Image* ImageParser::load(std::string filename)
{
  if(filename.compare(filename.length()-6, 6, ".mhdri")==0)
  {
    MHDRImageParser parser;
    return parser.load(filename);
  }
  else
  {
    RGBImageParser parser;
    return parser.load(filename);
  }
}

/**
 * Save an image to a file. The file type will be determined by the
 * extension.
 * @param image : the Image to save
 * @param filename : the filename of the file to save
 */
void ImageParser::save(Image& image, std::string filename)
{
  if(filename.compare(filename.length()-6, 6, ".mhdri")==0)
  {
    MHDRImageParser parser;
    parser.save(image, filename);
  }
  else
  {
    RGBImageParser parser;
    parser.save(image, filename);
  }
}
