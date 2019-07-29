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

void help( void ) {
    fprintf( stdout, "\n" );
	fprintf( stdout, "Program: Count base distribution of sequences.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: baseSeq INPUT [INPUT2 INPUT3 ..] > OUTPUT\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       INPUT  Input FASTQ/FASTA/BAM/SAM file/directory\n" );
	fprintf( stdout, "       OUTPUT A table of base distribution\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options: -h  Show help menu\n" );
	fprintf( stdout, "         -O  Output with chromosome, strand, position, and count (default=STDOUT)\n" );
	fprintf( stdout, "\n" );
    fprintf( stdout, "Output:         A       C       G       T       N\n" );
    fprintf( stdout, "        1       635     956     435     474     0\n" );
    fprintf( stdout, "        2       907     753     341     448     5\n" );
    fprintf( stdout, "        3       756     948     785     11      0\n" );
    fprintf( stdout, "        4       481     3       1002    1014    0\n" );
    fprintf( stdout, "\n" );
	fprintf( stdout, "Note: Very simple program for quality control.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: 2013/05/27  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2011/11/30  Print outs the table.\n" );
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
		while( readNextSequence( reader ) ) addCharDistribution( distribution, reader->currentSequence );
		freeSequenceReader( reader );
	}
	printBaseDistribution( distribution, writer );
	freeIntDistribution( distribution );
	freeStringVector( files );
	closeHandler( writer, output_file );
	freeInterface( interface );
	return 0;
}
