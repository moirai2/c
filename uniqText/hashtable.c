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

#include "hashtable.h"

/******************** STRING HASHTABLE ********************/

struct StringHashtable* newStringHashtable( int size ) {
	struct StringHashtable* constructor = 0;
	constructor = malloc( sizeof( struct StringHashtable ) );
	constructor->array = 0;
	constructor->size  = 0;
	constructor->array = malloc( sizeof( struct StringHashtableObject* ) * size );
	for( int i = 0; i < size; i++ ) constructor->array[ i ] = 0;
	constructor->randomizer = 0;
	constructor->randomizer = malloc( sizeof( int ) * 256 ); // 256 is size of character
	for( int i = 0; i < 256; i++ ) constructor->randomizer[ i ]= rand(); // create random value for each character
	constructor->size = size;
	return constructor;
}

void freeStringHashtable( struct StringHashtable* hashtable ) {
	if( hashtable == NULL ) return;
	for( int i = 0; i < hashtable->size; i++ ) if( hashtable->array[ i ] != NULL ) freeStringHashtableObject( hashtable->array[ i ] );
	free( hashtable->array );
	free( hashtable->randomizer );
	free( hashtable );
}

int getStringKey( struct StringHashtable* hashtable, char* key ) {
	int length = strlen( key );
	int value = hashtable->randomizer[ (int) key[ 0 ] ] << 1;
	for( int i = 1; i < length; i++ ) value = ( value ^ hashtable->randomizer[ (int) key[ i ] ] ) << 1;
	value = abs( value );
	return value % hashtable->size; // make sure value is within hashtable size
}

void countStringHashtable( struct StringHashtable* hashtable, char* key, int value ) {
	int index = getStringKey( hashtable, key );
	struct StringHashtableObject* object = hashtable->array[ index ];
	if( object == NULL ) { hashtable->array[ index ] = newStringHashtableObject( key, "", value ); return; }
	while( strcmp( object->key, key ) != 0 ) {
		if( object->next == NULL ) { object->next = newStringHashtableObject( key, "", value ); return; }
		object = object->next;
	}
	object->size += value;
}

void putStringHashtable( struct StringHashtable* hashtable, char* key, char* value ) {
	int index = getStringKey( hashtable, key );
	struct StringHashtableObject* object = hashtable->array[ index ];
	if( object == NULL ) { hashtable->array[ index ] = newStringHashtableObject( key, value, 1 ); return; }
	while( strcmp( object->key, key ) != 0 ) {
		if( object->next == NULL ) { object->next = newStringHashtableObject( key, value, 1 ); return; }
		object = object->next;
	}
	freeStringVector( object->vector );
	object->vector = newStringVector();
	addStringVector( object->vector, value );
}

void addStringHashtable( struct StringHashtable* hashtable, char* key, char* value ) {
	int index = getStringKey( hashtable, key );
	struct StringHashtableObject* object = hashtable->array[ index ];
	if( object == NULL ) { hashtable->array[ index ] = newStringHashtableObject( key, value, 1 ); return; }
	while( strcmp( object->key, key ) != 0 ) {
		if( object->next == NULL ) { object->next = newStringHashtableObject( key, value, 1 ); return; }
		object = object->next;
	}
	addStringVector( object->vector, value );
	object->size++;
}

void printStringHashtableCount( struct StringHashtable* hashtable, FILE* handler ) {
	for( int i = 0; i < hashtable->size; i++ ) {
		if( hashtable->array[ i ] == NULL ) continue;
		struct StringHashtableObject* object = hashtable->array[ i ];
		while( object != NULL ) {
			fprintf( handler, "%s\t%d\n", object->key, object->size );
			object = object->next;
		}
	}
}

void printStringHashtable( struct StringHashtable* hashtable, FILE* handler ) {
	for( int i = 0; i < hashtable->size; i++ ) {
		if( hashtable->array[ i ] == NULL ) continue;
		struct StringHashtableObject* object = hashtable->array[ i ];
		while( object != NULL ) {
			printStringHashtableObject( object, handler );
			object = object->next;
		}
	}
}

