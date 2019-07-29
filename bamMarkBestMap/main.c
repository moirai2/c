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
	fprintf( stdout, "Program: Mark best map with primary flag and others with secondary for SAM/BAM format file(s).\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: bamMarkBestMap [Options] INPUT > OUTPUT\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       INPUT  SAM/BAM input file(s)\n" );
	fprintf( stdout, "       OUTPUT Properly flagged SAM formated lines\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options: -h  Show help menu\n" );
	fprintf( stdout, "         -O  Output file (default=STDOUT)\n" );
	fprintf( stdout, "         -f  always pick first (for test check purpose)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Note: Assumes lines are sorted by IDs beforehand.\n" );
	fprintf( stdout, "      Some alignment program doesn't mark bad ones with secondary alignment flag.\n" );
	fprintf( stdout, "      This program picks the best mapq of BAM/SAM files as primary alignments.\n" );
	fprintf( stdout, "      If mapq are same, best alignments is picked randomly unless option -f is used.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: 2013/05/27  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2011/12/20  Mark bad ones as secondary flag.\n" );
	fprintf( stdout, "\n" );
	exit( 0 );
}

int main ( int argc, char* argv[] ) {
	struct Interface* interface = newInterface( "fhO:", argc, argv ); // new instance of interface
	if( getBoolean( interface, 'h' ) ) { // h character option
		freeInterface( interface ); // free interface
		help(); // show help message
		return 0; // exit application
	}
	srand( (unsigned) time( NULL ) ); // seed random number
	char* output_file = ( getBoolean( interface, 'O' ) ) ? getString( interface, 'O' ) : "stdout"; // if nothing is specified use STDOUT
	FILE* writer = openWriter( output_file ); // open new writer
	struct StringVector* files = newStringVector(); // file array
	for( int i = 0; i < interface->argument_size; i++ ) getFiles( interface->arguments[ i ], files ); // get files
	int firstLine = 1;
	while( hasMoreStringVector( files ) ) { // go through all input files
		char* file = nextStringVector( files ); // get next input file
		struct SAMReader* reader = newSAMReader( file ); // open reader
		while( readNextSAMs( reader ) ) { // go through all lines
			if( firstLine ) { printSAMHeaders( reader->sequenceReader, writer ); firstLine = 0; } // print header lines from BAM/SAM
			mark_best_map( reader, getBoolean( interface, 'f' ) ); // assign secondary mapping flag to bad quality ones
			printSAMData( reader, writer ); // print SAM data
		}
		freeSAMReader( reader ); // close reader
	}
	freeStringVector( files ); // free all files
	closeHandler( writer, output_file ); // free writer
	freeInterface( interface ); // free interface
	return 0; // exit application
}

void mark_best_map( struct SAMReader* reader, int pick_first ) {
	struct SAMData* rootData = reader->rootData; // assign root data
	struct SAMData* currentData = rootData; // start from root
	int map_count = reader->size; // count number of mappings
	int secondary_count = 0; // count number of secondary mappings
	for( int i = 0; i < map_count; i++ ) { // go through all mappings
		if( ( currentData->flag & 256 ) > 0 ) secondary_count++; // increment
		currentData = currentData->next; // next mapping
	}
	if( secondary_count == map_count - 1 ) return; // flags have been properly numbered.
	int indeces[ 1000 ];
	int maximum_size = 0;
	int maximum_quality = 0;
	// fprintf( stdout, ">%s\n", currentData->qname );
	currentData = rootData; // from start
	for( int index = 0; index < map_count; index++ ) { // go through all mappings
		if( currentData->mapq > maximum_quality ) {
			maximum_size = 0;
			maximum_quality = currentData->mapq;
			indeces[ maximum_size++ ] = index;
		} else if( currentData->mapq == maximum_quality ) {
			indeces[ maximum_size++ ] = index;
		}
		// fprintf( stdout, "BEFORE: %d %d\n", currentData->flag, currentData->mapq );
		currentData = currentData->next; // next mapping
	}
	int maximum_index = 0;
	if( maximum_size == 1 ) { // pick best mapping
		maximum_index = indeces[ 0 ];
		// fprintf( stdout, "PICK: %d\n", maximum_index );
	} else if( pick_first ) {
		maximum_index = indeces[ 0 ]; // first index
	} else { // pick random index
		maximum_index = indeces[ rand() % maximum_size ]; // srand is used to make sure test cases always give same values
		// fprintf( stdout, "RANDOM: %d\n", maximum_index );
	}
	currentData = rootData; // from start
	for( int index = 0; index < map_count; index++ ) { // go through all mappings
		if( index != maximum_index ) { if( ( currentData->flag & 256 ) == 0 ) currentData->flag += 256; } // secondary mapping
		else { if( ( currentData->flag & 256 ) > 0 ) currentData->flag -= 256; } // primary mapping
		// fprintf( stdout, "AFTER: %d %d\n", currentData->flag, currentData->mapq ); // print information
		currentData = currentData->next; // next mapping
	}
}
