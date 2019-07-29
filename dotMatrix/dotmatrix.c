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

#include "dotmatrix.h"

/******************** METHOD ********************/

// create new dot matrix
struct DotMatrix* newDotMatrix() {
	struct DotMatrix* constructor = 0;
	constructor = malloc( sizeof( struct DotMatrix ) );
	constructor->maximum_sequence_length  = 0;
	constructor->maximum_reference_length = 0;
	constructor->sequence_length  = 0;
	constructor->reference_length = 0;
	constructor->array = 0;
	constructor->flag = 0;
	constructor->match = 0;
	constructor->mismatch = 0;
	constructor->indel = 0;
	constructor->sequence  = 0;
	constructor->reference = 0;
	constructor->traceback_sequence  = 0;
	constructor->traceback_match     = 0;
	constructor->traceback_reference = 0;
	constructor->match_count     = 0;
	constructor->mismatch_count  = 0;
	constructor->insertion_count = 0;
	constructor->deletion_count  = 0;
	constructor->subject_start = 0;
	constructor->subject_end = 0;
	constructor->query_start = 0;
	constructor->query_end = 0;
	constructor->match_reward = 2;
	constructor->mismatch_penalty = -1;
	constructor->gap_penalty = -2;
	constructor->error_allowed = 0;
	return constructor;
}

void resizeDotMatrix( struct DotMatrix* matrix, int maximum_sequence_length, int maximum_reference_length ) {
	// free array, if necessary
	if( matrix->array != NULL ) {
		for( int i = 0; i < matrix->maximum_sequence_length + 1; i++ ) free( matrix->array[ i ] ); 
		free( matrix->array );
	}
	if( matrix->flag != NULL ) {
		for( int i = 0; i < matrix->maximum_sequence_length + 1; i++ ) free( matrix->flag[ i ] ); 
		free( matrix->flag );
	}
	if( matrix->match != NULL ) {
		for( int i = 0; i < matrix->maximum_sequence_length + 1; i++ ) free( matrix->match[ i ] ); 
		free( matrix->match );
	}
	if( matrix->mismatch != NULL ) {
		for( int i = 0; i < matrix->maximum_sequence_length + 1; i++ ) free( matrix->mismatch[ i ] ); 
		free( matrix->mismatch );
	}
	if( matrix->indel != NULL ) {
		for( int i = 0; i < matrix->maximum_sequence_length + 1; i++ ) free( matrix->indel[ i ] ); 
		free( matrix->indel );
	}
	if( matrix->sequence  != NULL ) free( matrix->sequence );
	if( matrix->reference != NULL ) free( matrix->reference );
	if( matrix->traceback_sequence  != NULL ) free( matrix->traceback_sequence  );
	if( matrix->traceback_reference != NULL ) free( matrix->traceback_reference );
	if( matrix->traceback_match     != NULL ) free( matrix->traceback_match     );
	// construct
	matrix->maximum_sequence_length  = maximum_sequence_length;
	matrix->maximum_reference_length = maximum_reference_length;
	matrix->array = malloc( sizeof( int* ) * ( maximum_sequence_length + 1 ) );
	for( int i = 0; i < maximum_sequence_length + 1; i++ ) {
		matrix->array[ i ] = malloc( sizeof( int ) * ( maximum_reference_length + 1 ) );
		for( int j = 0; j < maximum_reference_length + 1; j++ ) matrix->array[ i ][ j ] = 0;
	}
	matrix->flag = malloc( sizeof( int* ) * ( maximum_sequence_length + 1 ) );
	for( int i = 0; i < maximum_sequence_length + 1; i++ ) {
		matrix->flag[ i ] = malloc( sizeof( int ) * ( maximum_reference_length + 1 ) );
		for( int j = 0; j < maximum_reference_length + 1; j++ ) matrix->flag[ i ][ j ] = 0;
	}
	matrix->match = malloc( sizeof( int* ) * ( maximum_sequence_length + 1 ) );
	matrix->mismatch = malloc( sizeof( int* ) * ( maximum_sequence_length + 1 ) );
	matrix->indel = malloc( sizeof( int* ) * ( maximum_sequence_length + 1 ) );
	for( int i = 0; i < maximum_sequence_length + 1; i++ ) {
		matrix->match[ i ] = malloc( sizeof( int ) * ( maximum_reference_length + 1 ) );
		for( int j = 0; j < maximum_reference_length + 1; j++ ) matrix->match[ i ][ j ] = 0;
		matrix->mismatch[ i ] = malloc( sizeof( int ) * ( maximum_reference_length + 1 ) );
		for( int j = 0; j < maximum_reference_length + 1; j++ ) matrix->mismatch[ i ][ j ] = 0;
		matrix->indel[ i ] = malloc( sizeof( int ) * ( maximum_reference_length + 1 ) );
		for( int j = 0; j < maximum_reference_length + 1; j++ ) matrix->indel[ i ][ j ] = 0;
	}
	matrix->sequence  = malloc( sizeof( char ) * ( maximum_sequence_length + 1 ) );
	matrix->reference = malloc( sizeof( char ) * ( maximum_reference_length + 1 ) );
	matrix->traceback_sequence  = malloc( sizeof( char ) * ( maximum_sequence_length + maximum_reference_length + 1 ) );
	matrix->traceback_match     = malloc( sizeof( char ) * ( maximum_sequence_length + maximum_reference_length + 1 ) );
	matrix->traceback_reference = malloc( sizeof( char ) * ( maximum_sequence_length + maximum_reference_length + 1 ) );
}

