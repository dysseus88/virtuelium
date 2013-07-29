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

#ifndef _EXCEPTION_HPP
#define _EXCEPTION_HPP

#include <string>
#include <ostream>
#include <errno.h>

/**
 * Exception class used for handeling exceptions. This is a simple exception that
 * only carry a message string. The << operator of the ostream classe had been 
 * overloaded for using this class. 
 */
class Exception{
public :
  /**
   * Create the exception.
   * @param message : the message that explain the exception.
   */
  inline Exception(std::string message);
  
  /**
   * Print the message of the exception into the given stream.
   * @param output : the stream where to print the message.
   */
  inline void printMessage(std::ostream& output) const;
  
  /**
   * Return the message of the exception.
   * @return the message of the exception.
   */
  inline std::string getMessage() const;
private :
  std::string _message;
};

/**
 * Create the exception.
 * message : the message that explain the exception.
 */
inline Exception::Exception(std::string message)
{
  _message=message;
}

/**
 * Print the message of the exception into the given stream.
 * output : the stream where to print the message.
 */
inline void Exception::printMessage(std::ostream& output) const
{
  output << _message;
}

/**
 * Return the message of the exception.
 */
inline std::string Exception::getMessage() const
{
  return _message;
}

/**
 * The operator << of the ostream class overloaded to help printing
 * easily the exceptions.
 */
inline std::ostream& operator<<(std::ostream& output, const Exception& e)
{
  e.printMessage(output);
  return output;
}

#endif //_EXCEPTION_HPP
