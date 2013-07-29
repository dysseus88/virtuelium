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
 
#include "io/XMLParser.hpp"
#include "exceptions/Exception.hpp"
#include <sstream>
#include <fstream>
#include <ctype.h>
 
const int XMLParser::NORMAL_MARKUP  = 0;
const int XMLParser::CLOSING_MARKUP = 1;
const int XMLParser::COMMENT_MARKUP = 2;
const int XMLParser::PROCESS_MARKUP = 3;
const int XMLParser::ESCAPE_MARKUP  = 4;

/**
 * Parse the given XML stream and create an XML tree that contain the tree data.
 * input : the input stream to parse.
 */
XMLTree* XMLParser::parseAndCreateTree(std::istream& input)
{
  XMLTree* tree=NULL;
  if(getNextMarkup(input, tree)!=NORMAL_MARKUP)
    throw Exception("(XMLParser::parseAndCreateTree) Fichier XML vide ou mal formé.");
  return tree;
}

/**
 * Parse the given XML file and create an XMLTree that contain the tree data.
 * @param input : the input stream to parse.
 */
XMLTree* XMLParser::getXMLTree(const std::string& filename)
{
  // Open the file
  std::fstream xmlfile(filename.c_str(), std::fstream::in);
  if(!xmlfile.is_open())
    throw Exception("(XMLParser::getXMLTree) N'a pas pu ouvrir le fichier " + filename);
  
  // Build parser and parse
  XMLTree* xmltree = parseAndCreateTree(xmlfile);
  
  // Close and return the tree
  xmlfile.close();
  return xmltree;
}

/**
 * Parse the given XML stream and extract an XML subtree.
 * input : the input stream to parse.
 */
int XMLParser::getNextMarkup(std::istream& input, XMLTree*& tree)
{
  int type;
  //Advance until we reach a normal markup.
  do{
    jumpToNextMarkup(input);
    type = getMarkupType(input);
    
    switch(type)
    {
    case PROCESS_MARKUP :
      jumpToEndOfMarkup(input);
      break;
    case ESCAPE_MARKUP  :
      jumpToEndOfEscape(input);
      break;
    case COMMENT_MARKUP :
      jumpToEndOfComment(input);
      break;
    case CLOSING_MARKUP :
      return CLOSING_MARKUP;
    }
  }while(type != NORMAL_MARKUP);
  
  //Get markup name and create the node
  std::string label = readLabel(input);
  tree = new XMLTree(label);
  
  //Get and add attributes
  readWhiteSpace(input);
  readAttributes(input, tree);
  
  //Verify if this markup is "self closed"
  if(input.get()=='/')
    return NORMAL_MARKUP;
  
  //Get all childs markup
  type = NORMAL_MARKUP;
  while(type!=CLOSING_MARKUP)
  {
    //Get next markup
    XMLTree* subtree;
    type = getNextMarkup(input, subtree);
    
    //Add the child if it is a normal markup
    if(type==NORMAL_MARKUP)
      tree->addChild(subtree);
  }
  
  //Done !
  return NORMAL_MARKUP;
}

/**
 * Read and ignore all whitespace character.
 * input : the input stream to parse.
 */
void XMLParser::readWhiteSpace(std::istream& input)
{
  char tmp;
  while(!input.eof() && isspace(tmp=input.peek()))
    input.ignore(1);
}

/**
 * Read and return the next label.
 * input : the input stream to parse.
 */
std::string XMLParser::readLabel(std::istream& input)
{
  std::stringstream label;
  char tmp;
  
  //First character must be an alphabetic character
  if(input.eof()) throw Exception("(XMLParser::readLabel) Fin de fichier inattendue.");
  if(!isalpha(tmp=input.get())) throw Exception("(XMLParser::readLabel) Identifiant invalide pour un nom d'attribut ou de balise.");
  label << tmp;
  
  //Following characters must be alphanumeric characters
  while(!input.eof() && isalnum(tmp=input.get()))
    label << tmp;
  if(input.eof()) throw Exception("(XMLParser::readLabel) Fin de fichier inattendue.");
  input.putback(tmp);
  
  return label.str();
}

/**
 * Read and return the next label.
 * input : the input stream to parse.
 */