void freeDotMatrix( struct DotMatrix* matrix ) {
	if( matrix->array != NULL ) {
		for( int i = 0; i < matrix->maximum_sequence_length + 1; i++ ) free( matrix->array[ i ] ); 
		free( matrix->array );
		matrix->array = 0;
	}
	if( matrix->flag != NULL ) {
		for( int i = 0; i < matrix->maximum_sequence_length + 1; i++ ) free( matrix->flag[ i ] ); 
		free( matrix->flag );
		matrix->flag = 0;
	}
	if( matrix->match != NULL ) {
		for( int i = 0; i < matrix->maximum_sequence_length + 1; i++ ) free( matrix->match[ i ] ); 
		free( matrix->match );
		matrix->match = 0;
	}
	if( matrix->mismatch != NULL ) {
		for( int i = 0; i < matrix->maximum_sequence_length + 1; i++ ) free( matrix->mismatch[ i ] ); 
		free( matrix->mismatch );
		matrix->mismatch = 0;
	}
	if( matrix->indel != NULL ) {
		for( int i = 0; i < matrix->maximum_sequence_length + 1; i++ ) free( matrix->indel[ i ] ); 
		free( matrix->indel );
		matrix->indel = 0;
	}
	if( matrix->sequence  != NULL ) free( matrix->sequence  );
	if( matrix->reference != NULL ) free( matrix->reference );
	if( matrix->traceback_sequence  != NULL ) free( matrix->traceback_sequence  );
	if( matrix->traceback_reference != NULL ) free( matrix->traceback_reference );
	if( matrix->traceback_match     != NULL ) free( matrix->traceback_match     );
	free( matrix );
}

void initializeDotMatrix( struct DotMatrix* matrix, char* sequence, char* reference, int error_allowed ) {
	int sequence_length = strlen( sequence );
	int reference_length = strlen( reference );
	if( sequence_length > matrix->maximum_sequence_length || reference_length > matrix->maximum_reference_length ) resizeDotMatrix( matrix, sequence_length, reference_length );
	matrix->sequence_length = sequence_length;
	matrix->reference_length = reference_length;
	strcpy( matrix->sequence, sequence );
	strcpy( matrix->reference, reference );
	toUpperSequence( matrix->sequence );
	toUpperSequence( matrix->reference );
	for( int i = 0; i < sequence_length + 1; i++ ) { // go through sequence
		for( int j = 0; j < reference_length + 1; j++ ) { // go through reference
			matrix->array[ i ][ j ] = 0; // intialize matrix
		}
	}
	for( int i = 0; i < sequence_length + 1; i++ ) { // go through sequence
		for( int j = 0; j < reference_length + 1; j++ ) { // go through reference
			matrix->flag[ i ][ j ] = 0; // intialize matrix
		}
	}
	for( int i = 0; i < sequence_length + 1; i++ ) { // go through sequence
		for( int j = 0; j < reference_length + 1; j++ ) { // go through reference
			matrix->match[ i ][ j ] = 0; // intialize matrix
		}
	}
	for( int i = 0; i < sequence_length + 1; i++ ) { // go through sequence
		for( int j = 0; j < reference_length + 1; j++ ) { // go through reference
			matrix->mismatch[ i ][ j ] = 0; // intialize matrix
		}
	}
	for( int i = 0; i < sequence_length + 1; i++ ) { // go through sequence
		for( int j = 0; j < reference_length + 1; j++ ) { // go through reference
			matrix->indel[ i ][ j ] = 0; // intialize matrix
		}
	}
	matrix->error_allowed = error_allowed;
}

