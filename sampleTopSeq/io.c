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

#include "io.h"

/******************** BASENAME ********************/

char* getBasename( char* path, char* output ) {
	int index = getLastIndex( path, '/', strlen( path ) );
	if( index > -1 ) path += index + 1;
	index = getLastIndex( path, '.', strlen( path ) );
	if( strcmp( ".gz",    path + index ) == 0 ) index = getLastIndex( path, '.', index - 1 );
	if( strcmp( ".bz2",   path + index ) == 0 ) index = getLastIndex( path, '.', index - 1 );
	if( strcmp( ".gzip",  path + index ) == 0 ) index = getLastIndex( path, '.', index - 1 );
	if( strcmp( ".bzip2", path + index ) == 0 ) index = getLastIndex( path, '.', index - 1 );
	if( index < 0 ) index = strlen( path );
	strncpy( output, path, index );
	output[ index ] = '\0';
	return output;
}

char* getSuffix( char* path, char* output ) {
	int index = getLastIndex( path, '/', strlen( path ) );
	if( index > -1 ) path += index + 1;
	index = getLastIndex( path, '.', strlen( path ) );
	if( strcmp( ".gz",    path + index ) == 0 ) index = getLastIndex( path, '.', index - 1 );
	if( strcmp( ".bz2",   path + index ) == 0 ) index = getLastIndex( path, '.', index - 1 );
	if( strcmp( ".gzip",  path + index ) == 0 ) index = getLastIndex( path, '.', index - 1 );
	if( strcmp( ".bzip2", path + index ) == 0 ) index = getLastIndex( path, '.', index - 1 );
	if( index < 0 ) index = strlen( path ) - 1;
	strcpy( output, path + index + 1 );
	return output;
}

char* getFilename( char* path, char* output ) {
	int index = getLastIndex( path, '/', strlen( path ) );
	strcpy( output, path + index + 1 );
	return output;
}

char* getDirectory( char* path, char* output ) {
	int index = getLastIndex( path, '/', strlen( path ) );
	if( index < 0 ) index = 0;
	strncpy( output, path, index );
	output[ index ] = '\0';
	return output;
}

/******************** HANDLER ********************/

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

FILE* openWriter( char* filename ) {
	char command[ 1000 ];
	FILE* handler = 0;
	if( strcmp( filename, "-" ) == 0 ) return stdout;
	else if( strcmp( filename, "stdout" ) == 0 ) return stdout;
	else if( strcmp( filename, "STDOUT" ) == 0 ) return stdout;
	else if( strlen( filename ) == 0 ) return stdout;
	if( strcmp( ".gz", filename + ( strlen( filename ) - 3 ) ) == 0 ) {
		strcpy( command, "" );
		strcat( command, "gzip -c > " );
		strcat( command, filename );
		if( ! ( handler = popen( command, "w" ) ) ) {
			fprintf( stderr, "Can't open gzip output file \"%s\".\n", filename );
			exit( -1 );
		}
	} else if( strcmp( ".bz2", filename + ( strlen( filename ) - 4 ) ) == 0 ) {
		strcpy( command, "" );
		strcat( command, "bzip2 -c > " );
		strcat( command, filename );
		if( ! ( handler = popen( command, "w" ) ) ) {
			fprintf( stderr, "Can't open bzip2 output file \"%s\".\n", filename );
			exit( -1 );
		}
	} else {
		handler = fopen( filename, "w" );
	}
	return handler;
}

void closeHandler( FILE* file, char* filename ) {
	if( file == NULL ) return;
	if( strcmp( ".gz", filename + ( strlen( filename ) - 3 ) ) == 0 ) pclose( file );
	else if( strcmp( ".bz2", filename + ( strlen( filename ) - 4 ) ) == 0 ) pclose( file );
	else if( strcmp( ".bam", filename + ( strlen( filename ) - 4 ) ) == 0 ) pclose( file );
	else fclose( file );
}

/******************** INFORMATION ********************/

// http://homepage1.nifty.com/herumi/prog/gcc-and-vc.html
long int getFileSize( char* filename ) {
	FILE* reader = fopen( filename, "rb" );
	fseek( reader, 0, SEEK_END );
	long int fsize = ftell( reader );
	fclose( reader );
	return fsize;
}

int isDirectory( char* path ) {
	DIR* reader = opendir( path );
	if( reader == NULL ) return 0;
	closedir( reader );
	return 1;
}

int isFile( char* path ) {
	if( isDirectory( path ) ) return 0;
	return ( access( path, R_OK ) == 0 );
}

int existsFile( char* path ) { return ( access( path, F_OK ) == 0 ); }

/******************** UTILITY ********************/

void chomp( char* line ) {
	int length = strlen( line );
	for( int i = 0; i < length; i++ ) {
		if( line[ i ] == '\n' ) line[ i ] = '\0';
		else if( line[ i ] == '\r' ) line[ i ] = '\0';
	}
}

void extract( char* input, char* output, int start, int end ) {
	if( start < 0 || end < 0 ) {
		strcpy( output, "" );
		return;
	}
	int length = end - start;
	strncpy( output, input + start, length );
	output[ length ] = '\0';
}

int getIndex( char* path, char c, int start ) {
	for( int index = start; index < strlen( path ); index++ ) if( path[ index ] == c ) return index;
	return -1;
}

int getLastIndex( char* path, char c, int end ) {
	for( int index = end; index >= 0; index-- ) if( path[ index ] == c ) return index;
	return -1;
}

int replaceChar( char* line, char c1, char c2 ) {
	int count = 0;
	int length = strlen( line );
	for( int i = 0; i < length; i++ ) if( line[ i ] == c1 ) { line[ i ] = c2; count++; }
	return count;
}

int countChar( char* line, char c ) {
	int count = 0;
	int length = strlen( line );
	for( int i = 0; i < length; i++ ) if( line[ i ] == c ) count++;
	return count;
}
