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
	fprintf( stdout, "Program: Split sequences files by TruSeq's read2 file.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: splitTruSeqByBarcode -b BARCODE -N NOMATCH -O OUTPUT INPUT1 INPUT2\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       INPUT1   Input raw FASTQ/FASTA sequences (read1 sequence file)\n" );
	fprintf( stdout, "       INPUT2   Sequences with barcode information (read2 sequence file)\n" );
	fprintf( stdout, "       OUTPUT   Directory to store extracted sequences\n" );
	fprintf( stdout, "       BARCODE  barcode sequences to look for.\n" );
	fprintf( stdout, "       NOMATCH  A file to store no-match sequences.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options: -b  barcode sequences separated by ','.\n" );
	fprintf( stdout, "         -g  gzip splitted output files.\n" );
	fprintf( stdout, "         -h  Show help menu\n" );
	fprintf( stdout, "         -n  Basename of output files.\n" );
	fprintf( stdout, "         -N  A file to store no-match sequences.\n" );
	fprintf( stdout, "         -O  Directory to store extracted read1 sequences.\n" );
	// fprintf( stdout, "         -r  regular expression of barcode sequences separated by ',' (default is barcode sequences).\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Note: Input can be gzipped or bzipped (handled by filename).\n" );
	fprintf( stdout, "      Program assumes IDs of a raw and barcode file are in right order.\n" );
	fprintf( stdout, "      There can be missing sequences in a raw file (for example, after N removal), but not in a barcode file.\n" );
	fprintf( stdout, "      Barcode sequence will be added a filename before suffix (for example, 'input.fq' => 'input.ACA.fq').\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: 2013/05/30  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2012/05/07  Split TruSeq sequeces\n" );
	fprintf( stdout, "\n" );
	exit( 0 );
}

struct Barcode* addBarcode( struct Barcode* original, char* basename, char* barcode, char* directory, int gzip ) {
	struct Barcode* constructor = 0;
	constructor = malloc( sizeof( struct Barcode ) );
	// barcode
	constructor->barcode = 0;
	constructor->barcode = malloc( sizeof( char ) * strlen( barcode ) + 1 );
	strcpy( constructor->barcode, barcode );
	// regular expression
	constructor->regexp = 0;
	constructor->regexp = constructor->barcode;
	// file
	constructor->file = 0;
	char* filename = malloc( sizeof( char ) * MAX_FILENAME );
	strcpy( filename, directory );
	strcat( filename, "/" );
	strcat( filename, basename );
	strcat( filename, "." );
	strcat( filename, barcode );
	strcat( filename, ".fq" );
	if( gzip ) strcat( filename, ".gz" );
	constructor->file = filename;
	// writer
	constructor->writer = 0;
	constructor->writer = openWriter( filename );
	// next
	constructor->next = 0;
	if( original != NULL ) {
		struct Barcode* temp = original;
		while( temp->next != NULL ) temp = temp->next;
		temp->next = constructor;
		return original;
	} else {
		return constructor;
	}
}

void freeBarcode( struct Barcode* barcode ) {
	if( barcode == NULL ) return;
	if( barcode->next != NULL ) freeBarcode( barcode->next );
	if( barcode->writer != NULL ) closeHandler( barcode->writer, barcode->file );
	free( barcode->file );
	free( barcode->barcode );
	free( barcode );
}

int sequence_compare( char* sequence1, char* sequence2 ) {
	int length1 = strlen( sequence1 );
	int length2 = strlen( sequence2 );
	if( length1 > length2 ) return -1; // sequence is shorter
	for( int i = 0; i < length1; i++ ) if( sequence1[ i ] != sequence2[ i ] ) return -1; // doesn't match
	return 0; // equal
}

void searchAndWrite( struct Barcode* barcodes, struct SequenceReader* reader1, struct SequenceReader* reader2, FILE* notfound ) {
	struct Barcode* pointer = barcodes;
	while( pointer != NULL ) {
		if( sequence_compare( pointer->regexp, reader2->currentSequence ) == 0 ) {
			printCurrentSequence( reader1, pointer->writer );
			return; // break
		}
		pointer = pointer->next; // next
	}
	if( notfound != NULL ) printCurrentSequence( reader1, notfound );
}

int main ( int argc, char* argv[] ) {
	struct Interface* interface = newInterface( "b:ghn:N:O:r:", argc, argv );
	if( getBoolean( interface, 'h' ) || interface->argument_size == 0 ) {
		freeInterface( interface ); // free interface
		help();
		return 0;
	}
	char* directory = ".";
	if( getBoolean( interface, 'O' ) ) {
		directory = getString( interface, 'O' );
		mkdir( directory, 0755 );
	}
	int gzip = getBoolean( interface, 'g' );
	char* file1 = 0;
	char* file2 = 0;
	if( interface->argument_size == 1 ) {
		file1 = "stdin";
		file2 = interface->arguments[ 0 ];
	} else if( interface->argument_size == 2 ) {
		file1 = interface->arguments[ 0 ];
		file2 = interface->arguments[ 1 ];
	}
	struct SequenceReader* reader1 = newSequenceReader( file1 );
	struct SequenceReader* reader2 = newSequenceReader( file2 );
	char* basename = 0;
	if( getBoolean( interface, 'n' ) ) basename = getString( interface, 'n' );
	else if( interface->argument_size == 1 ) basename = reader2->basename; // since input1 can be from STDIN
	else basename = reader1->basename; // get basename from input1
	struct Barcode* barcodes = 0;
	FILE* notfound = 0;
	if( getBoolean( interface, 'N' ) ) notfound = openWriter( getString( interface, 'N' ) );
	if( getBoolean( interface, 'b' ) ) { // barcode found
		char* string = getString( interface, 'b' );
		int index1 = 0;
		int index2 = getIndex( string, ',', index1 );
		char barcode[ MAX_LINE ];
		while( index2 > -1 ) {
			extract( string, barcode, index1, index2 );
			barcodes = addBarcode( barcodes, basename, barcode, directory, gzip );
			index1 = index2 + 1;
			index2 = getIndex( string, ',', index1 );
		}
		index2 = strlen( string );
		extract( string, barcode, index1, index2 );
		barcodes = addBarcode( barcodes, basename, barcode, directory, gzip );
	}
	readNextSequence( reader1 );
	while( readNextSequence( reader2 ) ) {
		if( checkSequenceIDs( reader1->currentID, reader2->currentID ) ) {
			searchAndWrite( barcodes, reader1, reader2, notfound );
			readNextSequence( reader1 );
		}
	}
	freeBarcode( barcodes );
	freeSequenceReader( reader1 );
	freeSequenceReader( reader2 );
	freeInterface( interface );
	return 0;
}
