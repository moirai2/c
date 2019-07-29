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
	fprintf( stdout, "Program: Match and select paired sequences by comparing against original input files for paired-mapping like tophat.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: matchPairedEndSeq ORIGIN5 ORIGIN3 FILTER5 FILTER3 OUTPUT5 OUTPUT3\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       ORIGIN5  Original read1 sequence file\n" );
	fprintf( stdout, "       ORIGIN3  Original read2 sequence file\n" );
	fprintf( stdout, "       FILTER5  Filtered read1 sequence file\n" );
	fprintf( stdout, "       FILTER3  Filtered read2 sequence file\n" );
	fprintf( stdout, "       OUTPUT5  Paired read1 output sequence file\n" );
	fprintf( stdout, "       OUTPUT3  Paired read2 output sequence file\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options: -h  Show help menu\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Note: Make sure original and filtered sequences are in right order.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: 2013/05/29  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2011/12/06  Program go through original sequences and if IDs match, paired sequences are written to output files.\n" );
	fprintf( stdout, "\n" );
	exit( 0 );
}

int main ( int argc, char* argv[] ) {
	struct Interface* interface = newInterface( "h", argc, argv );
	if( getBoolean( interface, 'h' ) || ( interface->argument_size < 6 ) ) {
		freeInterface( interface ); // free interface
		help();
		return 0;
	}
	struct SequenceReader* original5 = newSequenceReader( interface->arguments[ 0 ] );
	struct SequenceReader* original3 = newSequenceReader( interface->arguments[ 1 ] );
	struct SequenceReader* filtered5 = newSequenceReader( interface->arguments[ 2 ] );
	struct SequenceReader* filtered3 = newSequenceReader( interface->arguments[ 3 ] );
	FILE* writer5 = openWriter( interface->arguments[ 4 ] );
	FILE* writer3 = openWriter( interface->arguments[ 5 ] );
	readNextSequence( filtered5 ); // read first sequence
	readNextSequence( filtered3 ); // read first sequence
	char* filtered_id5 = filtered5->currentID;
	char* filtered_id3 = filtered3->currentID;
	while( readNextSequence( original5 ) ) {
		readNextSequence( original3 ); // read 3prime original sequence too
		char* id5 = original5->currentID; // get current 5prime ID
		char* id3 = original3->currentID; // get current 3prime ID
		// if( strcmp( id5, id3 ) != 0 ) { fprintf( stderr, "ERROR: Original IDs don't match.\n" ); break; } // original files contained a problem
		int match5 = checkSequenceIDs( filtered_id5, id5 ); // match with filtered ID?
		int match3 = checkSequenceIDs( filtered_id3, id3 ); // match with filtered ID?
		if( match5 == 1 && match3 == 1 ) { // both matched!!
			printCurrentSequence( filtered5, writer5 ); // print 5prime
			printCurrentSequence( filtered3, writer3 ); // print 3prime
		}
		if( match5 == 1 && readNextSequence( filtered5 ) == 0 ) break; // end of 5prime filtered
		if( match3 == 1 && readNextSequence( filtered3 ) == 0 ) break; // end of 3prime filtered
	}
	// free everything
	freeSequenceReader( original5 );
	freeSequenceReader( original3 );
	freeSequenceReader( filtered5 );
	freeSequenceReader( filtered3 );
	closeHandler( writer5, interface->arguments[ 4 ] );
	closeHandler( writer3, interface->arguments[ 5 ] );
	freeInterface( interface );
	return 0;
}
