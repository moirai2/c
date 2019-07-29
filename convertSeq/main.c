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
	fprintf( stdout, "Program: Convert sequence of FASTA/FASTQ/SAM/BAM from char1 to char2.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: convertSeq FROM TO < INPUT > OUTPUT\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       INPUT  FASTA/FASTQ/SAM/BAM format file\n" );
	fprintf( stdout, "       OUTPUT FASTA/FASTQ/SAM/BAM format file\n" );
	fprintf( stdout, "       FROM   Character to change from (char1)\n" );
	fprintf( stdout, "       TO     Character to change to (char2)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options: -h  Show help menu\n" );
	fprintf( stdout, "         -I  Input sequence file (default=STDIN)\n" );
	fprintf( stdout, "         -O  Output sequence file (default=STDOUT)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Note: Used to change from '.' to 'N', for example.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: 2013/05/27  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2012/01/19  Print outs converted sequences.\n" );
	fprintf( stdout, "\n" );
	exit( 0 );
}

int main ( int argc, char* argv[] ) {
	struct Interface* interface = newInterface( "hI:O:", argc, argv );
	if( getBoolean( interface, 'h' ) || ( interface->argument_size < 2 ) ) {
		freeInterface( interface ); // free interface
		help(); //show help
		return 0; // done
	}
	// handle output
	char from = interface->arguments[ 0 ][ 0 ]; // from character
	char to   = interface->arguments[ 1 ][ 0 ]; // to character
	char* input_file  = ( getBoolean( interface, 'I' ) ) ? getString( interface, 'I' ) : "stdin"; // stdin
	char* output_file = ( getBoolean( interface, 'O' ) ) ? getString( interface, 'O' ) : "stdout"; // stdout
	FILE* writer = openWriter( output_file ); // open writer
	struct StringVector* files = newStringVector(); // vector of strings
	getFiles( input_file, files ); // expand files
	while( hasMoreStringVector( files ) ) { // has more input files
		char* input_file = nextStringVector( files ); // next input file
		struct SequenceReader* reader = newSequenceReader( input_file ); // initialize reader
		while( readNextSequence( reader ) ) { // go through sequences
			int length = strlen( reader->currentSequence ); // get sequence length
			for( int i = 0; i < length; i++ ) if( reader->currentSequence[ i ] == from ) reader->currentSequence[ i ] = to; // convert
			printCurrentSequence( reader, writer ); // print sequence
		}
		freeSequenceReader( reader ); // close reader
	}
	freeStringVector( files ); // free
	closeHandler( writer, output_file ); // close writer
	freeInterface( interface ); // free
	return 0;
}
