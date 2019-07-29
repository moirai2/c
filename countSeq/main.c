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
	fprintf( stdout, "Program: Count sequences of files or files under specified directory.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: countSeq DIR > OUTPUT\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       INPUT  FASTA/FASTA/BAM/SAM file(s)/directory\n" );
	fprintf( stdout, "       OUTPUT Counts per file\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options: -a  Print out sequence counts per arguments/directories specified user (default='per files')\n" );
	fprintf( stdout, "         -h  Show help menu\n" );
	fprintf( stdout, "         -O  Counts per file(default=STDOUT)\n" );
	fprintf( stdout, "         -n  Show total number of sequences only (default='show filename and count')\n" );
	fprintf( stdout, "         -N  Show total number of sequences in this text format using c text format (default='')\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Output: test/input/input.fa	4\n" );
	fprintf( stdout, "        test/input/input.fq	4\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Note: Used for statistical purpose.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: 2013/05/27  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2012/11/29  Print outs counts.\n" );
	fprintf( stdout, "\n" );
	exit( 0 );
}

int main ( int argc, char* argv[] ) {
	struct Interface* interface = newInterface( "ahnN:O:", argc, argv );
	if( getBoolean( interface, 'h' ) ) {
		freeInterface( interface ); // free interface
		help();
		return 0;
	}
	char* totalString = 0;
	if( getBoolean( interface, 'N' ) ) {
		totalString = getString( interface, 'N' );
	}
	long int sequenceTotal = 0;
	int number_only = 0;
	if( getBoolean( interface, 'n' ) ) number_only = 1;
	// handle output
	char* output_file = ( getBoolean( interface, 'O' ) ) ? getString( interface, 'O' ) : "stdout";
	FILE* writer = openWriter( output_file );
	if( getBoolean( interface, 'a' ) ) { // count per argument
		for( int i = 0; i < interface->argument_size; i++ ) {
			struct StringVector* files = newStringVector();
			getFiles( interface->arguments[ i ], files );
			long int sequenceCount = 0;
			while( hasMoreStringVector( files ) ) {
				char* file = nextStringVector( files );
				struct SequenceReader* reader = newSequenceReader( file );
				while( readNextSequence( reader ) ) sequenceCount++;
				sequenceTotal += sequenceCount;
				freeSequenceReader( reader );
			}
			if( number_only != 1 ) fprintf( writer, "%s\t%ld\n", interface->arguments[ i ], sequenceCount );
			else fprintf( writer, "%ld\n", sequenceCount );
			freeStringVector( files );
		}
	} else { // count per files
		struct StringVector* files = newStringVector();
		for( int i = 0; i < interface->argument_size; i++ ) getFiles( interface->arguments[ i ], files );
		while( hasMoreStringVector( files ) ) {
			char* file = nextStringVector( files );
			long int sequenceCount = 0;
			struct SequenceReader* reader = newSequenceReader( file );
			while( readNextSequence( reader ) ) sequenceCount++;
			sequenceTotal += sequenceCount;
			if( number_only != 1 ) fprintf( writer, "%s\t%ld\n", file, sequenceCount );
			freeSequenceReader( reader );
		}
		if( number_only == 1 ) {
			if( totalString != 0 && strlen( totalString ) > 0 ) fprintf( writer, totalString, sequenceTotal );
			else fprintf( writer, "%ld", sequenceTotal );
			fprintf( writer, "\n" );

		}
		freeStringVector( files );
	}
	closeHandler( writer, output_file );
	freeInterface( interface );
	return 0;
}
