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
	fprintf( stdout, "Program: Remove sequence with N from FASTA or FASTQ sequence files.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: removeN < INPUT > OUTPUT\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       INPUT   FASTA/FASTQ format file\n" );
	fprintf( stdout, "       OUTPUT  FASTA/FASTQ format file without sequence with N\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options: -h  Show help menu\n" );
	fprintf( stdout, "         -N  Sequence with N file\n" );
	fprintf( stdout, "         -O  Output file (default='STDOUT')\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: 2013/05/29  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2011/11/30  Remove sequence with base N.\n" );
	fprintf( stdout, "\n" );
	exit( 0 );
}

int main ( int argc, char* argv[] ) {
	struct Interface* interface = newInterface( "hN:O:", argc, argv );
	if( getBoolean( interface, 'h' ) ) {
		freeInterface( interface ); // free interface
		help();
		return 0;
	}
	char* output_n_file = 0;
	FILE* writer_n = 0;
	if( getBoolean( interface, 'N' ) ) {
		output_n_file =  getString( interface, 'N' );
		writer_n = openWriter( output_n_file );
	}
	// handle output
	char* output_file = ( getBoolean( interface, 'O' ) ) ? getString( interface, 'O' ) : "stdout";
	FILE* writer = openWriter( output_file );
	if( interface->argument_size == 0 ) addArgument( interface, "stdin" );
	struct StringVector* files = newStringVector();
	for( int i = 0; i < interface->argument_size; i++ ) getFiles( interface->arguments[ i ], files );
	long int sequenceCount = 0;
	long int nSequenceCount = 0;
	while( hasMoreStringVector( files ) ) {
		char* input_file = nextStringVector( files );
		struct SequenceReader* reader = newSequenceReader( input_file );
		while( readNextSequence( reader ) ) {
			char* sequence = reader->currentSequence;
			int length = strlen( sequence );
			int found = 0;
			for( int i = 0; i < length; i++ ) if( sequence[ i ] == 'N' || sequence[ i ] == 'n' ) { found = 1; break; }
			if( found == 1 ) {
				printCurrentSequence( reader, writer_n );
				nSequenceCount++;
			} else {
				printCurrentSequence( reader, writer );
				sequenceCount++;
			}
		}
		freeSequenceReader( reader );
	}
	closeHandler( writer_n, output_n_file );
	closeHandler( writer, output_file );
	fprintf( stdout, "Sequence With N = %ld\n", nSequenceCount );
	fprintf( stdout, "Sequence count = %ld\n", sequenceCount );
	freeStringVector( files );
	freeInterface( interface );
	return 0;
}
