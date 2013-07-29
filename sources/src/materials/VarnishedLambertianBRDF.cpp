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

#include <materials/VarnishedLambertianBRDF.hpp>
#include <physics/DielectricFormula.hpp>
#include <exceptions/Exception.hpp>

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
 */
VarnishedLambertianBRDF::VarnishedLambertianBRDF(std::vector<Spectrum> R12p, std::vector<Spectrum> R12s, std::vector<Spectrum> T12p, std::vector<Spectrum> T12s, std::vector<Spectrum> R21p, std::vector<Spectrum> R21s, const Spectrum& n, const Spectrum& k, Real thickness, const Spectrum& R23d)
: Material(true, true), _opaque(true), _samples(R12s.size()), _Rs(R12p), _Rp(R12s), _Rd(R12s.size()*R12s.size()), _Td(R12s.size()*R12s.size())
{
  //Compute the diffuse contribution
  for(unsigned int l=0; l<GlobalSpectrum::nbWaveLengths(); l++)
  {
    Real n2 = n[l];
    Real R23 = R23d[l]/M_PI;

    //Integrate the infinite reflection coeficient for the sublayer
    Real integral = 0;
    for(int i=0; i<_samples; i++)
    {
      Real O = i*0.5*M_PI/(Real)_samples;
      Real cosO = std::cos(O);
      Real sinO = std::sqrt(1 - cosO*cosO);
      Real R21 = 0.5*(R21p[i][l] + R21s[i][l]); 
      Real A = DielectricFormula::dielectricAbsorption(thickness/cosO, GlobalSpectrum::getWaveLength(l),  k[l]/n[l]);
      integral += A*A*R21*R23*sinO*cosO;
    }
    integral *= M_PI/(Real)_samples;
  
    //Compute the refletcance for each pair of incidence/view angle
    for(int iOl=0; iOl<_samples; iOl++)
      for(int iOv=0; iOv<_samples; iOv++)
      {
        Real Ol = iOl * 0.5*M_PI/(Real)_samples;
        Real Ov = iOv * 0.5*M_PI/(Real)_samples;
        Real cosOl1 = std::cos(Ol);
        Real cosOv1 = std::cos(Ov);

        _Rd[iOl*_samples + iOv][l] = 0.0;

        if(1-(1 - cosOl1*cosOl1)/(n2*n2)>0.0 && 1-(1 - cosOv1*cosOv1)/(n2*n2)>0.0)
        {
          Real cosOl2 = std::sqrt(1 - (1 - cosOl1*cosOl1)/(n2*n2));
          Real cosOv2 = std::sqrt(1 - (1 - cosOv1*cosOv1)/(n2*n2));

          Real T12_Ol = 0.5*(T12s[iOl][l]+T12p[iOl][l]);
          Real T12_Ov =  0.5*(T12s[iOv][l]+T12p[iOv][l]);

          Real A2_Ol = DielectricFormula::dielectricAbsorption(thickness/cosOl2, GlobalSpectrum::getWaveLength(l),  k[l]/n[l]); 
          Real A2_Ov = DielectricFormula::dielectricAbsorption(thickness/cosOv2, GlobalSpectrum::getWaveLength(l),  k[l]/n[l]); 
          
          _Rd[iOl*_samples + iOv][l] = T12_Ol*A2_Ol*R23*A2_Ov*T12_Ov*cosOl1 / (1.0 - integral);
        }
      }
  }
}

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
 * @param T23 : The spectrum of the transmittance of the lambertian substrate
 */
