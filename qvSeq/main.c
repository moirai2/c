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
	fprintf( stdout, "Program: Create a table of sequence base distribution.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: qvSeq INPUT [ INPUT2 INPUT3 .. ] > OUTPUT\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       INPUT   Input FASTQ/FASTA/BAM/SAM file/directory\n" );
	fprintf( stdout, "       OUTPUT  A table of base distribution row is QV and column is base position\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options: -h  Show help menu\n" );
	fprintf( stdout, "         -O  Output file (default=STDOUT)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Input:\n" );
	fprintf( stdout, "@SequenceA\n" );
	fprintf( stdout, "CGATACGGGATAGTGTTTTTGGGGTAAGGACAAGGG\n" );
	fprintf( stdout, "+SequenceA\n" );
	fprintf( stdout, "ff_f^f[a`fecbfbffecf`def[ffefcfacffg\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Output:    0   1   2   3   4   5\n" );
	fprintf( stdout, "       1  19  30  31  21  20  39\n" );
	fprintf( stdout, "       2  22  51  36  80  19  51\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Note: This is used for quality control check.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: 2013/05/29  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2011/11/30  Print outs a table of base distribution.\n" );
	fprintf( stdout, "\n" );
	exit( 0 );
}

// main function
int main ( int argc, char* argv[] ) {
	struct Interface* interface = newInterface( "hO:sz:", argc, argv );
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
	struct IntDistribution* distribution = newCharDistribution( MAXIMUM_SEQUENCE_LENGTH );
	while( hasMoreStringVector( files ) ) {
		char* input_file = nextStringVector( files );
		struct SequenceReader* reader = newSequenceReader( input_file );
		while( readNextSequence( reader ) ) addCharDistribution( distribution, reader->currentQV );
		freeSequenceReader( reader );
	}
	printQVDistribution( distribution, writer );
	freeIntDistribution( distribution );
	freeStringVector( files );
	closeHandler( writer, output_file );
	freeInterface( interface );
	return 0;
}
