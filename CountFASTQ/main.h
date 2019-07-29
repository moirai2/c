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

/******************** OPEN ********************/

// Open file
FILE* openReader( char* filename );

// Close file
// takes care of both fclose and pclose
void closeHandler( FILE* file, char* filename );

// get file size
long int getFileSize( char* filename );

// replace '\n' with '\0'
void chomp( char* line );

/******************** MAIN ********************/

// Show help message
void help( void );
