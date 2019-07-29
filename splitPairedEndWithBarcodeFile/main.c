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
	fprintf( stdout, "Program: Split read1 and read2 sequences with thirs index/barcode sequence file.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: splitPairedEndWithBarcodeFile [Options} -b BARCODE -M NOMATCH1 -N NOMATCH2 INPUT1 INPUT2 INPUT3 OUTPUT OUTPUT2\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       INPUT1    5prime sequences (read1 sequence file)\n" );
	fprintf( stdout, "       INPUT2    3prime sequences (read2 sequence file)\n" );
	fprintf( stdout, "       INPUT3    barcode sequences (barcode sequence file)\n" );
	fprintf( stdout, "       OUTPUT1   Directory to store extracted 5prime sequences\n" );
	fprintf( stdout, "       OUTPUT2   Directory to store extracted 3prime sequences\n" );
	fprintf( stdout, "       NOMATCH1  5prime sequences which couldn't be extracted.\n" );
	fprintf( stdout, "       NOMATCH2  3prime sequences which couldn't be extracted.\n" );
	fprintf( stdout, "       BARCODE   barcode sequences to look for.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options: -b  barcode sequences separated by ','.\n" );
	fprintf( stdout, "         -g  gzip splitted output files.\n" );
	fprintf( stdout, "         -h  Show help menu\n" );
	fprintf( stdout, "         -M  files to store unextracted 5prime sequences.\n" );
	fprintf( stdout, "         -N  files to store unextracted 3prime sequences.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Note: Input can be gzipped or bzipped (handled by filename).\n" );
	fprintf( stdout, "      Program assumes IDs of raw files are in right order.\n" );
	fprintf( stdout, "      Barcode sequence will be added a filename before suffix (for example, 'input.fq' => 'input.ACA.fq').\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: 2013/05/30  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2012/08/02  Split paired-end by looking at the third sequence file.\n" );
	fprintf( stdout, "\n" );
	exit( 0 );
}

struct Barcode* addBarcode( struct Barcode* original, char* barcode, char* basename5, char* basename3, char* output5, char* output3, int gzip ) {
	struct Barcode* constructor = 0;
	constructor = malloc( sizeof( struct Barcode ) );
	// barcode
	constructor->barcode = 0;
	constructor->barcode = malloc( sizeof( char ) * strlen( barcode ) + 1 );
	strcpy( constructor->barcode, barcode );
	// regular expression
	constructor->regexp = 0;
	constructor->regexp = constructor->barcode;
	// file5
	constructor->file5 = 0;
	char* filename5 = malloc( sizeof( char ) * MAX_FILENAME );
	strcpy( filename5, output5 );
	strcat( filename5, "/" );
	strcat( filename5, basename5 );
	strcat( filename5, "." );
	if( strlen( barcode ) == 0 ) strcat( filename5, "nobarcode" );
	else strcat( filename5, barcode );
	strcat( filename5, ".fq" );
	if( gzip ) strcat( filename5, ".gz" );
	constructor->file5 = filename5;
	// file3
	constructor->file3 = 0;
	char* filename3 = malloc( sizeof( char ) * MAX_FILENAME );
	strcpy( filename3, output3 );
	strcat( filename3, "/" );
	strcat( filename3, basename3 );
	strcat( filename3, "." );
	if( strlen( barcode ) == 0 ) strcat( filename3, "nobarcode" );
	else strcat( filename3, barcode );
	strcat( filename3, ".fq" );
	if( gzip ) strcat( filename3, ".gz" );
	constructor->file3 = filename3;
	// writer
	constructor->writer5 = 0;
	constructor->writer5 = openWriter( filename5 );
	// writer2
	constructor->writer3 = 0;
	constructor->writer3 = openWriter( filename3 );
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
	if( barcode->writer5 != NULL ) closeHandler( barcode->writer5, barcode->file5 );
	if( barcode->writer3 != NULL ) closeHandler( barcode->writer3, barcode->file3 );
	free( barcode->file5 );
	free( barcode->file3 );
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

void searchAndWrite( struct Barcode* barcodes, struct SequenceReader* reader5, struct SequenceReader* reader3, struct SequenceReader* reader0, FILE* notfound5, FILE* notfound3 ) {
	struct Barcode* pointer = barcodes;
	while( pointer != NULL ) {
		if( sequence_compare( pointer->regexp, reader0->currentSequence ) == 0 ) {
			printCurrentSequence( reader5, pointer->writer5 );
			printCurrentSequence( reader3, pointer->writer3 );
			return; // break
		}
		pointer = pointer->next; // next
	}
	if( notfound5 != NULL ) printCurrentSequence( reader5, notfound5 );
	if( notfound3 != NULL ) printCurrentSequence( reader3, notfound3 );
}

int main ( int argc, char* argv[] ) {
	struct Interface* interface = newInterface( "b:ghM:N:rt", argc, argv );
	if( getBoolean( interface, 'h' ) || interface->argument_size < 5 ) {
		freeInterface( interface ); // free interface
		help();
		return 0;
	}
	int gzip = getBoolean( interface, 'g' );
	char* input5 = 0;
	input5 = interface->arguments[ 0 ];
	char* input3 = 0;
	input3 = interface->arguments[ 1 ];
	char* input0 = 0;
	input0 = interface->arguments[ 2 ];
	char* output5 = 0;
	output5 = interface->arguments[ 3 ];
	mkdir( output5, 0755 );
	char* output3 = 0;
	output3 = interface->arguments[ 4 ];
	mkdir( output3, 0755 );
	struct SequenceReader* reader5 = newSequenceReader( input5 );
	struct SequenceReader* reader3 = newSequenceReader( input3 );
	struct SequenceReader* reader0 = newSequenceReader( input0 );
	// get basename from input5
	char* basename5 = 0;
	basename5 = reader5->basename;
	// get basename from input3
	char* basename3 = 0;
	basename3 = reader3->basename;
	// not found...
	FILE* notfound5 = 0;
	FILE* notfound3 = 0;
	if( getBoolean( interface, 'M' ) ) notfound5 = openWriter( getString( interface, 'M' ) );
	if( getBoolean( interface, 'N' ) ) notfound3 = openWriter( getString( interface, 'N' ) );
	// assign barcodes
	struct Barcode* barcodes = 0;
	if( getBoolean( interface, 'b' ) ) { // barcode found
		char* string = getString( interface, 'b' );
		int index1 = 0;
		int index2 = getIndex( string, ',', index1 );
		char barcode[ MAX_LINE ];
		while( index2 > -1 ) {
			extract( string, barcode, index1, index2 );
			barcodes = addBarcode( barcodes, barcode, basename5, basename3, output5, output3, gzip );
			index1 = index2 + 1;
			index2 = getIndex( string, ',', index1 );
		}
		index2 = strlen( string );
		extract( string, barcode, index1, index2 );
		barcodes = addBarcode( barcodes, barcode, basename5, basename3, output5, output3, gzip );
	}
	// read files
	while( readNextSequence( reader0 ) ) {
		readNextSequence( reader5 );
		readNextSequence( reader3 );
		searchAndWrite( barcodes, reader5, reader3, reader0, notfound5, notfound3 );
	}
	freeBarcode( barcodes );
	freeSequenceReader( reader5 );
	freeSequenceReader( reader3 );
	freeSequenceReader( reader0 );
	freeInterface( interface );
	return 0;
}
