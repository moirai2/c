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
	fprintf( stdout, "Program: Select sequences by specifying sequence length.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: selectSeqLength < INPUT > OUTPUT\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       INPUT   FASTQ, FASTA, SAM, BAM, UNIQ files\n" );
	fprintf( stdout, "       OUTPUT  A list of unique sequences with summed total of others\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options: -h  Show help menu\n" );
	fprintf( stdout, "         -n  Size of length to select (default='20').\n" );
	fprintf( stdout, "         -O  Output file (default='STDOUT')\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Note: It's like grepping sequences with sequence length.\n" );
	fprintf( stdout, "      You can select by length like 15-20.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: 2013/05/29  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2011/12/05  Select sequences by sequence length.\n" );
	fprintf( stdout, "\n" );
	exit( 0 );
}

int main ( int argc, char* argv[] ) {
	struct Interface* interface = newInterface( "hO:n:", argc, argv );
	if( getBoolean( interface, 'h' ) ) {
		freeInterface( interface ); // free interface
		help();
		return 0;
	}
	long int totalCount = 0;
	long int matchCount = 0;
	int minimumLength = 0;
	int maximumLength = 0;
	struct StringVector* tokens = split( getString( interface, 'n' ), '-' );
	for( int i = 0; hasMoreStringVector( tokens ); i++ ) {
		char* string = nextStringVector( tokens );
		if( i == 0 ) {
			minimumLength = atoi( string );
			maximumLength = minimumLength;
		} else if( i == 1 ) {
			maximumLength = atoi( string );
		}
	}
	freeStringVector( tokens );
	// handle output
	char* output_file = ( getBoolean( interface, 'O' ) ) ? getString( interface, 'O' ) : "stdout";
	if( interface->argument_size == 0 ) addArgument( interface, "stdin" );
	struct StringVector* files = newStringVector();
	for( int i = 0; i < interface->argument_size; i++ ) getFiles( interface->arguments[ i ], files );
	FILE* writer = openWriter( output_file );
	while( hasMoreStringVector( files ) ) {
		char* input_file = nextStringVector( files );
		struct SequenceReader* reader = newSequenceReader( input_file );
		while( readNextSequence( reader ) ) {
			totalCount++;
			int length = strlen( reader->currentSequence );
			if( length >= minimumLength && length <= maximumLength ) {
				printCurrentSequence( reader, writer );
				matchCount++;
			}
		}
		freeSequenceReader( reader );
	}
	fprintf( stderr, "Extracted %ld sequences out of %ld.\n", matchCount, totalCount );
	freeStringVector( files );
	closeHandler( writer, output_file );
	freeInterface( interface );
	return 0;
}
