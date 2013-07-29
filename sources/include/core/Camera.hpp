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
 
#ifndef _CAMERA_HPP
#define _CAMERA_HPP

#include <core/3DBase.hpp>
#include <core/LightBase.hpp>
#include <core/Scenery.hpp>

#include <renderers/Renderer.hpp>
#include <camerashapes/CameraShape.hpp>
#include <colorhandlers/ColorHandler.hpp>
#include <structures/Image.hpp>

class Camera{
public :
  /**
   * Default constructor
   */
  inline Camera();
  
  /**
   * Constructor of complete Camera.
   * shape : the CameraShape of this source
   * colorhandler : the ColorHandler of this source
   */
  inline Camera(CameraShape* shape, ColorHandler* colorhandler, const std::string& name, const std::string& output);

  /**
   * Destructor. Free also the renderer, the shape and the color representation (if any).
   */
  inline ~Camera();

  /**
   * Set the renderer, the shape and the color representation of this object. If
   * there were already one (or more) of theses properties, the previous will be
   * freed. It is possible to passe 0 for changing only one property.
   * shape : the CameraShape of this source
   * colorhandler : the ColorHandler of this source
   */
  inline void set(CameraShape* shape, ColorHandler* colorhandler, const std::string& name, const std::string& output);
  
  /**
   * Return the width of the computable image.
   */
  inline unsigned int getWidth() const;
  
  /**
   * Return the height of the computable image.
   */
  inline unsigned int getHeight() const;  

  /**
   * Return the number of channels of the computable image.
   */
  inline unsigned int getNumberOfChannels() const;

  /**
   * Return the name of the i-th channel.
   */
  inline std::string getChannelName(unsigned int i);

  /**
   * Return the name of the camera.
   */
  inline std::string getName();

  ColorHandler* getColorHandler(void) { return _colorhandler; }

  /**
   * Return the filename of the computable image.
   */
  inline std::string getOutputFilename();

  bool getRay(const int& i, const int& j, LightVector& ray);

  /**
   * Compute the image.
   * scenery : the scereny into which we have to compute the image
   * image : the computed image will be placed into this parameter.
   */
  void takeShot(Scenery& scenery, Image& image);
 
  /**
   * Compute a part of the image. The part is a rectangular shape defined by
   * the two point (minx, miny)-(maxx, maxy).
   * scenery : the scereny into which we have to compute the image
   * minx, miny, maxx, maxy : boundaries of the part to compute.
   * image : the computed image will be placed into this parameter.
   */
  void takeShot(Scenery& scenery, unsigned int minx, unsigned int maxx, unsigned int miny, unsigned int maxy, Image& image);

  void local_takeshot(Scenery& scenery, 
                      unsigned int local_minx, unsigned int local_maxx, 
                      unsigned int local_miny, unsigned int local_maxy, 
                      Image& image);
  
private :
  CameraShape* _shape;
  ColorHandler* _colorhandler;
  std::string _name;
  std::string _output;
};

/**
 * Default constructor
 */
inline Camera::Camera()
: _shape(0), _colorhandler(0)
{
  // Nothing to do more.
}
  
/**
 * Constructor of complete Camera.
 * renderer : the Renderer of this source
 * shape : the CameraShape of this source
 * colorhandler : the ColorHandler of this source
 */
inline Camera::Camera(CameraShape* shape, ColorHandler* colorhandler, const std::string& name, const std::string& output)
: _shape(shape), _colorhandler(colorhandler), _name(name), _output(output)
{
  // Nothing to do more.  
}

/**
 * Destructor. Free also the renderer, the shape and the color representation (if any).
 */
inline Camera::~Camera()
{
  if(_shape!=0)
    delete _shape;    
  if(_colorhandler!=0)
    delete _colorhandler;    
}

/**
 * Set the renderer, the shape and the color representation of this object. If
 * there were already one (or more) of theses properties, the previous will be
 * freed. It is possible to passe 0 for changing only one property.
 * renderer : the Renderer of this source
 * shape : the CameraShape of this source
 * colorhandler : the ColorHandler of this source
 */
inline void Camera::set(CameraShape* shape, ColorHandler* colorhandler, const std::string& name, const std::string& output)
{
  _name = name;
  _output=output;

  if(shape!=0)
  {
    if(_shape!=0)
      delete _shape;
    _shape = shape;
  }

  if(colorhandler!=0)
  {
    if(_colorhandler!=0)
      delete _colorhandler;
    _colorhandler = colorhandler;
  }
}

/**
 * Return the width of the computable image.
 */
inline unsigned int Camera::getWidth() const
{
  if(_shape==0)
    return 0;
  else
    return _shape->getWidth();
}
  
/**
 * Return the height of the computable image.
 */
inline unsigned int Camera::getHeight() const
{
  if(_shape==0)
    return 0;
  else
    return _shape->getHeight();
}

/**
 * Return the number of channels of the computable image.
 */
inline unsigned int Camera::getNumberOfChannels() const
{
  if(_colorhandler==0)
    return 0;
  else
    return _colorhandler->getNumberOfChannels();
}

/**
 * Return the name of the i-th channel.
 */
inline std::string Camera::getChannelName(unsigned int i)
{
  if(_colorhandler==0)
    return 0;
  else
    return _colorhandler->getChannelName(i);
}

/**
 * Return the name of the camera.
 */
inline std::string Camera::getName()
{
  return _name;  
}

/**
 * Return the filename of the computable image.
 */
inline std::string Camera::getOutputFilename()
{
  return _output;
}

#endif //_CAMERA_HPP