VarnishedLambertianBRDF::VarnishedLambertianBRDF(std::vector<Spectrum> R12p, std::vector<Spectrum> R12s, std::vector<Spectrum> T12p, std::vector<Spectrum> T12s, std::vector<Spectrum> R21p, std::vector<Spectrum> R21s, const Spectrum& n, const Spectrum& k, Real thickness, const Spectrum& R23d, const Spectrum& T23d)
: Material(true, true), _opaque(true), _samples(R12s.size()), _Rs(R12p), _Rp(R12s), _Rd(R12s.size()*R12s.size()), _Td(R12s.size()*R12s.size())
{
  //Compute the diffuse contribution
  for(unsigned int l=0; l<GlobalSpectrum::nbWaveLengths(); l++)
  {
    Real n2 = n[l];
    Real R23 = R23d[l]/M_PI;
    Real T23 = T23d[l]/M_PI;

    //Integrate the infinite reflection coeficient for the sublayer
    Real integral = 0;
    for(int i=0; i<_samples; i++)
    {
      Real O = i*0.5*M_PI/(Real)_samples;
      Real cosO = std::cos(O);
      Real sinO = std::sqrt(1 - cosO*cosO);
      Real R21 = 0.5*(R21p[i][l] + R21s[i][l]); 
      Real A = DielectricFormula::dielectricAbsorption(thickness/cosO, GlobalSpectrum::getWaveLength(l),  k[l]/n[l]);
      integral += A*A*R21*R23*sinO*cosO;
    }
    integral *= M_PI/(Real)_samples;
  
    //Compute the refletcance for each pair of incidence/view angle
    for(int iOl=0; iOl<_samples; iOl++)
      for(int iOv=0; iOv<_samples; iOv++)
      {
        Real Ol = iOl * 0.5*M_PI/(Real)_samples;
        Real Ov = iOv * 0.5*M_PI/(Real)_samples;
        Real cosOl1 = std::cos(Ol);
        Real cosOv1 = std::cos(Ov);

        _Rd[iOl*_samples + iOv][l] = 0.0;
        _Td[iOl*_samples + iOv][l] = 0.0;

        if(1-(1 - cosOl1*cosOl1)/(n2*n2)>0.0 && 1-(1 - cosOv1*cosOv1)/(n2*n2)>0.0)
        {
          Real cosOl2 = std::sqrt(1 - (1 - cosOl1*cosOl1)/(n2*n2));
          Real cosOv2 = std::sqrt(1 - (1 - cosOv1*cosOv1)/(n2*n2));

          Real T12_Ol = 0.5*(T12s[iOl][l]+T12p[iOl][l]);
          Real T12_Ov =  0.5*(T12s[iOv][l]+T12p[iOv][l]);

          Real A2_Ol = DielectricFormula::dielectricAbsorption(thickness/cosOl2, GlobalSpectrum::getWaveLength(l),  k[l]/n[l]); 
          Real A2_Ov = DielectricFormula::dielectricAbsorption(thickness/cosOv2, GlobalSpectrum::getWaveLength(l),  k[l]/n[l]); 
          
          _Rd[iOl*_samples + iOv][l] = T12_Ol*A2_Ol*R23*A2_Ov*T12_Ov*cosOl1 / (1.0 - integral);
          _Td[iOl*_samples + iOv][l] = T12_Ov*A2_Ol*T23*cosOl1 / (1.0 - integral);
        }
      }
  }
}


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
void VarnishedLambertianBRDF::getDiffuseReemited(const Basis& localBasis, const Point2D& surfaceCoordinate, const LightVector& incidentLight, LightVector& reemitedLight)
{
  Real cosOi = - localBasis.k.dot(incidentLight.getRay().v);
  Real cosOv = - localBasis.k.dot(reemitedLight.getRay().v);

  if(cosOv<=0.0 || (cosOi<=0.0 && _opaque))
  {
    reemitedLight.clear();
    return;
  }

  if(cosOi>0.0)
  {
    for(unsigned int i=0; i<incidentLight.size(); i++)
    {
      Real R;
      getDiffuseReflectance(cosOi, cosOv, incidentLight[i].getIndex(), R);
      reemitedLight[i].setRadiance(incidentLight[i].getRadiance()*R);
    }
  }
  else
  {
    for(unsigned int i=0; i<incidentLight.size(); i++)
    {
      Real T;
      getDiffuseTransmittance(-cosOi, cosOv, incidentLight[i].getIndex(), T);
      reemitedLight[i].setRadiance(incidentLight[i].getRadiance()*T);
    }    
  }

  reemitedLight.changeReemitedPolarisationFramework(localBasis.k);
}

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
void VarnishedLambertianBRDF::getSpecularReemited(const Basis& localBasis, const Point2D& surfaceCoordinate, const LightVector& incidentLight, LightVector& reemitedLight)
{
  const Vector& incident = incidentLight.getRay().v;
  Vector normal=localBasis.k;

  Real cosOi = - normal.dot(incident);              //Oi : Incident angle with normal
  Real cosOv = - normal.dot(reemitedLight.getRay().v);

  if(cosOi<=0 || cosOv<=0){
    reemitedLight.clear();
    return;
  }

  //Setting the polarisation framework
  LightVector localIncidentLight(incidentLight);
  localIncidentLight.changeIncidentPolarisationFramework(localBasis.k);
  localIncidentLight.flip();
  reemitedLight.changeReemitedPolarisationFramework(localBasis.k);

  for(unsigned int i=0; i<localIncidentLight.size(); i++)
  {
    Real ROrth, RPara;
    getSpecularReflectance(cosOi, localIncidentLight[i].getIndex(), ROrth, RPara);
    reemitedLight[i].applyReflectance(localIncidentLight[i], RPara, ROrth);  
  }
}

