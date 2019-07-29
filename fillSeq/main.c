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
	fprintf( stdout, "Program: Fill sequences upto specified length with specified base.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: fillSeq [Options] < INPUT > OUTPUT\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       INPUT  Input FASTQ file/directory\n" );
	fprintf( stdout, "       OUTPUT Filld sequence in FASTQ format\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options: -h  Show help menu\n" );
	fprintf( stdout, "         -c  Base to fill with (default='N').\n" );
	fprintf( stdout, "         -e  Fill bases from end (default='beginning').\n" );
	fprintf( stdout, "         -n  Number to fill (default='50').\n" );
	fprintf( stdout, "         -O  Output file (default='STDOUT')\n" );
	fprintf( stdout, "         -q  QV to fill with (default='/').\n" );
    fprintf( stdout, "\n" );
	fprintf( stdout, "Inputs:  @Number1\n" );
	fprintf( stdout, "         AAAAAAAAAAA\n" );
	fprintf( stdout, "         +Number1\n" );
	fprintf( stdout, "         acghaidkajj\n" );   
	fprintf( stdout, "\n" );
	fprintf( stdout, "Outputs: @Number1\n" );
	fprintf( stdout, "         NNNNNNNNNAAAAAAAAAAA\n" );
	fprintf( stdout, "         +Number1\n" );
	fprintf( stdout, "         /////////acghaidkajj\n" );    
    fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: 2013/05/27  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2011/11/30  Print outs filled sequences.\n" );
	fprintf( stdout, "\n" );
	exit( 0 );
}

int main ( int argc, char* argv[] ) {
	struct Interface* interface = newInterface( "c:ehn:O:q:", argc, argv );
	if( getBoolean( interface, 'h' ) ) {
		freeInterface( interface ); // free interface
		help();
		return 0;
	}
	char baseChar    = getBoolean( interface, 'c' ) ? getChar( interface, 'c' ) : 'N';
	char qvChar      = getBoolean( interface, 'q' ) ? getChar( interface, 'q' ) : '/';
	int numberToFill = getBoolean( interface, 'n' ) ?  getInt( interface, 'n' ) : 50;
	// handle output
	char* output_file = ( getBoolean( interface, 'O' ) ) ? getString( interface, 'O' ) : "stdout";
	if( interface->argument_size == 0 ) addArgument( interface, "stdin" );
	struct StringVector* files = newStringVector();
	for( int i = 0; i < interface->argument_size; i++ ) getFiles( interface->arguments[ i ], files );
	FILE* writer = openWriter( output_file );
	while( hasMoreStringVector( files ) ) {
		char* input_file = nextStringVector( files );
		struct SequenceReader* reader = newSequenceReader( input_file );
		if( getBoolean( interface, 'e' ) ) {
			while( readNextSequence( reader ) ) {
				int length = strlen( reader->currentSequence );
				if( length < numberToFill ) {
					for( int i = length; i < numberToFill; i++ ) {
						reader->currentSequence[ i ] = baseChar;
						reader->currentQV[ i ] = qvChar;
					}
					reader->currentSequence[ numberToFill ] = '\0';
					reader->currentQV[ numberToFill ] = '\0';
				}
				printCurrentSequence( reader, writer );
			}
		} else {
			while( readNextSequence( reader ) ) {
				int length = strlen( reader->currentSequence );
				if( length < numberToFill ) {
					int diff = numberToFill - length;
					for( int i = 0; i < length; i++ ) {
						reader->currentSequence[ i + diff ] = reader->currentSequence[ i ];
						reader->currentQV[ i + diff ] = reader->currentQV[ i ];
					}
					for( int i = 0; i < diff; i++ ) {
						reader->currentSequence[ i ] = baseChar;
						if( strlen( reader->currentQV ) > 0 ) reader->currentQV[ i ] = qvChar;
					}
					reader->currentSequence[ numberToFill ] = '\0';
					reader->currentQV[ numberToFill ] = '\0';
				}
				printCurrentSequence( reader, writer );
			}
		}
		freeSequenceReader( reader );
	}
	freeStringVector( files );
	closeHandler( writer, output_file );
	freeInterface( interface );
	return 0;
}
