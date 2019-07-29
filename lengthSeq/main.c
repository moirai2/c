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

/******************** MAIN ********************/

// Show help information
void help( void ) {
    fprintf( stdout, "\n" );
	fprintf( stdout, "Program: Creates a distribution table of sequence lengths.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: lengthSeq INPUT INPUT2 INPUT3 .. > OUTPUT\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       Input  FASTQ/FASTA/BAM/SAM file/directory\n" );
	fprintf( stdout, "       OUTPUT  A table of length distribution\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options: -h  Show help menu\n" );
	fprintf( stdout, "         -O  Output file\n" );
	fprintf( stdout, "         -s  Count length separately\n" );
    fprintf( stdout, "\n" );
	fprintf( stdout, "Output: 0       0\n" );
	fprintf( stdout, "        1       20\n" );
	fprintf( stdout, "        2       25\n" );
	fprintf( stdout, "        3       1\n" );
	fprintf( stdout, "        4       3\n" );
	fprintf( stdout, "        5       40\n" );
	fprintf( stdout, "        6       22\n" );
    fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: 2013/05/29  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2013/03/13  Outputs table of length distribution.\n" );
	fprintf( stdout, "\n" );
	exit( 0 );
}

// main function
int main ( int argc, char* argv[] ) {
	struct Interface* interface = newInterface( "hO:s", argc, argv );
	if( getBoolean( interface, 'h' ) ) {
		freeInterface( interface ); // free interface
		help();
		return 0;
	}
	// handle output
	char* output_file = ( getBoolean( interface, 'O' ) ) ? getString( interface, 'O' ) : "stdout";
	FILE* writer = openWriter( output_file );
	// handle input
	if( interface->argument_size == 0 ) addArgument( interface, "stdin" );
	struct StringVector* files = newStringVector();
	for( int i = 0; i < interface->argument_size; i++ ) getFiles( interface->arguments[ i ], files );
	// read
	int maximumLength = 0;
	if( getBoolean( interface, 's' ) ) { // count separately
		int fileSize = getStringVectorSize( files );
		int** distributions = 0;
		distributions = malloc( sizeof( int* ) * fileSize );
		int fileIndex = 0;
		while( hasMoreStringVector( files ) ) {
			char* input_file = nextStringVector( files );
			distributions[ fileIndex ] = malloc( sizeof( int ) * MAXIMUM_SEQUENCE_LENGTH );
			for( int i = 0; i < MAXIMUM_SEQUENCE_LENGTH; i++ ) distributions[ fileIndex ][ i ] = 0;
			struct SequenceReader* reader = newSequenceReader( input_file );
			while( readNextSequence( reader ) ) {
				int length = strlen( reader->currentSequence );
				distributions[ fileIndex ][ length ]++;
				if( length > maximumLength ) maximumLength = length;
			}
			freeSequenceReader( reader );
			fileIndex++;
		}
		for( fileIndex = 0; fileIndex < fileSize; fileIndex++ ) {
			char* filename = getStringVector( files, fileIndex );
			fprintf( writer, "\t%s", filename );
		}
		fprintf( writer, "\n" );
		freeStringVector( files );
		for( fileIndex = 0; fileIndex < fileSize; fileIndex++ )
		for( int i = 0; i <= maximumLength; i++ ) {
			fprintf( writer, "%d", i );
			for( fileIndex = 0; fileIndex < fileSize; fileIndex++ ) fprintf( writer, "\t%d", distributions[ fileIndex ][ i ] );
			fprintf( writer, "\n" );
		}
		for( int i = 0; i < fileSize; i++ ) free( distributions[ i ] );
		free( distributions );
	} else { // count as a whole
		int* distribution = 0;
		distribution = malloc( sizeof( int ) * MAXIMUM_SEQUENCE_LENGTH );
		for( int i = 0; i < MAXIMUM_SEQUENCE_LENGTH; i++ ) distribution[ i ] = 0;
		while( hasMoreStringVector( files ) ) {
			char* input_file = nextStringVector( files );
			struct SequenceReader* reader = newSequenceReader( input_file );
			while( readNextSequence( reader ) ) {
				int length = strlen( reader->currentSequence );
				distribution[ length ]++;
				if( length > maximumLength ) maximumLength = length;
			}
			freeSequenceReader( reader );
		}
		freeStringVector( files );
		for( int i = 0; i <= maximumLength; i++ ) fprintf( writer, "%d\t%d\n", i, distribution[ i ] );
		free( distribution );
	}
	closeHandler( writer, output_file );
	freeInterface( interface );
	return 0;
}
