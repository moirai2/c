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
	fprintf( stdout, "Program: Convert BAM/SAM file to list of start positions.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: bamStartPosition [Options] INPUT > OUTPUT\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       INPUT  BAM input file(s)\n" );
	fprintf( stdout, "       OUTPUT insert size distribution\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options: -h  Show help menu\n" );
	fprintf( stdout, "         -O  Output with chromosome, strand, position, and count (default=STDOUT)\n" );
	fprintf( stdout, "\n" );
    fprintf( stdout, "Output: chr1    +       4820351 1\n" );
    fprintf( stdout, "        chr1    -       4818700 1\n" );
    fprintf( stdout, "        chr1    -       4818700 1\n" );
    fprintf( stdout, "\n" );
	fprintf( stdout, "Note: Very simple program which prints outs the first positions.\n" );
	fprintf( stdout, "      Count is always 1, since this program doesn't sum up the counts.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: 2013/05/27  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2012/08/08  Print outs start positions.\n" );
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
	char* output_file = ( getBoolean( interface, 'O' ) ) ? getString( interface, 'O' ) : "stdout"; // if nothing is specified use STDOUT
	FILE* writer = openWriter( output_file ); // open new writer
	struct StringVector* files = newStringVector(); // file array
	for( int i = 0; i < interface->argument_size; i++ ) getFiles( interface->arguments[ i ], files ); // get files
	while( hasMoreStringVector( files ) ) { // go through all input files
		char* file = nextStringVector( files ); // get next input file
		struct SequenceReader* reader = newSequenceReader( file ); // open reader
		while( readNextSequence( reader ) ) { // go through all lines
			char direction = ( ( reader->flag & 16 ) > 0 ) ? '-' : '+';
			fprintf( writer, "%s\t%c\t%d\t1\n", reader->rname, direction, getStartLocation( reader ) );
		}
		freeSequenceReader( reader ); // close reader
	}
	freeStringVector( files ); // free all files
	closeHandler( writer, output_file ); // free writer
	freeInterface( interface ); // free interface
	return 0; // exit application
}