void toUpperSequence( char* sequence ) {
	int length = strlen( sequence );
	for( int i = 0; i< length; i++ ) {
		sequence[ i ] = toupper( sequence[ i ] );
		if(      sequence[ i ] == 'A' || sequence[ i ] == 'a' ) sequence[ i ] = 'A';
		else if( sequence[ i ] == 'C' || sequence[ i ] == 'c' ) sequence[ i ] = 'C';
		else if( sequence[ i ] == 'G' || sequence[ i ] == 'g' ) sequence[ i ] = 'G';
		else if( sequence[ i ] == 'T' || sequence[ i ] == 't' ) sequence[ i ] = 'T';
		else sequence[ i ] = 'N';
	}
}

void printDotMatrixArray( struct DotMatrix* matrix, FILE* writer ) {
	fprintf( writer, ">array\n" );
	for( int i = 0; i < matrix->reference_length; i++ ) fprintf( writer, "\t%c", matrix->reference[ i ] );
	fprintf( writer, "\n" );
	for( int i = 0; i < matrix->sequence_length; i++ ) {
		fprintf( writer, "%c", matrix->sequence[ i ] );
		for( int j = 0; j < matrix->reference_length; j++ ) fprintf( writer, "\t%d", matrix->array[ i + 1 ][ j + 1 ] );
		fprintf( writer, "\n" );
	}
}

void printDotMatrixPointer( struct DotMatrix* matrix, FILE* writer ) {
	fprintf( writer, ">pointer\n" );
	for( int i = 0; i < matrix->reference_length; i++ ) fprintf( writer, "\t%c", matrix->reference[ i ] );
	fprintf( writer, "\n" );
	for( int i = 0; i < matrix->sequence_length; i++ ) {
		fprintf( writer, "%c", matrix->sequence[ i ] );
		for( int j = 0; j < matrix->reference_length; j++ ) fprintf( writer, "\t%d", matrix->flag[ i + 1 ][ j + 1 ] );
		fprintf( writer, "\n" );
	}
}

void printDotMatrixError( struct DotMatrix* matrix, FILE* writer ) {
	fprintf( writer, ">match\n" );
	for( int i = 0; i < matrix->reference_length; i++ ) fprintf( writer, "\t%c", matrix->reference[ i ] );
	fprintf( writer, "\n" );
	for( int i = 0; i < matrix->sequence_length; i++ ) {
		fprintf( writer, "%c", matrix->sequence[ i ] );
		for( int j = 0; j < matrix->reference_length; j++ ) fprintf( writer, "\t%d", matrix->match[ i + 1 ][ j + 1 ] );
		fprintf( writer, "\n" );
	}
	fprintf( writer, ">mismatch\n" );
	for( int i = 0; i < matrix->reference_length; i++ ) fprintf( writer, "\t%c", matrix->reference[ i ] );
	fprintf( writer, "\n" );
	for( int i = 0; i < matrix->sequence_length; i++ ) {
		fprintf( writer, "%c", matrix->sequence[ i ] );
		for( int j = 0; j < matrix->reference_length; j++ ) fprintf( writer, "\t%d", matrix->mismatch[ i + 1 ][ j + 1 ] );
		fprintf( writer, "\n" );
	}
	fprintf( writer, ">indel\n" );
	for( int i = 0; i < matrix->reference_length; i++ ) fprintf( writer, "\t%c", matrix->reference[ i ] );
	fprintf( writer, "\n" );
	for( int i = 0; i < matrix->sequence_length; i++ ) {
		fprintf( writer, "%c", matrix->sequence[ i ] );
		for( int j = 0; j < matrix->reference_length; j++ ) fprintf( writer, "\t%d", matrix->indel[ i + 1 ][ j + 1 ] );
		fprintf( writer, "\n" );
	}
}

