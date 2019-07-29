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

#ifndef REGEXP
#define REGEXP
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <regex.h>

/******************** DEFINE ********************/

#define MAXIMUM_REGULAR_EXPRESSION_MATCH 20

/******************** STRUCT ********************/


// structure to contain regular expression
struct RegularExpression {
	char* stringExpression;
	regex_t regularExpression; // regular expression currently used to handle filename
	regmatch_t* currentMatches; // expression matches
	int matchSize; // size of currently matched regular expression
	char* currentLine; // current line which changes with new lines
};

/******************** REGULAR EXPRESSION ********************/

// constructor
struct RegularExpression* newRegularExpression( char* stringExpression );

// match against line
int matchRegularExpression( struct RegularExpression* regexp, char* line );

// retrieve matching information
// Store in temp string
void getRegularExpression( struct RegularExpression* regexp, int index, char* temp );

// return start matching index
int getMatchStartIndex( struct RegularExpression* regexp, int index );

// return end matching index
int getMatchEndIndex( struct RegularExpression* regexp, int index );

// deconstructor
void freeRegularExpression( struct RegularExpression* regexp );

#endif
