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

#include <core/Camera.hpp>
#include <core/VrtLog.hpp>
#include <iostream>

bool Camera::getRay(const int& i, const int& j, LightVector& ray) {
  //Generate the ray to cast
  Ray propagation;
  if(!_shape->getRay(i, j, propagation))
    return false;

  LightVector toCast;
  toCast.initSpectralData();
  toCast.setRay(propagation);
  
  if (propagation.v[2] < Real(1.0 - kEPSILON) 
       && propagation.v[2] > Real(-1.0 + kEPSILON)) {
    toCast.changeReemitedPolarisationFramework(Vector(0.0, 0.0, 1.0));
  } else {
    toCast.changeReemitedPolarisationFramework(Vector(1.0, 0.0, 0.0));
  }
  toCast.clear();

  return true; 
}

/**
 * Compute the image.
 * scenery : the scereny into which we have to compute the image
 * image : the computed image will be placed into this parameter.
 */
void Camera::takeShot(Scenery& scenery, Image& image)
{
  takeShot(scenery, 0, getWidth()-1, 0, getHeight()-1, image);
}

/**
 * Compute a part of the image. The part is a rectangular shape defined by
 * the two point (minx, miny)-(maxx, maxy).
 * scenery : the scereny into which we have to compute the image
 * minx, miny, maxx, maxy : boundaries of the part to compute. These boundaries
 *   are include in the part.
 * image : the computed image will be placed into this parameter.
 */
