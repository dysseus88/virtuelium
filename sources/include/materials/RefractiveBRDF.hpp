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

#ifndef _REFRACTIVEBRDF_HPP
#define _REFRACTIVEBRDF_HPP

#include <core/3DBase.hpp>
#include <core/LightBase.hpp>

#include <vector>
#include <materials/Material.hpp>

class RefractiveBRDF : public Material{
public :
  /**
   * Constructor
   * @param RPara : the P-Polarised Reflectance
   * @param ROrth : the S-Polarised Reflectance
   * @param TPara : the P-Polarised transmittance
   * @param TOrth : the S-Polarised transmittance
   * @param indices : the indices of refraction
   */
  RefractiveBRDF(std::vector<Spectrum> RPara, std::vector<Spectrum> ROrth, std::vector<Spectrum> TPara, std::vector<Spectrum> TOrth, Spectrum indices, bool dispersion=false, bool photondispersion=false);

  /**
   * Virtual destructor
   */
  virtual inline ~RefractiveBRDF();

  /**
   * Compute the specularly reemited light data and place the result into reemited.
   * localBasis : the object localBasis at the computation point.
   * surfaceCoordinate : the surface coordinate (texture coordinate) of the computation
   *   point on the object.
   * incident : the incident ray (light ray)
   * view : the view ray (from the camera or bounced)
   * incidentLight : the incident light comming from the incident ray.
   * reemitedLight : the light reemited into the view direction (result will be placed 
   *   here).
   */
  virtual void getSpecularReemited(const Basis& localBasis, const Point2D& surfaceCoordinate, const LightVector& incidentLight, LightVector& reemitedLight);

  /**
   * Compute the secondary rays for specular reflexion and place them into the subrays
   * vector.
   * localBasis : the local Basis on the object at the computation point.
   * surfaceCoordinate : the surface  coordinate (texture coordinate) of the computation 
   *   point on the object.
   * view : the view ray (from the camera or bounced)
   * subrays : the vector were the secondaries rays will be put.
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
  std::vector<Spectrum> _RPara;
  std::vector<Spectrum> _ROrth;
  std::vector<Spectrum> _TPara;
  std::vector<Spectrum> _TOrth;
  Spectrum _indices;
  Real _maxIndex;
  bool _dispersion;
  bool _photondispersion;

  /**
   * Get the reflectance in the reflection direction
   * @param cosOi : the cosinus of the angle between the normal and incident.
   * @param index : the wavelenght index to get
   * @param ROrth : the othogonal polarised computed Reflectance will be placed here
   * @param Rpara : the parallel polarised computed Reflectance will be placed here
   */
  void getReflectance(const Real& cosOi, const int& index, Real& ROrth, Real& RPara);

  /**
   * Get the transmittance in the reflection direction
   * @param cosOi : the cosinus of the angle between the normal and incident.
   * @param index : the wavelenght index to get
   * @param TOrth : the othogonal polarised computed Transmittance will be placed here
   * @param TPara : the parallel polarised computed Transmittance will be placed here
   */
  void getTransmittance(const Real& cosOi, const int& index, Real& TOrth, Real& TPara);
};

/**
 * Virtual destructor
 */
inline RefractiveBRDF::~RefractiveBRDF()
{
  //Nothing to do
}

#endif // _REFRACTIVEBRDF_HPP
