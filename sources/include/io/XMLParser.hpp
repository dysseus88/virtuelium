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
#ifndef _XMLPARSER_HPP
#define _XMLPARSER_HPP

#include <istream>
#include <string>
#include "io/XMLTree.hpp"

/**
 * This class is designed to parse XML files with some restrictions. It will only
 * parse markups and markup attributes. All other (doctypes, text...) will be
 * ignored ! It produce a XMLTree that will contain the parsed informations.
 *
 * @author Sylvain "Greewi" Dumazet
 */
class XMLParser{
public :
  /**
   * Parse the given XML stream and create an XMLTree that contain the tree data.
   * @param input : the input stream to parse.
   */
  XMLTree* parseAndCreateTree(std::istream& input);

  /**
   * Parse the given XML file and create an XMLTree that contain the tree data.
   * @param input : the input stream to parse.
   */
  XMLTree* getXMLTree(const std::string& filename);
  
private :
  //Markup types
  static const int NORMAL_MARKUP;
  static const int CLOSING_MARKUP;
  static const int COMMENT_MARKUP;
  static const int PROCESS_MARKUP;
  static const int ESCAPE_MARKUP;
  
  /**
   * Parse the given XML stream and extract an XML subtree.
   * @param input : the input stream to parse.
   * @param tree : the subtree will be put here.
   */
  int getNextMarkup(std::istream& input, XMLTree*& tree);

  /**
   * Read and ignore all whitespace character.
   * @param input : the input stream to parse.
   */
  void readWhiteSpace(std::istream& input);

  /**
   * Read and return the next label.
   * @param input : the input stream to parse.
   */
  std::string readLabel(std::istream& input);

  /**
   * Read and return the next label.
   * @param input : the input stream to parse.
   */
  std::string readQuotedString(std::istream& input);

  /**
   * Read, parse and add attribute of the current markup to the tree node.
   * @param input : the input stream to parse.
   * @param tree : the tree node where to add extracted attributes.
   */
  void readAttributes(std::istream& input, XMLTree*& tree);

  /**
   * Return the type of the current markup.
   * @param input : the input stream to parse.
   */
  int getMarkupType(std::istream& input);

  /**
   * Jump to the next markup (it will eat the first '<' of the markup).
   * @param input : the input stream to parse.
   */
  void jumpToNextMarkup(std::istream& input);

  /**
   * Jump to the end of the current comment.
   * @param input : the input stream to parse.
   */
  void jumpToEndOfComment(std::istream& input);

  /**
   * Jump to the end of the current markup.
   * @param input : the input stream to parse.
   */
  void jumpToEndOfMarkup(std::istream& input);
  
  /**
   * Jump to the end of the current escape markup.
   * @param input : the input stream to parse.
   */
  void jumpToEndOfEscape(std::istream& input);  
    
};

#endif //_XMLPARSER_HPP
