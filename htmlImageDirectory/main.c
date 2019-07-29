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
	fprintf( stdout, "Program: List image files under a directory in HTML format.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: htmlImageDirectory [Options] INPUT [INPUT2] > OUTPUT\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       INPUT  Input image directory(s)\n" );
	fprintf( stdout, "       OUTPUT HTML lines showing image\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options -d  delim (default=none)\n" );
	fprintf( stdout, "        -f  Show filename under image\n" );
	fprintf( stdout, "        -h  Show help menu\n" );
	fprintf( stdout, "        -H  set image width (default=none)\n" );
	fprintf( stdout, "        -l  link to original image\n" );
	fprintf( stdout, "        -O  Output HTML file (default=STDOUT)\n" );
	fprintf( stdout, "        -r  root directory of HTML (default=current)\n" );
	fprintf( stdout, "        -W  set image width (default=none)\n" );
    fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: 2013/05/29  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2011/11/11  Outputs HTML lines linking to input images.\n" );
	fprintf( stdout, "\n" );
	exit( 0 );
}

// main function
int main ( int argc, char* argv[] ) {
	struct Interface* interface = newInterface( "d:hH:flO:r:W:", argc, argv );
	if( getBoolean( interface, 'h' ) ) {
		freeInterface( interface ); // free interface
		help();
		return 0;
	}
	char* delim = getString( interface, 'd' );
	int showLink = getBoolean ( interface, 'l' );
	int showFilename = getBoolean ( interface, 'f' );
	char* width = getString( interface, 'W' );
	char* height = getString( interface, 'H' );
	if( getBoolean( interface, 'r' ) ) {
		if( chdir( getString( interface, 'r' ) ) == -1 ) fprintf( stderr, "Couldn't change directory." );
	}
	// In case filename was not specified in arguments
	if( interface->argument_size == 0 ) addArgument( interface, "STDIN" );
	char* outputFile = getBoolean( interface, 'O' ) ? getString( interface, 'O' ) : "STDOUT";
	struct StringVector* list = newStringVector();
	struct RegularExpression* suffix = newRegularExpression( "\\.png$|\\.gif$|\\.je?pg$" );
	for( int i = 0; i < interface->argument_size; i++ ) getFilesRecursively( interface->arguments[ i ], -1, suffix, NULL, NULL, list );
	char line[ MAX_LINE ];
	FILE* writer = openWriter( outputFile );
	for( int i = 0; hasMoreStringVector( list ); i++ ) {
		char* path = nextStringVector( list );
		while( path[ strlen( path ) -1 ] == '/' ) path[ strlen( path ) - 1 ] = '\0';
		strcpy( line, "" );
		if( ( i > 0 ) && ( delim != NULL ) ) {
			strcat( line, delim );
		}
		if( showLink ) {
			strcat( line, "<A HREF=\"" );
			strcat( line, path );
			strcat( line, "\">" );
		}
		strcat( line, "<IMG" );
		if( width != NULL ) {
			strcat( line, " width=\"" );
			strcat( line, width );
			strcat( line, "\"" );
		}
		if( height != NULL ) {
			strcat( line, " height=\"" );
			strcat( line, height );
			strcat( line, "\"" );
		}
		strcat( line, " SRC=\"" );
		strcat( line, path );
		strcat( line, "\">" );
		if( showFilename ) {
			strcat( line, "<BR>" );
			strcat( line, path );
		}
		if( showLink ) {
			strcat( line, "</A>" );
		}
		fprintf( writer, "%s\n", line );
	}
	closeHandler( writer, outputFile );
	freeRegularExpression( suffix );
	freeStringVector( list );
	freeInterface( interface );
	return 0;
}
