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

#include "advanced_io.h"

/******************** LIST ********************/

struct StringVector* getFiles( char* path, struct StringVector* vector ) {
	while( path[ strlen( path ) -1 ] == '/' ) path[ strlen( path ) - 1 ] = '\0'; // remove last '\'
	if( vector == NULL ) vector = newStringVector(); // remember first path
	if( ( strcmp( path, "stdin" ) == 0 ) || ( strcmp( path, "STDIN" ) == 0 ) ) {
		addStringVector( vector, path );
		return vector;
	}
	if( isFile( path ) ) {
		addStringVector( vector, path );
		return vector;
	}
	struct dirent* dir;
	DIR* reader = opendir( path );
	if( reader == NULL ) return vector;
	char temp[ MAX_FILENAME ]; // temporary string to concat directory and file name
	for( dir = readdir( reader ); dir != NULL; dir = readdir( reader ) ) {
		if( dir->d_name[ 0 ] == '.' ) continue;
		temp[ 0 ] = '\0'; // reset
		if( strcmp( path, "." ) != 0 ) {
			strcat( temp, path ); // directory
			strcat( temp, "/" ); // '/'
		}
		strcat( temp, dir->d_name ); // filename
		addStringVector( vector, temp );
	}
	closedir( reader );
	sortStringVector( vector ); // sort filenames
	return vector;
}

struct StringVector* getFilesRecursively( char* path, int depth, struct RegularExpression* suffix, struct RegularExpression* grep, struct RegularExpression* vgrep, struct StringVector* vector ) {
	if( vector == NULL ) vector = newStringVector(); // remember first path
	if( ( strcmp( path, "stdin" ) == 0 ) || ( strcmp( path, "STDIN" ) == 0 ) ) {
		addStringVector( vector, path );
		return vector;
	}
	if( isFile( path ) ) {
		int addPath = 1;
		if( suffix != NULL ) if( matchRegularExpression( suffix, path ) == 0 ) addPath = 0;
		if( vgrep != NULL ) if( matchRegularExpression( vgrep, path ) > 0 ) addPath = 0;
		if( grep != NULL ) if( matchRegularExpression( grep, path ) == 0 ) addPath = 0;
		if( addPath == 1 ) addStringVector( vector, path );
		return vector;
	}
	if( depth == 0 ) return vector; // directory of depth < 0 is neglected
	struct dirent* dir;
	DIR* reader = opendir( path );
	if( reader == NULL ) return NULL;
	char temp[ MAX_FILENAME ]; // temporary string to concat directory and file name
	for( dir = readdir( reader ); dir != NULL; dir = readdir( reader ) ) {
		if( dir->d_name[ 0 ] == '.' ) continue;
		temp[ 0 ] = '\0'; // reset
		if( strcmp( path, "." ) != 0 ) {
			strcat( temp, path ); // directory
			strcat( temp, "/" ); // '/'
		}
		strcat( temp, dir->d_name ); // filename
		getFilesRecursively( temp, depth - 1, suffix, grep, vgrep, vector ); // go recursively
	}
	closedir( reader );
	return vector;
}

struct StringVector* getDirectoriesRecursively( char* path, int depth, struct RegularExpression* grep, struct RegularExpression* vgrep, struct StringVector* vector ) {
	if( vector == NULL ) vector = newStringVector(); // remember first path
	if( isDirectory( path ) ) {
		int addPath = 1;
		if( vgrep != NULL ) if( matchRegularExpression( vgrep, path ) > 0 ) addPath = 0;
		if( grep != NULL ) if( matchRegularExpression( grep, path ) == 0 ) addPath = 0;
		if( addPath == 1 ) addStringVector( vector, path );
		return vector;
	}
	if( depth < 0 ) return vector; // directory of depth < 0 is neglected
	struct dirent* dir;
	DIR* reader = opendir( path );
	if( reader == NULL ) return NULL;
	char temp[ MAX_FILENAME ]; // temporary string to concat directory and file name
	for( dir = readdir( reader ); dir != NULL; dir = readdir( reader ) ) {
		if( dir->d_name[ 0 ] == '.' ) continue;
		temp[ 0 ] = '\0'; // reset
		if( strcmp( path, "." ) != 0 ) {
			strcat( temp, path ); // directory
			strcat( temp, "/" ); // '/'
		}
		strcat( temp, dir->d_name ); // filename
		getDirectoriesRecursively( temp, depth - 1, grep, vgrep, vector ); // go recursively
	}
	closedir( reader );
	return vector;
}
