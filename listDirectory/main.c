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
	fprintf( stdout, "Program: List files under a directory mostly for learning purpose.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: listDirectory INPUT INPUT2 > OUTPUT\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       Input  file or directory\n" );
	fprintf( stdout, "       OUTPUT  List of files\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options: -h  Show help menu\n" );
	fprintf( stdout, "         -O  Output file (default=STDOUT)\n" );
    fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: 2013/05/29  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2011/11/30  Outputs list of files.\n" );
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
	// In case filename was not specified in arguments
	if( interface->argument_size == 0 ) addArgument( interface, "STDIN" );
	char* outputFile = getBoolean( interface, 'O' ) ? getString( interface, 'O' ) : "STDOUT";
	struct StringVector* list = newStringVector();
	for( int i = 0; i < interface->argument_size; i++ ) {
		char* path = 0;
		path = interface->arguments[ i ];
		while( path[ strlen( path ) -1 ] == '/' ) path[ strlen( path ) - 1 ] = '\0';
		getFiles( path, list );
	}
	FILE* writer = openWriter( outputFile );
	printStringVector( list, writer );
	closeHandler( writer, outputFile );
	freeStringVector( list );
	freeInterface( interface );
	return 0;
}
