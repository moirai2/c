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

#ifndef DOTMATRIX
#define DOTMATRIX
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

/******************** STRUCT ********************/

// Structure to store unique sequences and counts
struct DotMatrix {
	int** array; // table of int to store scores
	int** flag; // table of int to store flags
	int** mismatch; // table of mismatches
	int** indel; // table of insertion and deletions
	int** match; // table of insertion and deletions
	int sequence_length; // current sequence length
	int reference_length; // current reference length
	int maximum_sequence_length; // maximum sequence length
	int maximum_reference_length; // maximum reference length
	char* sequence; // current sequence
	char* reference; // current reference
	char* traceback_sequence;
	char* traceback_reference;
	char* traceback_match;
	int match_count;
	int mismatch_count;
	int insertion_count;
	int deletion_count;
	int subject_start;
	int subject_end;
	int query_start;
	int query_end;
	int match_reward; // 2
	int mismatch_penalty; // -1
	int gap_penalty; // -2
	int error_allowed;
};

/******************** METHOD ********************/

// create new dot matrix
struct DotMatrix* newDotMatrix();

// resize dot matrix
void resizeDotMatrix( struct DotMatrix* matrix, int maximum_sequence_length, int maximum_reference_length );

// free
void freeDotMatrix( struct DotMatrix* matrix );

// upper sequence
void toUpperSequence( char* sequence );

// print
void printDotMatrix( struct DotMatrix* matrix, FILE* writer );

// print array
void printDotMatrixArray( struct DotMatrix* matrix, FILE* writer );

// print pointer array
void printDotMatrixPointer( struct DotMatrix* matrix, FILE* writer );

// print score array
void printDotMatrixError( struct DotMatrix* matrix, FILE* writer );

// initialize
void initializeDotMatrix( struct DotMatrix* matrix, char* sequence, char* reference, int error_allowed );

// calculate dot matrix
int calculateDotMatrix( struct DotMatrix* matrix );

// calculate dot matrix
int maximumDotMatrix( struct DotMatrix* matrix );

// trace back
int tracebackDotMatrix( struct DotMatrix* matrix, int start );

// reverse sequence
void reverse_sequence( char* sequence );
#endif
