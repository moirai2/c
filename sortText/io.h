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

#ifndef IO
#define IO
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

/******************** DEFINE ********************/

// maximum characters per line
#define MAX_LINE 50000
#define MAX_FILENAME 500

/******************** BASENAME ********************/

// extract basename from a path
char* getBasename( char* path, char* output );

// extract suffix from a path
char* getSuffix( char* path, char* output );

// extract filename from a path
char* getFilename( char* path, char* output );

// extract directory from a path
char* getDirectory( char* path, char* output );

/******************** HANDLER ********************/

// Open input file as text file
// Takes care of bam, gzip, and bzip
// '-', "STDIN" or "stdin" returns standard input
// bam format requires samtools installed
// gzcat => zcat
// bzcat => zcat
FILE* openReader( char* filename );

// Open output file
// Takes care of gzip and bzip files
FILE* openWriter( char* filename );

// Close file
// takes care of both fclose and pclose
void closeHandler( FILE* file, char* filename );

/******************** INFORMATION ********************/

// Get file size
// for zipped file, file has to opened by binary mode.
long int getFileSize( char* filename );

// Check if specified path is a directory or not
// path = path to get from
int isDirectory( char* path );

// Check if specified path is a readable file or not
// path = path to get from
int isFile( char* path );

// Check if specified path is a directory or not
// path = path to search
int existsFile( char* path );

/******************** UTILITY ********************/

// replace '\n' with '\0'
void chomp( char* line );

// Extract text from input from start to end
void extract( char* input, char* output, int start, int end );

// get index of char from a path
int getIndex( char* path, char c, int index );

// get last index of char from a path
int getLastIndex( char* path, char c, int index );

// replace c1 with c2
int replaceChar( char* line, char c1, char c2 );

// Count number of characters found in a line
int countChar( char* line, char c );

#endif
