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

#include <io/image/RGBImageParser.hpp>
#include <exceptions/Exception.hpp>
#include <IL/il.h>
#include <IL/ilu.h>
#include <iostream>

bool RGBImageParser::_init=false;

/**
 * Constructor of the parser
 */
RGBImageParser::RGBImageParser()
{
  if(!RGBImageParser::_init)
  {
    ilInit();
    ilEnable(IL_FILE_OVERWRITE);
    //ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
    //ilEnable(IL_ORIGIN_SET);
    RGBImageParser::_init=true;
  }
}

/**
 * Load an image from a file.
 * filename : the filename of the file that contain the image
 */
Image* RGBImageParser::load(std::string filename)
{
  //Creating DevIL image name
  ILuint ImageName;
  ilGenImages(1, &ImageName);
  ilBindImage(ImageName);

  //Open the file
  if(!ilLoadImage((char*)filename.c_str()))
  {
    ilDeleteImages(1, &ImageName);
    throw Exception("(RGBImageParser::loadImage)Echec de l'ouverture du fichier "+filename);
  }


  //Create the raster
  ILuint width  = ilGetInteger(IL_IMAGE_WIDTH);
  ILuint height = ilGetInteger(IL_IMAGE_HEIGHT);
  unsigned char* raster = new unsigned char[width*height*3];

  //Load the raster
  ilCopyPixels(0, 0, 0, width, height, 1, IL_RGB, IL_UNSIGNED_BYTE, raster);

  //Converting the raster from unsigned char to float format
  float* fraster = new float[width*height*3];
  for(unsigned int i=0; i<width*height*3; i++)
    fraster[i] = (float)raster[i]/255.0;
  delete[] raster;

  //Create the image
  Image* image = new Image(width, height, 3, fraster);

  //Free DevIL data
  ilDeleteImages(1, &ImageName);

  //Done !
  return image;
}

/**
 * Save an image to a file.
 * filename : the filename of the file that contain the image
 */
void RGBImageParser::save(Image& image, std::string filename)
{
  //Creating DevIL image name
  ILuint ImageName;
  ilGenImages(1, &ImageName);
  ilBindImage(ImageName);

  //Converting the raster from float to unsigned char format
  ILuint width  = image.getWidth();
  ILuint height = image.getHeight();
  unsigned char* raster = new unsigned char[width*height*3];
  for(unsigned int i=0; i<width*height; i++)
  {
    Pixel pixel = image.getPixel(i%width, i/width);
    for(unsigned int k=0; k<3; k++)
    {
      float data = (k<image.getNumberOfChannels())? pixel[k]*255.0 : 0.0;
      if(data<0) data=0.0;
      if(data>255) data=255.0;
      raster[i*3 + k] = (unsigned char)data;
    }
  }
  
  //Creating DevIL image
  ilTexImage(width, height, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, raster);
  iluFlipImage();
  delete[] raster;

  //Saving the file
  if(!ilSaveImage((char*)filename.c_str()))
  {
    ilDeleteImages(1, &ImageName);
    throw Exception("(RGBImageParser::saveImage)Echec de la sauvegarde du fichier "+filename);
  }
  
  //Free DevIL data
  ilDeleteImages(1, &ImageName);
}

