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

#include "vector.h"

/******************** STRING VECTOR ********************/

struct StringVector* newStringVector() {
	struct StringVector* constructor = 0;
	constructor = malloc( sizeof( struct StringVector ) );
	constructor->current = 0;
	constructor->last    = 0;
	constructor->objects = 0;
	return constructor;
}

struct StringVectorObject* newStringVectorObject( char* string ) {
	struct StringVectorObject* constructor = 0;
	constructor = malloc( sizeof( struct StringVectorObject ) );
	constructor->string = 0;
	constructor->string = malloc( sizeof( char ) * ( strlen( string ) + 1 ) );
	constructor->next = 0;
	strcpy( constructor->string, string );
	return constructor;
}

long int getStringVectorSize( struct StringVector* vector ) {
	struct StringVectorObject* object = vector->objects;
	long int size = 0;
	while( object != NULL ) {
		size++;
		object = object->next;
	}
	return size;
}

void addStringVector( struct StringVector* vector, char* string ) {
	if( vector->last == NULL ) {
		vector->objects = newStringVectorObject( string );
		vector->last    = vector->objects;
		vector->current = vector->objects;
	} else {
		vector->last->next = newStringVectorObject( string );
		vector->last = vector->last->next;
	}
}

char* getStringVector( struct StringVector* vector, long int index ) {
	long int size = 0;
	struct StringVectorObject* object = vector->objects;
	while( object != NULL ) {
		if( index == size++ ) return object->string;
		object = object->next;
	}
	return NULL;
}

void printStringVector( struct StringVector* vector, FILE* handler ) {
	resetStringVectorIndex( vector );
	struct StringVectorObject* object = vector->objects;
	while( object != NULL ) {
		fprintf( handler, "%s\n", object->string );
		object = object->next;
	}
}

int hasMoreStringVector( struct StringVector* vector ) { return vector->current != NULL; }

char* nextStringVector( struct StringVector* vector ) {
	if( vector->current == NULL ) return NULL;
	char* string = vector->current->string;
	vector->current = vector->current->next;
	return string;
}

char* peekStringVector( struct StringVector* vector ) {
	if( vector->current == NULL ) return NULL;
	else return vector->current->string;
}

void resetStringVectorIndex( struct StringVector* vector ) { vector->current = vector->objects; }

struct StringVectorObject* quickSort( struct StringVectorObject* pivot ) {
	if( pivot == NULL ) return NULL;
	struct StringVectorObject* larger_s  = 0; // start pointer
	struct StringVectorObject* larger_p  = 0; // end pointer
	struct StringVectorObject* equals_s  = pivot; // start pointer
	struct StringVectorObject* equals_p  = pivot; // end pointer
	struct StringVectorObject* smaller_s = 0; // start pointer
	struct StringVectorObject* smaller_p = 0; // end pointer
	struct StringVectorObject* object    = pivot->next;
	pivot->next = NULL;
	while( object != NULL ) {
		int compare = strcmp( object->string, pivot->string );
		if( compare < 0 ) { // smaller
			if( smaller_s == NULL ) {
				smaller_s = object;
				smaller_p = object;
			} else {
				smaller_p->next = object;
				smaller_p = object;
			}
		} else if( compare > 0 ) { // bigger
			if( larger_s == NULL ) {
				larger_s = object;
				larger_p = object;
			} else {
				larger_p->next = object;
				larger_p = object;
			}
		} else { // equals
			equals_p->next = object;
			equals_p = object;
		}
		object = object->next;
	}
	struct StringVectorObject* output_s = 0;
	struct StringVectorObject* output_e = 0;
	if( smaller_p != NULL ) {
		smaller_p->next = NULL;
		smaller_s = quickSort( smaller_s );
		output_s = smaller_s;
		output_e = smaller_s;
		while( output_e->next != NULL ) output_e = output_e->next;
	}
	if( equals_p != NULL ) {
		equals_p->next = NULL;
		if( output_s == NULL ) {
			output_s = equals_s;
			output_e = equals_s;
		} else {
			output_e->next = equals_s;
		}
		while( output_e->next != NULL ) output_e = output_e->next;
	}
	if( larger_p != NULL ) {
		larger_p->next  = NULL;
		larger_s = quickSort( larger_s );
		if( output_s == NULL ) {
			output_s = larger_s;
			output_e = larger_s;
		} else {
			output_e->next = larger_s;
		}
		while( output_e->next != NULL ) output_e = output_e->next;
	}
	return output_s;
}

void sortStringVector( struct StringVector* vector ) {
	vector->objects = quickSort( vector->objects );
	vector->current = vector->objects;
	vector->last    = vector->objects;
	while( vector->last->next != NULL ) vector->last = vector->last->next;
}

void freeStringVector( struct StringVector* vector ) {
	struct StringVectorObject* object = vector->objects;
	struct StringVectorObject* next = 0;
	while( object != NULL ) {
		next = object->next;
		free( object->string );
		free( object );
		object = next;
	}
	free( vector );
}

struct StringVector* split( char* string, char delim ) {
	struct StringVector* constructor = newStringVector(); // construct new string vector
	int start  = 0; // start
	int end    = 0; // end
	char* temp = 0; // temporary string
	temp = malloc( sizeof( char ) * ( strlen( string ) + 1 ) ); // allocate string memory
	for( end = 0; end < strlen( string ); end++ ) { // go through all characters
		if( string[ end ] == delim ) { // found delim
			strcpy( temp, "" ); // reset
			int length = end - start;
			strncpy( temp, string + start, length ); // copy from original string
			temp[ length ] = '\0'; // add EOF always after strncpy
			addStringVector( constructor, temp ); // add new string
			start = end + 1;
		}
	}
	strcpy( temp, "" ); // reset
	int length = end - start;
	strncpy( temp, string + start, length ); // copy from original string
	temp[ length ] = '\0'; // add EOF always after strncpy
	addStringVector( constructor, temp ); // add new string
	free( temp ); // free string
	return constructor;
}

struct StringVector* loadStringVector( struct StringVector* vector, char* path ) {
	FILE* filehandler = openReader( path );
	if( filehandler == NULL ) return NULL;
	if( vector == NULL ) vector = newStringVector();
	char line[ MAX_LINE ];
	while( fgets( line, MAX_LINE, filehandler ) ) {
		chomp( line );
		addStringVector( vector, line );
	}
	closeHandler( filehandler, path );
	return vector;
}
