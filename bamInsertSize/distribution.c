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

#include "distribution.h"

struct IntDistribution* newCharDistribution( int size ) {
	struct IntDistribution* constructor = 0;
	constructor = malloc( sizeof( struct IntDistribution ) );
	constructor->row = size;
	constructor->column = 256;
	constructor->array = 0;
	constructor->array = malloc( sizeof( int* ) * constructor->row );
	for( int i = 0; i < constructor->row; i++ ) {
		constructor->array[ i ] = malloc( sizeof( int ) * constructor->column );
		for( int j = 0; j < constructor->column; j++ ) constructor->array[ i ][ j ] = 0;
	}
	constructor->labelRow = 0;
	constructor->labelColumn = 0;
	return constructor;
}

struct IntDistribution* newIntDistribution( int row, int column ) {
	struct IntDistribution* constructor = 0;
	constructor = malloc( sizeof( struct IntDistribution ) );
	constructor->row = row;
	constructor->column = column;
	constructor->array = 0;
	constructor->array = malloc( sizeof( int* ) * constructor->row );
	for( int i = 0; i < constructor->row; i++ ) {
		constructor->array[ i ] = malloc( sizeof( int ) * constructor->column );
		for( int j = 0; j < constructor->column; j++ ) constructor->array[ i ][ j ] = 0;
	}
	constructor->labelRow = 0;
	constructor->labelRow = malloc( sizeof( char* ) * constructor->row );
	for( int i = 0; i < constructor->row; i++ ) constructor->labelRow[ i ] = 0;
	constructor->labelColumn = 0;
	constructor->labelColumn = malloc( sizeof( char* ) * constructor->column );
	for( int i = 0; i < constructor->column; i++ ) constructor->labelColumn[ i ] = 0;
	return constructor;
}

void freeIntDistribution( struct IntDistribution* distribution ) {
	for( int i = 0; i < distribution->row; i++ ) free( distribution->array[ i ] );
	free( distribution->array );
	if( distribution->labelRow != NULL ) {
		for( int i = 0; i < distribution->row; i++ ) if( distribution->labelRow[ i ] != NULL ) free( distribution->labelRow[ i ] );
		free( distribution->labelRow );
	}
	if( distribution->labelColumn != NULL ) {
		for( int i = 0; i < distribution->column; i++ ) if( distribution->labelColumn[ i ] != NULL ) free( distribution->labelColumn[ i ] );
		free( distribution->labelColumn );
	}
	free( distribution );
}

void resetIntDistribution( struct IntDistribution* distribution ) {
	for( int i = 0; i < distribution->row; i++ ) {
		for( int j = 0; j < distribution->column; j++ ) distribution->array[ i ][ j ] = 0;
	}
}

void addLabelRowDistribution( struct IntDistribution* distribution, char* label ) {
	for( int i = 0; i < distribution->row; i++ ) {
		if( distribution->labelRow[ i ] == NULL ) {
			distribution->labelRow[ i ] = 0;
			distribution->labelRow[ i ] = malloc( sizeof( char ) * ( strlen( label ) + 1 ) );
			strcpy( distribution->labelRow[ i ], label  );
			return;
		}
	}
}

void addLabelColumnDistribution( struct IntDistribution* distribution, char* label ) {
	for( int i = 0; i < distribution->column; i++ ) {
		if( distribution->labelColumn[ i ] == NULL ) {
			distribution->labelColumn[ i ] = 0;
			distribution->labelColumn[ i ] = malloc( sizeof( char ) * ( strlen( label ) + 1 ) );
			strcpy( distribution->labelColumn[ i ], label  );
			return;
		}
	}
}

void replaceLabelRowDistribution( struct IntDistribution* distribution, int index, char* label ) {
	if( distribution->labelRow[ index ] != NULL ) free( distribution->labelRow[ index ] );
	distribution->labelRow[ index ] = 0;
	distribution->labelRow[ index ] = malloc( sizeof( char ) * ( strlen( label ) + 1 ) );
	strcpy( distribution->labelRow[ index ], label  );
	return;
}

void replaceLabelColumnDistribution( struct IntDistribution* distribution, int index, char* label ) {
	if( distribution->labelColumn[ index ] != NULL ) free( distribution->labelColumn[ index ] );
	distribution->labelColumn[ index ] = 0;
	distribution->labelColumn[ index ] = malloc( sizeof( char ) * ( strlen( label ) + 1 ) );
	strcpy( distribution->labelColumn[ index ], label  );
	return;
}

