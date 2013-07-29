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
#ifndef GUARD_VRT_VRTLOG_HPP
#define GUARD_VRT_VRTLOG_HPP
//!
//! @file VrtLog.hpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file is a singleton which manages log files. Each thread has 
//!  its own log file
//! @remarks This version of virtuelium is MPI-only. please refer to MPI docs!
//!
#include <iostream>
#include <fstream>
#include <ctime>
#include <stdarg.h>

#include <structures/Spectrum.hpp>
////////////////////////////////////////////////////////////////////////////////
class VrtLog{
 public:
  //! @brief Create the log file
  //! @param rank MPI rank
  //! @param basenamme Root name for the log  file. The complete name will be 
  //!  <rank>_<basename>
  inline static void Create(int rank, const char* basename);
  //! @brief Write in the log file
  //! @param logline,... Same role than arguments of printf(char*, ...)
  inline static void Write(const char* logline, ...);
  //! @brief Write matrix in the log file
  //! @param varname Name of the variable
  //! @param var Array of values
  //! @param size Size of the array
  template <class T>
  inline static void WriteArray(const char* varname, T* var, int size);
  //! @brief Close the log file
  inline static void Close(void);
  //! @brief Compute date and time
  inline static void GetDateAndTime(char* result);

 private:
  //! File handler 
  static std::ofstream m_stream;
  //! Flag to say if the instance is ready to be used
  static bool b_ready;
}; // clas VrtLog
////////////////////////////////////////////////////////////////////////////////
inline void VrtLog::Create(int rank, const char* basename) {
  if (b_ready || !basename)
    return;

  // create
  char filename[260];
  sprintf(filename, "%d_%s", rank, basename); 
  m_stream.open(filename);

  //print date and time
  time_t now = time(0);
  struct tm tstruct;
  tstruct = *localtime(&now);
  char buf[80];
  strftime(buf, sizeof(buf), "\n ---------- %Y-%m-%d ---- %X ----------\n", 
            &tstruct);  
  m_stream << buf  << std::endl;

  b_ready = true;
}
////////////////////////////////////////////////////////////////////////////////
inline void VrtLog::Write(const char* logline, ...) {
 if (!b_ready)
   return;

  // write texte
  va_list argList;
  char cbuffer[1024];
  va_start(argList, logline);
  vsnprintf(cbuffer, 1024, logline, argList);
  va_end(argList);

  m_stream << cbuffer << std::endl;
}
////////////////////////////////////////////////////////////////////////////////
 template <class T>
 inline void VrtLog::WriteArray(const char* varname, T* var, int size) {
 if (!b_ready)
   return;

  // write texte
  m_stream << varname << ": " << std::endl;
  for (int i = 0; i < size; i++) {
     m_stream << var[i] << " ";
  }
  m_stream << std::endl;
}
////////////////////////////////////////////////////////////////////////////////
inline void VrtLog::Close(void){
  if (b_ready) {
    m_stream.close();
    b_ready = false;
  }
}
////////////////////////////////////////////////////////////////////////////////
inline void VrtLog::GetDateAndTime(char* buf) {
  time_t now = time(0);
  struct tm tstruct;
  tstruct = *localtime(&now);

  strftime(buf, sizeof(buf), "\n ---------- %Y-%m-%d ---- %X ----------\n", 
           &tstruct);  
}
////////////////////////////////////////////////////////////////////////////////
#endif // GUARD_VRT_VRTLOG_HPP