/**
 * Compute the secondary rays for specular reflexion and place them into the subrays
 * vector.
 * localBasis : the local base on the object at the computation point.
 * surfaceCoordinate : the surface  coordinate (texture coordinate) of the computation 
 *   point on the object.
 * view : the view ray (from the camera or bounced)
 * subrays : the vector were the secondaries rays will be put.
 */
void VarnishedLambertianBRDF::getSpecularSubRays(const Basis& localBasis, const Point2D& surfaceCoordinate, LightVector& reemitedLight, std::vector<LightVector>& subrays)
{
  const Vector& view = reemitedLight.getRay().v;
  Vector normal = localBasis.k;
  LightVector subray;

  //Reflexion
  Vector reflected = view;
  DielectricFormula::reflect(normal, reflected);
  subray.initSpectralData(reemitedLight);
  subray.setRay(localBasis.o, reflected);
  subray.changeReemitedPolarisationFramework(normal);
  subrays.push_back(subray);
}

/**
 * Compute the bounce of the given photon and modify it. Use the russian 
 * roulette to return if it bounce (true) or if it's absorbed. Tel also if 
 * this bounce is specular or not.
 * localBasis : the local base on the object at the computation point.
 * surfaceCoordinate : the surface  coordinate (texture coordinate) of the 
 *   computation point on the object.
 * photon : the photon to bounce. This parametre will be modified to reflect
 *   the bounce.
 * specular : this function will set it to true if this bounce is specular
 */
bool VarnishedLambertianBRDF::bouncePhoton(const Basis& localBasis, const Point2D& surfaceCoordinate, MultispectralPhoton& photon, bool& specular)
{
  //Computing base angles
  Vector normal = localBasis.k;
  Real cosOi = - normal.dot(photon.direction);
  if(cosOi < 0)
    return false;

  //Computing photon absorption at the reflexion
  Real mean=0;
  Real rmean=0;
  Real tmean=0;
  Real smean=0;
  MultispectralPhoton rphoton=photon;
  MultispectralPhoton tphoton=photon;
  for(unsigned int i=0; i<GlobalSpectrum::nbWaveLengths(); i++)
  { 
    Real R, T, ROrth, RPara;
  
    getDiffuseReflectance(cosOi, 1.0, i, R);
    rphoton.radiance[i] *= R;
    rmean += rphoton.radiance[i];
    
    if(!_opaque){
      getDiffuseTransmittance(cosOi, 1.0, i, T);
      tphoton.radiance[i] *= T;
      tmean += tphoton.radiance[i];
    }
  
    getSpecularReflectance(cosOi, i, ROrth, RPara);
    photon.radiance[i]  *= (ROrth+RPara)*0.5;
    smean += photon.radiance[i];

    mean  += photon.radiance[i] + rphoton.radiance[i] + tphoton.radiance[i];
  }

  //Russian roulette (Absorption)
  if(rand()/(Real)RAND_MAX > mean)
    return false;

  //Specular reflexion
  if(rand()/(Real)RAND_MAX < smean/mean)
  {
    //Normalizing photon energy
    for(unsigned int i=0; i<GlobalSpectrum::nbWaveLengths(); i++)
      photon.radiance[i] /= mean;

    //Compute the reflected ray
    DielectricFormula::reflect(normal, photon.direction);
    specular = true;    
    return true;
  }

  //Reflexion
  if(rand()/(Real)RAND_MAX < rmean/((1-smean)*mean))
  {
    photon = rphoton;
    generatePhoton(normal, photon, rmean);
    specular = false;    
    return true;
  }
  //Transmission
  else if(!_opaque)
  {
    photon = tphoton;
    normal.mul(-1.0);
    generatePhoton(normal, photon, tmean);
    specular = false;    
    return true;
  }

  return false;
}

