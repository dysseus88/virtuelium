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

#ifndef _HASHMAP_HPP
#define _HASHMAP_HPP

template <typename TKey, typename TValue>
class HashMapEntry;

/**
 * HashMap
 * This class is a container that is supposed to have insertion and search query
 * with a complexity of O(1).
 *
 * The TKey and TValue type need to have the following features :
 * - have a default constructor
 * - be affectable ('=' operator)
 *
 * The THashFunc class is a functor that need to have this prototype :
 * class MyHashFunctor{
 *   public : unsigned int operator()(const TKey&) const;
 * };
 * This functor return an index for the given key. This index must be the same for
 * the same key and should be different for differents keys. The more varied are
 * the hashed index, the more the HashMap will be effcicient.
 */
template <typename TKey, typename TValue, typename THashFunc>
class HashMap{
public :
  /**
   * Create the Hashmap. You will need to give it a haching function and a
   * capacity. The capacity should be like twice the number that will be stored
   * into the hash map to have good performances.
   * hashFunction : the haching function that will be used.
   * capacity : the number of element that can be put into the hashmap.
   */
  HashMap(const THashFunc& hachFunction, int capacity=300);
  
  /**
   * Destructor.
   */
  ~HashMap();
  
  /**
   * Add an element (Key, Value) into the HashMap. If the key is already in the 
   * HashMap it's value is replaced.
   * key : the key to insert into the HashMap.
   * value : the value to link to the key.
   */
  void add(const TKey& key, const TValue& value);
  
  /**
   * Return the value linked to the given key. Return default instance (as builded
   * with default constructor) if the key doesn't exist.
   * key : the key that we search.
   */
  TValue get(const TKey& key) const;
  
  /**
   * Return true if the key is stored in the HashMap, false otherwise.
   * key : the key that we search.
   */
  bool contain(const TKey& key) const;  
  
private :
  THashFunc _hashFunction;
  HashMapEntry<TKey, TValue>* _map;
  unsigned int _capacity;
  unsigned int _size;

  /**
   * Return the index corresponding to the key in the array of the map.
   * key : the key that we search for it's index.
   */
  unsigned int getMapIndex(const TKey& key) const;
};

template <typename TKey, typename TValue>
class HashMapEntry{
public :
  bool empty;
  TKey key;
  TValue value;
};

template <typename TKey, typename TValue, typename THashFunc> 
HashMap<TKey, TValue, THashFunc>::HashMap(const THashFunc& hashFunction, int capacity)
: _hashFunction(hashFunction), _capacity(capacity), _size(0)
{
  //Create and initialize the map array
  _map = new HashMapEntry<TKey, TValue>[capacity]; 
  for(int i=0; i<capacity; i++)
    _map[i].empty=true;
}
  
/**
 * Destructor.
 */
template <typename TKey, typename TValue, typename THashFunc>
HashMap<TKey, TValue, THashFunc>::~HashMap()
{
  delete[] _map;
}
  
/**
 * Add an element (Key, Value) into the HashMap. If the key is already in the 
 * HashMap it's value is replaced.
 * key : the key to insert into the HashMap.
 * value : the value to link to the key.
 */
template <typename TKey, typename TValue, typename THashFunc>
void HashMap<TKey, TValue, THashFunc>::add(const TKey& key, const TValue& value)
{
  //If the hashmap is full, resize it
  if(_size>_capacity*0.6)
  {
    unsigned int oldcapacity = _capacity;
    HashMapEntry<TKey, TValue>* oldmap = _map;
    
    //Create and initialize the new map array
    _size=0;
    _capacity *= 2;
    _map = new HashMapEntry<TKey, TValue>[_capacity]; 
    for(unsigned int i=0; i<_capacity; i++)
      _map[i].empty=true;

    //Fill the new map
    for(unsigned int i=0; i<oldcapacity; i++)
      if(oldmap[i].empty==false)
        add(oldmap[i].key, oldmap[i].value);

    //Destroy the old map
    delete[] oldmap;    
  } 

  //Get the key index
  unsigned int index = getMapIndex(key); 
  
  //If a new key is inserted, increase the number of elements
  if(_map[index].empty)
    _size++;
  
  //Insert the key & value
  _map[index].key=key;
  _map[index].value=value;
  _map[index].empty=false;
}
  
/**
 * Return the value linked to the given key. Return default instance (as builded
 * with default constructor) if the key doesn't exist.
 * key : the key that we search.
 */
template <typename TKey, typename TValue, typename THashFunc>
TValue HashMap<TKey, TValue, THashFunc>::get(const TKey& key) const
{
  unsigned int index = getMapIndex(key);
  if(_map[index].empty)
    return TValue();
  else
    return _map[index].value;
}
  
/**
 * Return true if the key is stored in the HashMap, false otherwise.
 * key : the key that we search.
 */
template <typename TKey, typename TValue, typename THashFunc>
bool HashMap<TKey, TValue, THashFunc>::contain(const TKey& key) const
{
  if(_map[getMapIndex(key)].empty)
    return false;
  else
    return true;
}
  
/**
 * Return the index corresponding to the key in the array of the map.
 * key : the key that we search for it's index.
 */
template <typename TKey, typename TValue, typename THashFunc>
unsigned int HashMap<TKey, TValue, THashFunc>::getMapIndex(const TKey& key) const
{
  //Get hashed index
  unsigned int index = _hashFunction(key)%_capacity;
  
  //Search for the first free entry (or an entry that have this key)
  unsigned int i=1;
  while(!_map[index].empty && !(_map[index].key==key))
    index = (index + i++)%_capacity;

  return index;
}

#endif //_HASHMAP_HPP

