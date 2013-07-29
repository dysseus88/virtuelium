#include <materials/ConcentrationMap.hpp>
#include <stdio.h>
////////////////////////////////////////////////////////////////////////////////
ConcentrationMap::ConcentrationMap(std::vector<Material*> materials, 
                                   Image* maps)
    : Material(hasDiffuseMaterial(materials), hasSpecularMaterial(materials)), 
      m_materials(materials), 
      p_map(maps),
      p_factor(NULL) {
  
  // normalizing the image
  if (p_map != NULL)
    p_map->normalize();
}
////////////////////////////////////////////////////////////////////////////////
ConcentrationMap::ConcentrationMap(std::vector<Material*> materials, 
                                   Image* maps,
                                   const Real& tileU, const Real& tileV,
                                   Texture::TEXTURE_REPEAT_MODE texRepeatModeU,
		                               Texture::TEXTURE_REPEAT_MODE texRepeatModeV)								   
    : Material(hasDiffuseMaterial(materials), hasSpecularMaterial(materials)), 
      m_materials(materials), 
      p_map(maps),
      p_factor(NULL) {

  // normalizing the image
  if (p_map != NULL)
    p_map->normalize();
  
  m_tileU = tileU;
  m_tileV = tileV;
  m_texRepeatModeU = texRepeatModeU;
	m_texRepeatModeV = texRepeatModeV;
	
  p_factor = new Real[m_materials.size()];
}
////////////////////////////////////////////////////////////////////////////////
ConcentrationMap::~ConcentrationMap(void) {
  for(unsigned int i = 0; i < m_materials.size(); i++) {
	  if(m_materials[i] != NULL)
		  delete m_materials[i];
	  m_materials[i] = NULL;
  }
  if(p_map != NULL)
	  delete p_map;
  p_map = NULL;

  if(p_factor != NULL)
	  delete p_factor;
  p_factor = NULL;
}
////////////////////////////////////////////////////////////////////////////////
void ConcentrationMap::getDiffuseReemited(const Basis& localBasis, 
                                     const Point2D& surfaceCoordinate, 
                                     const LightVector& incidentLight, 
                                     LightVector& reemitedLight) {


  LightVector tmp_0, tmp_1;
  reemitedLight.clear();
 
  //get the pixel
  Pixel pixel = getPixel(surfaceCoordinate);

  for (unsigned int i = 0; i < pixel.numberOfChannel(); i++) {
    // add first material
    if (m_materials[i] != NULL && m_materials[i]->isSpecular() == true) {
      tmp_0.initSpectralData(reemitedLight);
      tmp_0.initGeometricalData(reemitedLight);
      m_materials[i]->getDiffuseReemited(localBasis, surfaceCoordinate, 
                                         incidentLight, tmp_0);
      tmp_0.mul(pixel[i]);
      reemitedLight.add(tmp_0);
    }

    // add second material
    if (m_materials[i+1] != NULL && m_materials[i+1]->isSpecular() == true) {
      tmp_1.initSpectralData(reemitedLight);
      tmp_1.initGeometricalData(reemitedLight);
      m_materials[i+1]->getDiffuseReemited(localBasis, surfaceCoordinate, 
                                           incidentLight, tmp_1);
      tmp_1.mul(Real(1.0) - pixel[i]);
      reemitedLight.add(tmp_1);
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
void ConcentrationMap::getSpecularReemited(const Basis& localBasis, 
                                      const Point2D& surfaceCoordinate, 
                                      const LightVector& incidentLight, 
                                      LightVector& reemitedLight) {
  LightVector tmp_0, tmp_1;
  reemitedLight.clear();
  
  //get the pixel
  Pixel pixel = getPixel(surfaceCoordinate);
  
  for (unsigned int i = 0; i < pixel.numberOfChannel(); i++) {
    // add first material
    if (m_materials[i] != NULL && m_materials[i]->isSpecular() == true) {
      tmp_0.initSpectralData(reemitedLight);
      tmp_0.initGeometricalData(reemitedLight);
      m_materials[i]->getSpecularReemited(localBasis, surfaceCoordinate, 
        incidentLight, tmp_0);
      tmp_0.mul(pixel[i]);
      reemitedLight.add(tmp_0);
    }

    // add second material
    if (m_materials[i+1] != NULL && m_materials[i+1]->isSpecular() == true) {
      tmp_1.initSpectralData(reemitedLight);
      tmp_1.initGeometricalData(reemitedLight);
      m_materials[i+1]->getSpecularReemited(localBasis, surfaceCoordinate, 
                                            incidentLight, tmp_1);
      tmp_1.mul(Real(1.0) - pixel[i]);
      reemitedLight.add(tmp_1);
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
void ConcentrationMap::getSpecularSubRays(const Basis& localBasis, 
                                     const Point2D& surfaceCoordinate, 
                                     LightVector& reemitedLight, 
                                     std::vector<LightVector>& subrays) {

  if (hasSpecularMaterial(m_materials) == true) {                                    
    if(p_factor[0] > kEPSILON) {
      m_materials[0]->getSpecularSubRays(localBasis, 
        surfaceCoordinate, 
        reemitedLight, 
        subrays);
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
bool ConcentrationMap::bouncePhoton(const Basis& localBasis, 
                               const Point2D& surfaceCoordinate, 
                               MultispectralPhoton& photon, 
                               bool& specular) {

  //get the pixel
  Pixel pixel = getPixel(surfaceCoordinate);

  //init factor
  Real* factor = new Real[m_materials.size()];

  for(unsigned int i = 0; i < m_materials.size(); i++)
    p_factor[i] = Real(1.0);

  for(unsigned int i = 0; i < pixel.numberOfChannel(); i++) {
    p_factor[i] *= pixel[i];
    p_factor[i+1] *= (Real(1.0) - pixel[i]);
  }

  Real factorsum = 1.0;
  for(unsigned int i=0; i < m_materials.size(); i++) {
	  
    if(rand() < p_factor[i] * RAND_MAX / factorsum)  {
		  if(m_materials[i]->bouncePhoton(localBasis, surfaceCoordinate, photon, 
                                      specular)) {
			  delete[] factor;
        return true;
      }
	  }
	 
    factorsum -= p_factor[i];
  }

  delete[] factor;
  return false;
}
////////////////////////////////////////////////////////////////////////////////
void ConcentrationMap::getRandomDiffuseRay(const Basis& localBasis, 
                                      const Point2D& surfaceCoordinate, 
                                      LightVector& reemitedLight, 
                                      unsigned int nbRays, 
                                      std::vector<LightVector>& subrays) {
  int nbDiffuseMat=0;
  for(unsigned int i = 0; i < m_materials.size(); i++)
    if(m_materials[i]->isDiffuse())
      nbDiffuseMat++;

  for(unsigned int i = 0; i < m_materials.size(); i++)
    m_materials[i]->getRandomDiffuseRay(localBasis, surfaceCoordinate, 
                                       reemitedLight, 1 + nbRays / nbDiffuseMat, 
                                       subrays);
}
////////////////////////////////////////////////////////////////////////////////
void ConcentrationMap::getDiffuseReemitedFromAmbiant(
    const Basis& localBasis, 
    const Point2D& surfaceCoordinate, 
    LightVector& reemitedLight, 
    const Spectrum& incident) {
  
  Real cosOi = Real(1.0);//-incidentLight.getRay().v.dot(localBasis.k);
  Real cosOv = -reemitedLight.getRay().v.dot(localBasis.k);

  if(cosOv <= kEPSILON) {
    reemitedLight.clear();
    return;
  }
  
  reemitedLight.changeReemitedPolarisationFramework(localBasis.k);
}
////////////////////////////////////////////////////////////////////////////////
Pixel& ConcentrationMap::getPixel(const Point2D& surfaceCoordinate) {
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

  u *= p_map->getWidth();
  v *= p_map->getHeight();

  Pixel p = p_map->getInterpolatedPixel(u, v);
  return p;
}
////////////////////////////////////////////////////////////////////////////////
bool ConcentrationMap::hasDiffuseMaterial(
    const std::vector<Material*>& materials) {

  for(unsigned int i = 0; i < materials.size(); i++)
    if(materials[i]->isDiffuse())
      return true;
  return false;
}
////////////////////////////////////////////////////////////////////////////////
bool ConcentrationMap::hasSpecularMaterial(
    const std::vector<Material*>& materials) {
  
  for(unsigned int i = 0; i < materials.size(); i++)
    if(materials[i]->isSpecular())
      return true;
  return false;  
}
////////////////////////////////////////////////////////////////////////////////