void VarnishedLambertianBRDF::generatePhoton(const Vector& normal, MultispectralPhoton& photon, Real mean)
{
  //Normalizing photon energy
  for(unsigned int i=0; i<GlobalSpectrum::nbWaveLengths(); i++)
    photon.radiance[i] /= mean;

  Real norm2;
  Real cosOi;
  do{
    photon.direction[0]=rand()*2.0/RAND_MAX - 1.0;
    photon.direction[1]=rand()*2.0/RAND_MAX - 1.0;
    photon.direction[2]=rand()*2.0/RAND_MAX - 1.0;
    norm2=photon.direction.square();
    photon.direction.normalize();
    cosOi = photon.direction.dot(normal);
  }while(norm2>cosOi*cosOi || cosOi<=0);
}

/**
 * Compute the secondary rays for diffuse reflexion and place them into the subrays
 * vector.
 * localBasis : the local base on the object at the computation point.
 * surfaceCoordinate : the surface  coordinate (texture coordinate) of the computation 
 *   point on the object.
 * view : the view ray (from the camera or bounced)
 * nbRays : the number of wanted ray. It is an indicative information.
 * subrays : the vector were the secondaries rays will be put.
 * weights : the weights corresponding to the distribution
 */
void VarnishedLambertianBRDF::getRandomDiffuseRay(const Basis& localBasis, const Point2D& surfaceCoordinate, LightVector& reemitedLight, unsigned int nbRays, std::vector<LightVector>& subrays)
{
 Vector normal=localBasis.k;
  if(normal.dot(reemitedLight.getRay().v)>0)
    return;

  if(!_opaque)
    nbRays = (nbRays+1)/2;

  //Reflexion 
  generateRandomeDiffuseRay(normal, localBasis.o, nbRays, reemitedLight, subrays);

  if(_opaque)
    return;

  //Transmission 
  normal.mul(-1.0);
  generateRandomeDiffuseRay(normal, localBasis.o, nbRays, reemitedLight, subrays);
}

void VarnishedLambertianBRDF::generateRandomeDiffuseRay(const Vector& normal, const Point& origin, unsigned int nbRays, LightVector& reemitedLight, std::vector<LightVector>& subrays)
{
  for(unsigned int i=0; i<nbRays; i++)
  {
    Vector incident;
    Real norm2;
    Real cosOi;
    do{
      incident[0]=rand()*2.0/RAND_MAX - 1.0;
      incident[1]=rand()*2.0/RAND_MAX - 1.0;
      incident[2]=rand()*2.0/RAND_MAX - 1.0;
      norm2=incident.square();
      incident.normalize();
      cosOi=incident.dot(normal);
    }while(norm2>cosOi*cosOi || cosOi<=0.0);

    LightVector subray;
    subray.setRay(origin, incident);
    subray.changeReemitedPolarisationFramework(normal);
    subray.initSpectralData(reemitedLight);
    subray.setWeight(1.0/M_PI);
    subrays.push_back(subray);
  }
}

/**
 * Get the reflectance in the reflection direction
 * @param cosOi : the cosinus of the angle between the normal and incident.
 * @param index : the wavelenght index to get
 * @param ROrth : the othogonal polarised computed Reflectance will be placed here
 * @param Rpara : the parallel polarised computed Reflectance will be placed here
 */
void VarnishedLambertianBRDF::getDiffuseReflectance(const Real& cosOi, const Real& cosOv, const int& index, Real& R)
{
  Real factori = 2.0*std::acos(cosOi)*_samples/M_PI;
  if(cosOi>1.0) factori=0.0;
  int iOi0 = (int)factori;
  int iOi1 = iOi0+1;
  factori -= iOi0;
  
  if(iOi0>=_samples-1)
  {
    iOi0 = _samples-1;
    iOi1 = _samples-1;
  }

  Real factorv = 2.0*std::acos(cosOv)*_samples/M_PI;
  if(cosOv>1.0) factorv=0.0;
  int iOv0 = (int)factorv;
  int iOv1 = iOv0+1;
  factorv -= iOv0;

  if(iOv0>=_samples-1)
  {
    iOv0 = _samples-1;
    iOv1 = _samples-1;
  }

  Real R0=_Rd[iOi0*_samples+iOv0][index];
  Real R1=_Rd[iOi1*_samples+iOv0][index];
  Real R2=_Rd[iOi0*_samples+iOv1][index];
  Real R3=_Rd[iOi1*_samples+iOv1][index];

  R = R0*(1-factori)*(1-factorv)
    + R1*(factori)*(1-factorv)
    + R2*(1-factori)*(factorv)
    + R3*(factori)*(factorv);
}

