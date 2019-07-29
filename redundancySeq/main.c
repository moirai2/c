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
	fprintf( stdout, "Program: Calculate redundancy of sequences.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: redundancySeq INPUT [ INPUT2 INPUT3 .. ] > OUTPUT\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       INPUT   Input FASTQ/FASTA/BAM/SAM file/directory\n" );
	fprintf( stdout, "       OUTPUT  filename, unique count, and total counts (and redundancy number)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options: -h  Show help menu\n" );
	fprintf( stdout, "         -O  Output file (default='STDIN')\n" );
	fprintf( stdout, "         -r  calculate redundancy\n" );
	fprintf( stdout, "         -z  Hashtable size (default=1000000)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Output: test/input1.fq   2015  2500\n" );
	fprintf( stdout, "        test/input2.fq   4     12\n" );
	fprintf( stdout, "        test/input3.fa   2     2\n" );
	fprintf( stdout, "        test/input4.sam  2     4\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Output(-r): test/input1.fq   2015  2500  1.24\n" );
	fprintf( stdout, "            test/input2.fq   4     12    3.00\n" );
	fprintf( stdout, "            test/input3.fa   2     2     1.00\n" );
	fprintf( stdout, "            test/input4.sam  2     4     2.00\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Note: When option -r is used, total / unique counts will be displayed.\n" );
	fprintf( stdout, "      When input is big, use bigger hashtable size to speed up the process.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: 2013/05/29  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2011/12/07  Print out redundancy at the end with option '-r'.\n" );
	fprintf( stdout, "\n" );
	exit( 0 );
}

// main function
int main ( int argc, char* argv[] ) {
	struct Interface* interface = newInterface( "hO:rz:", argc, argv );
	if( getBoolean( interface, 'h' ) ) {
		freeInterface( interface ); // free interface
		help();
		return 0;
	}
	// handle output
	char* output_file = ( getBoolean( interface, 'O' ) ) ? getString( interface, 'O' ) : "stdout";
	int hashtable_size = ( getBoolean( interface, 'z' ) ) ? atoi( getString( interface, 'z' ) ) : 1000000;
	FILE* writer = openWriter( output_file );
	if( interface->argument_size == 0 ) addArgument( interface, "stdin" );
	struct StringVector* files = newStringVector();
	for( int i = 0; i < interface->argument_size; i++ ) getFiles( interface->arguments[ i ], files );
	while( hasMoreStringVector( files ) ) {
		struct StringHashtable* hashtable = newStringHashtable( hashtable_size );
		char* input_file = nextStringVector( files );
		struct SequenceReader* reader = newSequenceReader( input_file );
		int totalCount = 0;
		while( readNextSequence( reader ) ) {
			totalCount++;
			countStringHashtable( hashtable, reader->currentSequence, 1 );
		}
		int uniqueCount = getStringHashtableKeySize( hashtable );
		freeSequenceReader( reader );
		freeStringHashtable( hashtable );
		if( getBoolean( interface, 'r' ) ) fprintf( writer ,"%s\t%d\t%d\t%.2f\n", input_file, uniqueCount, totalCount, ( (double) totalCount / uniqueCount ) );
		else fprintf( writer ,"%s\t%d\t%d\n", input_file, uniqueCount, totalCount );
	}
	freeStringVector( files );
	closeHandler( writer, output_file );
	freeInterface( interface );
	return 0;
}
