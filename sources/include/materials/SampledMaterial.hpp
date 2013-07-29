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
#ifndef _SAMPLEDMATERIAL_HPP
#define _SAMPLEDMATERIAL_HPP

#include <materials/Material.hpp>
#include <string>
#include <vector>

class SampledBRDFEntry{
public :
  Real theta;
  Real phi;
  Real rOrth;
  Real rPara;
};

//! @class SampledMaterial
class SampledMaterial: public Material{
public :

  /**
   * Constructor
   * @param diffuse : did the material has a diffuse contribution
   */
  SampledMaterial(std::string filename);

  /**
   * Virtual destructor.
   */
  virtual inline ~SampledMaterial();
  
  /**
   * Compute the diffuse part of a reflexion. Use the given incident light and
   * place the computed result into the reemitedLight parameter.
   * @param localBasis : the object localBasis at the computation point.
   * @param surfaceCoordinate : the surface coordinate (texture coordinate) of the computation
   *   point on the object.
   * @param incidentLight : the incident light comming from the incident ray.
   * @param reemitedLight : the light reemited into the view direction (result will be placed 
   *   here).
   */
  virtual void getDiffuseReemited(const Basis& localBasis, const Point2D& surfaceCoordinate, const LightVector& incidentLight, LightVector& reemitedLight);

  /**
   * Compute the specular part of a reflexion. Use the given incident light and 
   * place the computed result into the reemitedLight parameter.
   *
   * Most of the implementations of this methode dont use the geometric information
   * of the reemitedLight. They assume that the given incidentLight is build from
   * the result (backwarded) of getSpecularSubRays().
   *
   * @param localBasis : the object localBasis at the computation point.
   * @param surfaceCoordinate : the surface coordinate (texture coordinate) of the computation
   *   point on the object.
   * @param incidentLight : the incident light comming from the incident ray.
   * @param reemitedLight : the light reemited into the view direction (result will be placed 
   *   here).
   */
  virtual void getSpecularReemited(const Basis& localBasis, const Point2D& surfaceCoordinate, const LightVector& incidentLight, LightVector& reemitedLight);

  /**
   * Compute a set of reflected ray for the speculars reflexions. As it is designed
   * for a backward ray-traing it is a set of incoming ray that is realy builded 
   * from the reflexion direction (observation direction).
   * 
   * Non specular material should not override this method. The default implementation
   * dont create any secondary ray as wanted for non specular material.
   *
   * @param localBasis : the local Basis on the object at the computation point.
   * @param surfaceCoordinate : the surface  coordinate (texture coordinate) of the computation 
   *   point on the object.
   * @param reemitedLight : the light data that will be computed by using the rays
   *   this method will compute. 
   * @param subrays : the vector were the secondaries rays will be put.
   */
  virtual void getSpecularSubRays(const Basis& localBasis, const Point2D& surfaceCoordinate, LightVector& reemitedLight, std::vector<LightVector>& subrays);

  /**
   * This method compute a random reflexion for the given photon based on an
   * russian roulette for handeling the absorption of the photon.
   *
   * The default implementation absorb every received photon.
   *
   * This method tel also if this bounce is specular or not.
   * 
   * @param localBasis : the local Basis on the object at the computation point.
   * @param surfaceCoordinate : the surface  coordinate (texture coordinate) of the 
   *   computation point on the object.
   * @param photon : the photon to bounce. This parametre will be modified to reflect
   *   the bounce.
   * @param specular : this function will set it to true if this bounce is specular
   *   false otherwise
   *
   * @return true if the photon had been reemited or false if it was absorbed.
   */
  virtual bool bouncePhoton(const Basis& localBasis, const Point2D& surfaceCoordinate, MultispectralPhoton& photon, bool& specular);

  /**
   * Compute the reemited light that come from an istrotropique ambiant illumination.
   *
   * The default implementation doesn't do anything !
   *
   * @param localBasis : the object localBasis at the computation point.
   * @param surfaceCoordinate : the surface coordinate (texture coordinate) of the computation
   *   point on the object.
   * @param reemitedLight : the light reemited into the view direction (result will be placed 
   *   here).
   * @param incident : the incident light spectrum as if all the energy come from an
   *   isotropic ambiant term.
   */
  virtual void getDiffuseReemitedFromAmbiant(const Basis& localBasis, const Point2D& surfaceCoordinate, LightVector& reemitedLight, const Spectrum& incident);

private :
  std::vector<SampledBRDFEntry>* _BRDF;
  int _OiStep;
  int _PiStep;
  int _OiSample;
  int _PiSample;
  int _LambdaStart;
  int _LambdaStep;
  int _LambdaSample;

  inline unsigned int getIndex(int Oi, int Pi, int lambda ) const;
  
};

inline SampledMaterial::~SampledMaterial()
{
  delete[] _BRDF;
}

inline unsigned int SampledMaterial::getIndex(int Oi, int Pi, int lambda ) const
{
  if(Oi<0) Oi = 0;
  if(Oi>=_OiSample) Oi=_OiSample-1;
  if(Pi<0) Pi = 0;
  if(Pi>=_PiSample) Pi= _PiSample-1;

  return (Oi*_PiSample + Pi)*_LambdaSample + lambda;
}

#endif //_SAMPLEDMATERIAL_HPP
