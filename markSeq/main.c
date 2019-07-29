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
	fprintf( stdout, "Program: Mark sequence with a symbol characters.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: markSeq [Options] REFERENCE FASTA > OUTPUT\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       REFERENCE  Reference FASTA file\n" );
	fprintf( stdout, "       FASTA      Original FASTA file\n" );
	fprintf( stdout, "       OUTPUT     Marked FASTA file\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options: -f  Character used for forward match (default='+')\n" );
	fprintf( stdout, "         -h  Show help menu\n" );
	fprintf( stdout, "         -I  Marked FASTA file from previous run (default='none')\n" );
	fprintf( stdout, "         -m  number of mismatch, insertion, deletion allowed for alignment (default='0')\n" );
	fprintf( stdout, "         -O  Output file (default='STDOUT')\n" );
	fprintf( stdout, "         -r  Character used for reverse match (default='-')\n" );
    fprintf( stdout, "\n" );
	fprintf( stdout, "Reference: >reference\n" );
	fprintf( stdout, "           AATTCCGG\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Fasta: >SequenceA\n" );
	fprintf( stdout, "       CGAAATTCCGG\n" );
	fprintf( stdout, "       >SequenceB\n" );
	fprintf( stdout, "       AATTCCGGCGA\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Output: >SequenceA\n" );
	fprintf( stdout, "        ...++++++++\n" );
	fprintf( stdout, "        >SequenceB\n" );
	fprintf( stdout, "        ++++++++...\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Note: By running markSeq multiple times using INPUT, different sequence can be marked hierarchically.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: 2013/05/29  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2012/01/27  Program can handle insertion/deletion for alignment.\n" );
	fprintf( stdout, "         2012/01/15  Outputs marked sequences.\n" );
	fprintf( stdout, "\n" );
	exit( 0 );
}

char* reverse_complement( char* sequence, char* temp ) {
	int length = strlen( sequence );
	for( int i = 0; i < length; i++ ) {
		char c = toupper( sequence[ length - i - 1 ] );
		if( c == 'A' ) temp[ i ] = 'T';
		else if( c == 'C' ) temp[ i ] = 'G';
		else if( c == 'G' ) temp[ i ] = 'C';
		else if( c == 'T' ) temp[ i ] = 'A';
		else if( c == 'N' ) temp[ i ] = 'N';
	}
	temp[ length ] = '\0';
	return temp;
}

// main function
int main ( int argc, char* argv[] ) {
	struct Interface* interface = newInterface( "f:e:hI:m:O:r:", argc, argv );
	if( getBoolean( interface, 'h' ) || ( interface->argument_size < 2 ) ) {
		freeInterface( interface ); // free interface
		help();
		return 0;
	}
	// handle options
	int mismatch = getBoolean( interface, 'm' ) ? getInt( interface,  'm' ) : 0;
	char forward = getBoolean( interface, 'f' ) ? getChar( interface, 'f' ) : '+';
	char reverse = getBoolean( interface, 'r' ) ? getChar( interface, 'r' ) : '-';
	// handle output
	char* output_file = getBoolean( interface, 'O' ) ? getString( interface, 'O' ) : "stdout";
	FILE* writer = openWriter( output_file );
	// get list of references
	struct StringVector* reference_files = newStringVector();
	struct StringVector* references = newStringVector();
	getFiles( interface->arguments[ 0 ], reference_files );
	while( hasMoreStringVector( reference_files ) ) {
		char* reference_file = nextStringVector( reference_files );
		struct SequenceReader* reader = newSequenceReader( reference_file );
		while( readNextSequence( reader ) ) addStringVector( references, reader->currentSequence );
		freeSequenceReader( reader );
	}
	freeStringVector( reference_files );
	// get reverse complement of references
	char temp[ 1000 ];
	struct StringVector* reversed_references = newStringVector();
	while( hasMoreStringVector( references ) ) {
		char* reference = nextStringVector( references );
		addStringVector( reversed_references, reverse_complement( reference, temp ) );
	}
	resetStringVectorIndex( references );
	// get list of fasta files
	struct StringVector* fasta_files = newStringVector();
	getFiles( interface->arguments[ 1 ], fasta_files );
	// list of input files
	int exists_input = FALSE;
	struct StringVector* input_files = newStringVector();
	if( getBoolean( interface, 'I' ) ) {
		getFiles( getString( interface, 'I' ), input_files );
		exists_input = TRUE;
	} else {
		while( hasMoreStringVector( fasta_files ) ) {
			char* fasta_file = nextStringVector( fasta_files );
			addStringVector( input_files, fasta_file );
		}
		resetStringVectorIndex( fasta_files );
	}
	struct DotMatrix* matrix = newDotMatrix();
	while( hasMoreStringVector( fasta_files ) ) {
		char* fasta_file = nextStringVector( fasta_files );
		char* input_file = nextStringVector( input_files );
		struct SequenceReader* reader1 = newSequenceReader( fasta_file );
		struct SequenceReader* reader2 = newSequenceReader( input_file );
		while( readNextSequence( reader2 ) ) {
			readNextSequence( reader1 );
			if( exists_input == FALSE ) for( int i = 0; i < strlen( reader2->currentSequence ); i++ ) reader2->currentSequence[ i ] = '.'; // reset to empty lines
			char* sequence = reader1->currentSequence;
			// forward
			if( hasMoreStringVector( references ) ) {
				char* reference = nextStringVector( references );
				initializeDotMatrix( matrix, sequence, reference, mismatch );
				calculateDotMatrix( matrix );
				for( int start_point = 0; tracebackDotMatrix( matrix, start_point ); start_point = matrix->query_end + mismatch ) {
					int length = strlen( matrix->traceback_match );
					int deletion = 0;
					for( int i = 0; i < length; i++ ) { // mark forward
						if( matrix->traceback_sequence[ i ] == '-' ) { // deletion
							deletion++;
						} else if( matrix->traceback_reference[ i ] == '-' ) { // insertion
							continue;
						} else if( matrix->traceback_match[ i ] == '|' ) { // match
							reader2->currentSequence[ i + matrix->query_start - 1 - deletion ] = forward;
						}
					}
				}
			}
			resetStringVectorIndex( references );
			// reverse
			if( hasMoreStringVector( reversed_references ) ) {
				char* reversed_reference = nextStringVector( reversed_references );
				initializeDotMatrix( matrix, sequence, reversed_reference, mismatch );
				calculateDotMatrix( matrix );
				for( int start_point = 0; tracebackDotMatrix( matrix, start_point ); start_point = matrix->query_end + mismatch ) {
					int length = strlen( matrix->traceback_match );
					int deletion = 0;
					for( int i = 0; i < length; i++ ) { // mark reverse
						if( matrix->traceback_sequence[ i ] == '-' ) { // deletion
							deletion++;
						} else if( matrix->traceback_reference[ i ] == '-' ) { // insertion
							continue;
						} else if( matrix->traceback_match[ i ] == '|' ) { // match
							reader2->currentSequence[ i + matrix->query_start - 1 - deletion ] = reverse;
						}
					}
				}
			}
			resetStringVectorIndex( reversed_references );
			printCurrentFASTA( reader2, writer );
		}
		freeSequenceReader( reader1 );
		freeSequenceReader( reader2 );
	}
	freeDotMatrix( matrix );
	freeStringVector( input_files );
	freeStringVector( fasta_files );
	freeStringVector( references );
	freeStringVector( reversed_references );
	closeHandler( writer, output_file );
	freeInterface( interface );
	return 0;
}
