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
	fprintf( stdout, "Program: Return match results of input and reference sequences like BLAST output using dot matrix.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: dotMatrix REFERENCE INPUT > OUTPUT\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       INPUT  Input FASTQ/FASTA/BAM/SAM file/directory\n" );
	fprintf( stdout, "       OUTPUT Alignment\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options: -h  Show help menu\n" );
	fprintf( stdout, "         -e  number of error (indel+mismatch) allowed (default=0)\n" );
	fprintf( stdout, "         -O  Output alignments and match/mismatch and indel counts\n" );
    fprintf( stdout, "\n" );
	fprintf( stdout, "Outputs: >sequenceA\n" );
	fprintf( stdout, "         Query 1 CAATTCAGTTA 11\n" );
	fprintf( stdout, "                  ||||||||||\n" );
	fprintf( stdout, "         Sbjct 1 GAATTCAGTTA 11\n" );
	fprintf( stdout, "         10 match, 1 mismatch, 0 insertion, 0 deletion\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Note: Error values can be double (ratio) instead of int.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: 2013/05/27  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2012/01/16  Print outs alignments.\n" );
	fprintf( stdout, "\n" );
	exit( 0 );
}

// main function
int main ( int argc, char* argv[] ) {
	struct Interface* interface = newInterface( "e:hO:", argc, argv );
	if( getBoolean( interface, 'h' ) || ( interface->argument_size < 1 ) ) {
		freeInterface( interface ); // free interface
		help();
		return 0;
	}
	// handle output
	int error_allowed = ( getBoolean( interface, 'e' ) ) ? getInt( interface, 'e' ) : 0;
	char* output_file = ( getBoolean( interface, 'O' ) ) ? getString( interface, 'O' ) : "stdout";
	FILE* writer = openWriter( output_file );
	if( interface->argument_size == 0 ) addArgument( interface, "stdin" );
	struct StringVector* reference_files = newStringVector();
	struct StringVector* references = newStringVector();
	getFiles( interface->arguments[ 0 ], reference_files );
	int maximum_reference_length = 0; // Stores maximum reference sequence length
	while( hasMoreStringVector( reference_files ) ) {
		char* reference_file = nextStringVector( reference_files );
		struct SequenceReader* reader = newSequenceReader( reference_file );
		while( readNextSequence( reader ) ) {
			int length = strlen( reader->currentSequence );
			if( length > maximum_reference_length ) maximum_reference_length = length;
			addStringVector( references, reader->currentSequence );
		}
		freeSequenceReader( reader );
	}
	int reference_size = getStringVectorSize( references );
	freeStringVector( reference_files );
	struct DotMatrix* matrix = newDotMatrix();
	struct StringVector* input_files = newStringVector();
	for( int i = 1; i < interface->argument_size; i++ ) getFiles( interface->arguments[ i ], input_files );
	while( hasMoreStringVector( input_files ) ) {
		char* input_file = nextStringVector( input_files );
		struct SequenceReader* reader = newSequenceReader( input_file );
		while( readNextSequence( reader ) ) {
			for( int index = 0; index < reference_size; index++ ) {
				char* reference = getStringVector( references, index );
				initializeDotMatrix( matrix, reader->currentSequence, reference, error_allowed );
				calculateDotMatrix( matrix );
				if( tracebackDotMatrix( matrix, 0 ) ) {
					fprintf( writer, ">%s\n", reader->currentID );
					// printDotMatrixArray( matrix, writer );
					// printDotMatrixPointer( matrix, writer );
					// printDotMatrixError( matrix, writer );
					printDotMatrix( matrix, writer );
				}
			}
		}
		freeSequenceReader( reader );
	}
	freeDotMatrix( matrix );
	freeStringVector( references );
	freeStringVector( input_files );
	closeHandler( writer, output_file );
	freeInterface( interface );
	return 0;
}
