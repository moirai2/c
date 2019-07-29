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

#include "chromosome.h"

/******************** CONSTRUCTOR ********************/

struct Chromosome* newChromosome( int size ) {
	struct Chromosome* constructor = 0;
	constructor = malloc( sizeof( struct Chromosome ) );
	constructor->array = 0;
	constructor->size  = 0;
	constructor->array = malloc( sizeof( struct ChromosomeObject* ) * size );
	for( int i = 0; i < size; i++ ) constructor->array[ i ] = 0;
	constructor->randomizer = 0;
	constructor->randomizer = malloc( sizeof( int ) * 256 ); // 256 is size of character
	for( int i = 0; i < 256; i++ ) constructor->randomizer[ i ]= rand(); // create random value for each character
	constructor->size = size;
	return constructor;
}

void freeChromosome( struct Chromosome* chromosomes ) {
	if( chromosomes == NULL ) return;
	for( int i = 0; i < chromosomes->size; i++ ) if( chromosomes->array[ i ] != NULL ) freeChromosomeObject( chromosomes->array[ i ] );
	free( chromosomes->array );
	free( chromosomes->randomizer );
	free( chromosomes );
}

struct ChromosomeObject* newChromosomeObject( char* key ) {
	struct ChromosomeObject* constructor = 0;
	constructor = malloc( sizeof( struct ChromosomeObject ) );
	constructor->key               = 0;
	constructor->positive_strand   = 0;
	constructor->negative_strand   = 0;
	constructor->next              = 0;
	constructor->primary_mapping   = 0;
	constructor->secondary_mapping = 0;
	constructor->key = malloc( sizeof( char ) * ( strlen( key ) + 1 ) );
	strcpy( constructor->key, key );
	return constructor;
}

void freeChromosomeObject( struct ChromosomeObject* object ) {
	if( object == NULL ) return;
	if( object->next != NULL ) freeChromosomeObject( object->next );
	free( object->key );
	free( object );
}

/******************** CHROMOSOME ********************/

int getStringKey( struct Chromosome* chromosomes, char* key ) {
	int length = strlen( key );
	unsigned int value = chromosomes->randomizer[ (int) key[ 0 ] ] << 1;
	for( int i = 1; i < length; i++ ) value ^= chromosomes->randomizer[ (int) key[ i ] ] << 1;
	// value = abs( value ); // make negative value -> positive value
	return value % chromosomes->size; // make sure value is within chromosomes size
}

struct StringVector* getChromosomeKeys( struct Chromosome* chromosomes ) {
	struct StringVector* vector = newStringVector();
	for( int i = 0; i < chromosomes->size; i++ ) {
		struct ChromosomeObject* object = chromosomes->array[ i ];
		while( object != NULL ) {
			addStringVector( vector, object->key );
			object = object->next;
		}
	}
	return vector;
}

void putChromosome( struct Chromosome* chromosomes, char* key ) {
	int index = getStringKey( chromosomes, key );
	struct ChromosomeObject* object = chromosomes->array[ index ];
	if( object == NULL ) { chromosomes->array[ index ] = newChromosomeObject( key ); return; }
	while( strcmp( object->key, key ) != 0 ) {
		if( object->next == NULL ) { object->next = newChromosomeObject( key ); return; }
		object = object->next;
	}
}

struct ChromosomeObject* getChromosomeObject( struct Chromosome* chromosomes, char* key ) {
	int index = getStringKey( chromosomes, key );
	struct ChromosomeObject* object = chromosomes->array[ index ];
	if( object == NULL ) return object;
	while( strcmp( object->key, key ) != 0 ) {
		if( object->next == NULL ) return object->next;
		object = object->next;
	}
	return object;
}
