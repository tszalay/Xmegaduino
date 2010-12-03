/*
 WString.cpp - String library for Wiring & Arduino
 (mostly rewritten for improved performance, by Paul Stoffregen)
 Copyright (c) 2009-10 Hernando Barragan.  All rights reserved.
 Copyright 2010, Paul Stoffregen

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <stdlib.h>
#include "WProgram.h"
#include "WString.h"


String::String( const char *value )
{
	if ( value == NULL ) {
		_buffer = NULL;
		_length = _capacity = 0;
	} else {
		getBuffer( _length = strlen( value ) );
		if (_buffer) strcpy( _buffer, value );
	}
}

String::String( const String &value )
{
	_length = value._length;
	if (_length == 0) {
		_buffer = NULL;
		_capacity = 0;
	} else {
		getBuffer(_length);
		if (_buffer) strcpy( _buffer, value._buffer );
	}
}

String::String( const char value )
{
 _length = 1;
 getBuffer(1);
 if (_buffer) {
   _buffer[0] = value;
   _buffer[1] = 0;
 }
}

String::String( const unsigned char value )
{
 _length = 1;
 getBuffer(1);
 if (_buffer) {
   _buffer[0] = value;
   _buffer[1] = 0;
 }
}

String::String( const int value, const int base )
{
 char buf[33];   
 itoa((signed long)value, buf, base);
 getBuffer( _length = strlen(buf) );
 if (_buffer) strcpy( _buffer, buf );
}

String::String( const unsigned int value, const int base )
{
 char buf[33];   
 ultoa((unsigned long)value, buf, base);
 getBuffer( _length = strlen(buf) );
 if (_buffer) strcpy( _buffer, buf );
}

String::String( const long value, const int base )
{
 char buf[33];   
 ltoa(value, buf, base);
 getBuffer( _length = strlen(buf) );
 if (_buffer) strcpy( _buffer, buf );
}

String::String( const unsigned long value, const int base )
{
 char buf[33];   
 ultoa(value, buf, 10);
 getBuffer( _length = strlen(buf) );
 if (_buffer) strcpy( _buffer, buf );
}

char String::charAt( unsigned int loc ) const
{
 return operator[]( loc );
}

void String::setCharAt( unsigned int loc, const char aChar ) 
{
 if(_length > loc) {
   _buffer[loc] = aChar;
 }
}

int String::compareTo( const String &s2 ) const
{
	if (!_buffer || !s2._buffer) {
		if (s2._buffer) return 0 - *(unsigned char *)s2._buffer;
		if (_buffer) return *(unsigned char *)_buffer;
		return 0;
	}
	return strcmp( _buffer, s2._buffer );
}

const String & String::concat( const String &s2 )
{
 return (*this) += s2;
}

const String & String::operator=( const String &rhs )
{
	if (this == &rhs) return *this;
	if (rhs._length == 0) {
		if (_buffer) free(_buffer);
		_buffer = NULL;
		_length = _capacity = 0;
		return *this;
	}
	if (rhs._length > _capacity) {
		char *newbuffer = (char *)realloc(_buffer, rhs._length + 1);
		if (newbuffer) {
			_buffer = newbuffer;
			_capacity = rhs._length;
		} else {
			if (_buffer) free(_buffer);
			_buffer = NULL;
			_length = _capacity = 0;
			return *this;
		}
	}
	_length = rhs._length;
	strcpy(_buffer, rhs._buffer);
	return *this;
/*
 if ( rhs._length > _capacity )
 {
   if (_buffer) free(_buffer);
   getBuffer( rhs._length );
 }
 if (_buffer) {
   _length = rhs._length;
   if (rhs._buffer) strcpy( _buffer, rhs._buffer );
 }
 return *this;
*/
}

//const String & String::operator+=( const char aChar )
//{
//  if ( _length == _capacity )
//    doubleBuffer();
//
//  _buffer[ _length++ ] = aChar;
//  _buffer[ _length ] = '\0';
//  return *this;
//}

