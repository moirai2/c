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
	fprintf( stdout, "Program: Open zipped or unzipped sequence files.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: openSeq [Options] INPUT [INPUT2 ..] > OUTPUT\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       INPUT   Input zipped or unzipped FASTQ/FASTA/SAM/BAM sequence file(s)\n" );
	fprintf( stdout, "       OUTPUT  Output lines read\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options: -h  Show help menu\n" );
	fprintf( stdout, "         -O  Output file (default=STDOUT)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Note: Program looks at the suffix of a file and judges if it needs special treatments.\n" );
	fprintf( stdout, "      Program is used before commands which doesn't accept gzipped/bzipped files for input.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: 2013/05/29  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2011/10/27  Program print outs sequences.\n" );
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
	char* output_file = getBoolean( interface, 'O' ) ? getString( interface, 'O' ) : "STDOUT";
	// In case filename was not specified in arguments
	if( interface->argument_size == 0 ) addArgument( interface, "STDIN" );
	FILE* writer = openWriter( output_file );
	struct StringVector* files = newStringVector();
	for( int i = 0; i < interface->argument_size; i++ ) getFiles( interface->arguments[ i ], files );
	char line[ MAX_LINE ];
	while( hasMoreStringVector( files ) ) {
		char* path = nextStringVector( files );
		FILE* reader = openReader( path );
		while( fgets( line, MAX_LINE, reader ) ) fprintf( writer, "%s", line );
		closeHandler( reader, path );
	}
	freeStringVector( files );
	closeHandler( writer, output_file );
	freeInterface( interface );
	return 0;
}
