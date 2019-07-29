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

#include "regexp.h"

struct RegularExpression* newRegularExpression( char* stringExpression ) {
	struct RegularExpression* constructor = 0;
	constructor = malloc( sizeof( struct RegularExpression ) );
	constructor->stringExpression = malloc( sizeof( char ) * ( strlen( stringExpression ) + 1 ) );
	strcpy( constructor->stringExpression, stringExpression );
	if( regcomp( &constructor->regularExpression, stringExpression, REG_EXTENDED | REG_NEWLINE ) != 0 ) fprintf( stderr, "regex compile failed \"%s\"\n", stringExpression );
	constructor->currentMatches = malloc( sizeof( regmatch_t ) * MAXIMUM_REGULAR_EXPRESSION_MATCH );
	return constructor;
}

int matchRegularExpression( struct RegularExpression* regexp, char* line ) {
	regexp->currentLine = line;
	if ( regexec( &regexp->regularExpression, line, MAXIMUM_REGULAR_EXPRESSION_MATCH, regexp->currentMatches, 0 ) != 0 ) { // try to match
		regexp->matchSize = 0; // no hit...
	} else { // there was a hit
		for( regexp->matchSize = 0; regexp->matchSize < MAXIMUM_REGULAR_EXPRESSION_MATCH; regexp->matchSize++ ) { // check how many hits are there
			if( regexp->currentMatches[ regexp->matchSize ].rm_so < 0 || regexp->currentMatches[ regexp->matchSize ].rm_eo < 0 ) break;
		}
	}
	return regexp->matchSize;
}

int replaceRegularExpression( struct RegularExpression* regexp, char* input, char* output ) {
	int size = matchRegularExpression( regexp, input );
	if( size == 0 ) {
		strcpy( output, input );
		return size;
	} else {
		
		return size;
	}
}

void getRegularExpression( struct RegularExpression* regexp, int index, char* temp ) {
	int start = regexp->currentMatches[ index ].rm_so;
	int end   = regexp->currentMatches[ index ].rm_eo;
	if( start < 0 || end < 0 ) { // not found
		temp[ 0 ] = '\0';
	} else { // found
		int length = end - start;
		strcpy( temp, "" );
		strncpy( temp, regexp->currentLine + start, length );
		temp[ length ] = '\0';
	}
}

int getMatchStartIndex( struct RegularExpression* regexp, int index ) { return regexp->currentMatches[ index ].rm_so; }

int getMatchEndIndex( struct RegularExpression* regexp, int index ) { return regexp->currentMatches[ index ].rm_eo; }

void freeRegularExpression( struct RegularExpression* regexp ) {
	free( regexp->stringExpression );
	free( regexp->currentMatches );
	regfree( &regexp->regularExpression );
	free( regexp );
}
