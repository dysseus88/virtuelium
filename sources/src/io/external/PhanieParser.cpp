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
#include <io/external/PhanieParser.hpp>
//!
//! @file PhanieParser.cpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file implements classs declared in PhanieParser.hpp 
//!  @arg PhanieParser
//!
#include <fstream>
#include <iostream>

#include <structures/Spectrum.hpp>

#include <core/Object.hpp>
#include <core/Source.hpp>

#include <objectshapes/ObjectShape.hpp>
#include <objectshapes/Mesh.hpp>
#include <objectshapes/Plane.hpp>
#include <objectshapes/Sphere.hpp>

#include <lightsources/LightSource.hpp>
#include <lightsources/PointLightSource.hpp>

#include <materials/Material.hpp>
#include <materials/NullBRDF.hpp>
#include <materials/TwoSidedBRDF.hpp>
#include <materials/LambertianBRDF.hpp>
#include <materials/TextureBRDF.hpp>
#include <materials/RefractiveBRDF.hpp>

#include <structures/Medium.hpp>
#include <structures/Texture.hpp>

#include <core/VrtLog.hpp>
#include <exceptions/Exception.hpp>
////////////////////////////// class PhanieParser //////////////////////////////
bool PhanieParser::LoadPhanie(std::string filename, 
                              std::vector<Object*>& objects,
                              std::vector<Source*>& sources) {
  VrtLog::Write("PhanieParser::LoadPhanie(%s, objects, sources)", 
                filename.c_str());

  // Try to open
  std::fstream input(filename.c_str(), std::fstream::in);
  if(!input.is_open())
    return false;

  // Current read word and value
  std::string word;
  std::string value;
  
  // Layer info
  int nb_layers;
  std::string layer_name;
  int layer_type;
  std::string material_filelink;

  // Geometrical Object info
  int nb_geo_object;
  std::string geo_object_name;
  std::string basis_type;
  Real geo_basis[9];
  Real txt_basis[8];
  int nb_faces;

  // Simple Media (for tests only)
  Spectrum r, r2, t;
  Medium *air = new Medium;
  air->useFresnelModel = true;
  Medium *white = new Medium;
  white->isOpaque = true;
  white->useLambertianModel = true;
  for (unsigned int wl = 0; wl < GlobalSpectrum::nbWaveLengths(); wl++) {
    r2[wl] = white->r[wl] = Real(0.5);
    air->n[wl] = r[wl] = Real(1.0);
  }
  while (input.good()) {
    // get first word of the line
    input >> word;
     
    // Number of layers in the scene
    if (word.find("layers:") == 0) {
      value = word.substr(word.find_first_not_of("layers:"));
      nb_layers = atoi(value.c_str());      
      //VrtLog::Write("Number of layers in the scene: %d", nb_layers);

    // Current layer
    } else if (word.find("layer:") == 0) {
      layer_name = word.substr(word.find_first_not_of("layer:"));
      VrtLog::Write("Current layer: %s", layer_name.c_str());

    // Type (light source or object)
    } else if (word.find("type:") == 0) {
      // Retrieve the type
      value = word.substr(word.find_first_not_of("type:"));
      layer_type = atoi(value.c_str());      
      //VrtLog::Write("Type (1=object; 2=lightsource): %d", layer_type);

    // Use material for this layer (link to a filename in the material library)
    } else if (word.find("materiau:") == 0) {
      material_filelink = word.substr(word.find_first_not_of("materiau:"));
      VrtLog::Write("Use material for this layer: %s", 
                    material_filelink.c_str());

    // Number of objects for this layer
    } else if (word.find("objets:") == 0) {
      value = word.substr(word.find_first_not_of("objets:"));
      nb_geo_object = atoi(value.c_str());      
      VrtLog::Write(" -- Number of objects for this layer: %d", nb_geo_object);

    // Current object 
    } else if (word.find("objet:") == 0) {
      geo_object_name = word.substr(word.find_first_not_of("objet:"));
      VrtLog::Write(" -- Current object: %s", geo_object_name.c_str());
      // Reset basis
      geo_basis[0] = geo_basis[1] = geo_basis[2] = Real(0);
      geo_basis[3] = geo_basis[4] = geo_basis[5] = Real(0);
      geo_basis[6] = geo_basis[7] = geo_basis[8] = Real(0);
      txt_basis[0] = txt_basis[1] = txt_basis[2] = txt_basis[3] = Real(0);
      txt_basis[4] = txt_basis[5] = txt_basis[6] = txt_basis[7] = Real(0);
      basis_type = "";

    // Local basis for light sources or materials or texture coefficients
    } else if (word.compare("repere") == 0) {
      input >> basis_type;
      //VrtLog::Write(" -- Basis type: %s", basis_type.c_str());
      // If the basis concerns a source or an object
      if (basis_type.compare("source") == 0 
          || basis_type.compare("materiau") == 0) {
        input >> geo_basis[0] >> geo_basis[1] >> geo_basis[2];
        input >> geo_basis[3] >> geo_basis[4] >> geo_basis[5];
        input >> geo_basis[6] >> geo_basis[7] >> geo_basis[8];
        //VrtLog::Write(" --   %f  %f  %f", geo_basis[0], geo_basis[1], geo_basis[2]);
        //VrtLog::Write(" --   %f  %f  %f", geo_basis[3], geo_basis[4], geo_basis[5]);
        //VrtLog::Write(" --   %f  %f  %f", geo_basis[6], geo_basis[7], geo_basis[8]);
      // If the basis concerns textures
      } else if (basis_type.compare("texture") == 0) {
        input >> txt_basis[0] >> txt_basis[1] >> txt_basis[2] >> txt_basis[3];
        input >> txt_basis[4] >> txt_basis[5] >> txt_basis[6] >> txt_basis[7];       
        //VrtLog::Write(" --   %f  %f  %f %f", txt_basis[0], txt_basis[1], txt_basis[2], txt_basis[3]);
        //VrtLog::Write(" --   %f  %f  %f %f", txt_basis[4], txt_basis[5], txt_basis[6], txt_basis[7]);
      // Error case
      } else {
        input.close();
        return false;
      }
      
    // Number of faces to come
    } else if (word.find("facettes:") == 0) {
      value = word.substr(word.find_first_not_of("facettes:"));
      nb_faces = atoi(value.c_str());
      VrtLog::Write("Number of faces to come: %d", nb_faces);
      // Fill the list of triangles
      std::vector<Triangle> triangle_list;
      for (int i = 0; i < nb_faces; i++) {
        // Get the type of face
        input >> word;
        VrtLog::Write("Face %d", i);

        // Triangle
        if (word.compare("T") == 0) {
          Point v[3];
          input >> v[0][0] >> v[0][1] >> v[0][2];
          input >> v[1][0] >> v[1][1] >> v[1][2];
          input >> v[2][0] >> v[2][1] >> v[2][2];
          Point2D t[3];
          // Use defined texture mapping
          if (basis_type.compare("texture") == 0) {
            ComputeTextureCoordinate(v[0], t[0], txt_basis);
            ComputeTextureCoordinate(v[1], t[1], txt_basis);
            ComputeTextureCoordinate(v[2], t[2], txt_basis);
          // Use default texture mapping
          } else {
            t[0][0] = Real(0); t[0][1] = Real(1);
            t[1][0] = Real(0); t[1][1] = Real(0);
            t[2][0] = Real(1); t[2][1] = Real(1);
          }

          Triangle tri_1(v, t);
          triangle_list.push_back(tri_1);
          tri_1.Print();

        // Quad : must be triangulated         
        } else if (word.compare("Q") == 0) {
          Point v_1[3], v_2[3];
          input >> v_1[0][0] >> v_1[0][1] >> v_1[0][2];
          input >> v_1[1][0] >> v_1[1][1] >> v_1[1][2];
          input >> v_2[1][0] >> v_2[1][1] >> v_2[1][2];
          input >> v_2[2][0] >> v_2[2][1] >> v_2[2][2];
          v_1[2][0] = v_2[2][0]; v_1[2][1] = v_2[2][1]; v_1[2][2] = v_2[2][2];
          v_2[0][0] = v_1[1][0]; v_2[0][1] = v_1[1][1]; v_2[0][2] = v_1[1][2];
          Point2D t_1[3], t_2[3];
          // Use defined texture mapping
          if (basis_type.compare("texture") == 0) {
            ComputeTextureCoordinate(v_1[0], t_1[0], txt_basis);
            ComputeTextureCoordinate(v_1[1], t_1[1], txt_basis);
            ComputeTextureCoordinate(v_1[2], t_1[2], txt_basis);
            ComputeTextureCoordinate(v_2[0], t_2[0], txt_basis);
            ComputeTextureCoordinate(v_2[1], t_2[1], txt_basis);
            ComputeTextureCoordinate(v_2[2], t_2[2], txt_basis);
          // Use default texture mapping
          } else {
            t_1[0][0] = Real(0); t_1[0][1] = Real(1);
            t_1[1][0] = Real(0); t_1[1][1] = Real(0);
            t_1[2][0] = Real(1); t_1[2][1] = Real(1);
            t_2[0][0] = Real(0); t_2[0][1] = Real(0);
            t_2[1][0] = Real(1); t_2[1][1] = Real(0);
            t_2[2][0] = Real(1); t_2[2][1] = Real(1);
          }
          // Fill first triangle
          Triangle tri_1(v_1, t_1);
          triangle_list.push_back(tri_1);
          tri_1.Print();
          // Fille second triangle
          Triangle tri_2(v_2, t_2);
          triangle_list.push_back(tri_2);
          tri_2.Print();

          // Error case 
        } else {
          input.close();
          return false;
        }
      } // end of face loop
      
      // Create shape
      ObjectShape* current_shape = new Mesh(&triangle_list[0], 
                                            triangle_list.size());
      triangle_list.clear();
      
      // Case: Object => new Object
      if (layer_type == 1) {
        Texture *txt = new Texture;
        txt->SetImage("projects/phanie/data/textures/text.png");
        Material* in = new TextureBRDF(new LambertianBRDF(r2, t, true), txt, 1, 1, 1);
        Material* ext = new NullBRDF();
        Object* current_object = new Object(current_shape, new TwoSidedBRDF(ext, in), air, white);
        objects.push_back(current_object);

      // Case: light sources => new Source
      } else if (layer_type == 2) {
        LightSource* current_lightsource = new PointLightSource(r, 500, Point(0,0,0.25), 0.01);
        Source* current_source = new Source(current_lightsource, current_shape);
        sources.push_back(current_source);
      }

    // This line shloud be ignore
    } else {
      std::string trashline;
      getline(input, trashline);
    }
  }

  input.close();
  return true;
}
////////////////////////////////////////////////////////////////////////////////