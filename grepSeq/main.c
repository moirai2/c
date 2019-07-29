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
	fprintf( stdout, "Program: Grep sequence from FASTA/FASTQ/SAM/BAM sequence file.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: grepSeq REGEXP REGEXP < INPUT > OUTPUT\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       INPUT  Input FASTA/FASTQ/SAM/BAM format file\n" );
	fprintf( stdout, "       OUTPUT Output FASTA/FASTQ/SAM/BAM format fil\n" );
	fprintf( stdout, "       REGEXP Regular expression for grepping sequences.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options: -a  Remove sequence after the matched REGEXP NOT IMPLEMENTED YET\n" );
	fprintf( stdout, "         -b  Remove sequence before the matched REGEXP NOT IMPLEMENTED YET\n" );
	fprintf( stdout, "         -c  Remove sequence matched to regexp sequence\n" );
	fprintf( stdout, "         -k  Keep unmatched sequences used for clipping option -a and -b (default='discard').\n" );
	fprintf( stdout, "         -h  Show help menu\n" );
	fprintf( stdout, "         -i  Grep ID line instead of sequence line\n" );
	fprintf( stdout, "         -I  Input file (default=STDIN).\n" );
	fprintf( stdout, "         -O  Output file (default=STDOUT)\n" );
	fprintf( stdout, "         -v  invert-match\n" );
    fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: 2013/05/27  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2013/04/15  Added before '-b', after '-a', and discard '-d' options for clipping CAGE sequences.\n" );
	fprintf( stdout, "\n" );
	exit( 0 );
}

int main ( int argc, char* argv[] ) {
	struct Interface* interface = newInterface( "abchiI:kO:v", argc, argv );
	if( getBoolean( interface, 'h' ) ) {
		freeInterface( interface ); // free interface
		help();
		return 0;
	}
	int trim_after     = getBoolean( interface, 'a' );
	int trim_before    = getBoolean( interface, 'b' );
	int keep_unmatched = getBoolean( interface, 'k' );
	// Regular exprssions
	struct RegularExpression** expressions = malloc( sizeof( struct RegularExpression* ) * interface->argument_size );
	for( int i = 0; i < interface->argument_size; i++ ) expressions[ i ] = newRegularExpression( interface->arguments[ i ] );
	// handle output
	char* input_file  = ( getBoolean( interface, 'I' ) ) ? getString( interface, 'I' ) : "stdin";
	char* output_file = ( getBoolean( interface, 'O' ) ) ? getString( interface, 'O' ) : "stdout";
	int grepID = getBoolean( interface, 'i' );
	struct StringVector* files = newStringVector();
	getFiles( input_file, files );
	FILE* writer = openWriter( output_file );
	while( hasMoreStringVector( files ) ) {
		char* file = nextStringVector( files );
		struct SequenceReader* reader = newSequenceReader( file );
		if( getBoolean( interface, 'v' ) ) { // inverse match
			while( readNextSequence( reader ) ) {
				int found = 1;
				if( grepID ) for( int i = 0; i < interface->argument_size; i++ ) { if( matchRegularExpression( expressions[ i ], reader->currentID ) > 0 ) { found = 0; break; } }
				else for( int i = 0; i < interface->argument_size; i++ ) { if( matchRegularExpression( expressions[ i ], reader->currentSequence ) > 0 ) { found = 0; break; } }
				if( found ) printCurrentSequence( reader, writer );
			}
		} else { // normal match
			while( readNextSequence( reader ) ) {
				int found = 0;
				int start = -1;
				int end   = -1;
				if( grepID ) {
					for( int i = 0; i < interface->argument_size; i++ ) {
						if( matchRegularExpression( expressions[ i ], reader->currentID ) > 0 ) {
							found = 1;
							start = getMatchStartIndex( expressions[ i ], 0 );
							end   = getMatchEndIndex( expressions[ i ], 0 );
							break;
						}
					}
				} else {
					for( int i = 0; i < interface->argument_size; i++ ) {
						if( matchRegularExpression( expressions[ i ], reader->currentSequence ) > 0 ) {
							found = 1;
							start = getMatchStartIndex( expressions[ i ], 0 );
							end   = getMatchEndIndex( expressions[ i ], 0 );
							break;
						}
					}
				}
				if( found ) {
					if( trim_after ) { // trim after
						fprintf( stderr, "after: %d %d\n", start, end );
						
					} else if( trim_before ) { // trim before
						fprintf( stderr, "before: %d %d\n", start, end );
					}
					printCurrentSequence( reader, writer );
				} if( keep_unmatched ) printCurrentSequence( reader, writer );
			}
		}
		freeSequenceReader( reader );
	}
	freeStringVector( files );
	closeHandler( writer, output_file );
	for( int i = 0; i < interface->argument_size; i++ ) freeRegularExpression( expressions[ i ] );
	free( expressions );
	freeInterface( interface );
	return 0;
}
