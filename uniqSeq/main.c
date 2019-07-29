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
	fprintf( stdout, "Program: Make unique sequences with counts.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: uniqSeq [Options] INPUT INPUT2 INPUT3 .. > OUTPUT\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       INPUT   Input FASTQ/FASTA/BAM/SAM file/directory\n" );
	fprintf( stdout, "       OUTPUT  A list of unique sequences\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options: -h  Show help menu\n" );
	fprintf( stdout, "         -O  Output file\n" );
	fprintf( stdout, "         -s  Sort output keys (default='do not')\n" );
	fprintf( stdout, "         -z  Hashtable size (default=100,000,000)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Input: >SequenceA\n" );
	fprintf( stdout, "       @akira\n" );
	fprintf( stdout, "       ATATAT\n" );
	fprintf( stdout, "       +\n" );
	fprintf( stdout, "       //////\n" );
	fprintf( stdout, "       @SequenceB\n" );
	fprintf( stdout, "       CGCGCG\n" );
	fprintf( stdout, "       +\n" );
	fprintf( stdout, "       //////\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Output: ATATAT  1\n" );
	fprintf( stdout, "        CGCGCG  1\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Note: Big hahstable size is needed to handle big inputs.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: 2013/05/30  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2011/11/30  Make sequences unique with counts.\n" );
	fprintf( stdout, "\n" );
	exit( 0 );
}

// main function
int main ( int argc, char* argv[] ) {
	struct Interface* interface = newInterface( "hO:sz:", argc, argv );
	if( getBoolean( interface, 'h' ) ) {
		freeInterface( interface ); // free interface
		help();
		return 0;
	}
	// handle output
	char* output_file = ( getBoolean( interface, 'O' ) ) ? getString( interface, 'O' ) : "stdout";
	int hashtable_size = ( getBoolean( interface, 'z' ) ) ? atoi( getString( interface, 'z' ) ) : 100000000;
	FILE* writer = openWriter( output_file );
	if( interface->argument_size == 0 ) addArgument( interface, "stdin" );
	struct StringVector* files = newStringVector();
	for( int i = 0; i < interface->argument_size; i++ ) getFiles( interface->arguments[ i ], files );
	struct StringHashtable* hashtable = newStringHashtable( hashtable_size );
	int size = 0;
	while( hasMoreStringVector( files ) ) {
		char* input_file = nextStringVector( files );
		struct SequenceReader* reader = newSequenceReader( input_file );
		while( readNextSequence( reader ) ) {
			countStringHashtable( hashtable, reader->currentSequence, 1 );
			size++;
		}
		freeSequenceReader( reader );
	}
	if( getBoolean( interface, 's' ) ) {
		struct StringVector* keys = getStringHashtableKeys( hashtable );
		sortStringVector( keys );
		while( hasMoreStringVector( keys ) ) {
			char* key = nextStringVector( keys );
			struct StringHashtableObject* object = getStringHashtableObject( hashtable, key );
			fprintf( writer, "%s\t%d\n", object->key, object->size );
		}
		freeStringVector( keys );
	} else {
		printStringHashtableCount( hashtable, writer );
	}
	// fprintf( stdout, "used %d / %d\n", getStringHashtableKeySize( hashtable ), hashtable_size );
	freeStringHashtable( hashtable );
	freeStringVector( files );
	closeHandler( writer, output_file );
	freeInterface( interface );
	return 0;
}
