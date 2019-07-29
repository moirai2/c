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
	fprintf( stdout, "Program: Just print outs file size.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: CountUnknownFile INPUT [INPUT2 ..] > OUTPUT\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       INPUT   Input file\n" );
	fprintf( stdout, "       OUTPUT  File size of files.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Output: test/input.txt	192 B\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Note: This program is used by MOIRAI to get filesize of a file.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2011/10/31  Outputs statistical counts of file.\n" );
	fprintf( stdout, "\n" );
}

int main ( int argc, char* argv[] ) {
    if( argc == 2 && strcmp( argv[ 1 ], "-h" ) == 0 ) {
        help();
        return 0;
    }
	for( int i = 1; i < argc; i++ ) fprintf( stdout, "%s\t%ld B\n", argv[ i ], getFileSize( argv[ i ] ) );
	return 0;
}

long int getFileSize( char* filename ) {
	FILE* reader = fopen( filename, "rb" );
	fseek( reader, 0, SEEK_END );
	long int fsize = ftell( reader );
	fclose( reader );
	return fsize;
}
