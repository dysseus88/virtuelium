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
#ifndef GUARD_VRT_TEXTURE_HPP
#define GUARD_VRT_TEXTURE_HPP

#include <string>

#include <common.hpp>

#include <io/image/ImageParser.hpp>

#include <structures/Image.hpp>
#include <structures/Spectrum.hpp>
//!
//! @file Texture.hpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @remarks
//! @details Stores all data about textures and RGB spectralizations.\n\n
//!  The spectralization process is made based on a sample diagram. From a 
//!  sample file, a color diagram of known n rgb-to-spectrum conversion samples
//!  n built (n=7 for best tested results). Then, triangular areas are made from 
//!  the sample points in order to allow the definition of every rgb points as
//!  a barycenter of known points. Here we make the hypothesis the  
//!  spectralization process is linear despites the colorimetric concept of 
//!  black metamerism.
//!
////////////////////////////////////////////////////////////////////////////////
//! @see ReferenceSample
class ReferenceSample;
//! Error value
static const int kERROR = -1;
////////////////////////////////////////////////////////////////////////////////
//! @class SampleColor
//! @brief Color used in the sample diagram
class SampleColor {
 public:
	//! @brief Constructor 1
  SampleColor(void);
  //! @brief Constructor 2
  SampleColor(double R=0.0, double G=0.0, double B=0.0);
  //!@brief Destructor
  ~SampleColor(void);
	
 public:
	//! @brief Set the rgb value of the color sample
  void SetRGB(double R=0.0, double G=0.0, double B=0.0);
  //! @brief clear the spectrum value of the color sample
	void ClearSpectrum(void);
  //! @brief Set the spectrum value of the color sample
	void SetSpectrum(const Spectrum& spectrum);
	//! @brief Set the color sample to NULL
  void SetNull(void);
	//! @brief Set the color sample to an error value
  void SetError(void);
	
 public:
  //! Get R value
  double GetR(void);
  //! Get G value
  double GetG(void);
  //! Get sum of the RGB color
  double GetSum(void);
  //! Get the spectrum
	Spectrum GetSpectrum(void);
  
 public:
  //! @brief Get the distance betwwen the current color sample and another one
  double GetDistanceFrom(const SampleColor& otherPoint);
	
 private:
  //! RGB value of the color sample
  double r, g, sum;
  //! Spectrum value of the color sample
  Spectrum m_spectrum;
}; // SampleColor
////////////////////////////////////////////////////////////////////////////////
//! @class SampleSegment
//! @brief Segment betwwen to rgb-to-spectrum conversion samples
class SampleSegment {
 public:
  //!@brief Constructor
  SampleSegment(int firstPoint, int secondPoint);
  //! @brief Destructor
  ~SampleSegment(void);

 public:
  //! @brief Tests if a point beelong to the segment
  bool Contains(int point);
  //! @brief Get the first point of the segment
  int GetPoint1(void);
  //! @brief Get the second point of the segment
  int GetPoint2(void);
  //! @brief Use the current segment to define an sample area and border
  void SetArea1(int area, int border);
  //! @brief Get the area the segment is linked to
  int GetArea1(void);
  //! @brief Get the border the segment is linked to 
  int GetBorderArea1(void);

 private:
  //! Index of the first point of the segment
  int i_point1;
  //! Index of the second point of the segment
  int i_point2;
  //! Index of the area the segment is linked to
  int i_area1;
  //! Index of the border the segment is linked to
  int i_borderArea1;
}; // class SampleSegment
////////////////////////////////////////////////////////////////////////////////
//! @class SampleArea
//! @brief Area betwwen three rgb-to-spectrum conversion samples
class SampleArea {
 public:
  //! @brief Constructor 1
  SampleArea(int firstPoint, int secondPoint, int thirdPoint, 
             ReferenceSample* parentIN = NULL);
  //! @brief Constructor
  SampleArea(SampleSegment& firstSegment, int thirdPoint, 
             ReferenceSample* parentIN = NULL);
   //! @brief Desctructor
  ~SampleArea(void);
  //! @brief Test if an area contains a sample segment
  bool Contains(SampleSegment& seg);
  //! @brief Tells which point of the area is the third one for a sample segment
  int ThirdPoint(SampleSegment& seg);
  //! @brief Get the first point of the area
  int GetPoint1(void);
  //! @brief Get the second point of the area
  int GetPoint2(void);
  //! @brief Get the third point of the area
  int GetPoint3(void);
  //! @brief Set the indices of the neightbor areas and borders
  //! @details An area has a maximum of 3 neighbor areas
  void SetNeighbor(int neighbor, int border);
  //! @brief Get the first neighor area
  int GetNeighbor1(void);
  //! @brief Get the second neighor area
  int GetNeighbor2(void);
  //! @brief Get the third neighor area
  int GetNeighbor3(void);
  //! @brief Return the right distance to the border
  double GetSquareLength(int border);

 public:
  //! @brief Tests if a point is include inside an area
  bool IsIncluded(double x, double y);
  //! @brief Tests if a point is inside the border 1 an area
  bool IsInsideBorder1(double x, double y);
  //! @brief Tests if a point is inside  the border 2 an area
  bool IsInsideBorder2(double x, double y);
  //! @brief Tests if a point is inside  the border 3 an area
  bool IsInsideBorder3(double x, double y);
  //! @brief Compute the samples coeeficients
  std::vector<double> calculateCoefficients(double x, double y);
  //! @brief Compute the transformation matrix;
  void computeMatrixForLocation();

