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
	fprintf( stdout, "Program: Show difference of sequences (INPUT1 - INPUT2 = DIFF) in original (INPUT1) format.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: subtractSeq INPUT INPUT2 > OUTPUT\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       INPUT1  Input FASTQ/FASTA/BAM/SAM file/directory\n" );
	fprintf( stdout, "       INPUT2  Input FASTQ/FASTA/BAM/SAM file/directory\n" );
	fprintf( stdout, "       OUTPUT  Sequence difference (INPUT1 - INPUT2) in original (INPUT1) format\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options: -h  Show help menu\n" );
	fprintf( stdout, "         -M  Print matched sequence to a file (default='none')\n" );
	fprintf( stdout, "         -O  Output file (default='STDOUT')\n" );
	fprintf( stdout, "         -z  Hashtable size (default='1000000')\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Note: hashtable is used to remember names of IDs.  It needs a big size if input is big.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: 2013/05/30  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2011/11/30  Show difference between two sequence fiels.\n" );
	fprintf( stdout, "\n" );
	exit( 0 );
}

// main function
int main ( int argc, char* argv[] ) {
	struct Interface* interface = newInterface( "hM:O:", argc, argv );
	char* matchFile = 0;
	FILE* matchWriter = 0;
	if( existsOption( interface, 'M' ) ) {
		matchFile = getString( interface, 'M' );
		matchWriter = openWriter( matchFile );
	}
	if( getBoolean( interface, 'h' ) || interface->argument_size < 2 ) {
		freeInterface( interface ); // free interface
		help();
		return 0;
	}
	// handle output
	char* output_file = ( getBoolean( interface, 'O' ) ) ? getString( interface, 'O' ) : "stdout";
	FILE* writer = openWriter( output_file );
	int hashtable_size = ( getBoolean( interface, 'z' ) ) ? atoi( getString( interface, 'z' ) ) : 1000000;	
	// get IDs of second input file
	struct StringHashtable* hashtable = newStringHashtable( hashtable_size );
	struct SequenceReader* reader2 = newSequenceReader( interface->arguments[ 1 ] );
	while( readNextSequence( reader2 ) ) countStringHashtable( hashtable, reader2->currentID, 1 );
	freeSequenceReader( reader2 );
	struct SequenceReader* reader1 = newSequenceReader( interface->arguments[ 0 ] );
	while( readNextSequence( reader1 ) ) {
		struct StringHashtableObject* object = getStringHashtableObject( hashtable, reader1->currentID );
		if( object == NULL ) {
			printCurrentSequence( reader1, writer );
		} else if( matchWriter != NULL ) {
			printCurrentSequence( reader1, matchWriter );
		}
	}
	freeSequenceReader( reader1 );
	freeStringHashtable( hashtable );
	closeHandler( writer, output_file );
	freeInterface( interface );
	return 0;
}