std::string XMLParser::readQuotedString(std::istream& input)
{
  std::stringstream text;
  char tmp;
  
  //First character must be a '"'
  if(input.eof()) throw Exception("(XMLParser::readQuotedString) Fin de fichier inattendue.");
  if((tmp=input.get())!='"') throw Exception("(XMLParser::readQuotedString) Valeur manquante pour un des attributs.");
  
  //Read until we reach a '"'
  while(!input.eof() && (tmp=input.get())!='"')
    text << tmp;
  if(input.eof()) throw Exception("(XMLParser::readQuotedString) Fin de fichier inattendue.");
  
  return text.str();
}

/**
 * Read, parse and add attribute of the current markup to the tree node.
 * input : the input stream to parse.
 * tree : the tree node where to add extracted attributes.
 */
void XMLParser::readAttributes(std::istream& input, XMLTree*& tree)
{
  char tmp;
  
  //Reading all attributes
  while(!input.eof() && isalpha(input.peek()))
  {
    //Getting the name of the attribute
    std::string name = readLabel(input);
    
    //Getting separator (' = ')
    readWhiteSpace(input);
    if((tmp=input.get()) != '=') throw Exception(((std::string)"(XMLParser::readAttributes) Valeur manquante pour l'attribut ") + name);
    readWhiteSpace(input);
    
    //Getting the value string 
    std::string value = readQuotedString(input);
    
    //Eating white space between the last attribute.
    readWhiteSpace(input);
    
    //Adding attribute to the node
    tree->addAttribute(name, value);
  }
}

/**
 * Return the type of the current markup.
 * input : the input stream to parse.
 */
int XMLParser::getMarkupType(std::istream& input)
{
  //Get the first character inside the markup
  char c = input.peek();
  
  switch(c)
  {
  case '?': return PROCESS_MARKUP;
  case '!': 
    input.ignore(1);
    c=input.peek();
    if(c!='-')
      return ESCAPE_MARKUP;
    else
      return COMMENT_MARKUP;
  case '/': return CLOSING_MARKUP;
  default : return NORMAL_MARKUP;
  }
}

/**
 * Jump to the next markup (it will eat the first '<' of the markup).
 * input : the input stream to parse.
 */
void XMLParser::jumpToNextMarkup(std::istream& input)
{
  char tmp=' ';
  while(tmp!='<' && !input.eof())
    input >> tmp;
}

/**
 * Jump to the end of the current comment.
 * input : the input stream to parse.
 */
void XMLParser::jumpToEndOfComment(std::istream& input)
{
  char tmp;
  
  int state=1;  
  while(state!=4 && !input.eof())
  {
    //Read the next character
    input >> tmp;
    
    switch (state)
    {
    case 1:
      switch(tmp){
      case '-' : state = 2; break ;
      default  : state = 1; break ;
      }
      break;
    case 2:
      switch(tmp){
      case '-' : state = 3; break ;
      default  : state = 1; break ;
      }
      break;
    case 3:
      switch(tmp){
      case '-' : state = 3; break ;
      case '>' : state = 4; break ;
      default  : state = 1; break ;
      }
      break;
    }
  }
}

/**
 * Jump to the end of the current markup.
 * input : the input stream to parse.
 */
void XMLParser::jumpToEndOfMarkup(std::istream& input)
{
  char tmp;
   
  int state=1;  
  while(state!=3 && !input.eof())
  {
    //Read the next character
    input >> tmp;
    
    switch (state)
    {
    case 1:
      switch(tmp){
      case '"' : state = 2; break ;
      case '>' : state = 3; break ;      
      default  : state = 1; break ;
      }
      break;
    case 2:
      switch(tmp){
      case '"' : state = 1; break ;
      default  : state = 2; break ;
      }
      break;
    }
  }
}

/**
 * Jump to the end of the current escape markup.
 * input : the input stream to parse.
 */
void XMLParser::jumpToEndOfEscape(std::istream& input)
{
  char tmp;
   
  int state = 1;  
  int stack = 0;
  while((state!=3 || stack != 0) && !input.eof())
  {
    //Read the next character
    input >> tmp;
    
    switch (state)
    {
    case 1:
      switch(tmp){
      case '"' : state = 2; break ;
      case '[' : state = 1; stack++; break ;
      case ']' : state = 1; stack--; break ;
      case '>' : if(stack==0) state = 3; break ;      
      default  : state = 1; break ;
      }
      break;
    case 2:
      switch(tmp){
      case '"' : state = 1; break ;
      default  : state = 2; break ;
      }
      break;
    }
  }
}














