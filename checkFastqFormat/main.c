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

void help( void ) {
    fprintf( stdout, "\n" );
	fprintf( stdout, "Program: Check format of FASTQ sequence file and report if there is something wrong.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: checkFastqFormat INPUT [INPUT2 INPUT3 ..] > OUTPUT\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       INPUT  FASTQ format file\n" );
	fprintf( stdout, "       OUTPUT Text with error messages\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options: -h  Show help menu\n" );
	fprintf( stdout, "         -O  Output error message with '#' and actual FASTQ lines (default=STDOUT)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Outputs: # Lengths of sequence (11bp) and quality (10bp) are not same.\n" );
	fprintf( stdout, "         @Number2\n" );
	fprintf( stdout, "         CCCCCCCCCCC\n" );
	fprintf( stdout, "         +Number2\n" );
	fprintf( stdout, "         //////////\n" );
	fprintf( stdout, "         # IDs of sequence 'Number3' and quality 'Number5' are not same.\n" );
	fprintf( stdout, "         @Number3\n" );
	fprintf( stdout, "         GGGGGGGGGGG\n" );
	fprintf( stdout, "         +Number5\n" );
	fprintf( stdout, "         ///////////\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Note: Very simple program for checking format of FASTQ file.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: 2013/05/27  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2011/11/30  Print outs error messages.\n" );
	fprintf( stdout, "\n" );
	exit( 0 );
}

int main ( int argc, char* argv[] ) {
	struct Interface* interface = newInterface( "hO:", argc, argv );
	if( getBoolean( interface, 'h' ) ) {
		freeInterface( interface ); // free interface
		help();
		return 0;
	}
	// handle output
	char* output_file = ( getBoolean( interface, 'O' ) ) ? getString( interface, 'O' ) : "stdout";
	FILE* writer = openWriter( output_file );
	if( interface->argument_size == 0 ) addArgument( interface, "stdin" );
	struct StringVector* files = newStringVector();
	for( int i = 0; i < interface->argument_size; i++ ) getFiles( interface->arguments[ i ], files );
	long int sequenceCount = 0;
	long int errorCount = 0;
	while( hasMoreStringVector( files ) ) {
		char* input_file = nextStringVector( files );
		struct SequenceReader* reader = newSequenceReader( input_file );
		while( readNextSequence( reader ) ) {
			int error = 0;
			int sequenceLength = strlen( reader->currentSequence );
			int qvLength = strlen( reader->currentQV );
			if( sequenceLength != qvLength ) {
				fprintf( writer, "# Lengths of sequence (%dbp) and quality (%dbp) are not same.\n", sequenceLength, qvLength );
				error++;
			}
			if( strcmp( reader->currentID, reader->currentID2 ) != 0 ) {
				if( strcmp( reader->currentID2, "" ) != 0 ) {
					fprintf( writer, "# IDs of sequence '%s' and quality '%s' are not same.\n", reader->currentID, reader->currentID2 );
					error++;
				}
			}
			if( error > 0 ) {
				printCurrentFASTQ( reader, writer );
				errorCount++;
			}
			sequenceCount++;
		}
		freeSequenceReader( reader );
	}
	closeHandler( writer, output_file );
	fprintf( stdout, "Sequence count = %ld\n", sequenceCount );
	fprintf( stdout, "Sequence error = %ld\n", errorCount );
	freeStringVector( files );
	freeInterface( interface );
	return 0;
}
