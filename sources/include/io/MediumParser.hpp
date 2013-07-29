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
#ifndef _MEDIUMPARSER_HPP
#define _MEDIUMPARSER_HPP

#include <string>
#include <vector>
#include <structures/Medium.hpp>

class MediumParser{
public :
  /**
   * Load a medium from the given file.
   * @param filename : the filename of the of the file that contain the medium
   *   info.
   * @return : the loaded medium.
   */
  Medium* loadMedium(const std::string& filename);

  /**
   * Load a medium from a list of multiples files.
   * @param filenames : a vector that contain the filenames of the files that
   *   describes the medium.
   * @return : the loaded medium.
   */
  Medium* loadMedium(const std::vector<std::string>& filenames);
};

#endif //_MEDIUMPARSER_HPP
