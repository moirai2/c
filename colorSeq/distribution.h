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

#ifndef DISTRIBUTION
#define DISTRIBUTION
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
struct IntDistribution {
	int** array;
	int row;
	int column;
	char** labelRow;
	char** labelColumn;
};

/******************** CHAR ********************/

// Create new character distirbution with specified size
struct IntDistribution* newCharDistribution( int size );

// Create new int 2D distirbution with specified size
struct IntDistribution* newIntDistribution( int row, int column );

// Reset int distribution
void resetIntDistribution( struct IntDistribution* distribution );

// Free distribution
void freeIntDistribution( struct IntDistribution* distribution );

// Add labelRow
void addLabelRowDistribution( struct IntDistribution* distribution, char* label );

// Add labelColumn
void addLabelColumnDistribution( struct IntDistribution* distribution, char* label );

// Replace row label at specified index
void replaceLabelRowDistribution( struct IntDistribution* distribution, int index, char* label );

// Replace column label at specified index
void replaceLabelColumnDistribution( struct IntDistribution* distribution, int index, char* label );

// add int distribution with labelRow and labelColumn
void addIntDistribution( struct IntDistribution* distribution, char* labelRow, char* labelColumn, int value );

// add new character distribution with a new line
void addCharDistribution( struct IntDistribution* distribution, char* line );

// Print base distribution with character distribution
void printBaseDistribution( struct IntDistribution* distribution, FILE* writer );

// Print QV distribution with character distribution
void printQVDistribution( struct IntDistribution* distribution, FILE* writer );

// Print int distribution with character distribution
void printIntDistribution( struct IntDistribution* distribution, FILE* writer );

// Return maximum length of character distribution
int getMaximumLength( struct IntDistribution* distribution );

// Return maximum QV value from character distribution
int getMaximumQV( struct IntDistribution* distribution );

// Return minimum QV value from character distribution
int getMinimumQV( struct IntDistribution* distribution );
#endif
