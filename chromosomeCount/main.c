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
	fprintf( stdout, "Program: Count primary and secondary counts per chromosome of SAM/BAM files.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: chromosomeCount INPUT > OUTPUT\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       INPUT  BAM/SAM format file\n" );
	fprintf( stdout, "       OUTPUT Chromosome, primary and secondary counts\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options: -h  Show help menu\n" );
	fprintf( stdout, "         -O  Output chromosome and primary and secondary counts (default=STDOUT)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Outputs: chromosome	primary	   secondary\n" );
	fprintf( stdout, "         chr1	        53	       24\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Note: Very simple program for quality control.\n" );
	fprintf( stdout, "      Results can be used for making a table.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: 2013/05/27  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2011/12/07  Print outs satistics.\n" );
	fprintf( stdout, "\n" );
	exit( 0 );
}

int main ( int argc, char* argv[] ) {
	struct Interface* interface = newInterface( "hO:", argc, argv ); // new instance of interface
	if( getBoolean( interface, 'h' ) ) { // h character option
		freeInterface( interface ); // free interface
		help(); // show help message
		return 0; // exit application
	}
	struct Chromosome* chromosomes = newChromosome( 100 ); // prepare hashtable with 100 chromosomes
	char* output_file = ( getBoolean( interface, 'O' ) ) ? getString( interface, 'O' ) : "stdout"; // if nothing is specified use STDOUT
	FILE* writer = openWriter( output_file ); // open new writer
	struct StringVector* files = newStringVector(); // file array
	for( int i = 0; i < interface->argument_size; i++ ) getFiles( interface->arguments[ i ], files ); // get files
	while( hasMoreStringVector( files ) ) { // go through all input files
		char* file = nextStringVector( files ); // get next input file
		struct SequenceReader* reader = newSequenceReader( file ); // open reader
		while( readNextSequence( reader ) ) { // go through all lines
			int flag = reader->flag; // get current flag
			struct ChromosomeObject* chromosome = getChromosomeObject( chromosomes, reader->rname ); //get
			if( chromosome == NULL ) { // not found
				putChromosome( chromosomes, reader->rname ); // remember
				chromosome = getChromosomeObject( chromosomes, reader->rname ); // get
			}
			if( ( flag & 256 ) > 0 ) chromosome->secondary_mapping++; // secondaryary alignment
			else chromosome->primary_mapping++; // primary alignment
			if( ( flag & 16 ) > 0 ) chromosome->negative_strand++; // negative direction
			else chromosome->positive_strand++; // positive direction
		}
		freeSequenceReader( reader ); // close reader
	}
	struct StringVector* keys = getChromosomeKeys( chromosomes ); // get keys
	sortStringVector( keys ); // sort keys
	fprintf( writer, "chromosome\tprimary\tsecondary\n" ); // print label
	while( hasMoreStringVector( keys ) ) { // go through all keys
		char* key = nextStringVector( keys ); // get next key
		struct ChromosomeObject* chromosome = getChromosomeObject( chromosomes, key ); // get chromosome with key
		fprintf( writer, "%s\t%d\t%d\n", key, chromosome->primary_mapping, chromosome->secondary_mapping ); // print information
	}
	freeStringVector( keys ); // free string array
	freeChromosome( chromosomes ); // free all chromosomes
	freeStringVector( files ); // free all files
	closeHandler( writer, output_file ); // free writer
	freeInterface( interface ); // free interface
	return 0; // exit application
}