void printDotMatrix( struct DotMatrix* matrix, FILE* writer ) {
	int query_size = (int) ( log10( matrix->query_start ) ) + 1;
	int subject_size = (int) ( log10( matrix->subject_start ) ) + 1;
	int largest_size = ( query_size > subject_size ) ? query_size : subject_size;
	fprintf( writer, "Query " );
	for( int i = query_size; i < largest_size; i++ ) fprintf( writer, " " );
	fprintf( writer, "%d %s %d\n", matrix->query_start, matrix->traceback_sequence, matrix->query_end );
	fprintf( writer, "      " );
	for( int i = 0; i < largest_size + 1; i++ ) fprintf( writer, " " );
	fprintf( writer, "%s\n", matrix->traceback_match );
	fprintf( writer, "Sbjct " );
	for( int i = subject_size; i < largest_size; i++ ) fprintf( writer, " " );
	fprintf( writer, "%d %s %d \n", matrix->subject_start, matrix->traceback_reference, matrix->subject_end );
	fprintf( writer, "%d match, %d mismatch, %d insertion, %d deletion\n", matrix->match_count, matrix->mismatch_count, matrix->insertion_count, matrix->deletion_count );
}

int calculateDotMatrix( struct DotMatrix* matrix ) {
	int maximum_value = 0;
	for( int i = 1; i < matrix->sequence_length + 1; i++ ) {
		for( int j = 1; j < matrix->reference_length + 1; j++ ) {
			int reward = matrix->mismatch_penalty; // default is mismatch
			if(      matrix->sequence[ i - 1 ]  == 'N'                        ) reward = matrix->match_reward; // match
			else if( matrix->reference[ j - 1 ] == 'N'                        ) reward = matrix->match_reward; // match
			else if( matrix->sequence[ i - 1 ]  == matrix->reference[ j - 1 ] ) reward = matrix->match_reward; // match
			int max_up     = matrix->array[ i - 1 ][ j ] + matrix->gap_penalty; // deletion
			int max_left   = matrix->array[ i ][ j - 1 ] + matrix->gap_penalty; // insertion
			int max_center = matrix->array[ i - 1 ][ j - 1 ] + reward; // match or mismatch
			if( max_up > max_center ) { // insertion
				matrix->array[ i ][ j ]    = max_up;
				matrix->flag[ i ][ j ]     = 2;
				matrix->match[ i ][ j ]    = matrix->match[ i - 1 ][ j ];
				matrix->mismatch[ i ][ j ] = matrix->mismatch[ i - 1 ][ j ];
				matrix->indel[ i ][ j ]    = matrix->indel[ i - 1 ][ j ] + 1;
			} else if( max_left > max_center ) { // deletion
				matrix->array[ i ][ j ]    = max_left;
				matrix->flag[ i ][ j ]     = 4;
				matrix->match[ i ][ j ]    = matrix->match[ i ][ j - 1 ];
				matrix->mismatch[ i ][ j ] = matrix->mismatch[ i ][ j - 1 ];
				matrix->indel[ i ][ j ]    = matrix->indel[ i ][ j - 1 ] + 1;
			} else if( reward == matrix->mismatch_penalty ) { // mismatch
				matrix->array[ i ][ j ]    = max_center;
				matrix->flag[ i ][ j ]     = 1;
				matrix->match[ i ][ j ]    = matrix->match[ i - 1 ][ j - 1 ];
				matrix->mismatch[ i ][ j ] = matrix->mismatch[ i - 1 ][ j - 1 ] + 1;
				matrix->indel[ i ][ j ]    = matrix->indel[ i - 1 ][ j - 1 ];
			} else if( reward == matrix->match_reward ) { // match
				matrix->array[ i ][ j ]    = max_center;
				matrix->flag[ i ][ j ]     = 1;
				matrix->match[ i ][ j ]    = matrix->match[ i - 1 ][ j - 1 ] + 1;
				matrix->mismatch[ i ][ j ] = matrix->mismatch[ i - 1 ][ j - 1 ];
				matrix->indel[ i ][ j ]    = matrix->indel[ i - 1 ][ j - 1 ];
			}
			if( max_center > maximum_value ) maximum_value = max_center;
		}
	}
	return maximum_value;
}

int maximumDotMatrix( struct DotMatrix* matrix ) {
	int maximum_value = 0;
	for( int i = 0; i < matrix->sequence_length; i++ ) {
		for( int j = 0; j < matrix->reference_length; j++ ) {
			if( matrix->array[ i ][ j ] > maximum_value ) maximum_value = matrix->array[ i ][ j ];
		}
	}
	return maximum_value;
}

