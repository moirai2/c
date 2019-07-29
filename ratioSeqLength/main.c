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

#include "main.h"

// Show help information
void help( void ) {
    fprintf( stdout, "\n" );
	fprintf( stdout, "Program: Calculate map rate by sequence length.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: ratioSeqLength DIR1 [ DIR2 DIR3 ..] > OUTPUT\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       DIR     Input FASTA/FASTQ/SAM/BAM format file/directory\n" );
	fprintf( stdout, "       OUTPUT  Map rate by sequence lengths\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options: -h  Show help menu\n" );
	fprintf( stdout, "         -l  label to display for row labels instead of directory paths separated by ',' (default=directory path)\n" );
	fprintf( stdout, "         -O  Output file (default='STDOUT')\n" );
	fprintf( stdout, "         -t  type (diff)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Output: length  test/dir2  test/dir1\n" );
	fprintf( stdout, "        21      1          3\n" );
	fprintf( stdout, "        22      1          3\n" );
	fprintf( stdout, "        23      1          3\n" );
	fprintf( stdout, "        24      1          3\n" );
	fprintf( stdout, "        25      0          0\n" );
	fprintf( stdout, "        26      0          0\n" );
	fprintf( stdout, "        27      0          1\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Note: This is used for quality control for creating summary figure.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: 2013/05/29  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2011/12/05  Print outs map ratio between different directories and lengths.\n" );
	fprintf( stdout, "\n" );
	exit( 0 );
}

int main ( int argc, char* argv[] ) {
	struct Interface* interface = newInterface( "hl:O:t:", argc, argv );
	if( getBoolean( interface, 'h' ) || ( interface->argument_size < 1 ) ) {
		freeInterface( interface ); // free interface
		help();
		return 0;
	}
	// create 2D array with argument size and file size
	struct IntDistribution* distribution = newIntDistribution( MAXIMUM_SEQUENCE_LENGTH, interface->argument_size );
	// set labelYs of 2D array
	for( int i = 0; i < MAXIMUM_SEQUENCE_LENGTH; i++ ) {
		char number[ 100 ];
		sprintf( number, "%d", i );
		addLabelRowDistribution( distribution, number );
	}
	// set labelXs of 2D array
	for( int i = 0; i < interface->argument_size; i++ ) {
		char* directory = interface->arguments[ i ];
		if( directory[ strlen( directory ) - 1 ] == '/' ) { directory[ strlen( directory ) - 1 ] = '\0'; }
		addLabelColumnDistribution( distribution, directory );
	}
	// get values of 2D array
	int minimumLength = MAXIMUM_SEQUENCE_LENGTH;
	int maximumLength = 0;
	for( int i = 0; i < interface->argument_size; i++ ) {
		// directory name
		char* directory = interface->arguments[ i ];
		if( directory[ strlen( directory ) - 1 ] == '/' ) { directory[ strlen( directory ) - 1 ] = '\0'; }
		// filename
		struct StringVector* files = getFiles( interface->arguments[ i ], NULL );
		while( hasMoreStringVector( files ) ) {
			char* file = nextStringVector( files );
			struct SequenceReader* reader = newSequenceReader( file );
			while( readNextSequence( reader ) ) {
				char* sequence = reader->currentSequence;
				int length = strlen( sequence );
				distribution->array[ length ][ i ]++;
				if( length > maximumLength ) maximumLength = length;
				if( length < minimumLength ) minimumLength = length;
			}
			freeSequenceReader( reader );
		}
		freeStringVector( files );
	}
	// diff?
	if( getBoolean( interface, 't' ) ) {
		if( strcmp( getString( interface, 't' ), "diff" ) == 0 ) { // take differences
			for( int i = 0; i < distribution->row; i++ ) {
				int number = distribution->array[ i ][ 0 ];
				for( int j = 1; j < distribution->column; j++ ) {
					int number2 = distribution->array[ i ][ j ];
					distribution->array[ i ][ j ] -= number;
					number = number2;
				}
			}
		}
	}
	if( getBoolean( interface, 'l' ) ) {
		struct StringVector* tokens = split( getString( interface, 'l' ), ',' );
		for( int i = 0; hasMoreStringVector( tokens ); i++ ) {
			char* label = nextStringVector( tokens );
			replaceLabelColumnDistribution( distribution, i, label );
		}
		freeStringVector( tokens );
	}
	// handle output
	char* output_file = ( getBoolean( interface, 'O' ) ) ? getString( interface, 'O' ) : "stdout";
	FILE* writer = openWriter( output_file );
	// Print distribution
	fprintf( writer, "length" );
	for( int i = 0; i < distribution->column; i++ ) fprintf( writer, "\t%s", distribution->labelColumn[ i ] );
	fprintf( writer, "\n" );
	for( int i = minimumLength; i <= maximumLength; i++ ) {
		fprintf( writer, "%s", distribution->labelRow[ i ] );
		for( int j = 0; j < distribution->column; j++ ) fprintf( writer, "\t%d", distribution->array[ i ][ j ] );
		fprintf( writer, "\n" );
	}
	
	closeHandler( writer, output_file );
	freeIntDistribution( distribution );
	freeInterface( interface );
	return 0;
}
