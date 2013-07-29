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

#ifndef _ROUGHLAMBERTIANBRDF_HPP
#define _ROUGHLAMBERTIANBRDF_HPP

#include <core/3DBase.hpp>
#include <core/LightBase.hpp>

#include <vector>
#include <materials/Material.hpp>

class RoughLambertianBRDF : public Material{
public :
  /**
   * Constructor
   * spectrum : the normal incident reflection spectrum.
   * ambiant : the spectrum of the ambiant light.
   * power : the power of ambiant light (if negative or null no ambiant).
   */
  RoughLambertianBRDF(const Spectrum& spectrum, const Real& roughness);  

  /**
   * Virtual destructor.
   */
  virtual inline ~RoughLambertianBRDF();
  
  /**
   * Compute the diffusely reemited light data and place the result into reemited.
   * localBasis : the object localBasis at the computation point.
   * surfaceCoordinate : the surface coordinate (texture coordinate) of the computation
   *   point on the object.
   * incident : the incident ray (light ray)
   * view : the view ray (from the camera or bounced)
   * incidentLight : the incident light comming from the incident ray.
   * reemitedLight : the light reemited into the view direction (result will be placed 
   *   here).
   */
  virtual void getDiffuseReemited(const Basis& localBasis, const Point2D& surfaceCoordinate, const LightVector& incidentLight, LightVector& reemitedLight);

  /**
   * Compute the bounce of the given photon and modify it. Use the russian 
   * roulette to return if it bounce (true) or if it's absorbed. Tel also if 
   * this bounce is specular or not.
   * localBasis : the local Basis on the object at the computation point.
   * surfaceCoordinate : the surface  coordinate (texture coordinate) of the 
   *   computation point on the object.
   * photon : the photon to bounce. This parametre will be modified to reflect
   *   the bounce.
   * specular : this function will set it to true if this bounce is specular
   */
  virtual void getRandomDiffuseRay(const Basis& localBasis, const Point2D& surfaceCoordinate, LightVector& reemitedLight, unsigned int nbRays, std::vector<LightVector>& subrays);

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
  Spectrum _spectrum; //The normaly reflected spectrum.
  Real _power; //The normaly reflected spectrum.
  Real _roughness;
};

inline RoughLambertianBRDF::~RoughLambertianBRDF()
{
  //Nothing to do
}

#endif //_ROUGHLAMBERTIANBRDF_HPP
