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

#ifndef _ROUGHVARNISHEDLAMBERTIANBRDF_HPP
#define _ROUGHVARNISHEDLAMBERTIANBRDF_HPP

#include <core/3DBase.hpp>
#include <core/LightBase.hpp>

#include <vector>
#include <materials/Material.hpp>

class RoughVarnishedLambertianBRDF : public Material{
public :

  /**
   * Constructor
   * @param R12p : A vector that contain the P-polarsed reflectance of the varnish for each sampled wavelenght
   * @param R12s : A vector that contain the S-polarsed reflectance of the varnish for each sampled wavelenght
   * @param T12p : A vector that contain the P-polarsed transmittance of the varnish for each sampled wavelenght
   * @param T12s : A vector that contain the S-polarsed transmittance of the varnish for each sampled wavelenght
   * @param R21p : A vector that contain the P-polarsed reflectance of the under interface of the varnish for each sampled wavelenght
   * @param R21s : A vector that contain the S-polarsed reflectance of the under interface of the varnish for each sampled wavelenght
   * @param n : The real part of indices of refraction of the varnish (for the refraction direction)
   * @param k : The imaginary part of indices of refraction of the varnish (for the absorption)
   * @param thickness : The thickness of the layer in micrometer
   * @param R23 : The spectrum of the reflectance of the lambertian substrate
   * @param roughness : The roughness of the external surface of the varnish
   */
  RoughVarnishedLambertianBRDF(std::vector<Spectrum> R12p, std::vector<Spectrum> R12s, std::vector<Spectrum> T12p, std::vector<Spectrum> T12s, std::vector<Spectrum> R21p, std::vector<Spectrum> R21s, const Spectrum& n, const Spectrum& k, Real thickness, Real roughness, const Spectrum& R23); 

  /**
   * Virtual destructor.
   */
  virtual inline ~RoughVarnishedLambertianBRDF();
  
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
  bool _opaque;
  int _samples;
  std::vector<Spectrum> _Rs;
  std::vector<Spectrum> _Rp;
  std::vector<Spectrum> _Rd; //Index coded as Oi*_samples + Ov
  Real _roughness;

  /**
   * Get the reflectance in the reflection direction
   * @param cosOi : the cosinus of the angle between the normal and incident.
   * @param index : the wavelenght index to get
   * @param ROrth : the othogonal polarised computed Reflectance will be placed here
   * @param Rpara : the parallel polarised computed Reflectance will be placed here
   */
  void getDeepReflectance(const Real& cosOi, const Real& cosOv, const int& index, Real& R);

  /**
   * Get the reflectance in the reflection direction
   * @param cosOi : the cosinus of the angle between the normal and incident.
   * @param index : the wavelenght index to get
   * @param ROrth : the othogonal polarised computed Reflectance will be placed here
   * @param Rpara : the parallel polarised computed Reflectance will be placed here
   */
  void getSpecularReflectance(const Real& cosOi, const int& index, Real& ROrth, Real& RPara);

  void generatePhoton(const Vector& normal, MultispectralPhoton& photon, Real mean);

  void generateRandomeDiffuseRay(const Vector& normal, const Point& origin, unsigned int nbRays, LightVector& reemitedLight, std::vector<LightVector>& subrays);
};

inline RoughVarnishedLambertianBRDF::~RoughVarnishedLambertianBRDF()
{
  //Nothing to do
}

#endif //_ROUGHVARNISHEDLAMBERTIANBRDF_HPP
