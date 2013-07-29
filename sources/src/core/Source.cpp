/*
 *  Copyright 2013 Remi "Programmix" Cerise
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
#include <core/Source.hpp>
//!
//! @file Source.cpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file implements the class declared in 
//!  Source.hpp 
//!  @arg Source
//! @todo 
//! @remarks 
//!
#include <lightsources/LightSource.hpp>
#include <objectshapes/ObjectShape.hpp>
////////////////////////////////////////////////////////////////////////////////
Source::Source(void)
  : p_shape(NULL), p_source(NULL) { }
////////////////////////////////////////////////////////////////////////////////
Source::Source(LightSource* source, ObjectShape* shape)
  : p_shape(shape), p_source(source) { }
////////////////////////////////////////////////////////////////////////////////
Source::~Source(void) {
  if(p_shape != NULL) {
    delete p_shape;
    p_shape = NULL;
  }
  if(p_source != NULL) {
    delete p_source;
    p_source = NULL;
  }
}
////////////////////////////////////////////////////////////////////////////////
void Source::set(LightSource* source, ObjectShape* shape) {
  if(source != NULL) {
    if(p_source != NULL)
      delete p_source;
    p_source = source;
  }

  if(shape != NULL) {
    if(p_shape != NULL)
      delete p_shape;
    p_shape = shape;
  }
}
////////////////////////////////////////////////////////////////////////////////
void Source::getIncidentLight(const Point& receiver, 
                              const LightVector& reemited, 
                              std::vector<LightVector>& incidents) {
  if(p_source != NULL)
    p_source->getIncidentLight(receiver, reemited, incidents);
}
////////////////////////////////////////////////////////////////////////////////
Real Source::getPower(void) {
  if(p_source != NULL)
    return p_source->getPower();
  return 0.0;
}
////////////////////////////////////////////////////////////////////////////////
void Source::getEmittedLight(const Basis& localBasis, 
                             const Point2D& surfaceCoordinate, 
                             LightVector& emitted) {
  if(p_source != NULL)
    p_source->getEmittedLight(localBasis, surfaceCoordinate, emitted);
}
////////////////////////////////////////////////////////////////////////////////
void Source::getRandomPhoton(MultispectralPhoton& photon) {
  if(p_source != NULL)
    p_source->getRandomPhoton(photon);
}
////////////////////////////////////////////////////////////////////////////////
bool Source::intersect(const Ray& ray, Real& distance) {
  if(p_shape != NULL)
    return p_shape->intersect(ray, distance);
  return false;
}
////////////////////////////////////////////////////////////////////////////////
void Source::getLocalBasis(const Ray& ray, const Real& distance, 
                           Basis& localBasis, Point2D& surfaceCoordinate) {
  if(p_shape != NULL)
    p_shape->getLocalBasis(ray, distance, localBasis, surfaceCoordinate);  
}
////////////////////////////////////////////////////////////////////////////////
void Source::getBoundingBox(BoundingBox& boundingBox) {
  if(p_shape != NULL)
    p_shape->getBoundingBox(boundingBox);  
}
////////////////////////////////////////////////////////////////////////////////
