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

#ifndef INTERFACE
#define INTERFACE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

/******************** DEFINE ********************/

#define MAXIMUM_INTERFACE_NUMBER 100
#define FALSE 0
#define TRUE 1

/******************** STRUCT ********************/

// Structure to contain interface information
struct Interface {
	int option_size;
	int* option_types; // 0 means no option value, 1 mean option value
	char* option_keys;
	char** option_values;
	int argument_size;
	char** arguments;
};

/******************** INTERFACE ********************/

// constructor
struct Interface* newInterface( char* option, int argc, char** argv );

// check if option exists or not
int existsOption( struct Interface* interface, char c );

// deconstructor
void freeInterface( struct Interface* interface );

// Add "stdin" if arguments are empty
void addStdinIfEmpty( struct Interface* interface );

// Add new argument to interface
void addArgument( struct Interface* interface, char* argument );

// get i-th argument from interface
char* getArgument( struct Interface* interface, int index );

// retrieve string option associated with specified char
char* getString( struct Interface* interface, char c );

// retrieve int option associated with specified char
int getInt( struct Interface* interface, char c );

// retrieve char option associated with specified char
char getChar( struct Interface* interface, char c );

// retrieve double option associated with specified char
double getDouble( struct Interface* interface, char c );

// retrieve boolean option associated with specified char
int getBoolean( struct Interface* interface, char c );

#endif
