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

#include "interface.h"

struct Interface* newInterface( char* option, int argc, char** argv ) {
	struct Interface* constructor = malloc( sizeof( struct Interface ) );
	constructor->option_size   = 0;
	constructor->argument_size = 0;
	constructor->option_types  = 0;
	constructor->option_keys   = 0;
	constructor->option_values = 0;
	constructor->arguments     = 0;
	constructor->option_types  = malloc( sizeof( int  ) * MAXIMUM_INTERFACE_NUMBER );
	constructor->option_keys   = malloc( sizeof( char ) * MAXIMUM_INTERFACE_NUMBER );
	constructor->option_values = malloc( sizeof( char* ) * MAXIMUM_INTERFACE_NUMBER );
	constructor->arguments     = malloc( sizeof( char* ) * MAXIMUM_INTERFACE_NUMBER );
	for( int i = 1; i < argc; i++ ) {
		int incremented = 0;
		if( *argv[ i ] == '-' ) {
			for( int j = 1; j < strlen( argv[ i ] ); j++ ) {
				for( int k = 0; k < strlen( option ); k++ ) {
					if( option[ k ] == argv[ i ][ j ] ) {
						if( option[ k + 1 ] == ':' ) {
							constructor->option_types[ constructor->option_size ] = 1;
							constructor->option_keys[ constructor->option_size ] = option[ k ];
							if( j == strlen( argv[ i ] ) - 1 ) { // If it's the last option, use next argument for the value
								if( i + 1 < argc ) constructor->option_values[ constructor->option_size ] = argv[ i + 1 ];
								else constructor->option_values[ constructor->option_size ] = "";
								incremented = 1;
								i++; // increment
							} else {
								constructor->option_values[ constructor->option_size ] = argv[ i ] + j + 1;
							}
						} else {
							constructor->option_types[ constructor->option_size ] = 0;
							constructor->option_keys[ constructor->option_size ] = option[ k ];
							constructor->option_values[ constructor->option_size ] = "1";
						}
						constructor->option_size++;
					}
					if( incremented ) break;
				}
				if( incremented ) break;
			}
		} else {
			for( ; i < argc; i++ ) {
				constructor->arguments[ constructor->argument_size ] = 0;
				constructor->arguments[ constructor->argument_size ] = malloc( sizeof( char ) * ( strlen( argv[ i ] ) + 1 ) );
				strcpy( constructor->arguments[ constructor->argument_size ], argv[ i ] );
				constructor->argument_size++;
			}
		}
	}
	return constructor;
}

void addArgument( struct Interface* interface, char* argument ) {
	interface->arguments[ interface->argument_size ] = malloc( sizeof( char ) * ( strlen( argument ) + 1 ) ); // stdin
	strcpy( interface->arguments[ interface->argument_size ], argument );
	interface->argument_size++;
}

void addStdinIfEmpty( struct Interface* interface ) {
	if( interface->argument_size > 0 ) return;
	addArgument( interface, "stdin" );
}

char* getArgument( struct Interface* interface, int index ) { return interface->arguments[ index ]; }

int existsOption( struct Interface* interface, char c ) {
	for( int i = 0; i < interface->option_size; i++ ) {
		if( interface->option_keys[ i ] == c ) { // character matches
			return 1; // return true
		}
	}
	return 0; // return false
}

int getBoolean( struct Interface* interface, char c ) {
	for( int i = 0; i < interface->option_size; i++ ) if( interface->option_keys[ i ] == c ) return TRUE;
	return FALSE;
}

int getInt( struct Interface* interface, char c ) {
	for( int i = 0; i < interface->option_size; i++ ) {
		if( interface->option_keys[ i ] == c ) { // character matches
			if( interface->option_types[ i ] == 0 ) return 0; // return contained
			return atoi( interface->option_values[ i ] ); // return value
		}
	}
	return 0; // return 0
}

char getChar( struct Interface* interface, char c ) {
	for( int i = 0; i < interface->option_size; i++ ) {
		if( interface->option_keys[ i ] == c ) { // character matches
			if( interface->option_types[ i ] == 0 ) return '\0'; // return contained
			return interface->option_values[ i ][ 0 ]; // return value
		}
	}
	return '\0';
}

double getDouble( struct Interface* interface, char c ) {
	for( int i = 0; i < interface->option_size; i++ ) {
		if( interface->option_keys[ i ] == c ) { // character matches
			if( interface->option_types[ i ] == 0 ) return 0.0; // return contained
			return atof( interface->option_values[ i ] ); // return value
		}
	}
	return 0.0; // return 0
}

char* getString( struct Interface* interface, char c ) {
	for( int i = 0; i < interface->option_size; i++ ) {
		if( interface->option_keys[ i ] == c ) { // character matches
			if( interface->option_types[ i ] == 0 ) return ""; // return contained
			return interface->option_values[ i ]; // return value
		}
	}
	return ""; // return 0
}

void freeInterface( struct Interface* interface ) {
	for( int i = 0; i < interface->argument_size; i++ ) free( interface->arguments[ i ] );
	free( interface->option_types );
	free( interface->option_keys );
	free( interface->option_values );
	free( interface->arguments );
	free( interface );
}
