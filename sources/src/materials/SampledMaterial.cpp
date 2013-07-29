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

#include <materials/SampledMaterial.hpp>
#include <exceptions/Exception.hpp>
#include <stdio.h>

/**
 * Constructor
 * spectrum : the normal incident reflection spectrum.
 */
SampledMaterial::SampledMaterial(std::string filename)
: Material(false, true)
{
  //Opening file
  FILE* file = fopen(filename.c_str(), "r");
  if(!file)
    throw Exception("(SampledMaterial::SampledMaterial) Ouverture du fichier "+filename+" impossible.");

  //Eating comments
  int tmp;
  while((tmp=fgetc(file))=='#')
    while(tmp!='\n' && tmp!=EOF)
      tmp=fgetc(file);

  //Case of Sampled JSBRDF
  if(tmp=='S')
  {
    //Reading the header informations and allocate the needed memory
    if (fscanf(file, " %d %d %d %d %d ", 
              &_OiStep, &_PiStep, &_LambdaStart, 
              &_LambdaStep, &_LambdaSample) >= 0) {
    
      _OiSample=(90/_OiStep) + 1;
      _PiSample=(180/_PiStep) + 1;
      _BRDF=new std::vector<SampledBRDFEntry>[_OiSample*_PiSample*_LambdaSample];

      //Reading values
      int lambda, pol, Oi, Pi, Ov, Pv;
      float value;
      while(fscanf(file, " %d %d %d %d %d %d %f ", &lambda, &pol, &Oi, &Pi, &Ov, &Pv, &value)==7)
      {
        //Changing the angle reference for Phi v
        Pv = (Pv+Pi)%360;

        //Computing index
        Oi     = Oi/_OiStep;
        Pi     = Pi/_PiStep;
        lambda = (lambda - _LambdaStart) / _LambdaStep;
        int index = getIndex(Oi, Pi, lambda);

        //Try to add the data into an existing entry
        bool added=false;
        for(std::vector<SampledBRDFEntry>::iterator i = _BRDF[index].begin(); i!=_BRDF[index].end(); i++)
          if( (*i).theta==Ov && (*i).phi==Pv)
          {
            if(pol>0) (*i).rOrth=value;
            if(pol<0) (*i).rPara=value;
            added=true;
            break;
          }
        //If no entry have been found, create one
        if(!added)
        {
          SampledBRDFEntry entry;
          entry.theta=Ov;
          entry.phi=Pv;
          entry.rOrth=(pol>0)? value : 0;
          entry.rPara=(pol<0)? value : 0;
          _BRDF[index].push_back(entry);
        }
      }
    }
  }
 
  //Close the file
  fclose(file);
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
void SampledMaterial::getDiffuseReemited(const Basis& localBasis, const Point2D& surfaceCoordinate, const LightVector& incidentLight, LightVector& reemitedLight)
{
  reemitedLight.clear();
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
void SampledMaterial::getSpecularReemited(const Basis& localBasis, const Point2D& surfaceCoordinate, const LightVector& incidentLight, LightVector& reemitedLight)
{
  const Vector& incident = incidentLight.getRay().v;
  const Vector& view     = reemitedLight.getRay().v;

  Real Oi = std::acos(-localBasis.k.dot(incident)); 
  Real Pi = std::abs(std::atan2(-localBasis.j.dot(incident), -localBasis.i.dot(incident)));
  Real Ov = std::acos(-localBasis.k.dot(view));
  Real Pv = std::atan2(-localBasis.j.dot(view), -localBasis.i.dot(view));

  if(-localBasis.k.dot(incident)<0)
  {
    reemitedLight.clear();
    return;
  }

  //Setting the polarisation framework
  LightVector localIncidentLight(incidentLight);
  localIncidentLight.changeIncidentPolarisationFramework(localBasis.k);
  reemitedLight.changeReemitedPolarisationFramework(localBasis.k);

  for(unsigned int i=0; i<localIncidentLight.size(); i++)
  {
    //Computing index
    int iOv     = (int)(Ov*180.0/M_PI)/_OiStep;
    int iPv     = (int)(Pv*180.0/M_PI)/_PiStep;
    int index = getIndex(iOv, iPv, localIncidentLight[i].getIndex());

    if(_BRDF[index].size()!=0)
    {
      //Searching the best entry
      SampledBRDFEntry* entry = &_BRDF[index][0];
      for(unsigned int j=1; j<_BRDF[index].size(); j++)
        if(std::abs(Oi - entry->theta)+std::abs(Pi - entry->phi) > std::abs(Oi - _BRDF[index][j].theta)+std::abs(Pi - _BRDF[index][j].phi))
          entry = &_BRDF[index][j];

      //Applying the reflectance
      reemitedLight[i].applyReflectance(localIncidentLight[i], entry->rPara, entry->rOrth);  
    }
    else
      reemitedLight[i].setRadiance(0);  
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
void SampledMaterial::getSpecularSubRays(const Basis& localBasis, const Point2D& surfaceCoordinate, LightVector& reemitedLight, std::vector<LightVector>& subrays)
{
  const Vector& view     = reemitedLight.getRay().v;
  Real Ov = std::acos(-localBasis.k.dot(view)); 
  Real Pv = std::atan2(-localBasis.j.dot(view), -localBasis.i.dot(view));

  for(unsigned int i=0; i<reemitedLight.size(); i++)
  {
    //Computing index
    int iOv     = (int)(Ov*180.0/M_PI)/_OiStep;
    int iPv     = (int)(Pv*180.0/M_PI)/_PiStep;
    int index = getIndex(iOv, iPv, reemitedLight[i].getIndex());
    
    //Generate subrays
    for(unsigned int j=1; j<_BRDF[index].size(); j++)
    {
      //Generating the incident Ray
      Real Oi = _BRDF[index][i].theta*M_PI/180.0;
      Real Pi = _BRDF[index][i].phi*M_PI/180.0;
      Vector inbase(std::cos(Pi)*std::sin(Oi), std::sin(Pi)*std::sin(Oi) , std::cos(Oi));
      Vector direction;
      localBasis.getFromBasis(inbase, direction);

      //Generating the light vector
      LightVector subray;
      subray.setRay(localBasis.o, direction);      
      subray.initSpectralData(reemitedLight[i].getIndex()); 
      subray.changeReemitedPolarisationFramework(localBasis.k);
      subrays.push_back(subray);
    }
  }
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
bool SampledMaterial::bouncePhoton(const Basis& localBasis, const Point2D& surfaceCoordinate, MultispectralPhoton& photon, bool& specular)
{
  return false;
}

void SampledMaterial::getDiffuseReemitedFromAmbiant(const Basis& localBasis, const Point2D& surfaceCoordinate, LightVector& reemitedLight, const Spectrum& incident)
{
  reemitedLight.clear();
  reemitedLight.changeReemitedPolarisationFramework(localBasis.k);
}