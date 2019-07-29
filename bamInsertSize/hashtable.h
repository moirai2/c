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

#ifndef HASHTABLE
#define HASHTABLE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>
#include "vector.h"

/******************** STRUCT ********************/

// Hashtable of strings
struct StringHashtable {
	struct StringHashtableObject** array; // to store hashtable objects
	int* randomizer; // to have better hashtable key
	int size; // hashtable size
};

// Object to store key => vector
struct StringHashtableObject {
	char* key; // key => value
	int size; // to count numbers
	struct StringVector* vector; // to store values
	struct StringHashtableObject* next; // link to different key
};

/******************** STRING HASHTABLE ********************/

// Create new string hashtable
struct StringHashtable* newStringHashtable( int size );

// Delete string hashtable value
void freeStringHashtableObject( struct StringHashtableObject* value );

// Return string key
int getStringKey( struct StringHashtable* hashtable, char* key );

// add key and value information to string hashtable
// stores count only
void countStringHashtable( struct StringHashtable* hashtable, char* key, int value );

// add key and value information to string hashtable
// This is used to store multiple value information
void addStringHashtable( struct StringHashtable* hashtable, char* key, char* value );

// put key and value information to string hashtable
// It stores only the latest value information
void putStringHashtable( struct StringHashtable* hashtable, char* key, char* value );

// Print hashtable count
void printStringHashtableCount( struct StringHashtable* hashtable, FILE* handler );

// print out content of this string hashtable
void printStringHashtable( struct StringHashtable* hashtable, FILE* handler );

// Delete string hashtable
void freeStringHashtable( struct StringHashtable* hashtable );

// Create new string hashtable object from a string
struct StringHashtableObject* newStringHashtableObject( char* key, char* value, int size );

// get value string from key
char* getStringHashtableString( struct StringHashtable* hashtable, char* key );

// get value count from string key
int getStringHashtableCount( struct StringHashtable* hashtable, char* key );

// get size of hashtable keys
int getStringHashtableKeySize( struct StringHashtable* hashtable );

// get value from key
struct StringHashtableObject* getStringHashtableObject( struct StringHashtable* hashtable, char* key );

// print out content of this string hashtable object
void printStringHashtableObject( struct StringHashtableObject* object, FILE* handler );

// Get keys of hashtable
struct StringVector* getStringHashtableKeys( struct StringHashtable* hashtable );

// Sort string vector of keys by sequence count large to small
void sortStringVectorByCount( struct StringHashtable* hashtable, struct StringVector* vector );

// sort by count
struct StringVectorObject* quickSortByCount( struct StringHashtable* hashtable, struct StringVectorObject* pivot );

#endif