const String & String::operator+=( const String &other )
{
	if (other._length == 0) return *this;
	_length += other._length;
	if ( _length > _capacity ) {
		char *newbuffer = (char *)realloc(_buffer, _length + 1);
		if (newbuffer) {
			_buffer = newbuffer;
			_capacity = _length;
		} else {
			// out of memory, avoid buffer overflow by leaving string unchanged
			_length -= other._length;
			return *this;
		}
	}
	if (_buffer) strcat( _buffer, other._buffer );
	return *this;
}

int String::operator==( const String &rhs ) const
{
 return (_length == rhs._length && compareTo(rhs) == 0);
}

int String::operator!=( const String &rhs ) const
{
 return (_length != rhs.length() || compareTo(rhs) != 0);
}

int String::operator<( const String &rhs ) const
{
 return compareTo(rhs) < 0;
}

int String::operator>( const String &rhs ) const
{
 return compareTo(rhs) > 0;
}

int String::operator<=( const String &rhs ) const
{
 return compareTo(rhs) <= 0;
}

int String::operator>=( const String & rhs ) const
{
 return compareTo(rhs) >= 0;
}


char & String::operator[](unsigned int index)
{
	static char dummy_writable_char;
	if (index >= _length || !_buffer) {
		dummy_writable_char = 0;
		return dummy_writable_char;
	}
	return _buffer[index];
}

char String::operator[]( unsigned int index ) const
{
	if (index >= _length || !_buffer) return 0;
	return _buffer[ index ];
}

void String::reserve(unsigned int size)
{
	if (size < _length) return;
	char *newbuffer = (char *)realloc(_buffer, size + 1);
	if (newbuffer) {
		_capacity = size;
		if (!_buffer) *newbuffer = 0;
		_buffer = newbuffer;
	}
}

boolean String::endsWith( const String &s2 ) const
{
	if ( _length < s2._length || !_buffer || !s2._buffer) return 0;
	return strcmp(&_buffer[_length - s2._length], s2._buffer) == 0;
}

boolean String::equals( const String &s2 ) const
{
	return ( _length == s2._length && compareTo(s2) == 0);
}

boolean String::equalsIgnoreCase( const String &s2 ) const
{
	if (this == &s2) return true;
	if (_length != s2._length) return false;
	if (_length == 0) return true;
	const char *p1 = _buffer;
	const char *p2 = s2._buffer;
	while (*p1) {
		if (tolower(*p1++) != tolower(*p2++)) return false;
	} 
	return true;
}

String String::replace( char findChar, char replaceChar )
{
	if (!_buffer) return *this;
	String theReturn = _buffer;
	char *temp = theReturn._buffer;
	if (temp) {
		while( (temp = strchr( temp, findChar )) != 0 )
			*temp = replaceChar;
	}
	return theReturn;
}

String String::replace( const String& match, const String& replace )
{
	if (_length == 0 || match._length == 0) return *this;
	int diff = replace._length - match._length;
	char *foundAt, *readFrom, *copyTo;
	readFrom = _buffer;
	unsigned int size = _length; // compute size needed for result
	if (diff != 0) {
		while ((foundAt = strstr(readFrom, match._buffer)) != NULL) {
			readFrom = foundAt + match._length;
			size += diff;
		}
	}
	String result;
	result.getBuffer(size); // single allocation, avoid fragmentation
	if (!result._buffer) return result;
	result._length = size;
	readFrom = _buffer;
	copyTo = result._buffer;
	while ((foundAt = strstr(readFrom, match._buffer)) != NULL) {
		unsigned int offset = foundAt - readFrom;
		if (offset) {
			strncpy(copyTo, readFrom, offset);
			readFrom += offset;
			copyTo += offset;
		}
		strcpy(copyTo, replace._buffer);
		readFrom += match._length;
		copyTo += replace._length;
	}
	if (*readFrom) strcpy(copyTo, readFrom);
	return result;
}

int String::indexOf( char temp ) const
{
 return indexOf(temp, 0 );
}

int String::indexOf( char ch, unsigned int fromIndex ) const
{
	if (fromIndex >= _length) return -1;
	const char* temp = strchr(_buffer + fromIndex, ch);
	if (temp == NULL) return -1;
	return temp - _buffer;
}

