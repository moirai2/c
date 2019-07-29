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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

/******************** DEFINE ********************/

// maximum characters per line
#define MAX_LINE 50000
#define MAXIMUM_SEQUENCE_COUNT 10000000

/******************** STRUCT ********************/

// Structure to store unique sequences and counts
struct UniqSeq {
	struct UniqSeq* next;
	char* sequence;
	int count;
};

/******************** UNIQ SEQUENCE ********************/

// sequence statistics
struct UniqSeq* newUniqSeq( char* sequence, int count ); 

// free unique sequences
void freeUniqSeq( struct UniqSeq* sequences );

// search for unique sequence
int addUniqSeq( struct UniqSeq** sequences, char* sequence, int count );

// Return number of unique sequences
int countUniqSeq( struct UniqSeq** sequences );

// get hashkey from name
int getHashKey( char* key, int size );

/******************** OPEN ********************/

// Open file
FILE* openReader( char* filename );

// Close file
// takes care of both fclose and pclose
void closeHandler( FILE* file, char* filename );

// Return file size
long int getFileSize( char* filename );

/******************** MAIN ********************/

// Show help message
void help( void );
