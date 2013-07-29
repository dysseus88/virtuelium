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
#include <structures/Texture.hpp>
//!
//! @file Texture.cpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file implements the classes declared in Texture.hpp 
//!  @arg SampleColor
//!  @arg SampleSegment
//!  @arg SampleArea
//!  @arg ReferenceSample
//!  @arg Texture
//! @todo 
//! @remarks 
//!
#include <iostream>

#include <core/LightBase.hpp>

#include <exceptions/Exception.hpp>

#include <io/XMLParser.hpp>
#include <io/XMLTree.hpp>
#include <io/DataParser.hpp>

//#include <fstream>
//std::ofstream fluxFichierSVG("visualisation.svg");
////////////////////////////////////////////////////////////////////////////////
SampleColor::SampleColor(void) {
  SetRGB(0, 0, 0);
  m_spectrum.clear();
}
////////////////////////////////////////////////////////////////////////////////
SampleColor::SampleColor(double R, double G, double B) {
  SetRGB(R, G, B);
  m_spectrum.clear();
}
////////////////////////////////////////////////////////////////////////////////
SampleColor::~SampleColor(void) {
}
////////////////////////////////////////////////////////////////////////////////
void SampleColor::SetRGB(double R, double G, double B) {
  if (R + G + B != 0) {
    r = R / (R+G+B);
    g = G / (R+G+B);
    sum = R + G + B;
  
  } else {
    r = 0;
    g = 0;
    sum = 0;
  }
}
////////////////////////////////////////////////////////////////////////////////
void SampleColor::ClearSpectrum(void) { }
////////////////////////////////////////////////////////////////////////////////
void SampleColor::SetSpectrum(const Spectrum& spectrum) {
  m_spectrum = spectrum;
}
////////////////////////////////////////////////////////////////////////////////
Spectrum SampleColor::GetSpectrum(void) {
  return m_spectrum;
}
////////////////////////////////////////////////////////////////////////////////
void SampleColor::SetNull(void) {
  m_spectrum.clear();
}
////////////////////////////////////////////////////////////////////////////////
void SampleColor::SetError(void) {
  m_spectrum[0] = Real(kERROR);
}
////////////////////////////////////////////////////////////////////////////////
double SampleColor::GetR(void) {
    return r;
}
////////////////////////////////////////////////////////////////////////////////
double SampleColor::GetG(void) {
    return g;
}
////////////////////////////////////////////////////////////////////////////////
double SampleColor::GetSum(void) {
    return sum;
}
////////////////////////////////////////////////////////////////////////////////
double SampleColor::GetDistanceFrom(const SampleColor& otherPoint) {
    return (r - otherPoint.r) * (r - otherPoint.r)
           + (g - otherPoint.g) * (g - otherPoint.g);
}
////////////////////////////////////////////////////////////////////////////////
SampleSegment::SampleSegment(int firstPoint, int secondPoint) {
    i_point1 = firstPoint;
    i_point2 = secondPoint;
    i_area1 = kERROR;
    i_borderArea1 = kERROR;
}
////////////////////////////////////////////////////////////////////////////////
SampleSegment::~SampleSegment(void) {
}
////////////////////////////////////////////////////////////////////////////////
bool SampleSegment::Contains(int point) {
  if (i_point1 == point || i_point2 == point)
    return true;

  return false;
}
////////////////////////////////////////////////////////////////////////////////
int SampleSegment::GetPoint1(void) {
  return i_point1;
}
////////////////////////////////////////////////////////////////////////////////
int SampleSegment::GetPoint2(void) {
  return i_point2;
}
////////////////////////////////////////////////////////////////////////////////
void SampleSegment::SetArea1(int SampleArea, int border) {
  i_area1 = SampleArea;
  i_borderArea1 = border;
}
////////////////////////////////////////////////////////////////////////////////
int SampleSegment::GetArea1(void) {
  return i_area1;
}
////////////////////////////////////////////////////////////////////////////////
int SampleSegment::GetBorderArea1(void) {
  return i_borderArea1;
}
////////////////////////////////////////////////////////////////////////////////
SampleArea::SampleArea(int firstPoint, int secondPoint, int ThirdPoint, 
                       ReferenceSample* parentIN) {
  i_point1 = firstPoint;
  i_point2 = secondPoint;
  i_point3 = ThirdPoint;
  i_neighbor1 = kERROR;
  i_neighbor2 = kERROR;
  i_neighbor3 = kERROR;

  p_parent = parentIN;

  this->computeMatrixForLocation();
}
////////////////////////////////////////////////////////////////////////////////
SampleArea::SampleArea(SampleSegment& firstSegment, int ThirdPoint, 
                       ReferenceSample* parentIN) {
  i_point1 = firstSegment.GetPoint1();
  i_point2 = firstSegment.GetPoint2();
  i_point3 = ThirdPoint;
  i_neighbor1 = kERROR;
  i_neighbor2 = kERROR;
  i_neighbor3 = kERROR;

  p_parent = parentIN;

  this->computeMatrixForLocation();
}
////////////////////////////////////////////////////////////////////////////////
SampleArea::~SampleArea(void) { }
////////////////////////////////////////////////////////////////////////////////
bool SampleArea::Contains(SampleSegment& seg) {
  if (seg.GetPoint1() == i_point1) {
    if (seg.GetPoint2() == i_point2) 
      return true;
    if (seg.GetPoint2() == i_point3) 
      return true;
    return false;
  }

  if (seg.GetPoint1() == i_point2) {
    if (seg.GetPoint2() == i_point1) 
      return true;
    if (seg.GetPoint2() == i_point3) 
      return true;
    return false;
  }

  if (seg.GetPoint1() == i_point3) {
    if (seg.GetPoint2() == i_point1) 
      return true;
    if (seg.GetPoint2() == i_point2) 
      return true;
    return false;
  }

  return false;
}
////////////////////////////////////////////////////////////////////////////////
int SampleArea::ThirdPoint(SampleSegment& seg) {
  if(i_point1 != seg.GetPoint1() && i_point1 != seg.GetPoint2()) 
    return i_point1;
  
  if(i_point2 != seg.GetPoint1() && i_point2 != seg.GetPoint2()) 
    return i_point2;
  
  return i_point3;
}
////////////////////////////////////////////////////////////////////////////////
int SampleArea::GetPoint1(void) {
  return i_point1;
}
////////////////////////////////////////////////////////////////////////////////
int SampleArea::GetPoint2(void) {
  return i_point2;
}
////////////////////////////////////////////////////////////////////////////////
int SampleArea::GetPoint3(void) {
  return i_point3;
}
////////////////////////////////////////////////////////////////////////////////
int SampleArea::GetNeighbor1(void) {
  return i_neighbor1;
}
////////////////////////////////////////////////////////////////////////////////
int SampleArea::GetNeighbor2(void) {
  return i_neighbor2;
}
////////////////////////////////////////////////////////////////////////////////
int SampleArea::GetNeighbor3(void) {
  return i_neighbor3;
}
////////////////////////////////////////////////////////////////////////////////
void SampleArea::SetNeighbor(int neighbor, int border) {
  if (border == 1)  {
    i_neighbor1 = neighbor;
  
  } else if (border == 2) {
    i_neighbor2 = neighbor;
  
  } else if (border == 3) {
    i_neighbor3 = neighbor;
  }
}
////////////////////////////////////////////////////////////////////////////////
bool SampleArea::IsInsideBorder1(double x, double y) {
  if (f3*(a1*y + b1*x + c1) < 0.0)
    return false;

  return true;
}
////////////////////////////////////////////////////////////////////////////////
bool SampleArea::IsInsideBorder2(double x, double y) {
  if (f2*(a2*y + b2*x + c2) < 0.0)
    return false;

  return true;
}
////////////////////////////////////////////////////////////////////////////////
bool SampleArea::IsInsideBorder3(double x, double y) {
  if (f1*(a3*y + b3*x + c3) < 0.0)
    return false;

  return true;
}
////////////////////////////////////////////////////////////////////////////////
bool SampleArea::IsIncluded(double x, double y) {
  return (IsInsideBorder1(x, y) 
          && IsInsideBorder2(x, y) 
          && IsInsideBorder3(x, y));
}
////////////////////////////////////////////////////////////////////////////////
std::vector<double> SampleArea::calculateCoefficients(double x, double y) {
  std::vector<double> coefficients(3, 0.0);

  coefficients[0] = alpha[0]*x + alpha[1]*y + alpha[2];
  coefficients[1] = beta[0]*x + beta[1]*y + beta[2];
  coefficients[2] = gamma[0]*x + gamma[1]*y + gamma[2];

  return coefficients;
}
////////////////////////////////////////////////////////////////////////////////
void SampleArea::computeMatrixForLocation(void) {
  if (p_parent == NULL)
    return;

  double x1, x2, x3, y1, y2, y3, s1, s2, s3;
  s1 = p_parent->GetColor(i_point1).GetSum();
  s2 = p_parent->GetColor(i_point2).GetSum();
  s3 = p_parent->GetColor(i_point3).GetSum();
  x1 = p_parent->GetColor(i_point1).GetR();
  x2 = p_parent->GetColor(i_point2).GetR();
  x3 = p_parent->GetColor(i_point3).GetR();
  y1 = p_parent->GetColor(i_point1).GetG();
  y2 = p_parent->GetColor(i_point2).GetG();
  y3 = p_parent->GetColor(i_point3).GetG();

  // Let's inverse the 3x3 matrix:
  double det = x1*y2 + y1*x3 + x2*y3 - y2*x3 - y1*x2 - x1*y3;
  if (det != 0)  {
    alpha[0] = (y2-y3)/det/s1;
    alpha[1] = (x3-x2)/det/s1;
    alpha[2] = (x2*y3-y2*x3)/det/s1;

    beta[0] = (y3-y1)/det/s2;
    beta[1] = (x1-x3)/det/s2;
    beta[2] = (y1*x3-x1*y3)/det/s2;

    gamma[0] = (y1-y2)/det/s3;
    gamma[1] = (x2-x1)/det/s3;
    gamma[2] = (x1*y2-y1*x2)/det/s3;
  } else {
    //if (x1==x2 && x2==x3 && y1==y2 && y2==y3)
    //{
    alpha[0] = 0;
    alpha[1] = 0;
    alpha[2] = 1/s1;

    beta[0] = 0;
    beta[1] = 0;
    beta[2] = 0;

    gamma[0] = 0;
    gamma[1] = 0;
    gamma[2] = 0;
    //}
  }

  a1 = (x2-x1);    b1 = (y1-y2);    c1 = (y2-y1)*x1 - (x2-x1)*y1;
  f3 = a1*y3 + b1*x3 + c1;
  a2 = (x3-x1);    b2 = (y1-y3);    c2 = (y3-y1)*x1 - (x3-x1)*y1;
  f2 = a2*y2 + b2*x2 + c2;
  a3 = (x2-x3);    b3 = (y3-y2);    c3 = (y2-y3)*x3 - (x2-x3)*y3;
  f1 = a3*y1 + b3*x1 + c3;

  squareLength1 = (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1);
  squareLength2 = (x3-x1)*(x3-x1) + (y3-y1)*(y3-y1);
  squareLength3 = (x2-x3)*(x2-x3) + (y2-y3)*(y2-y3);
}
////////////////////////////////////////////////////////////////////////////////
double SampleArea::GetSquareLength(int border) {
  if (border == 1)
    return squareLength1;

  if (border == 2)
    return squareLength2;

  return squareLength3;
}
////////////////////////////////////////////////////////////////////////////////
ReferenceSample::ReferenceSample(
    const std::vector<std::string> list_sample_file) {
  m_list_points.clear();
  m_list_segments.clear();
  m_list_areas.clear();
	currentZone = 0;

  // Temp variables :
	double R = 0;
	double G = 0;
	double B = 0;


  // no sample files
  if (list_sample_file.size() == 0) {
    std::string msg = " -- ERREUR -- (ReferenceSample::ReferenceSample): ";
    msg += "Aucun fichier d'échantillonage n'est disponible !";
    throw Exception(msg);                    
  }

  // For each file, parse it and fill spectrum sample points
  for (unsigned int i = 0; i < list_sample_file.size(); i++) {
    std::string current_sample_file = list_sample_file[i];

    XMLParser root_parser;
    XMLTree * sample_root = NULL;

    // Open the current ssample file
    sample_root = root_parser.getXMLTree(current_sample_file.c_str());
    if (sample_root == NULL) {
      std::string msg = " -- ERREUR -- (ReferenceSample::ReferenceSample): ";
      msg += "Impossible de lire le fichier d'échantillonage ";
      msg += current_sample_file;
      msg += " !"; 
      throw Exception(msg);                    
    }    

    // Get RGB value;
    R = strtod(sample_root->getAttributeValue("red").c_str(), NULL);
    G = strtod(sample_root->getAttributeValue("green").c_str(), NULL);
    B = strtod(sample_root->getAttributeValue("blue").c_str(), NULL);

    // Parse the rest of the file
    DataParser data_parser;
    Spectrum current_spectrum;
    data_parser.loadSpectralMaterial(current_sample_file.c_str(),
                                     current_spectrum);


    m_list_points.push_back(SampleColor(R,G,B));
    m_list_points.at(m_list_points.size() - 1).SetSpectrum(current_spectrum);

    delete sample_root;
  }

  BuildAreas();
}
////////////////////////////////////////////////////////////////////////////////
ReferenceSample::~ReferenceSample(void) { }
////////////////////////////////////////////////////////////////////////////////
ReferenceSample::ReferenceSample(const ReferenceSample& r) {
 currentZone = r.currentZone;

 for (unsigned int i=0; i < r.m_list_points.size(); i++)
   m_list_points.push_back(r.m_list_points[i]);

 for (unsigned int i=0; i < r.m_list_segments.size(); i++)
   m_list_segments.push_back(r.m_list_segments[i]);

 for (unsigned int i=0; i < r.m_list_areas.size(); i++)
   m_list_areas.push_back(r.m_list_areas[i]);
}
////////////////////////////////////////////////////////////////////////////////
ReferenceSample& ReferenceSample::operator=(const ReferenceSample& r) {
	if (this != &r) {
    currentZone = r.currentZone;

    m_list_points.clear();
    for (unsigned int i=0; i < r.m_list_points.size(); i++)
      m_list_points.push_back(r.m_list_points[i]);

    m_list_segments.clear();
    for (unsigned int i=0; i < r.m_list_segments.size(); i++)
      m_list_segments.push_back(r.m_list_segments[i]);

    m_list_areas.clear();
    for (unsigned int i=0; i < r.m_list_areas.size(); i++)
      m_list_areas.push_back(r.m_list_areas[i]);
  }
  return *this;
}
////////////////////////////////////////////////////////////////////////////////
bool ReferenceSample::BuildAreas() {
  // 1. If there are less than 3 points, no zones can be created, process 
  // has to stop
  if (m_list_points.size() < 3) {
    return false;
  }

  // 2. Creation of the first segment (between the point 0 and the closest 
  // point to 0)
  int index = 0;
  double distance = 0.0;
  for (unsigned int i = 1; i < m_list_points.size(); i++) {
    if (i == 1) {
      index = 1;
      distance = m_list_points.at(1).GetDistanceFrom(m_list_points.at(0));

    } else {
      if (m_list_points.at(i).GetDistanceFrom(m_list_points.at(0)) < distance)
      {
        index = i;
        distance = m_list_points.at(i).GetDistanceFrom(m_list_points.at(0));
      }
    }
  }
  m_list_segments.push_back(SampleSegment(0, index)); // We add the new segment
  m_list_segments.at(0).SetArea1(0, 1); 

  // 3. Creation of the first zone
  double minimumArea = -1.0;
  double currentArea;
  for (unsigned int j = 0; j < m_list_points.size(); j++) {
    if (minimumArea == -1.0) {
      if (j != m_list_segments.at(0).GetPoint1() 
            && j != m_list_segments.at(0).GetPoint2()) {
        index = j;
        minimumArea = GetAreaOfTriangle(m_list_segments.at(0).GetPoint1(), 
                                        m_list_segments.at(0).GetPoint2(), j);
      }
    
    } else if (j != m_list_segments.at(0).GetPoint1() 
                && j != m_list_segments.at(0).GetPoint2()) {
      currentArea = GetAreaOfTriangle(m_list_segments.at(0).GetPoint1(), 
                                      m_list_segments.at(0).GetPoint2(), j);
      
      if (currentArea < minimumArea) {
        minimumArea = currentArea;
        index = j;
      }
    }
  }

  // We add the new zone
  m_list_areas.push_back(SampleArea(m_list_segments.at(0), index, this)); 


  // We add the 2 segments created
  m_list_segments.push_back(SampleSegment(m_list_segments.at(0).GetPoint1(), 
                                          index));
  m_list_segments.at(m_list_segments.size()-1).SetArea1(m_list_areas.size() - 1, 
                                                        2);

  m_list_segments.push_back(SampleSegment(m_list_segments.at(0).GetPoint2(), 
                                          index));
  m_list_segments.at(m_list_segments.size()-1).SetArea1(m_list_areas.size() - 1,
                                                        3);


  // 4. Here starts the algorithm of creaction of m_list_areas
  // We will scan the array of the segments, "s" is the segment currently 
  // processed
  int s = 0; 
  while (s < m_list_segments.size()) {
    int ThirdPoint = IndexOfTheThirdPoint(m_list_segments.at(s));
    // If the segment belongs to one and only one zone
    if (ThirdPoint >= 0 && ThirdPoint < m_list_points.size()) {
      index = -1;
      minimumArea = 1000000.0; // just a very high value
      // we scan the array of reference spectra
      for (unsigned int j=0; j < m_list_points.size(); j++) {

        // if the point j is on the other side of the segment, it is a prospect
        if (SameSide(m_list_segments.at(s), ThirdPoint, j) == false) {
          currentArea = GetAreaOfTriangle(m_list_segments.at(s).GetPoint1(),
                                          // the area of the triangle formed by 
                                          // segment s and point j
                                          m_list_segments.at(s).GetPoint2(), j); 
          
          if (currentArea < minimumArea) {
            if (CrossingSegments(m_list_segments.at(s),j) == false) {
              minimumArea = currentArea;
              index = j;
            }
          }
        }
      }

      // If indeed a point has been found
      if (index != -1) {
        // We add the new zone
        m_list_areas.push_back(SampleArea(m_list_segments.at(s), index, this));
        m_list_areas.at(m_list_areas.size() - 1)
                       .SetNeighbor(m_list_segments.at(s).GetArea1(), 1);
        m_list_areas.at(m_list_segments.at(s)
                       .GetArea1()).SetNeighbor(m_list_areas.size()-1, 
                                                m_list_segments.at(s).GetBorderArea1());

        // We add the 2 segments if they don't exist yet
        if (TheSegmentAlreadyExists(m_list_segments.at(s).GetPoint1(), 
                                    index) == false) {
          m_list_segments.push_back(SampleSegment(m_list_segments.at(s).GetPoint1(), index));
          m_list_segments.at(m_list_segments.size()-1).SetArea1(m_list_areas.size()-1, 2);
        } else {
          int otherZone = FindSegment(m_list_segments.at(s).GetPoint1(), index);
          m_list_areas.at(m_list_areas.size()-1).SetNeighbor(m_list_segments.at(otherZone).GetArea1(), 2);
          m_list_areas.at(m_list_segments.at(otherZone).GetArea1()).SetNeighbor(m_list_areas.size()-1, m_list_segments.at(otherZone).GetBorderArea1());
        }
        
        if (TheSegmentAlreadyExists(m_list_segments.at(s).GetPoint2(), index) == false)
        {
          m_list_segments.push_back(SampleSegment(m_list_segments.at(s).GetPoint2(), index));
          m_list_segments.at(m_list_segments.size()-1).SetArea1(m_list_areas.size()-1, 3);
        } else {
          int otherZone = FindSegment(m_list_segments.at(s).GetPoint2(), index);
          m_list_areas.at(m_list_areas.size()-1).SetNeighbor(m_list_segments.at(otherZone).GetArea1(), 3);
          m_list_areas.at(m_list_segments.at(otherZone).GetArea1()).SetNeighbor(m_list_areas.size()-1, m_list_segments.at(otherZone).GetBorderArea1());
        }
      } else {
        s++;
      }
    
    // If the segment "s" already belongs to 2 m_list_areas, we go to the next segment
    } else {
      s++;
    }
  }

  //// Dessine en svg les points et les Zones :
  //fluxFichierSVG << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>" << std::endl;
  //fluxFichierSVG << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1// EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">" << std::endl;
  //fluxFichierSVG << "<svg  xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" width=\"16cm\" height=\"16cm\" viewBox=\"-0.01 -0.01 1.01 1.01\">" << std::endl;
  //fluxFichierSVG << "<rect x=\"0\" y=\"0\" width=\"1\" height=\"1\" fill=\"white\" stroke-width=\"0.003\" stroke=\"grey\"/>" << std::endl;


  ////SEGMENTS
  //for (int i = 0; i < m_list_segments.size(); i++)
  //{
  //  std::cout << m_list_segments.at(i).GetPoint1() << "\t" << m_list_segments.at(i).GetPoint2() << "\t <->";
  //  fluxFichierSVG << "<line x1=\"" << m_list_points.at(m_list_segments.at(i).GetPoint1()).GetR();
  //  fluxFichierSVG << "\" y1=\""    << m_list_points.at(m_list_segments.at(i).GetPoint1()).GetG();
  //  fluxFichierSVG << "\" x2=\""    << m_list_points.at(m_list_segments.at(i).GetPoint2()).GetR();
  //  fluxFichierSVG << "\" y2=\""    << m_list_points.at(m_list_segments.at(i).GetPoint2()).GetG();
  //  fluxFichierSVG << "\" stroke-width=\"0.003\" stroke=\"#0000AA\"/>" << std::endl;
  //}


  ////POINTS
  //for (int i = 0; i < m_list_points.size(); i++)
  //{
  //  fluxFichierSVG << "<circle cx=\"" << m_list_points.at(i).GetR();
  //  fluxFichierSVG << "\" cy=\"" << m_list_points.at(i).GetG();
  //  fluxFichierSVG << "\" r=\"0.006\" fill=\"#BB0000\"/>" << std::endl;
  //  fluxFichierSVG << "<text x=\"" << m_list_points.at(i).GetR()+0.03;
  //  fluxFichierSVG << "\" y=\"" << m_list_points.at(i).GetG();
  //  fluxFichierSVG << "\" font-size=\"0.02\" fill=\"#000000\">" << i << "</text>" << std::endl;
  //}

  //std::cout  << std::endl;
  ////m_list_areas
  //for (int i = 0; i < m_list_areas.size(); i++)
  //{
  //  double cx = m_list_points.at(m_list_areas.at(i).GetPoint1()).GetR();
  //  cx += m_list_points.at(m_list_areas.at(i).GetPoint2()).GetR();
  //  cx += m_list_points.at(m_list_areas.at(i).GetPoint3()).GetR();
  //  cx /= 3.0;

  //  double cy = m_list_points.at(m_list_areas.at(i).GetPoint1()).GetG();
  //  cy += m_list_points.at(m_list_areas.at(i).GetPoint2()).GetG();
  //  cy += m_list_points.at(m_list_areas.at(i).GetPoint3()).GetG();
  //  cy /= 3.0;

  //  fluxFichierSVG << "<text x=\"" << cx;
  //  fluxFichierSVG << "\" y=\"" << cy+0.005;
  //  fluxFichierSVG << "\" font-size=\"0.01\" text-anchor=\"middle\">" << i << "(";
  //  fluxFichierSVG << m_list_areas.at(i).GetPoint1() << ";";
  //  fluxFichierSVG << m_list_areas.at(i).GetPoint2() << ";";
  //  fluxFichierSVG << m_list_areas.at(i).GetPoint3() << ")</text>" << std::endl;

  //}
  //fluxFichierSVG << "</svg>";

  if (m_list_areas.size() > 0) {
    //cout << "m_list_areas=" << m_list_areas.size() << std::endl;
    return true;
  }

  return false;
}
////////////////////////////////////////////////////////////////////////////////
double ReferenceSample::GetAreaOfTriangle(int point1, int point2, int point3) {
    //Heron's formula
    double a,b,c,s;
    a = m_list_points.at(point1).GetDistanceFrom(m_list_points.at(point3));
    b = m_list_points.at(point2).GetDistanceFrom(m_list_points.at(point3));
    c = m_list_points.at(point1).GetDistanceFrom(m_list_points.at(point2));
    s = a + b + c;
    return s * (s-a) * (s-b) * (s-c);
}
////////////////////////////////////////////////////////////////////////////////
bool ReferenceSample::SameSide(SampleSegment& seg, int point1, int point2) {
  double eq1, eq2;
  double a,b;

  if (seg.GetPoint1() == point1 
    || seg.GetPoint1() == point2 
    || point1 == point2) {
      return true;
  }
  if (seg.GetPoint2() == point1 || seg.GetPoint2() == point2)  {
    return true;
  }

  a = (m_list_points.at(seg.GetPoint1()).GetR() 
    - m_list_points.at(seg.GetPoint2()).GetR());


  b = (m_list_points.at(seg.GetPoint1()).GetG() 
    - m_list_points.at(seg.GetPoint2()).GetG());


  eq1 = a*m_list_points.at(point1).GetG() 
    - b*(m_list_points.at(point1).GetR() 
    - m_list_points.at(seg.GetPoint1()).GetR()) 
    - a*m_list_points.at(seg.GetPoint1()).GetG();
  eq2 = a*m_list_points.at(point2).GetG() 
    - b*(m_list_points.at(point2).GetR() 
    - m_list_points.at(seg.GetPoint1()).GetR()) 
    - a*m_list_points.at(seg.GetPoint1()).GetG();


  if (eq1 * eq2 >= 0) {
    return true;
  }
    return false;
}
////////////////////////////////////////////////////////////////////////////////
int ReferenceSample::IndexOfTheThirdPoint(SampleSegment& seg)
{
  int ThirdPoint = -1;
  int count = 0;
  for (int i = 0; i < m_list_areas.size(); i++) {
    if (m_list_areas.at(i).Contains(seg)) {
      ThirdPoint = m_list_areas.at(i).ThirdPoint(seg);
      count++;
    }
    // it would mean the segment belongs to more than 1 zone, we return the -2 error
    if (count > 1) {
      return -2;
    }

  }
  return ThirdPoint;
}
////////////////////////////////////////////////////////////////////////////////
int ReferenceSample::FindSegment(int point1, int point2) {
    for (unsigned int i=0; i < m_list_segments.size(); i++) {
        if (m_list_segments.at(i).GetPoint1() == point1 
              && m_list_segments.at(i).GetPoint2() == point2) 
          return i;
        if (m_list_segments.at(i).GetPoint1() == point2 
              && m_list_segments.at(i).GetPoint2() == point1) 
          return i;
    }
    return -1;
}
////////////////////////////////////////////////////////////////////////////////
bool ReferenceSample::TheSegmentAlreadyExists(int point1, int point2) {
  if (FindSegment(point1, point2) == kERROR) {
    return false;
  }
  return true;
}
////////////////////////////////////////////////////////////////////////////////
bool ReferenceSample::CrossingSegments(SampleSegment& seg, int point) {
  int A,B,C,D;
  A = seg.GetPoint1();
  B = seg.GetPoint2();

  if(point == A || point == B){
    return true;
  }

  for (unsigned int i=0; i<m_list_segments.size(); i++) {
    C = m_list_segments.at(i).GetPoint1();
    D = m_list_segments.at(i).GetPoint2();

    SampleSegment C_D(C,D);
    SampleSegment A_point(A, point);
    SampleSegment B_point(B, point);

    if (SameSide (C_D, A, point) == false 
      && SameSide (A_point, C, D) == false)
      return true;
    if (SameSide (C_D, B, point) == false 
      && SameSide (B_point, C, D) == false)
      return true;
  }

  return false;
}
////////////////////////////////////////////////////////////////////////////////
int ReferenceSample::FindArea(double x, double y) {
  bool untraceable = false;
  int nextZone = currentZone;

  while(untraceable == false) {
    if (m_list_areas.at(currentZone).IsInsideBorder1(x,y) == false) {
      nextZone = m_list_areas.at(currentZone).GetNeighbor1();
      if (nextZone == -1) {
        if (m_list_areas.at(currentZone).IsInsideBorder2(x,y) == false 
          && m_list_areas.at(currentZone).GetNeighbor2() >= 0) {
            nextZone = m_list_areas.at(currentZone).GetNeighbor2();

        } else if (m_list_areas.at(currentZone).IsInsideBorder3(x,y) == false 
          && m_list_areas.at(currentZone).GetNeighbor3() >= 0) {
            nextZone = m_list_areas.at(currentZone).GetNeighbor3();

        } else {
          return (-1)*currentZone -2;
        }
      }

    } else if (m_list_areas.at(currentZone).IsInsideBorder2(x,y) == false) {
      nextZone = m_list_areas.at(currentZone).GetNeighbor2();
      if (nextZone == -1) {
        if (m_list_areas.at(currentZone).IsInsideBorder3(x,y) == false 
          && m_list_areas.at(currentZone).GetNeighbor3() >= 0) {
            nextZone = m_list_areas.at(currentZone).GetNeighbor3();

        } else {
          return (-1)*currentZone -2;
        }
      }

    } else if (m_list_areas.at(currentZone).IsInsideBorder3(x,y) == false) {
      nextZone = m_list_areas.at(currentZone).GetNeighbor3();
      if (nextZone == -1) {
        return (-1)*currentZone -2;
      }

    } else {
      return currentZone;
    }

    currentZone = nextZone;
  }
  currentZone = 0;
  for (unsigned int i = 0; i<m_list_areas.size(); i++) {
    if (m_list_areas.at(i).IsIncluded(x, y))
      return i;
  }
  return -2;
}
////////////////////////////////////////////////////////////////////////////////
Spectrum ReferenceSample::RGBtoSpectrum(double R, double G, double B) {
  double sum = (double) (R + G + B);
  Spectrum theSpectrum;

  // black color => null spectrum
  if (sum == 0.0) {
    theSpectrum.clear();
    return theSpectrum;
  }

  double r = (double) R / sum; 
  double g = (double) G / sum; 
  double b = (double) B / sum; 
  int point1 = 0;
  int point2 = 0;
  int point3 = 0;
  double weight1 = 0, weight2 = 0, weight3 = 0;
  std::vector<double> coeffs;	

  int zoneOfTheColor = FindArea(r,g);
  bool isActuallyInAZone = true;
  if (zoneOfTheColor <= -2) {
    isActuallyInAZone = false;
    zoneOfTheColor = -(zoneOfTheColor+2);
    std::cout <<"\n -- ATTENTION --  " << zoneOfTheColor << std::endl 
              << std::cout.flush();
  }
  // If the point is in a Zone, we can calculate the coefficients
  if (isActuallyInAZone == true) {
    coeffs = m_list_areas.at(zoneOfTheColor).calculateCoefficients(r,g);

    if (coeffs.size()==3) {
      point1 = m_list_areas.at(zoneOfTheColor).GetPoint1();
      point2 = m_list_areas.at(zoneOfTheColor).GetPoint2();
      point3 = m_list_areas.at(zoneOfTheColor).GetPoint3();
      weight1 = coeffs.at(0)*sum;
      weight2 = coeffs.at(1)*sum;
      weight3 = coeffs.at(2)*sum;

    } else {
      //theSpectrum->setError(true);
      theSpectrum[0] = kERROR;
    }

  // If it is in no Zone
  } else {
    point1 = m_list_areas.at(zoneOfTheColor).GetPoint1();
    point2 = m_list_areas.at(zoneOfTheColor).GetPoint2();
    point3 = m_list_areas.at(zoneOfTheColor).GetPoint3();

    double squareLength;

    if (m_list_areas.at(zoneOfTheColor).IsInsideBorder1(r,g) == false) {
      double x1, x2, y1, y2, s1, s2;
      s1 = m_list_points.at(point1).GetSum();
      s2 = m_list_points.at(point2).GetSum();
      x1 = m_list_points.at(point1).GetR();
      x2 = m_list_points.at(point2).GetR();
      y1 = m_list_points.at(point1).GetG();
      y2 = m_list_points.at(point2).GetG();
      squareLength = m_list_areas.at(zoneOfTheColor).GetSquareLength(1);
      double up = (x2-r)*(x2-x1) + (y2-g)*(y2-y1);
      double lambda = up/squareLength;

      if (lambda < 0.0) {
        weight1 = 0.0;
        weight2 = sum/s2;
        weight3 = 0.0;
      } else if (lambda > 1.0) {
        weight1 = sum/s1;
        weight2 = 0.0;
        weight3 = 0.0;
      } else {
        weight1 = lambda*sum/s1;
        weight2 = (1-lambda)*sum/s2;
        weight3 = 0.0;
      }

    }
    else if (m_list_areas.at(zoneOfTheColor).IsInsideBorder2(r,g) == false) {
      double x1, x3, y1, y3, s1, s3;
      s1 = m_list_points.at(point1).GetSum();
      s3 = m_list_points.at(point3).GetSum();
      x1 = m_list_points.at(point1).GetR();
      x3 = m_list_points.at(point3).GetR();
      y1 = m_list_points.at(point1).GetG();
      y3 = m_list_points.at(point3).GetG();
      squareLength = m_list_areas.at(zoneOfTheColor).GetSquareLength(2);
      double up = (x3-r)*(x3-x1) + (y3-g)*(y3-y1);
      double lambda = up/squareLength;

      if (lambda < 0.0) {
        weight1 = 0.0;
        weight2 = 0.0;
        weight3 = sum/s3;
      } else if (lambda > 1.0) {
        weight1 = sum/s1;
        weight2 = 0.0;
        weight3 = 0.0;
      } else {
        weight1 = lambda*sum/s1;
        weight2 = 0.0;
        weight3 = (1-lambda)*sum/s3;
      }

    } else if (m_list_areas.at(zoneOfTheColor).IsInsideBorder3(r,g) == false) {
      double x3, x2, y3, y2, s3, s2;
      s3 = m_list_points.at(point3).GetSum();
      s2 = m_list_points.at(point2).GetSum();
      x3 = m_list_points.at(point3).GetR();
      x2 = m_list_points.at(point2).GetR();
      y3 = m_list_points.at(point3).GetG();
      y2 = m_list_points.at(point2).GetG();
      squareLength = m_list_areas.at(zoneOfTheColor).GetSquareLength(3);
      double up = (x2-r)*(x2-x3) + (y2-g)*(y2-y3);
      double lambda = up/squareLength;

      if (lambda < 0.0) {
        weight1 = 0.0;
        weight2 = sum/s2;
        weight3 = 0.0;
      } else if (lambda > 1.0) {
        weight1 = 0.0;
        weight2 = 0.0;
        weight3 = sum/s3;
      } else {
        weight1 = 0.0;
        weight2 = (1-lambda)*sum/s2;
        weight3 = lambda*sum/s3;
      }
    } else {
      theSpectrum[0] = kERROR;
    }
  }

    for(unsigned int i=0 ; i<GlobalSpectrum::nbWaveLengths() ; i++) {
    theSpectrum[i] 
      = weight1 * (m_list_points.at(point1).GetSpectrum())[i];
    theSpectrum[i] 
      += weight2 * (m_list_points.at(point2).GetSpectrum())[i];
    theSpectrum[i] 
      += weight3 * (m_list_points.at(point3).GetSpectrum())[i];
  }

  return theSpectrum;
}
////////////////////////////////////////////////////////////////////////////////
SampleColor& ReferenceSample::GetColor(int point)
{
  return m_list_points.at(point);
}
////////////////////////////////////////////////////////////////////////////////
Texture::Texture(void) 
	  : p_image(0), m_name(""), p_reference_sample(NULL) {
  std::vector<std::string> list_sample_files;
  list_sample_files.push_back(std::string("data/textures/spectralization/white.xml"));
  list_sample_files.push_back(std::string("data/textures/spectralization/red.xml"));
  list_sample_files.push_back(std::string("data/textures/spectralization/green.xml"));
  list_sample_files.push_back(std::string("data/textures/spectralization/blue.xml"));
  list_sample_files.push_back(std::string("data/textures/spectralization/cyan.xml"));
  list_sample_files.push_back(std::string("data/textures/spectralization/magenta.xml"));
  list_sample_files.push_back(std::string("data/textures/spectralization/yellow.xml"));

  p_reference_sample = new ReferenceSample(list_sample_files);
}
////////////////////////////////////////////////////////////////////////////////
Texture::~Texture() {
  if (p_reference_sample != NULL)
    delete p_reference_sample;
  p_reference_sample = NULL;

  ClearImage();
}
////////////////////////////////////////////////////////////////////////////////
Texture::Texture(const Texture& t)
    : p_image(0), m_name(""), p_reference_sample(NULL) {
  p_image = t.p_image;
  m_name = t.m_name;

  if (t.p_reference_sample != NULL)
    p_reference_sample = new ReferenceSample(*t.p_reference_sample);
}
////////////////////////////////////////////////////////////////////////////////
Texture& Texture::operator=(const Texture& t) {
	if (this != &t)
	{
    if (p_reference_sample != NULL)
      delete p_reference_sample;
    p_reference_sample = NULL;

    ClearImage();

    p_image = t.p_image;
    m_name = t.m_name;

    if (t.p_reference_sample != NULL)
      p_reference_sample = new ReferenceSample(*t.p_reference_sample);
	}
	return *this;
}
////////////////////////////////////////////////////////////////////////////////
void Texture::SetImage(const char* image_file) {	
	ClearImage();

	ImageParser parser;
	p_image = parser.load(image_file);
}
////////////////////////////////////////////////////////////////////////////////
Image* Texture::GetImage(void) {
	return p_image;
}
////////////////////////////////////////////////////////////////////////////////
void Texture::ClearImage(void) {
	if (p_image != NULL)
		delete p_image;
	p_image = NULL;
}
////////////////////////////////////////////////////////////////////////////////
void Texture::SetTextureName(const char* texture_name) {
	m_name = texture_name;
}
////////////////////////////////////////////////////////////////////////////////
const char* Texture::GetTextureName(void) const {
  return m_name.c_str();
}
////////////////////////////////////////////////////////////////////////////////
void Texture::GetPixelValue(const Real& x, const Real& y, Pixel& pixel) {
	pixel = p_image->getInterpolatedPixel(x * p_image->getWidth(), 
                                        y * p_image->getHeight());
}
////////////////////////////////////////////////////////////////////////////////
void Texture::GetSpectrumValue(const Real& x, const Real& y, Spectrum& sRGB)
{
	Pixel p(p_image->getNumberOfChannels());
	GetPixelValue(x, y, p);

	SpectralizeRGB(p[0], p[1], p[2], sRGB);
}
////////////////////////////////////////////////////////////////////////////////
void Texture::SpectralizeRGB(Real R, Real G, Real B, Spectrum& sRGB)
{
	Spectrum tempSpectrum;
  if (p_reference_sample != NULL)
    tempSpectrum = p_reference_sample->RGBtoSpectrum(R, G, B);

	for(unsigned int i = 0; i < GlobalSpectrum::nbWaveLengths(); i++) {
		sRGB[i] = tempSpectrum[i];
	}
}
////////////////////////////////////////////////////////////////////////////////
void Texture::GetAlphaValue(const Real& x, const Real& y, Real& alpha) {
	Pixel p(p_image->getNumberOfChannels());
	GetPixelValue(x, y, p);

	// Map must be in grey level
  alpha = p[0]; 
}
////////////////////////////////////////////////////////////////////////////////
//Spectrum81 Texture::RGBtoSpectrum(Real R8b, Real G8b, Real B8b)
//{
//        Spectrum81 spectrum;
//
//        // On vérifie que le triplet (R;G;B) est cohérent
//        if (R8b > 255 || R8b < 0 || G8b > 255 || G8b < 0 || B8b > 255 || B8b < 0)
//        {
//                for (int i=0; i < 81; i++)
//                {
//                        spectrum.value[i] = -1;
//                }
//                return spectrum; // On stoppe l'algo en retournant un spectre erreur -1
//        }
//
//        // On calcule les valeurs R G B entre 0 et 1
//        float R = (float) R8b/255;
//        float G = (float) G8b/255;
//        float B = (float) B8b/255;
//
//
//        // On applique l'aglo
//        if (R <= G && R <= B)
//        {
//                if (G <= B) // R <= G <= B
//                {
//                        for (int i=0; i < 81; i++)
//                        {
//                                spectrum.value[i] = R * SpWhite.value[i];
//                                spectrum.value[i] += (G-R) * SpCyan.value[i];
//                                spectrum.value[i] += (B-G) * SpBlue.value[i];
//                        }
//                }
//                else // R <= B <= G
//                {
//                        for (int i=0; i < 81; i++)
//                        {
//                                spectrum.value[i] = R * SpWhite.value[i];
//                                spectrum.value[i] += (B-R) * SpCyan.value[i];
//                                spectrum.value[i] += (G-B) * SpGreen.value[i];
//                        }
//                }
//        }
//
//
//
//        else if (G <= R && G <= B)
//        {
//                if (R <= B) // G <= R <= B
//                {
//                        for (int i=0; i < 81; i++)
//                        {
//                                spectrum.value[i] = G * SpWhite.value[i];
//                                spectrum.value[i] += (R-G) * SpMagenta.value[i];
//                                spectrum.value[i] += (B-R) * SpBlue.value[i];
//                        }
//                }
//                else // G <= B <= R
//                {
//                        for (int i=0; i < 81; i++)
//                        {
//                                spectrum.value[i] = G * SpWhite.value[i];
//                                spectrum.value[i] += (B-G) * SpMagenta.value[i];
//                                spectrum.value[i] += (R-B) * SpRed.value[i];
//                        }
//                }
//        }
//
//
//
//        else
//        {
//                if (R <= G) // B <= R <= G
//                {
//                        for (int i=0; i < 81; i++)
//                        {
//                                spectrum.value[i] = B * SpWhite.value[i];
//                                spectrum.value[i] += (R-B) * SpYellow.value[i];
//                                spectrum.value[i] += (G-R) * SpGreen.value[i];
//                        }
//                }
//                else // B <= G <= R
//                {
//                        for (int i=0; i < 81; i++)
//                        {
//                                spectrum.value[i] = B * SpWhite.value[i];
//                                spectrum.value[i] += (G-B) * SpYellow.value[i];
//                                spectrum.value[i] += (R-G) * SpRed.value[i];
//                        }
//                }
//        }
//
//
//        // On vérifie qu'il n'y a pas eu d'erreurs
////        float RfromSp = 0.0, GfromSp = 0.0, BfromSp = 0.0;
////        for (int i=0; i < 81; i++)
////                {
////                        RfromSp += spectrum.value[i] * Ar.value[i];
////                        GfromSp += spectrum.value[i] * Ag.value[i];
////                        BfromSp += spectrum.value[i] * Ab.value[i];
////                }
////        if ((int) (RfromSp*255) != R8b || (int) (GfromSp*255) != G8b || (int) (BfromSp*255) != B8b)
////        {
////                for (int i=0; i < 81; i++)
////                {
////                        spectrum.value[i] = -1;
////                }
////                return spectrum;
////        }
//
//        return spectrum;
//}

