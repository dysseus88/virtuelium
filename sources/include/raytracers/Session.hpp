/*
*  Copyright 2012 Sylvain "Greewi" Dumazet, Remi Cerise
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

#ifndef _SESSION_HPP
#define _SESSION_HPP

#include <sstream>

class Session{
public :
  inline void saveBand(const Image& band, int line);
  
};

inline void Session::saveBand(const Image& band, int line)
{
  std::stringstream filename(std::stringstream::in | std::stringstream::out);
  filename << "session/" << line << ".mhdri";
  MHDRImageParser parser;
  parser.save(band, filename.str());
}


#endif //_SESSION_HPP
