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
	fprintf( stdout, "Program: Sort text file line by line for leaning purpose.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: sortText -O OUTPUT INPUT\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       INPUT   Input FASTA/FASTQ/SAM/BAM format file\n" );
	fprintf( stdout, "       OUTPUT  Sorted text line(s)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options: -h  Show help menu\n" );
	fprintf( stdout, "         -O  Output file (default='STDOUT')\n" );
	fprintf( stdout, "         -u  print out with unique sequences and cunts (default=STDOUT)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: 2013/05/30  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2012/05/11  Sort text lines for learning purpose.\n" );
	fprintf( stdout, "\n" );
	exit( 0 );
}

// main function
int main ( int argc, char* argv[] ) {
	struct Interface* interface = newInterface( "hO:", argc, argv );
	if( getBoolean( interface, 'h' ) ) {
		freeInterface( interface ); // free interface
		help();
		return 0;
	}
	if( interface->argument_size == 0 ) addArgument( interface, "STDIN" );
	char* outputFile = getBoolean( interface, 'O' ) ? getString( interface, 'O' ) : "STDOUT";
	struct StringVector* files = newStringVector();
	FILE* writer = openWriter( outputFile );
	struct StringVector* vector = newStringVector();
	for( int i = 0; i < interface->argument_size; i++ ) getFiles( interface->arguments[ i ], files );
	while( hasMoreStringVector( files ) ) {
		char* input_file = nextStringVector( files );
		loadStringVector( vector, input_file );
	}
	freeStringVector( files );
	sortStringVector( vector );
	printStringVector( vector, writer );
	freeStringVector( vector );
	closeHandler( writer, outputFile );
	freeInterface( interface );
	return 0;
}