int String::indexOf( const String &s2 ) const
{
 return indexOf( s2, 0 );
}

int String::indexOf( const String &s2, unsigned int fromIndex ) const
{
	if (fromIndex >= _length) return -1;
	const char *theFind = strstr(_buffer + fromIndex, s2._buffer);
	if (theFind == NULL) return -1;
	return theFind - _buffer; // pointer subtraction
}

int String::lastIndexOf( char theChar ) const
{
 return lastIndexOf( theChar, _length - 1 );
}

int String::lastIndexOf( char ch, unsigned int fromIndex ) const
{
 if ( fromIndex >= _length )
   return -1;

 char tempchar = _buffer[fromIndex + 1];
 _buffer[fromIndex + 1] = '\0';
 char* temp = strrchr( _buffer, ch );
 _buffer[fromIndex + 1] = tempchar;

 if ( temp == NULL )
   return -1;

 return temp - _buffer;
}

int String::lastIndexOf( const String &s2 ) const
{
 return lastIndexOf( s2, _length - s2._length );
}

int String::lastIndexOf( const String &s2, unsigned int fromIndex ) const
{
 // check for empty strings
 if ( s2._length == 0 || s2._length - 1 > fromIndex || fromIndex >= _length )
   return -1;

 // matching first character
 char temp = s2[ 0 ];

// TODO: replace this with straightforward search that does not allocate memory!
 for ( int i = fromIndex; i >= 0; i-- )
 {
   if ( _buffer[ i ] == temp && (*this).substring( i, i + s2._length ).equals( s2 ) )
   return i;
 }
 return -1;
}

boolean String::startsWith( const String &s2 ) const
{
 if ( _length < s2._length )
   return 0;

 return startsWith( s2, 0 );
}

boolean String::startsWith( const String &s2, unsigned int offset ) const
{
  if ( offset > _length - s2._length )
    return 0;

  return strncmp( &_buffer[offset], s2._buffer, s2._length ) == 0;
}


String String::substring( unsigned int left ) const
{
	return substring(left, _length);
}

String String::substring(unsigned int left, unsigned int right) const
{
	if (left > right) {
		unsigned int temp = right;
		right = left;
		left = temp;
	}
	if (left > _length) return String(NULL);
	if (right > _length) right = _length;

	char temp = _buffer[right];  // save the replaced character
	_buffer[right] = '\0';	
	String outPut = _buffer + left;  // pointer arithmetic
	_buffer[right] = temp;  //restore character
	return outPut;
}

String String::toLowerCase() const
{
	String temp = _buffer;
	if (temp._buffer) {
		for (char *p = temp._buffer; *p; p++) {
			*p = tolower(*p);
		}
	}
	return temp;
}

String String::toUpperCase() const
{
	String temp = _buffer;
	if (temp._buffer) {
		for (char *p = temp._buffer; *p; p++) {
			*p = toupper(*p);
		}
	}
	return temp;
}

String String::trim() const
{
	unsigned int first, last;
	for (first = 0; first < _length; first++) {
		if (!isspace(_buffer[first])) break;
	}
	for (last = _length - 1; last > first; last--) {
		if (!isspace(_buffer[last])) break;
	}
	return substring(first, last + 1);
}

void String::getBytes(unsigned char *buf, unsigned int bufsize)
{
	if (!bufsize || !buf) return;
	unsigned int len = bufsize - 1;
	if (len > _length) len = _length;
	strncpy((char *)buf, _buffer, len);
	buf[len] = 0;
}

void String::toCharArray(char *buf, unsigned int bufsize)
{
	if (!bufsize || !buf) return;
	unsigned int len = bufsize - 1;
	if (len > _length) len = _length;
	strncpy(buf, _buffer, len);
	buf[len] = 0;
}

long String::toInt() {
  String temp = _buffer;
  long value = 0;
  
  for (unsigned int charPos = 0; charPos < _length; charPos++) {
    int thisChar = temp[charPos];
    if (isdigit(thisChar)) {
      value *= 10;
      value += (thisChar - '0');
    }
  }
  
  return value;
}