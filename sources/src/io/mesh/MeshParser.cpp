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
#include <io/mesh/MeshParser.hpp>
//!
//! @file MeshParser.cpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file implements classs declared in MeshParser.hpp 
//!  @arg MeshParser
//!
#include <fstream>
#include <iostream>

#include <core/VrtLog.hpp>
#include <exceptions/Exception.hpp>
//////////////////////////////// class MeshParser //////////////////////////////
Mesh* MeshParser::loadMesh3(std::string filename, bool double_sided)
{
  //Open the file
  std::fstream input(filename.c_str(), std::fstream::in);
  if(!input.is_open()) {
    throw Exception("(MeshParser::loadMesh3) Ouverture du fichier " 
                    + filename + " impossible.");
  }
  //Load the header
  int nbVertex, nbFace, nbBoundingBox;
  input >> nbVertex >> nbFace >> nbBoundingBox;

  //Drop the main bounding box  
  float ignoref;
  for(int i=0; i<6; i++)
    input >> ignoref;

  //Reading vertex data
  Point* vertices = new Point[nbVertex];
  Vector* normals = new Vector[nbVertex];
  int* faces = new int[3*nbFace];
  for(int i = 0; i < nbVertex; i++) {
    input >> vertices[i][0] >> vertices[i][1] >> vertices[i][2]
          >> normals[i][0]  >> normals[i][1]  >> normals[i][2];
  }

  //Reading face data
  int current_face=0;
  int numInBound=0;
  for(int i = 0; i < nbBoundingBox; i++) {
    //Reading Bounding Box information
    input >> numInBound;
    //Ignoring the 6 bound coordinates     
    for(int j = 0; j < 6; j++)
      input >> ignoref;
   
    //Reading face contained in the bounding box
    for(int j = 0; j < numInBound; j++) {
      //Reading face information
      for(int k = 0; k < 3; k++)
        input >> faces[current_face*3 + k];
      //Ignoring face normal
      for(int k = 0; k < 3; k++)
        input >> ignoref;
      current_face++;
    }
  }
  
  //Closing file
  input.close();

  //Building triangles
  Triangle* triangles = new Triangle[nbFace];
  for(int i = 0; i < nbFace; i++) { 
    Point2D surfaceCoordinates[3];
    surfaceCoordinates[0][0] = 0; surfaceCoordinates[0][1] = 0;
    surfaceCoordinates[1][0] = 0; surfaceCoordinates[1][1] = 0;
    surfaceCoordinates[2][0] = 0; surfaceCoordinates[2][1] = 0;

    Point verticestopass[3] = { vertices[faces[i*3]], 
                                vertices[faces[i*3+1]], 
                                vertices[faces[i*3+2]] };
    Vector normalstopass[3] = { normals[faces[i*3]], 
                                normals[faces[i*3+1]], 
                                normals[faces[i*3+2]] };

    triangles[i].set(verticestopass, normalstopass, surfaceCoordinates, double_sided);
  }

  //Building mesh
  Mesh* mesh= new Mesh(triangles, nbFace);
  
  //Free temporary arrays.
  delete[] vertices;
  delete[] normals;
  delete[] faces;
  delete[] triangles;
  
  //Done !
  return mesh;
}
//////////////////////////////// class MeshParser //////////////////////////////
Mesh* MeshParser::loadOBJ(std::string filename, bool double_sided) {
  VrtLog::Write("-- MeshParser::loadOBJ(%s)", filename.c_str());
  
  //Open the file
  std::fstream input(filename.c_str(), std::fstream::in);
  if(!input.is_open()) {
    throw Exception("(MeshParser::loadOBJ) Ouverture du fichier " 
                    + filename + " impossible.");
  }
  std::vector<Point> vertices;
  std::vector<Vector> normals;
  std::vector<Point2D> texcoords;
  std::vector<Face> faces;

  while(input.good()) {
    eatWhiteSpace(input);

    char tmp;
    input >> tmp;
    switch(tmp) {
    case 'v': {
      input >> tmp;
      if(tmp == 'n') {
        Vector norm;
        input >> norm[0] >> norm[1] >> norm[2];
        normals.push_back(norm);
      } else if(tmp=='t') {
        Point2D coord;
        input >> coord[0] >> coord[1];
        texcoords.push_back(coord);
      } else {
        input.unget();
        Point vert;
        input >> vert[0] >> vert[1] >> vert[2];
        vertices.push_back(vert);
      }
      break;
    }
    case 'f': {
      Face face;
      for(int i = 0; i < 3; i++) {
        int index;
        //Vertex
        input >> index;
        face.vertices[i] = vertices[index-1];
        input.ignore(1);
        //Texture coordinate
        input >> tmp;
        if(tmp != '/') {
          input.unget();
          input >> index;
          face.texcoords[i] = texcoords[index-1];
          input.ignore(1);
        }
        //Normal
        input >> index;
        face.normals[i] = normals[index-1];
      }
      faces.push_back(face);
      break;
    }
    default:  {
      std::string trashline;
      getline(input, trashline);
      break;
      }
    } 
  }
  
  //Building triangles
  Triangle* triangles = new Triangle[faces.size()];
  for(unsigned int i=0; i<faces.size(); i++) {
    triangles[i].set(faces[i].vertices, faces[i].normals, faces[i].texcoords, double_sided);
    triangles[i].Print();
  }
  //Building mesh
  Mesh* mesh = new Mesh(triangles, faces.size());
  
  //Free temporary arrays.
  delete[] triangles;
  
  //Done !
  return mesh;
}
//////////////////////////////// class MeshParser //////////////////////////////
void MeshParser::eatWhiteSpace(std::istream& stream)
{
  char tmp = 0;
  stream >> tmp;
  while(tmp==' ' || tmp=='\t' || tmp=='\n' || tmp=='\r'){
    stream >> tmp;
  }
  stream.unget();
}
////////////////////////////////////////////////////////////////////////////////