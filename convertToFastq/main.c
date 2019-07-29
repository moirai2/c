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
	fprintf( stdout, "Program: Convert BAM/SAM/FASTA to FASTQ format.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: convertToFastq INPUT > OUTPUT\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       INPUT  FASTA/FASTA/BAM/SAM file(s)\n" );
	fprintf( stdout, "       OUTPUT FASTQ format file\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options: -h  Show help menu\n" );
    fprintf( stdout, "         -O  FASTQ format file (default=STDOUT)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Note: A very simple converter.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: 2013/05/27  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2012/10/26  Print outs FASTQ lines.\n" );
	fprintf( stdout, "\n" );
	exit( 0 );
}

int main ( int argc, char* argv[] ) {
	struct Interface* interface = newInterface( "hO:", argc, argv );
	if( getBoolean( interface, 'h' ) ) {
		freeInterface( interface ); // free interface
		help();
		return 0;
	}
	// handle output
	char* output_file = ( getBoolean( interface, 'O' ) ) ? getString( interface, 'O' ) : "stdout";
	FILE* writer = openWriter( output_file );
	if( interface->argument_size == 0 ) addArgument( interface, "stdin" );
	struct StringVector* files = newStringVector();
	for( int i = 0; i < interface->argument_size; i++ ) getFiles( interface->arguments[ i ], files );
	while( hasMoreStringVector( files ) ) {
		char* file = nextStringVector( files );
		struct SequenceReader* reader = newSequenceReader( file );
		while( readNextSequence( reader ) ) printCurrentFASTQ( reader, writer );
		freeSequenceReader( reader );
	}
	freeStringVector( files );
	closeHandler( writer, output_file );
	freeInterface( interface );
	return 0;
}
