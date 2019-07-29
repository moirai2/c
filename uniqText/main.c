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
	fprintf( stdout, "Program: Make text unique for learning purpose.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: uniqText [Options] INPUT\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       INPUT   Original input text file(s)\n" );
	fprintf( stdout, "       OUTPUT  Unique output text file(s).t\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options: -h  Show help menu\n" );
	fprintf( stdout, "         -O  Output file (default='STDOUT')\n" );
	fprintf( stdout, "         -c  Show counts in the second column (default='none')\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Input: Akira\n" );
	fprintf( stdout, "       Yukiko\n" );
	fprintf( stdout, "       Akira\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Output: Akira\n" );
	fprintf( stdout, "        Yukiko\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Output(-c): Akira   2\n" );
	fprintf( stdout, "            Yukiko  1\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: 2013/05/30  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2011/11/23  Make text line unique with counts.\n" );
	fprintf( stdout, "\n" );
	exit( 0 );
}

// main function
int main ( int argc, char* argv[] ) {
	struct Interface* interface = newInterface( "chO:", argc, argv );
	if( getBoolean( interface, 'h' ) ) {
		freeInterface( interface ); // free interface
		help();
		return 0;
	}
	if( interface->argument_size == 0 ) addArgument( interface, "STDIN" );
	char* outputFile = getBoolean( interface, 'O' ) ? getString( interface, 'O' ) : "STDOUT";
	FILE* writer = openWriter( outputFile );
	struct StringHashtable* hashtable = newStringHashtable( 100 );
	char line[ MAX_LINE ];
	for( int i = 0; i < interface->argument_size; i++ ) {
		FILE* reader = openReader( interface->arguments[ i ] );
		while( fgets( line, MAX_LINE, reader ) ) {
			chomp( line );
			countStringHashtable( hashtable, line, 1 );
		}
		closeHandler( reader, interface->arguments[ i ] );
	}
	if( getBoolean( interface, 'c' ) ) {
		struct StringVector* vector = getStringHashtableKeys( hashtable );
		sortStringVector( vector );
		while( hasMoreStringVector( vector ) ) {
			char* key = nextStringVector( vector );
			int size = getStringHashtableCount( hashtable, key );
			fprintf( writer, "%s\t%d\n", key, size );
		}
		freeStringVector( vector );
		freeStringHashtable( hashtable );
	} else {
		struct StringVector* vector = getStringHashtableKeys( hashtable );
		freeStringHashtable( hashtable );
		sortStringVector( vector );
		printStringVector( vector, writer );
		freeStringVector( vector );
	}
	closeHandler( writer, outputFile );
	freeInterface( interface );
	return 0;
}
