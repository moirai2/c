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
	fprintf( stdout, "Program: Read in sequence file and print out top X occuring sequences by sampling.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: sampleTopSeq [Options] INPUT > OUTPUT\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       INPUT   FASTQ, FASTA, SAM, BAM, UNIQ files\n" );
	fprintf( stdout, "       OUTPUT  A list of unique sequences with summed total of others\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options: -h  Show help menu\n" );
	fprintf( stdout, "         -r  Print out estimated redundancy value from a sample size.\n" );
	fprintf( stdout, "         -R  Print out estimated redundancy value together with specified label in C text format (default='').\n" );
	fprintf( stdout, "         -n  Top X sequences (default='50')\n" );
	fprintf( stdout, "         -s  Sampling size = hashtable size (default='100,000')\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Note: First sequences specified in sample size will be read and count only those sequences from sampling step.\n" );
	fprintf( stdout, "      redundancy = sequence / unique.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: 2013/05/29  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2013/04/27  Picks highly found sequences.\n" );
	fprintf( stdout, "\n" );
	exit( 0 );
}

// main function
int main ( int argc, char* argv[] ) {
	struct Interface* interface = newInterface( "hn:O:rR:s:", argc, argv );
	if( getBoolean( interface, 'h' ) ) {
		freeInterface( interface ); // free interface
		help();
		return 0;
	}
	// handle output
	char* output_file  = ( getBoolean( interface, 'O' ) ) ? getString( interface, 'O' ) : "stdout";
	int top_size       = ( getBoolean( interface, 'n' ) ) ?    getInt( interface, 'n' ) : 50;
	int hashtable_size = ( getBoolean( interface, 's' ) ) ?    getInt( interface, 's' ) : 100000;
	FILE* writer = openWriter( output_file );
	if( interface->argument_size == 0 ) addArgument( interface, "stdin" );
	struct StringVector* files = newStringVector();
	for( int i = 0; i < interface->argument_size; i++ ) getFiles( interface->arguments[ i ], files );
	struct StringHashtable* hashtable = newStringHashtable( hashtable_size );
	int sequence_count = 0;
	while( hasMoreStringVector( files ) ) {
        char* input_file = nextStringVector( files );
        struct SequenceReader* reader = newSequenceReader( input_file );
		while( readNextSequence( reader ) ) {
			if( sequence_count < hashtable_size ) {
				countStringHashtable( hashtable, reader->currentSequence, reader->count );
			} else {
				struct StringHashtableObject* object = getStringHashtableObject( hashtable, reader->currentSequence );
				if( object != NULL ) object->size += reader->count;
			}
			sequence_count++;
		}
		freeSequenceReader( reader );
	}
	if( getBoolean( interface, 'r' ) ) { // print out number of redundancy
		int unique_count = getStringHashtableKeySize( hashtable );
		int total_count  = ( sequence_count > hashtable_size ) ? hashtable_size : sequence_count;
		fprintf( writer, "%.2f", (double) total_count / (double) unique_count );
	} else if( getBoolean( interface, 'R' ) ) { // print out number of redundancy
		char* label = getString( interface, 'R' );
		int unique_count = getStringHashtableKeySize( hashtable );
		int total_count  = ( sequence_count > hashtable_size ) ? hashtable_size : sequence_count;
		fprintf( writer, label, (double) total_count / (double) unique_count );
		fprintf( writer, "\n" );
	} else {
		struct StringVector* keys = getStringHashtableKeys( hashtable );
		sortStringVectorByCount( hashtable, keys );
		int other_count = 0; // number of sequences other than top X
		for( int i = 0; hasMoreStringVector( keys ); i++ ) {
			char* key = nextStringVector( keys );
			struct StringHashtableObject* object = getStringHashtableObject( hashtable, key );
			if( i < top_size ) fprintf( writer, "%s\t%d\n", object->key, object->size );
			else other_count += object->size;
		}
		freeStringVector( keys );
		fprintf( writer, "others\t%d\n", other_count );
		fprintf( stderr, "used %d / %d\n", getStringHashtableKeySize( hashtable ), hashtable_size );
	}
	freeStringHashtable( hashtable );
	freeStringVector( files );
	closeHandler( writer, output_file );
	freeInterface( interface );
	return 0;
}
