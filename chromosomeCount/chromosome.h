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

#ifndef CHROMOSOME
#define CHROMOSOME
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

// Hashtable of chromosomes
struct Chromosome {
	struct ChromosomeObject** array; // to store chromosomes objects
	int* randomizer; // to have better chromosomes key
	int size; // chromosomes size
};

// Object to store key => vector
struct ChromosomeObject {
	char* key; // key => value
	int positive_strand; // to count forward direction
	int negative_strand; // to count reverse direction
	int primary_mapping; // count numbers of primary mappings
	int secondary_mapping; // count numbers of secondary mappings
	int length; // length of this chromosome
	struct ChromosomeObject* next; // link to different key
};

/******************** CONSTRUCTOR ********************/

// Create new string chromosomes
struct Chromosome* newChromosome( int size );

// Delete string chromosomes
void freeChromosome( struct Chromosome* chromosomes );

// Create new string chromosomes object from a string
struct ChromosomeObject* newChromosomeObject( char* key );

// Delete string chromosomes value
void freeChromosomeObject( struct ChromosomeObject* value );

/******************** CHROMOSOME ********************/

// Return string key
int getStringKey( struct Chromosome* chromosomes, char* key );

// Get keys of chromosomes
struct StringVector* getChromosomeKeys( struct Chromosome* chromosomes );

// put new chromosome to chromosomes
void putChromosome( struct Chromosome* chromosomes, char* key );

// get value from key
struct ChromosomeObject* getChromosomeObject( struct Chromosome* chromosomes, char* key );

#endif
