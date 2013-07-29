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
#include <materials/TextureBRDF.hpp>
//!
//! @file TextureBRDF.cpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file implements the classes declared in TextureBRDF.hpp 
//!  @arg TextureBRDF
//! @todo 
//! @remarks 
//!
#include <cstdio>
#include <physics/DielectricFormula.hpp>
////////////////////////////////////////////////////////////////////////////////
static const Real oneOverPi = 1.0/M_PI;
////////////////////////////////////////////////////////////////////////////////
TextureBRDF::TextureBRDF(Material* material, Texture* map, const Real& amount, 
                        const Real& tileU, const Real& tileV, 
                        Texture::TEXTURE_REPEAT_MODE texReapeatModeU,
                        Texture::TEXTURE_REPEAT_MODE texReapeatModeV,
                        Texture::ALPHA_MODE alphaMode)
	  : Material(true, false), 
      p_mtl(material),
      p_map(NULL), 
      m_amount(amount), 
      m_tileU(tileU), 
      m_tileV(tileV), 
      m_texRepeatModeU(texReapeatModeU), 
      m_texRepeatModeV(texReapeatModeV), 
      m_alphaMode(alphaMode) {

  // WARNING : simple pointer affectation !
  p_map = map;
}
////////////////////////////////////////////////////////////////////////////////
TextureBRDF::~TextureBRDF(void) {
	if (p_mtl != NULL)
    delete p_mtl;
  p_mtl = NULL;
}
////////////////////////////////////////////////////////////////////////////////
void TextureBRDF::getSpecularReemited(const Basis& localBasis, 
                                      const Point2D& surfaceCoordinate, 
                                      const LightVector& incidentLight, 
                                      LightVector& reemitedLight) {
  const Vector& incident = incidentLight.getRay().v;
  const Vector& view = reemitedLight.getRay().v;
  Vector normal = localBasis.k;

  Real cosOi = - normal.dot(incident);
  Real cosOv = - normal.dot(view);

  if(cosOv <= 0 || cosOi == 0){
    reemitedLight.clear();
    return;
  }

  //Setting the polarisation framework
  LightVector localIncidentLight(incidentLight);
  localIncidentLight.changeIncidentPolarisationFramework(localBasis.k);
  localIncidentLight.flip();
  reemitedLight.changeReemitedPolarisationFramework(localBasis.k);

  // get spectrum value for the current pixel
  Spectrum sPix;
  Real u = surfaceCoordinate[0] * m_tileU;
  int ent_u = int(u);
  if ( m_texRepeatModeU == Texture::REPEAT_ON )
    u -= ent_u;
  else if ( m_texRepeatModeU == Texture::REPEAT_MIRROR && ent_u % 2 == 0 )
    u -= ent_u;
  else if ( m_texRepeatModeU == Texture::REPEAT_MIRROR && ent_u % 2 != 0 )
    u = 1 + ent_u - u;

  Real v = surfaceCoordinate[1] * m_tileV;
  int ent_v = int(v);
  if ( m_texRepeatModeV == Texture::REPEAT_ON )
    v -= ent_v;
  else if ( m_texRepeatModeV == Texture::REPEAT_MIRROR && ent_v % 2 == 0 )
    v -= ent_v;
  else if ( m_texRepeatModeV == Texture::REPEAT_MIRROR && ent_v % 2 != 0 )
    v = 1 + ent_v - v;

  p_map->GetSpectrumValue( u, v, sPix );

  // No alpha
  if (m_alphaMode == Texture::ALPHA_OFF) {
    for(unsigned int i = 0; i < reemitedLight.size(); i++)
      reemitedLight[i].setRadiance(incidentLight[i].getRadiance()
                                   * oneOverPi * cosOi * sPix[i] * m_amount);
  // alpha
  } else {
    LightVector map_refl;
    map_refl.initSpectralData(reemitedLight);
    map_refl.initGeometricalData(reemitedLight);

    LightVector embedded_refl;
    embedded_refl.initSpectralData(reemitedLight);
    embedded_refl.initGeometricalData(reemitedLight);

    Real alpha = 1.0;
    p_map->GetAlphaValue( u, v, alpha);
  
    if (p_mtl != NULL) {
      p_mtl->getSpecularReemited(localBasis, surfaceCoordinate, incidentLight, 
                                 embedded_refl);
    }

    for(unsigned int i = 0; i < reemitedLight.size(); i++) {
      //map_refl[i].setRadiance(sPix[i] * m_amount);
      map_refl[i].setRadiance(incidentLight[i].getRadiance() 
                              * oneOverPi * cosOi * sPix[i] * m_amount);

      reemitedLight[i].setRadiance(
                            embedded_refl[i].getRadiance() * (Real(1.0) - alpha) 
                          + map_refl[i].getRadiance() * alpha);
    }
  }

	reemitedLight.changeReemitedPolarisationFramework(localBasis.k);
}
////////////////////////////////////////////////////////////////////////////////
void TextureBRDF::getSpecularSubRays(const Basis& localBasis, 
                                     const Point2D& surfaceCoordinate, 
                                     LightVector& reemitedLight, 
                                     std::vector<LightVector>& subrays) {
  p_mtl->getSpecularSubRays(localBasis, surfaceCoordinate, reemitedLight, 
                            subrays);

  //const Vector& view = reemitedLight.getRay().v;
  //Vector normal = localBasis.k;
  //LightVector subray;

  ////Reflexion
  //Vector reflected = view;
  //DielectricFormula::reflect(normal, reflected);
  //subray.initSpectralData(reemitedLight);
  //subray.setRay(localBasis.o, reflected);
  //subray.changeReemitedPolarisationFramework(normal);
  //subrays.push_back(subray);
}
////////////////////////////////////////////////////////////////////////////////
void TextureBRDF::getDiffuseReemited(const Basis& localBasis, 
                                     const Point2D& surfaceCoordinate, 
                                     const LightVector& incidentLight, 
                                     LightVector& reemitedLight) {
	Real cosOi = -incidentLight.getRay().v.dot(localBasis.k);
	Real cosOv = -reemitedLight.getRay().v.dot(localBasis.k);
  Vector normal = localBasis.k;

	if (cosOv <= 0 || cosOi <= 0)	{
		reemitedLight.clear();
		return;
	}

  //Setting the polarisation framework
  LightVector localIncidentLight(incidentLight);
  localIncidentLight.changeIncidentPolarisationFramework(localBasis.k);
  localIncidentLight.flip();
  reemitedLight.changeReemitedPolarisationFramework(localBasis.k);
  
  // get spectrum value for the current pixel
	Spectrum sPix;
	Real u = surfaceCoordinate[0] * m_tileU;
	int ent_u = int(u);
	if ( m_texRepeatModeU == Texture::REPEAT_ON )
		u -= ent_u;
	else if ( m_texRepeatModeU == Texture::REPEAT_MIRROR && ent_u % 2 == 0 )
		u -= ent_u;
	else if ( m_texRepeatModeU == Texture::REPEAT_MIRROR && ent_u % 2 != 0 )
		u = 1 + ent_u - u;

	Real v = surfaceCoordinate[1] * m_tileV;
	int ent_v = int(v);
	if ( m_texRepeatModeV == Texture::REPEAT_ON )
		v -= ent_v;
	else if ( m_texRepeatModeV == Texture::REPEAT_MIRROR && ent_v % 2 == 0 )
		v -= ent_v;
	else if ( m_texRepeatModeV == Texture::REPEAT_MIRROR && ent_v % 2 != 0 )
		v = 1 + ent_v - v;

	p_map->GetSpectrumValue( u, v, sPix );
  
  // No alpha
  if (m_alphaMode == Texture::ALPHA_OFF) {
    for(unsigned int i = 0; i < reemitedLight.size(); i++)
      reemitedLight[i].setRadiance(incidentLight[i].getRadiance()
                                   * oneOverPi * cosOi * sPix[i] * m_amount);
  // alpha
  } else {
    LightVector map_refl;
    map_refl.initSpectralData(reemitedLight);
    map_refl.initGeometricalData(reemitedLight);

    LightVector embedded_refl;
    embedded_refl.initSpectralData(reemitedLight);
    embedded_refl.initGeometricalData(reemitedLight);

    Real alpha = 1.0;
    p_map->GetAlphaValue( u, v, alpha);
  
    if (p_mtl != NULL) {
      p_mtl->getDiffuseReemited(localBasis, surfaceCoordinate, incidentLight, 
                                 embedded_refl);
    }

    for(unsigned int i = 0; i < reemitedLight.size(); i++) {
      map_refl[i].setRadiance(incidentLight[i].getRadiance() 
                              * oneOverPi * cosOi * sPix[i] * m_amount);
      reemitedLight[i].setRadiance(
                            embedded_refl[i].getRadiance() * (Real(1.0) - alpha) 
                          + map_refl[i].getRadiance() * alpha);
    }
  }
  
  ////Reflectance
	//if(cosOi > 0)	{
	//	for(unsigned int i = 0; i < reemitedLight.size(); i++) {
	//		reemitedLight[i].setRadiance(incidentLight[i].getRadiance()
 //                                   * oneOverPi * cosOi * sPix[i] * m_amount );
 //   }
 // }

	reemitedLight.changeReemitedPolarisationFramework(localBasis.k);
}
////////////////////////////////////////////////////////////////////////////////
void TextureBRDF::getRandomDiffuseRay(const Basis& localBasis, 
                                      const Point2D& surfaceCoordinate, 
                                      LightVector& reemitedLight, 
                                      unsigned int nbRays, 
                                      std::vector<LightVector>& subrays) {
  p_mtl->getRandomDiffuseRay(localBasis, surfaceCoordinate, reemitedLight, 
                             nbRays, subrays);
  //Vector normal=localBasis.k;
	//if(normal.dot(reemitedLight.getRay().v) > 0)
	//	return;

	////Reflexion 
	//generateRandomeDiffuseRay(normal, localBasis.o, nbRays, 
 //                           reemitedLight, 
 //                           subrays);
}
////////////////////////////////////////////////////////////////////////////////
void TextureBRDF::generateRandomeDiffuseRay(const Vector& normal, 
                                            const Point& origin, 
                                            unsigned int nbRays, 
                                            LightVector& reemitedLight, 
                                            std::vector<LightVector>& subrays) {
	for(unsigned int i = 0; i < nbRays; i++) {
		Vector incident;
		Real norm2;
		Real cosOi;
		do {
			incident[0]=rand()*2.0/RAND_MAX - 1.0;
			incident[1]=rand()*2.0/RAND_MAX - 1.0;
			incident[2]=rand()*2.0/RAND_MAX - 1.0;
			norm2=incident.square();
			incident.normalize();
			cosOi=incident.dot(normal);
		} while( norm2 > cosOi*cosOi || cosOi <= 0.0 );

		LightVector subray;
		subray.setRay(origin, incident);
		subray.changeReemitedPolarisationFramework(normal);
		subray.initSpectralData(reemitedLight);
		subray.setWeight(1.0/M_PI);
		subrays.push_back(subray);
	}
}
////////////////////////////////////////////////////////////////////////////////
bool TextureBRDF::bouncePhoton(const Basis& localBasis, 
                               const Point2D& surfaceCoordinate, 
                               MultispectralPhoton& photon, bool& specular) {
	Vector normal=localBasis.k;
	if(normal.dot(photon.direction)>0)
		return false;

	// get spectrum value for the current pixel
	Spectrum sPix;
	Real u = surfaceCoordinate[0] * m_tileU;
	int ent_u = int(u);
	if ( m_texRepeatModeU == Texture::REPEAT_ON )
		u -= ent_u;
	else if ( m_texRepeatModeU == Texture::REPEAT_MIRROR && ent_u % 2 == 0 )
		u -= ent_u;
	else if ( m_texRepeatModeU == Texture::REPEAT_MIRROR && ent_u % 2 != 0 )
		u = 1 + ent_u - u;

	Real v = surfaceCoordinate[1] * m_tileV;
	int ent_v = int(v);
	if ( m_texRepeatModeV == Texture::REPEAT_ON )
		v -= ent_v;
	else if ( m_texRepeatModeV == Texture::REPEAT_MIRROR && ent_v % 2 == 0 )
		v -= ent_v;
	else if ( m_texRepeatModeV == Texture::REPEAT_MIRROR && ent_v % 2 != 0 )
		v = 1 + ent_v - v;

	p_map->GetSpectrumValue( u, v, sPix );

	//Compute photon absorption
	Real mean=0;
	Real rmean=0;
	Real tmean=0;
	MultispectralPhoton tphoton=photon;
	for(unsigned int i = 0; i < GlobalSpectrum::nbWaveLengths(); i++) {
		photon.radiance[i]*=sPix[i];
		mean+=photon.radiance[i]+tphoton.radiance[i];
		rmean+=photon.radiance[i];
		tmean+=tphoton.radiance[i];
	}

	//Russian roulette (Absorption)
	if(rand() / (Real)RAND_MAX > mean)
		return false;

	//Reflexion
	if(rand() / (Real)RAND_MAX < rmean/mean) {
		generatePhoton(normal, photon, rmean);
	//Transmission
  } else {
		photon = tphoton;
		normal.mul(-1.0);
		generatePhoton(normal, photon, tmean);
	}

	//Done
	specular=false;
	return true;
}
////////////////////////////////////////////////////////////////////////////////
void TextureBRDF::generatePhoton(const Vector& normal, 
                                 MultispectralPhoton& photon, 
                                 Real mean) {
	//Normalizing photon energy
	for(unsigned int i = 0; i < GlobalSpectrum::nbWaveLengths(); i++) {
    photon.radiance[i] /= mean;
  }

	Real norm2;
	Real cosOi;
	do {
		photon.direction[0]=rand()*2.0/RAND_MAX - 1.0;
		photon.direction[1]=rand()*2.0/RAND_MAX - 1.0;
		photon.direction[2]=rand()*2.0/RAND_MAX - 1.0;
		norm2=photon.direction.square();
		photon.direction.normalize();
		cosOi = photon.direction.dot(normal);
	} while(norm2>cosOi*cosOi || cosOi<=0);
}
////////////////////////////////////////////////////////////////////////////////
void TextureBRDF::getDiffuseReemitedFromAmbiant(
    const Basis& localBasis, 
    const Point2D& surfaceCoordinate, 
    LightVector& reemitedLight, 
    const Spectrum& incident) {
  Real cosOi = Real(1.0);//-incidentLight.getRay().v.dot(localBasis.k);
  Real cosOv = -reemitedLight.getRay().v.dot(localBasis.k);

  if(cosOv <= 0 )
  {
    reemitedLight.clear();
    return;
  }
  
 // get spectrum value for the current pixel
	Spectrum sPix;
	Real u = surfaceCoordinate[0] * m_tileU;
	int ent_u = int(u);
	if ( m_texRepeatModeU == Texture::REPEAT_ON )
		u -= ent_u;
	else if ( m_texRepeatModeU == Texture::REPEAT_MIRROR && ent_u % 2 == 0 )
		u -= ent_u;
	else if ( m_texRepeatModeU == Texture::REPEAT_MIRROR && ent_u % 2 != 0 )
		u = 1 + ent_u - u;

	Real v = surfaceCoordinate[1] * m_tileV;
	int ent_v = int(v);
	if ( m_texRepeatModeV == Texture::REPEAT_ON )
		v -= ent_v;
	else if ( m_texRepeatModeV == Texture::REPEAT_MIRROR && ent_v % 2 == 0 )
		v -= ent_v;
	else if ( m_texRepeatModeV == Texture::REPEAT_MIRROR && ent_v % 2 != 0 )
		v = 1 + ent_v - v;

	p_map->GetSpectrumValue( u, v, sPix );

  for(unsigned int wl=0; wl < reemitedLight.size(); wl++)
    reemitedLight[wl].setRadiance(incident[wl] * oneOverPi /** cosOv*/ * sPix[wl]);

  reemitedLight.changeReemitedPolarisationFramework(localBasis.k);
}
////////////////////////////////////////////////////////////////////////////////