struct StringHashtableObject* newStringHashtableObject( char* key, char* value, int size ) {
	struct StringHashtableObject* constructor = 0;
	constructor = malloc( sizeof( struct StringHashtableObject ) );
	constructor->key    = 0;
	constructor->vector = 0;
	constructor->next   = 0;
	constructor->size   = size;
	constructor->key = malloc( sizeof( char ) * ( strlen( key ) + 1 ) );
	strcpy( constructor->key, key );
	if( value != NULL ) {
		constructor->vector = newStringVector();
		addStringVector( constructor->vector, value );
	}
	return constructor;
}

char* getStringHashtableString( struct StringHashtable* hashtable, char* key ) {
	struct StringHashtableObject* object = getStringHashtableObject( hashtable, key );
	if( object == NULL ) return NULL;
	if( object->vector == NULL ) return NULL;
	if( object->vector->last == NULL ) return NULL;
	return object->vector->last->string;
}

int getStringHashtableCount( struct StringHashtable* hashtable, char* key ) {
	struct StringHashtableObject* object = getStringHashtableObject( hashtable, key );
	if( object == NULL ) return 0;
	return object->size;
}

int getStringHashtableKeySize( struct StringHashtable* hashtable ) {
	int total = 0;
	for( int i = 0; i < hashtable->size; i++ ) {
		if( hashtable->array[ i ] == NULL ) continue;
		struct StringHashtableObject* object = hashtable->array[ i ];
		while( object != NULL ) {
			total++;
			object = object->next;
		}
	}
	return total;
}

struct StringHashtableObject* getStringHashtableObject( struct StringHashtable* hashtable, char* key ) {
	int index = getStringKey( hashtable, key );
	struct StringHashtableObject* object = hashtable->array[ index ];
	if( object == NULL ) return object;
	while( strcmp( object->key, key ) != 0 ) {
		if( object->next == NULL ) return object->next;
		object = object->next;
	}
	return object;
}

void printStringHashtableObject( struct StringHashtableObject* object, FILE* handler ) {
	struct StringVectorObject* temp = object->vector->objects;
	while( temp != NULL ) {
		fprintf( handler, "%s(%d) => %s\n", object->key, object->size, temp->string );
		temp = temp->next;
	}
}

struct StringVector* getStringHashtableKeys( struct StringHashtable* hashtable ) {
	struct StringVector* vector = newStringVector();
	for( int i = 0; i < hashtable->size; i++ ) {
		struct StringHashtableObject* object = hashtable->array[ i ];
		while( object != NULL ) {
			addStringVector( vector, object->key );
			object = object->next;
		}
	}
	return vector;
}

void sortStringVectorByCount( struct StringHashtable* hashtable, struct StringVector* vector ) {
	vector->objects = quickSortByCount( hashtable, vector->objects );
	vector->current = vector->objects;
	vector->last    = vector->objects;
	while( vector->last != NULL && vector->last->next != NULL ) vector->last = vector->last->next;
}

struct StringVectorObject* quickSortByCount( struct StringHashtable* hashtable, struct StringVectorObject* pivot ) {
	if( pivot == NULL ) return NULL;
	struct StringVectorObject* larger_s  = 0; // start pointer
	struct StringVectorObject* larger_p  = 0; // end pointer
	struct StringVectorObject* equals_s  = pivot; // start pointer
	struct StringVectorObject* equals_p  = pivot; // end pointer
	struct StringVectorObject* smaller_s = 0; // start pointer
	struct StringVectorObject* smaller_p = 0; // end pointer
	struct StringVectorObject* object    = pivot->next;
	pivot->next = NULL;
	int pivot_count = getStringHashtableCount( hashtable, pivot->string );
	while( object != NULL ) {
		int object_count = getStringHashtableCount( hashtable, object->string );
		int compare = 0;
		if( pivot_count < object_count ) { // count bigger
			compare = -1;
		} else if( pivot_count > object_count ) { // count smaller
			compare = +1;
		} else { // counts are same, so compare by strings
			compare = strcmp( object->string, pivot->string );
		}
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
		smaller_s = quickSortByCount( hashtable, smaller_s );
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
		larger_s = quickSortByCount( hashtable, larger_s );
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

void freeStringHashtableObject( struct StringHashtableObject* object ) {
	if( object == NULL ) return;
	if( object->next != NULL ) freeStringHashtableObject( object->next );
	free( object->key );
	freeStringVector( object->vector );
	free( object );
}