 private:
  //! Link to the Reference sample class
  ReferenceSample* p_parent;
  //! Index of the points of the area
  int i_point1, i_point2, i_point3;
  //! Index of the neightbor to the area
  int i_neighbor1, i_neighbor2, i_neighbor3;
  //! Location matrix
  double alpha[3];
  //! Location matrix
  double beta[3];
  //! Location matrix
  double gamma[3];
  //! Equations of borders
  double a1, b1, c1, f3;
  //! Equations of borders
  double a2, b2, c2, f2;
  //! Equations of borders
  double a3, b3, c3, f1;
  //! To calculate closest point in the area
  double squareLength1;
  //! To calculate closest point in the area
  double squareLength2;
  //! To calculate closest point in the area
  double squareLength3;
}; // class SampleArea
////////////////////////////////////////////////////////////////////////////////
//! @class ReferenceSample
//! @brief This class stores the n conversion samples in a diagram
class ReferenceSample {
 public:
	//! @brief Constructor
  ReferenceSample(const std::vector<std::string> list_sample_file);
	//! @brief Destructor
  ~ReferenceSample(void);
  //! @brief Copy constructor
  ReferenceSample(const ReferenceSample& r);
  //! @brief Assignement operator
	ReferenceSample& operator=(const ReferenceSample& r);

 public:
	//! @brief Build the area from the sample file
  bool BuildAreas(void);
  //! @brief Tests if two points are on the same side of a sample segment
	bool SameSide(SampleSegment& seg, int point1, int point2);
	//! @brief Tests if a segment defines by two points has already been created
  bool TheSegmentAlreadyExists(int point1, int point2);
	//! @details Returns true if the segments formed by the points of the segment 
  //!  and the point "point" cross another segment
  bool CrossingSegments(SampleSegment& seg, int point);
	//! @brief Get the index of the third point of an sample area
  int IndexOfTheThirdPoint(SampleSegment& seg);
	//! @brief Find the segment defined by two points
  int FindSegment(int point1, int point2);
	//! @brief Find the area define by three points 
  double GetAreaOfTriangle(int point1, int point2, int point3);
	//! @brief Get the sample color of a given point 
  SampleColor& GetColor(int point);
	
 public:
  // @brief Find the area in which the point is
	int FindArea(double x, double y);
	
 public:
  //! @brief Do the spectralization
	Spectrum RGBtoSpectrum(double R, double G, double B);

 private:
  //! List of the sample points of the diagram
	std::vector<SampleColor> m_list_points;
  //! List of the sample segments of the diagram
	std::vector<SampleSegment> m_list_segments;
  //! List of the sample areas of the diagram
	std::vector<SampleArea> m_list_areas;
	//! Tells wat is the current areas we look at
	int currentZone;    
}; // class ReferenceSample
////////////////////////////////////////////////////////////////////////////////
//! @class Texture
//! @brief This class defines a data structure for all loaded textures
//! @details In the XML scene file, a texture must be declared bedore the 
//!  <renderer> tag using the bellow syntax. The same instance of a declared 
//!  texture can be used multiple times.
//!  @arg <texture name="..." file="... />
//! 
class Texture {
 public:
	//! @enum Alpha mode for Texture
  typedef enum {
		ALPHA_OFF = 0,
		ALPHA_ON  = 1
	} ALPHA_MODE;
  //! @enum Repeat mode for texture
  //! @details Describes the behaviors of the mapping when (u,v) are not in
  //!  [0, 1]
	typedef enum {
		REPEAT_OFF = 0,
		REPEAT_ON  = 1,
		REPEAT_MIRROR = 2
	} TEXTURE_REPEAT_MODE;

 public:
  //! @brief Constructor
	Texture(void);
  //! @brief Virtual destructor
	virtual ~Texture(void);
  //! @vbrief Copy Constructor
	Texture(const Texture& t);
  //! @brief Assignment operator
	Texture& operator=(const Texture& t);
  
 public:
  //! @brief Load an image file and link it to the class
	void SetImage(const char* image_file);
  //! @brief Get the image pointer
	Image* GetImage(void);
  //! @brief Clear the image
	void ClearImage(void);
  //! @brief Set the texture name
	void SetTextureName(const char* texture_name);
  //! brief Get the texture name
	const char* GetTextureName(void) const;
  
 public: 
  //! @brief Get the RGB value of a given pixel
	void GetPixelValue(const Real& x, const Real& y, Pixel& rgb);
  //! @brief Get the alpha value of a given pixel
	void GetAlphaValue(const Real& x, const Real& y, Real& alpha);
  //! @brief Get the spectralized value of a given pixel
	void GetSpectrumValue(const Real& x, const Real& y, 
                        Spectrum& spectralized_rgb);
	//! @brief Spectralize  triplet (R, G, B)
	void SpectralizeRGB(Real R, Real G, Real B, 
                      Spectrum& spectralized_rgb);

 public :
	//! Image file linked to the texture
	Image* p_image;
	//! Name of the texture
	std::string	m_name;
  //! Reference rgb-to-spectrum conversion samples for spectralization
  ReferenceSample* p_reference_sample;
};	// class Texture
////////////////////////////////////////////////////////////////////////////////
#endif // GUARD_VRT_TEXTURE_HPP
