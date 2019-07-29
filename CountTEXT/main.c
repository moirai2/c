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
	fprintf( stdout, "Program: Count line and size of TEXT files.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: CountTEXT INPUT [INPUT2 ..] > OUTPUT\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       INPUT   Input TEXT file\n" );
	fprintf( stdout, "       OUTPUT  Line count and file size of TEXT files.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Output: test/input.fa	192 B\n" );
	fprintf( stdout, "        test/input.fa	16 line\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Note: This program is used by MOIRAI to count sequences in TEXT file.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2011/10/31  Outputs statistical counts of TEXT file.\n" );
	fprintf( stdout, "\n" );
}

int main ( int argc, char* argv[] ) {
    if( argc == 2 && strcmp( argv[ 1 ], "-h" ) == 0 ) {
        help();
        return 0;
    }
	for( int i = 1; i < argc; i++ ) {
		long int lineCount = 0;
		char line[ MAX_LINE ];
		FILE* reader = openReader( argv[ i ] );
		while( fgets( line, MAX_LINE, reader ) ) lineCount++;
		fprintf( stdout, "%s\t%ld B\n", argv[ i ], getFileSize( argv[ i ] ) );
		fprintf( stdout, "%s\t%ld line\n", argv[ i ], lineCount );
		closeHandler( reader, argv[ i ] );
	}
	return 0;
}

FILE* openReader( char* filename ) {
	char command[ 1000 ];
	FILE* handler = 0;
	if( strcmp( filename, "-" ) == 0 ) return stdin;
	else if( strcmp( filename, "stdin" ) == 0 ) return stdin;
	else if( strcmp( filename, "STDIN" ) == 0 ) return stdin;
	else if( strlen( filename ) == 0 ) return stdin;
	else if( strcmp( ".bam", filename + ( strlen( filename ) - 4 ) ) == 0 ) {
		strcpy( command, "" );
		strcat( command, "samtools view " );
		strcat( command, filename );
		if( ! ( handler = popen( command, "r" ) ) ) {
			fprintf( stderr, "Can't open BAM file \"%s\".\n", filename );
			exit( -1 );
		}
	} else if( strcmp( ".gz", filename + ( strlen( filename ) - 3 ) ) == 0 ) {
		strcpy( command, "" );
		strcat( command, "gzip -cd " );
		strcat( command, filename );
		if( ! ( handler = popen( command, "r" ) ) ) {
			strcpy( command, "" );
			strcat( command, "zcat -d " );
			strcat( command, filename );
			if( ! ( handler = popen( command, "r" ) ) ) {
				fprintf( stderr, "Can't open gzip input file \"%s\".\n", filename );
				exit( -1 );
			}
		}
	} else if( strcmp( ".bz2", filename + ( strlen( filename ) - 4 ) ) == 0 ) {
		strcpy( command, "" );
		strcat( command, "bzip2 -cd " );
		strcat( command, filename );
		if( ! ( handler = popen( command, "r" ) ) ) {
			strcpy( command, "" );
			strcat( command, "zcat -d " );
			strcat( command, filename );
			if( ! ( handler = popen( command, "r" ) ) ) {
				fprintf( stderr, "Can't open bzip2 input file \"%s\".\n", filename );
				exit( -1 );
			}
		}
	} else {
		handler = fopen( filename, "r" );
	}
	return handler;
}

void closeHandler( FILE* file, char* filename ) {
	if( file == NULL ) return;
	if( strcmp( ".gz", filename + ( strlen( filename ) - 3 ) ) == 0 ) pclose( file );
	else if( strcmp( ".bz2", filename + ( strlen( filename ) - 4 ) ) == 0 ) pclose( file );
	else fclose( file );
}

long int getFileSize( char* filename ) {
	FILE* reader = fopen( filename, "rb" );
	fseek( reader, 0, SEEK_END );
	long int fsize = ftell( reader );
	fclose( reader );
	return fsize;
}
