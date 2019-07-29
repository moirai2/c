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
struct RegionVector {
	struct RegionVectorObject* current;
	struct RegionVectorObject* last;
	struct RegionVectorObject* objects;
};

// actual object to store vector content
struct RegionVectorObject {
	long int start;
	long int end;
	struct RegionVectorObject* next;
};

/******************** STRING VECTOR ********************/

// Constructor for string vector class
// Used to store multiple strings
// string = string to store
struct RegionVector* newRegionVector();

// Constructor for string vector object class
// Used to store multiple strings
// string = string to store
struct RegionVectorObject* newRegionVectorObject( long int start, long int end );

// return vector size
// vector = vector of interest
long int getRegionVectorSize( struct RegionVector* vector );

// add new string to a vector
// vector = vector of interest
// sring = string to add
void addRegionVector( struct RegionVector* vector, long int start, long int end );

// return string at specified index
// vector = vector of interest
// index of string to get
char* getRegionVector( struct RegionVector* vector, long int index );

// Print out content of a string vector to a filehandler,
void printRegionVector( struct RegionVector* vector, FILE* handler );

// return 1 if it has more strings
// return 0 if it's empty
int hasMoreRegionVector( struct RegionVector* vector );

// get next string vector
char* nextRegionVector( struct RegionVector* vector );

// get next string vector
char* peekRegionVector( struct RegionVector* vector );

// sort string vector using bubble sort
// by the way, it's very slow
void sortRegionVector( struct RegionVector* vector );

// quick sort called from sortRegionVector
struct RegionVectorObject* quickSort( struct RegionVectorObject* object );

// Reset index of string vector to the beginning
void resetRegionVectorIndex( struct RegionVector* vector );

// delete string vector to release memory
// deconstructor
// vector = vector of interest
void freeRegionVector( struct RegionVector* vector );

#endif