/**
 * Get the reflectance in the reflection direction
 * @param cosOi : the cosinus of the angle between the normal and incident.
 * @param index : the wavelenght index to get
 * @param ROrth : the othogonal polarised computed Reflectance will be placed here
 * @param Rpara : the parallel polarised computed Reflectance will be placed here
 */
void VarnishedLambertianBRDF::getDiffuseTransmittance(const Real& cosOi, const Real& cosOv, const int& index, Real& T)
{
  Real factori = 2.0*std::acos(cosOi)*_samples/M_PI;
  if(cosOi>1.0) factori=0.0;
  int iOi0 = (int)factori;
  int iOi1 = iOi0+1;
  factori -= iOi0;
  
  if(iOi0>=_samples-1)
  {
    iOi0 = _samples-1;
    iOi1 = _samples-1;
  }

  Real factorv = 2.0*std::acos(cosOv)*_samples/M_PI;
  if(cosOv>1.0) factorv=0.0;
  int iOv0 = (int)factorv;
  int iOv1 = iOv0+1;
  factorv -= iOv0;

  if(iOv0>=_samples-1)
  {
    iOv0 = _samples-1;
    iOv1 = _samples-1;
  }

  Real T0=_Td[iOi0*_samples+iOv0][index];
  Real T1=_Td[iOi1*_samples+iOv0][index];
  Real T2=_Td[iOi0*_samples+iOv1][index];
  Real T3=_Td[iOi1*_samples+iOv1][index];

  T = T0*(1-factori)*(1-factorv)
    + T1*(factori)*(1-factorv)
    + T2*(1-factori)*(factorv)
    + T3*(factori)*(factorv);
}


/**
 * Get the reflectance in the reflection direction
 * @param cosOi : the cosinus of the angle between the normal and incident.
 * @param index : the wavelenght index to get
 * @param ROrth : the othogonal polarised computed Reflectance will be placed here
 * @param Rpara : the parallel polarised computed Reflectance will be placed here
 */
void VarnishedLambertianBRDF::getSpecularReflectance(const Real& cosOi, const int& index, Real& ROrth, Real& RPara)
{
  Real factor = 2.0*std::acos(cosOi)*_samples/M_PI;
  if(cosOi>1.0) factor=0.0;
  int iOi =(int)factor;
  factor -= iOi;

  Real Rs0, Rp0, Rs1, Rp1;
  if(iOi>=_samples-1){
    Rs0=_Rs[_samples-1][index];
    Rp0=_Rp[_samples-1][index];
    Rs1=_Rs[_samples-1][index];
    Rp1=_Rp[_samples-1][index];
  }
  else{
    Rs0=_Rs[iOi][index];
    Rp0=_Rp[iOi][index];
    Rs1=_Rs[iOi+1][index];
    Rp1=_Rp[iOi+1][index];    
  }

  RPara = Rp0*(1-factor) + Rp1*factor;
  ROrth = Rs0*(1-factor) + Rs1*factor;
}

void VarnishedLambertianBRDF::getDiffuseReemitedFromAmbiant(const Basis& localBasis, const Point2D& surfaceCoordinate, LightVector& reemitedLight, const Spectrum& incident)
{
  Real cosOi = Real(1.0);//-incidentLight.getRay().v.dot(localBasis.k);
  Real cosOv = -reemitedLight.getRay().v.dot(localBasis.k);

  if(cosOv<=0.0 || (cosOi<=0.0 && _opaque))
  {
    reemitedLight.clear();
    return;
  }
  
  for(unsigned int wl=0; wl < reemitedLight.size(); wl++) {
    Real R;
    getDiffuseReflectance(cosOi, cosOv, wl, R);
    reemitedLight[wl].setRadiance(incident[wl] /** cosOv*/ * R);
  }
  reemitedLight.changeReemitedPolarisationFramework(localBasis.k);
}
