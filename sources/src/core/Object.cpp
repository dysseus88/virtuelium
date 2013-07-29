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
#include <core/Object.hpp>
//!
//! @file Object.cpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file implements the class declared in 
//!  Object.hpp 
//!  @arg Object
//! @todo 
//! @remarks 
//!
#include <structures/Medium.hpp>
#include <structures/MultispectralPhoton.hpp>
#include <materials/Material.hpp>
#include <objectshapes/ObjectShape.hpp>
////////////////////////////////////////////////////////////////////////////////
Object::Object(void)
    : p_shape(NULL), 
      p_material(NULL), 
      p_innerMedium(NULL), 
      p_outerMedium(NULL) { }
////////////////////////////////////////////////////////////////////////////////
Object::Object(ObjectShape* shape, Material* material, 
               Medium* innerMedium, Medium* outerMedium)
    : p_shape(shape), 
      p_material(material), 
      p_innerMedium(innerMedium), 
      p_outerMedium(outerMedium) { }
////////////////////////////////////////////////////////////////////////////////
Object::~Object(void) {
  if(p_shape != NULL) {
    delete p_shape;
    p_shape = NULL;
  }

  if(p_material != NULL) {
    delete p_material;
    p_material = NULL;
  }
}
////////////////////////////////////////////////////////////////////////////////
void Object::set(ObjectShape* shape, Material* material, 
                 Medium* innerMedium, Medium* outerMedium) {
  if(shape != NULL) {
    if(p_shape != NULL)
      delete p_shape;
    p_shape = shape;
  }

  if(material != NULL) {
    if(p_material != NULL)
      delete p_material;
    p_material = material;
  }

  if(innerMedium != NULL) {
    if(p_innerMedium != NULL)
      delete p_innerMedium;
    p_innerMedium = innerMedium;
  }

  if(outerMedium != NULL) {
    if(p_outerMedium != NULL)
      delete p_outerMedium;
    p_outerMedium = outerMedium;
  }
}
////////////////////////////////////////////////////////////////////////////////
void Object::setIndex(unsigned int index) {
  m_index = index;
}
////////////////////////////////////////////////////////////////////////////////
unsigned int Object::getIndex(void) {
  return m_index;
}
////////////////////////////////////////////////////////////////////////////////
void Object::getDiffuseReemited(const Basis& localBasis, 
                                const Point2D& surfaceCoordinate, 
                                LightVector& incidentLight, 
                                LightVector& reemitedLight) {
  if(p_material != 0)
    p_material->getDiffuseReemited(localBasis, surfaceCoordinate, 
                                   incidentLight, reemitedLight);
}
////////////////////////////////////////////////////////////////////////////////
void Object::getSpecularReemited(const Basis& localBasis, 
                                 const Point2D& surfaceCoordinate, 
                                 LightVector& incidentLight, 
                                 LightVector& reemitedLight) {
  if(p_material!=0)
    p_material->getSpecularReemited(localBasis, surfaceCoordinate, 
                                    incidentLight, reemitedLight);
}
////////////////////////////////////////////////////////////////////////////////
void Object::getDiffuseReemitedFromAmbiant(const Basis& localBasis, 
                                           const Point2D& surfaceCoordinate, 
                                           LightVector& reemitedLight, 
                                           const Spectrum& incident) {
  if(p_material!=0) {
    p_material->getDiffuseReemitedFromAmbiant(localBasis, surfaceCoordinate, 
                                              reemitedLight, incident);  
  }
}
////////////////////////////////////////////////////////////////////////////////
void Object::getSpecularSubRays(const Basis& localBasis, 
                                const Point2D& surfaceCoordinate, 
                                LightVector& reemitedLight, 
                                std::vector<LightVector>& subrays) {
  if(p_material!=0)
    p_material->getSpecularSubRays(localBasis, surfaceCoordinate, 
                                   reemitedLight, subrays);
}
////////////////////////////////////////////////////////////////////////////////
bool Object::bouncePhoton(const Basis& localBasis, 
                          const Point2D& surfaceCoordinate, 
                          MultispectralPhoton& photon, 
                          bool& specular) {
  if(p_material!=0)
    return p_material->bouncePhoton(localBasis, surfaceCoordinate, 
                                    photon, specular);
  return false;
}
////////////////////////////////////////////////////////////////////////////////
void Object::getRandomDiffuseRay(const Basis& localBasis, 
                                 const Point2D& surfaceCoordinate, 
                                 LightVector& reemitedLight, 
                                 unsigned int nbRays, 
                                 std::vector<LightVector>& subrays) {
  if(p_material!=0)
    p_material->getRandomDiffuseRay(localBasis, surfaceCoordinate, 
                                    reemitedLight, nbRays, subrays);
}
////////////////////////////////////////////////////////////////////////////////
bool Object::isDiffuse(void) const {
  if(p_material!=0)
    return p_material->isDiffuse();
  return false;
}
////////////////////////////////////////////////////////////////////////////////
bool Object::isSpecular(void) const {
  if(p_material!=0)
    return p_material->isSpecular();
  return false;
}
////////////////////////////////////////////////////////////////////////////////
bool Object::intersect(const Ray& ray, Real& distance) {
  if(p_shape!=0)
    return p_shape->intersect(ray, distance);
  return false;
}
////////////////////////////////////////////////////////////////////////////////
void Object::getLocalBasis(const Ray& ray, const Real& distance, 
                           Basis& localBasis, Point2D& surfaceCoordinate) {
  if(p_shape!=0)
    p_shape->getLocalBasis(ray, distance, localBasis, surfaceCoordinate);
}
////////////////////////////////////////////////////////////////////////////////
void Object::getBoundingBox(BoundingBox& boundingBox) {
  if(p_shape!=0)
    p_shape->getBoundingBox(boundingBox);
}
////////////////////////////////////////////////////////////////////////////////
Medium* Object::getInnerMedium(void) {
  return p_innerMedium;
}
////////////////////////////////////////////////////////////////////////////////
Medium* Object::getOuterMedium(void) {
  return p_outerMedium;
}
////////////////////////////////////////////////////////////////////////////////