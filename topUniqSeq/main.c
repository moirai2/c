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
	fprintf( stdout, "Program: Read sorted unique sequences list and print out top X unique sequences and a summed others.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: topUniqSeq -n 30 < INPUT > OUTPUT\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       INPUT   Unique sequence file sorted by the number high->low frequency\n" );
	fprintf( stdout, "       OUTPUT  A list of unique sequences with summed others\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options: -h  Show help menu\n" );
	fprintf( stdout, "         -n  Print out top X unique sequences (default=all).  Can be string keys separated by spaces.\n" );
	fprintf( stdout, "         -O  Output file (default='stdin')\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Input: ATT    16\n" );
	fprintf( stdout, "       ATG    15\n" );
	fprintf( stdout, "       ATC    14\n" );
	fprintf( stdout, "       ATA    13\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Output(-n 2): ATA    13\n" );
	fprintf( stdout, "              ATG    15\n" );
	fprintf( stdout, "              Others 27\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Note: If no number is specified, it'll print all the number\n" );
	fprintf( stdout, "      Used with uniqSeq and sort -n -r -k 2 to show top X sequences.\n" );
	fprintf( stdout, "      2012/06/15 - Added hashtable function to select keys to show.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: 2013/05/30  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2012/05/11  Print top X sequences.\n" );
	fprintf( stdout, "\n" );
	exit( 0 );
}

// main function
int main ( int argc, char* argv[] ) {
	struct Interface* interface = newInterface( "hn:O:", argc, argv );
	if( getBoolean( interface, 'h' ) ) {
		freeInterface( interface ); // free interface
		help();
		return 0;
	}
	struct StringHashtable* hashtable = newStringHashtable( 100 );
	int number = -1;
	if( getBoolean( interface, 'n' ) ) {
		number = getInt( interface, 'n' );
		if( number == 0 ) { // number is string like barcodes
			struct StringVector* tokens = split( getString( interface, 'n' ), ',' );
			while( hasMoreStringVector( tokens ) ) {
				char* string = nextStringVector( tokens );
				putStringHashtable( hashtable, string, "true" );
			}
			freeStringVector( tokens );
		}
	}
	// handle output
	char* output_file = getBoolean( interface, 'O' ) ? getString( interface, 'O' ) : "stdout";
	FILE* writer = openWriter( output_file );
	if( interface->argument_size == 0 ) addArgument( interface, "stdin" );
	long int count = 0;
	long int others = 0;
	char line[ MAX_LINE ];
	for( int i = 0; i < interface->argument_size; i++ ) {
		FILE* reader = openReader( interface->arguments[ i ] );
		if( number < 0 ) {
			while( fgets( line, MAX_LINE, reader ) ) {
				chomp( line );
				fprintf( writer, "%s\n", line );
				count++;
			}
		} else if( number == 0 ) {
			char key[ MAX_LINE ];
			while( fgets( line, MAX_LINE, reader ) ) {
				chomp( line );
				int index = getIndex( line, '\t', 0 );
				if( index < 0 ) continue;
				strncpy( key, line, index );
				if( getStringHashtableCount( hashtable, key ) > 0 ) {
					fprintf( writer, "%s\n", line );
					count++;
					continue;
				} else {
					others += atoi( line + index + 1 );
				}
			}
		} else { // there is a limit
			while( fgets( line, MAX_LINE, reader ) ) {
				chomp( line );
				if( count < number ) {
					fprintf( writer, "%s\n", line );
					count++;
					continue;
				}
				int index = getIndex( line, '\t', 0 );
				if( index < 0 ) continue;
				others += atoi( line + index + 1 );
			}
		}
		closeHandler( reader, interface->arguments[ i ] );
	}
	if( others > 0 ) fprintf( writer, "others\t%ld\n", others );
	closeHandler( writer, output_file );
	freeInterface( interface );
	freeStringHashtable( hashtable );
	return 0;
}