void addIntDistribution( struct IntDistribution* distribution, char* labelRow, char* labelColumn, int value ) {
	int row = -1;
	int column = -1;
	for( int i = 0; i < distribution->row;  i++ ) if( strcmp( distribution->labelRow[ i ], labelRow ) == 0 ) { row = i; break; }
	for( int i = 0; i < distribution->column; i++ ) if( strcmp( distribution->labelColumn[ i ], labelColumn ) == 0 ) { column = i; break; }
	if( row < 0 || column < 0 ) return;
	distribution->array[ row ][ column ] += value;
}

void addCharDistribution( struct IntDistribution* distribution, char* line ) {
	int length = strlen( line );
	for( int i = 0; i < length; i++ ) distribution->array[ i ][ (int) line[ i ] ]++;
}

void printBaseDistribution( struct IntDistribution* distribution, FILE* writer ) {
	int length = getMaximumLength( distribution );
	fprintf( writer, "\tA\tC\tG\tT\tN\n" );
	for( int i = 0; i < length; i++ ) {
		int a = 0;
		int c = 0;
		int g = 0;
		int t = 0;
		int n = 0;
		for( int j = 0; j < distribution->column; j++ ) {
			if( j == (int) 'A' ) a +=  distribution->array[ i ][ j ];
			else if( j == (int) 'a' ) a += distribution->array[ i ][ j ];
			else if( j == (int) 'T' ) t += distribution->array[ i ][ j ];
			else if( j == (int) 't' ) t += distribution->array[ i ][ j ];
			else if( j == (int) 'C' ) c += distribution->array[ i ][ j ];
			else if( j == (int) 'c' ) c += distribution->array[ i ][ j ];
			else if( j == (int) 'G' ) g += distribution->array[ i ][ j ];
			else if( j == (int) 'g' ) g += distribution->array[ i ][ j ];
			else n +=  distribution->array[ i ][ j ];
		}
		fprintf( writer, "%d\t%d\t%d\t%d\t%d\t%d\n", i + 1, a, c, g, t, n );
	}
}

void printQVDistribution( struct IntDistribution* distribution, FILE* writer ) {
	int minimum = getMinimumQV( distribution );
	int maximum = getMaximumQV( distribution );
	int length = getMaximumLength( distribution );
	for( int i = 0; i <= maximum - minimum; i++ ) fprintf( writer, "\t%d", i );
	fprintf( writer, "\n" );
	for( int i = 0; i < length; i++ ) {
		fprintf( writer, "%d", ( i + 1 ) );
		for( int j = minimum; j <= maximum; j++ ) {
			int value = distribution->array[ i ][ j ];
			fprintf( writer, "\t%d", value );
		}
		fprintf( writer, "\n" );
	}
}

void printIntDistribution( struct IntDistribution* distribution, FILE* writer ) {
	for( int i = 0; i < distribution->column; i++ ) fprintf( writer, "\t%s", distribution->labelColumn[ i ] );
	fprintf( writer, "\n" );
	for( int i = 0; i < distribution->row; i++ ) {
		fprintf( writer, "%s", distribution->labelRow[ i ] );
		for( int j = 0; j < distribution->column; j++ ) fprintf( writer, "\t%d", distribution->array[ i ][ j ] );
		fprintf( writer, "\n" );
	}
}

int getMaximumLength( struct IntDistribution* distribution ) {
	int maximumLength = 0;
	for( int i = 0; i < distribution->row; i++ ) {
		for( int j = 0; j < distribution->column; j++ ) {
			if( distribution->array[ i ][ j ] > 0 ) { maximumLength = i; break; }
		}
	}
	return maximumLength + 1;
}

int getMaximumQV( struct IntDistribution* distribution ) {
	int maximum = 0;
	for( int i = 0; i < distribution->row; i++ ) {
		for( int j = 0; j < distribution->column; j++ ) {
			if( distribution->array[ i ][ j ] == 0 ) continue;
			if( j > maximum ) maximum = j;
		}
	}
	return maximum;
}

int getMinimumQV( struct IntDistribution* distribution ) {
	int minimum = 256;
	for( int i = 0; i < distribution->row; i++ ) {
		for( int j = 0; j < distribution->column; j++ ) {
			if( distribution->array[ i ][ j ] == 0 ) continue;
			if( j < minimum ) minimum = j;
		}
	}
	if( minimum < 64 && minimum >= 33) return 33;
	else return 64;
}

