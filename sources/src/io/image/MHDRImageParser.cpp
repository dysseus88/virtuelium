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

#include <io/image/MHDRImageParser.hpp>
#include <exceptions/Exception.hpp>
#include <cstdio>

/**
 * Load an image from a file. The file type will be determined by the
 * extension.
 * @param filename : the filename of the file that contain the image
 */
Image* MHDRImageParser::load(std::string filename)
{
  //Open the file
  FILE* file = fopen(filename.c_str(), "r");
  if(file==NULL)
    throw Exception("(MHDRImageParser::saveImage)Echec de la lecture du fichier "+filename);

  //Read the header
  unsigned int width, height, channels;
  if(fscanf(file, "MHDRI\n%d\t%d\t%d\n", &width, &height, &channels)!=3){
    fclose(file);
    throw Exception("(MHDRImageParser::saveImage)Echec de la lecture du fichier "+filename);
  }

  //Read the channels names
  std::vector<std::string> channels_names(channels);
  for(unsigned int i=0; i<channels; i++)
  {
    char buffer[1024];
    if(fgets(buffer, 1024, file)==NULL){
      fclose(file);
      throw Exception("(MHDRImageParser::saveImage)Echec de la lecture du fichier "+filename);
    }
    channels_names[i]=buffer;
  }

  //Read the data
  unsigned int datanb = width*height*channels;
  float* raster = new float[datanb];
  if(fread(raster, sizeof(float), datanb, file)!=datanb){
    fclose(file);
    throw Exception("(MHDRImageParser::saveImage)Echec de la lecture du fichier "+filename);
  }

  //Close the file
  fclose(file);

  //Create the image
  Image* image = new Image(width, height, channels, raster);
  for(unsigned int i=0; i<channels; i++)
    image->setChannelName(i, channels_names[i]);

  return image;
}

/**
 * Save an image to a file. The file type will be determined by the
 * extension.
 * @param image : the Image to save
 * @param filename : the filename of the file to save
 */
void MHDRImageParser::save(Image& image, std::string filename)
{
  //Open the file
  FILE* file = fopen(filename.c_str(), "w");
  if(file==NULL)
    throw Exception("(MHDRImageParser::saveImage)Echec de la sauvegarde du fichier "+filename);

  //Write the header
  fprintf(file, "MHDRI\n%d\t%d\t%d\n", image.getWidth(), image.getHeight(), image.getNumberOfChannels());
  for(unsigned int i=0; i<image.getNumberOfChannels(); i++)
    fprintf(file, "%s\n", image.getChannelName(i).c_str());

  //Write the data
  float* raster = image.getRaster();
  unsigned int datanb = image.getWidth()*image.getHeight()*image.getNumberOfChannels();  
  if(fwrite(raster, sizeof(float), datanb, file)!=datanb){
    fclose(file);
    throw Exception("(MHDRImageParser::saveImage)Echec de la sauvegarde du fichier "+filename);
  }
  
  //Close the file
  fclose(file);
}

