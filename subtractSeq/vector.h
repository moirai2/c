/*
 Copyright (C) 2013 Akira Hasegawa <ah3q@gsc.riken.jp>
 
 This file is part of MOIRAI.
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef VECTOR
#define VECTOR
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>
#include "io.h"

/******************** STRUCT ********************/

// structure of vector
struct StringVector {
	struct StringVectorObject* current;
	struct StringVectorObject* last;
	struct StringVectorObject* objects;
};

// actual object to store vector content
struct StringVectorObject {
	char* string;
	struct StringVectorObject* next;
};

/******************** STRING VECTOR ********************/

// Constructor for string vector class
// Used to store multiple strings
// string = string to store
struct StringVector* newStringVector();

// Constructor for string vector object class
// Used to store multiple strings
// string = string to store
struct StringVectorObject* newStringVectorObject( char* string );

// return vector size
// vector = vector of interest
long int getStringVectorSize( struct StringVector* vector );

// add new string to a vector
// vector = vector of interest
// sring = string to add
void addStringVector( struct StringVector* vector, char* string );

// return string at specified index
// vector = vector of interest
// index of string to get
char* getStringVector( struct StringVector* vector, long int index );

// Print out content of a string vector to a filehandler,
void printStringVector( struct StringVector* vector, FILE* handler );

// return 1 if it has more strings
// return 0 if it's empty
int hasMoreStringVector( struct StringVector* vector );

// get next string vector
char* nextStringVector( struct StringVector* vector );

// get next string vector
char* peekStringVector( struct StringVector* vector );

// sort string vector using bubble sort
// by the way, it's very slow
void sortStringVector( struct StringVector* vector );

// quick sort called from sortStringVector
struct StringVectorObject* quickSort( struct StringVectorObject* object );

// Reset index of string vector to the beginning
void resetStringVectorIndex( struct StringVector* vector );

// delete string vector to release memory
// deconstructor
// vector = vector of interest
void freeStringVector( struct StringVector* vector );

// Constructor for string vector class from string an delim
// string = string to split
// delim = delim used to split
struct StringVector* split( char* string, char delim );

// load string vector
struct StringVector* loadStringVector( struct StringVector* vector, char* path );

#endif