void Camera::takeShot(Scenery& scenery, unsigned int minx, unsigned int maxx, unsigned int miny, unsigned int maxy, Image& image)
{
  ////Initializing the camera
  //image.clear();

  //Computing the image
  for(unsigned int j=miny; j<=maxy; j++)
  {
    for(unsigned int i=minx; i<=maxx; i++)
    {
      //Generate the ray to cast
      Ray propagation;
      if(!_shape->getRay(i, j, propagation))
        continue;

      LightVector toCast;
      toCast.initSpectralData();
      toCast.setRay(propagation);
      if(propagation.v[2]<0.999 && propagation.v[2]>-0.999)
        toCast.changeReemitedPolarisationFramework(Vector(0.0, 0.0, 1.0));
      else
        toCast.changeReemitedPolarisationFramework(Vector(1.0, 0.0, 0.0));
      toCast.clear();
      
      //Compute the light data
      scenery.getRenderer()->CastRay(scenery, toCast);
      
      //// White spectrum
      //for (unsigned int i = 0; i < GlobalSpectrum::nbWaveLengths(); i++) {
      //  toCast[i].setRadiance(1.0);
      //}

      //// Red spectrum
      //toCast[	0	].setRadiance(0.0278971176);
      //toCast[	1	].setRadiance(0.0259378349);
      //toCast[	2	].setRadiance(0.019931316);
      //toCast[	3	].setRadiance(0.0184205469);
      //toCast[	4	].setRadiance(0.0226859776);
      //toCast[	5	].setRadiance(0.033635829);
      //toCast[	6	].setRadiance(0.0281320507);
      //toCast[	7	].setRadiance(0.0271486275);
      //toCast[	8	].setRadiance(0.0345541878);
      //toCast[	9	].setRadiance(0.0277747692);
      //toCast[	10	].setRadiance(0.0204073348);
      //toCast[	11	].setRadiance(0.0228811087);
      //toCast[	12	].setRadiance(0.0097062957);
      //toCast[	13	].setRadiance(0.028173591);
      //toCast[	14	].setRadiance(0.0198739566);
      //toCast[	15	].setRadiance(0.0043229038);
      //toCast[	16	].setRadiance(0.0036012758);
      //toCast[	17	].setRadiance(0.0375527242);
      //toCast[	18	].setRadiance(0.0213658379);
      //toCast[	19	].setRadiance(0.0240974041);
      //toCast[	20	].setRadiance(0.0243731578);
      //toCast[	21	].setRadiance(0.0186610157);
      //toCast[	22	].setRadiance(0.009560532);
      //toCast[	23	].setRadiance(0.0112358896);
      //toCast[	24	].setRadiance(0.0229016676);
      //toCast[	25	].setRadiance(0.0295088681);
      //toCast[	26	].setRadiance(0.0153541681);
      //toCast[	27	].setRadiance(0.0362286556);
      //toCast[	28	].setRadiance(0.0407555103);
      //toCast[	29	].setRadiance(0.0187601052);
      //toCast[	30	].setRadiance(0.0265306453);
      //toCast[	31	].setRadiance(0.0289827876);
      //toCast[	32	].setRadiance(0.0213722227);
      //toCast[	33	].setRadiance(0.0362027932);
      //toCast[	34	].setRadiance(0.038903168);
      //toCast[	35	].setRadiance(0.0589730873);
      //toCast[	36	].setRadiance(0.0709136185);
      //toCast[	37	].setRadiance(0.1094406484);
      //toCast[	38	].setRadiance(0.1413359343);
      //toCast[	39	].setRadiance(0.1653855654);
      //toCast[	40	].setRadiance(0.1817860778);
      //toCast[	41	].setRadiance(0.2022811898);
      //toCast[	42	].setRadiance(0.2354916955);
      //toCast[	43	].setRadiance(0.2494296968);
      //toCast[	44	].setRadiance(0.2969206582);
      //toCast[	45	].setRadiance(0.5093232331);
      //toCast[	46	].setRadiance(0.4125072122);
      //toCast[	47	].setRadiance(0.4924117578);
      //toCast[	48	].setRadiance(0.5675669343);
      //toCast[	49	].setRadiance(0.6370166096);
      //toCast[	50	].setRadiance(0.7273272473);
      //toCast[	51	].setRadiance(0.7637626494);
      //toCast[	52	].setRadiance(0.7950393348);
      //toCast[	53	].setRadiance(0.7932438627);
      //toCast[	54	].setRadiance(0.8096393249);
      //toCast[	55	].setRadiance(0.8273970708);
      //toCast[	56	].setRadiance(0.8654898123);
      //toCast[	57	].setRadiance(0.8835114537);
      //toCast[	58	].setRadiance(0.8733406842);
      //toCast[	59	].setRadiance(0.874743088);
      //toCast[	60	].setRadiance(0.9073443877);
      //toCast[	61	].setRadiance(0.9618911769);
      //toCast[	62	].setRadiance(0.9455384975);
      //toCast[	63	].setRadiance(0.9191804379);
      //toCast[	64	].setRadiance(0.9295664426);
      //toCast[	65	].setRadiance(0.9628564148);
      //toCast[	66	].setRadiance(0.9846458873);
      //toCast[	67	].setRadiance(0.9856462563);
      //toCast[	68	].setRadiance(0.9613628294);
      //toCast[	69	].setRadiance(0.965389345);
      //toCast[	70	].setRadiance(1);
      //toCast[	71	].setRadiance(1);
      //toCast[	72	].setRadiance(1);
      //toCast[	73	].setRadiance(1);
      //toCast[	74	].setRadiance(1);
      //toCast[	75	].setRadiance(1);
      //toCast[	76	].setRadiance(1);
      //toCast[	77	].setRadiance(1);
      //toCast[	78	].setRadiance(1);
      //toCast[	79	].setRadiance(1);
      //toCast[	80	].setRadiance(1);

      //Project the light data into the color representation and update the image
      Pixel pixel(getNumberOfChannels());
      _colorhandler->lightDataToRGB(toCast, pixel);
      image.setPixel(i, j, pixel);
    }
  }
}

void Camera::local_takeshot(Scenery& scenery, 
                            unsigned int local_minx, 
                            unsigned int local_maxx, 
                            unsigned int local_miny, 
                            unsigned int local_maxy, 
                            Image& image) {
  //Computing the image
  for(unsigned int j = local_miny; j <= local_maxy; j++) {
    for(unsigned int i = local_minx; i <= local_maxx; i++) {
      //Generate the ray to cast
      Ray propagation;
      if(!_shape->getRay(i, j, propagation))
        continue;

      LightVector toCast;
      toCast.initSpectralData();
      toCast.setRay(propagation);

      if(propagation.v[2] < (Real(1.0) - kEPSILON) 
          && propagation.v[2] > (Real(-1.0) + kEPSILON)) {
        toCast.changeReemitedPolarisationFramework(Vector(0.0, 0.0, 1.0));
      } else {
        toCast.changeReemitedPolarisationFramework(Vector(1.0, 0.0, 0.0));
      }
      toCast.clear();
      
      //Compute the light data
      scenery.getRenderer()->CastRay(scenery, toCast);
      
      Pixel pixel(getNumberOfChannels());
      _colorhandler->lightDataToRGB(toCast, pixel);
      image.setPixel(i - local_minx, j - local_miny, pixel);
    }
  }
}