void reverse_sequence( char* sequence ) {
	int length = strlen( sequence );
	int half = length / 2;
	for( int i = 0; i < half; i++ ) {
		int c = sequence[ i ];
		sequence[ i ] = sequence[ length - i - 1 ];
		sequence[ length - i - 1 ] = c;
	}
}

int tracebackDotMatrix( struct DotMatrix* matrix, int start ) {
	strcpy( matrix->traceback_sequence, "" );
	strcpy( matrix->traceback_match, "" );
	strcpy( matrix->traceback_reference, "" );
	matrix->match_count     = 0;
	matrix->mismatch_count  = 0;
	matrix->insertion_count = 0;
	matrix->deletion_count  = 0;
	matrix->subject_start = 0;
	matrix->subject_end   = 0;
	matrix->query_start   = 0;
	matrix->query_end     = 0;
	int row = 0;
	int column = 0;
	int sequence_index = 0;
	int current_score = 0;
	int look_upto = matrix->sequence_length + 1;
	for( int i = start + 1; ( i < look_upto ) && ( i < matrix->sequence_length + 1 ); i++ ) {
		for( int j = 0; j < matrix->reference_length + 1; j++ ) {
			if( matrix->mismatch[ i ][ j ] + matrix->indel[ i ][ j ] > matrix->error_allowed ) continue; // check maximum error allowed
			if( matrix->array[ i ][ j ] > current_score ) { // check score
				row    = i; // remember position
				column = j; // remember position
				current_score = matrix->array[ i ][ j ]; // remember highest score
				if( matrix->match[ i ][ j ] < matrix->reference_length - matrix->error_allowed ) continue;
				look_upto = i + matrix->error_allowed + 1; // look upto maximum error allowed from highest score position
			}
		}
	}
	if( row == 0 && column == 0 ) return 0; // not found...
	matrix->query_end   = row;
	matrix->subject_end = column;
	while( ( row > 0 ) && ( column > 0 ) ) {
		int flag = matrix->flag[ row ][ column ];
		if( ( flag & 1 ) > 0 ) { // diagonal
			matrix->traceback_sequence[ sequence_index ] = matrix->sequence[ row - 1 ];
			if( matrix->array[ row - 1 ][ column - 1 ] < matrix->array[ row ][ column ] ) { // match
				matrix->traceback_match[ sequence_index ] = '|';
				matrix->match_count++;
			} else { // mismatch
				matrix->traceback_match[ sequence_index ] = ' ';
				matrix->mismatch_count++;
			}
			matrix->traceback_reference[ sequence_index ] = matrix->reference[ column - 1 ];
			row--;
			column--;
		} else if( ( flag & 2 ) > 0 ) { // up = insertion
			matrix->traceback_sequence[ sequence_index ] = matrix->sequence[ row - 1 ];
			matrix->traceback_match[ sequence_index ] = ' ';
			matrix->traceback_reference[ sequence_index ] = '-';
			matrix->insertion_count++;
			row--;
		} else if( ( flag & 4 ) > 0 ) { // left = deletion
			matrix->traceback_sequence[ sequence_index ] = '-';
			matrix->traceback_match[ sequence_index ] = ' ';
			matrix->traceback_reference[ sequence_index ] = matrix->reference[ column - 1 ];
			matrix->deletion_count++;
			column--;
		}
		sequence_index++;
	}
	matrix->query_start = row + 1;
	matrix->subject_start = column + 1;
	matrix->traceback_sequence[ sequence_index ]   = '\0';
	matrix->traceback_match[ sequence_index ] = '\0';
	matrix->traceback_reference[ sequence_index ] = '\0';
	reverse_sequence( matrix->traceback_sequence );
	reverse_sequence( matrix->traceback_match );
	reverse_sequence( matrix->traceback_reference );
	// printDotMatrixArray( matrix, stderr );
	// printDotMatrixPointer( matrix, stderr );
	// printDotMatrixError( matrix, stderr );
	// printDotMatrix( matrix, stderr );
	return ( matrix->match_count >= matrix->reference_length - matrix->error_allowed ) && ( matrix->mismatch_count + matrix->insertion_count + matrix->deletion_count <= matrix->error_allowed );
}
