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
	fprintf( stdout, "Program: Select X number of sequences from sequence files.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: sampleSeq [Options] INPUT\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       INPUT   Input raw FASTQ/FASTA/SAM/BAM sequence file(s)\n" );
	fprintf( stdout, "       OUTPUT  Output lines in the format of input file\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options: -h  Show help menu\n" );
	fprintf( stdout, "         -O  Output file (default='STDOUT')\n" );
	fprintf( stdout, "         -s  Sample size (default='1000000')\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Note:  Program first count number of sequences and then picks sequence in the right intervals.\n" );
	fprintf( stdout, "       When 0 is used for sample size, all the sequences will be printed.\n" );
	fprintf( stdout, "       Multiple input files will be treated as one big sequence set.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: 2013/05/29  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2013/04/04  Picks sequences in the right intervals.\n" );
	fprintf( stdout, "\n" );
	exit( 0 );
}

// main function
int main ( int argc, char* argv[] ) {
	struct Interface* interface = newInterface( "hO:s:", argc, argv );
	if( getBoolean( interface, 'h' ) ) {
		freeInterface( interface ); // free interface
		help();
		return 0;
	}
	long int sample_size = ( getBoolean( interface, 's' ) ) ? getInt( interface, 's' ) : 1000000;
	// handle output
	char* output_file = ( getBoolean( interface, 'O' ) ) ? getString( interface, 'O' ) : "stdout";
	FILE* writer = openWriter( output_file );
	struct StringVector* files = newStringVector();
	for( int i = 0; i < interface->argument_size; i++ ) getFiles( interface->arguments[ i ], files );
	int print_all_sequences = 0;
	if( sample_size > 0 ) {
		long int sequence_count = 0;
		while( hasMoreStringVector( files ) ) {
			char* file = nextStringVector( files );
			struct SequenceReader* reader = newSequenceReader( file );
			while( readNextSequence( reader ) ) sequence_count++;
			freeSequenceReader( reader );
		}
		resetStringVectorIndex( files );
		if( sample_size > sequence_count ) print_all_sequences = 1;
		else { // print sample
			double span = (double) sample_size / sequence_count;
			long int current_size = 0;
			long double total = 0.0;
			while( hasMoreStringVector( files ) ) {
				char* file = nextStringVector( files );
				struct SequenceReader* reader = newSequenceReader( file );
				while( readNextSequence( reader ) ) {
					total += span;
					if( total < current_size + 1 ) continue;
					current_size++;
					printCurrentSequence( reader, writer );
					if( current_size >= sample_size ) break;
				}
				freeSequenceReader( reader );
			}
		}
	} else print_all_sequences = 1;
	if( print_all_sequences == 1 ) {
		while( hasMoreStringVector( files ) ) {
			char* file = nextStringVector( files );
			struct SequenceReader* reader = newSequenceReader( file );
			while( readNextSequence( reader ) ) printCurrentSequence( reader, writer );
			freeSequenceReader( reader );
		}
	}
	freeStringVector( files );
	closeHandler( writer, output_file );
	freeInterface( interface );
	return 0;
}
