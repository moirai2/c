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
	fprintf( stdout, "Program: Split sequences by first sequence in read1 with read2 file.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: splitPairedEndByBarcode [Options] -b BARCODE -M NOMATCH1 -N NOMATCH2 INPUT1 INPUT2 OUTPUT OUTPUT2\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       INPUT1    5prime sequences with barcode at the beginning (read1 sequence file)\n" );
	fprintf( stdout, "       INPUT2    3prime sequences (read2 sequence file)\n" );
	fprintf( stdout, "       OUTPUT1   Directory to store extracted 5prime sequences\n" );
	fprintf( stdout, "       OUTPUT2   Directory to store extracted 3prime sequences\n" );
	fprintf( stdout, "       NOMATCH1  5prime sequences which couldn't be extracted.\n" );
	fprintf( stdout, "       NOMATCH2  3prime sequences which couldn't be extracted.\n" );
	fprintf( stdout, "       BARCODE   barcode sequences to look for.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options: -b  barcode sequences separated by ','.\n" );
	fprintf( stdout, "         -B  Barcode file used for splitting (default='none').\n" );
	fprintf( stdout, "         -d  Delimiter character (default='tab' for barcode file).\n" );
	fprintf( stdout, "         -e  regular expression of barcode sequences separated by ',' (default is '^barcode').\n" );
	fprintf( stdout, "         -f  filename information used to associate filename and barcodes (default='none') with a barcode file.\n" );
	fprintf( stdout, "         -g  gzip splitted output files.\n" );
	fprintf( stdout, "         -h  Show help menu\n" );
	fprintf( stdout, "         -M  files to store unextracted 5prime sequences.\n" );
	fprintf( stdout, "         -N  files to store unextracted 3prime sequences.\n" );
    fprintf( stdout, "         -n  remove barcode information from extracted files when only one barcode.\n" );
    fprintf( stdout, "         -r  remove barcode string from 5prime sequence.\n" );
    fprintf( stdout, "         -R  Don't put barcode string information to extracted files when one barcode is defined.\n" );
	fprintf( stdout, "         -s  sample name column in barcode file.\n" );
	fprintf( stdout, "         -t  Barcode is at the beginning of read2 sequences.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Note: Input can be gzipped or bzipped (handled by filename).\n" );
	fprintf( stdout, "      Program assumes IDs of raw files are in right order.\n" );
	fprintf( stdout, "      Barcode sequence will be added a filename before suffix (for example, 'input.fq' => 'input.ACA.fq').\n" );
	fprintf( stdout, "\n" );
    fprintf( stdout, "Updates: 2016/10/17  Added option R to remove barcode string from output files.\n" );
    fprintf( stdout, "         2013/05/30  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2012/05/11  Split pair-end sequences by barcode.\n" );
	fprintf( stdout, "\n" );
	exit( 0 );
}

struct Barcode* addBarcode( struct Barcode* original, char* barcode, char* regexp, char* basename5, char* basename3, char* output5, char* output3, int gzip, int noBarcode ) {
	// fprintf( stderr, "%s %s %s %s %s %s %d\n", barcode, regexp, basename5, basename3, output5, output3, gzip );
	struct Barcode* constructor = 0;
	constructor = malloc( sizeof( struct Barcode ) );
	// barcode
	constructor->barcode = 0;
	constructor->barcode = malloc( sizeof( char ) * strlen( barcode ) + 1 );
	strcpy( constructor->barcode, barcode );
	// regular expression
	constructor->regexp = 0;
	if( regexp != 0 && strlen( regexp ) > 0 ) {
		constructor->regexp = newRegularExpression( regexp );
	} else {
		char* barcode_string = malloc( sizeof( char ) * strlen( constructor->barcode ) + 2 );
		strcpy( barcode_string, "^" );
		strcat( barcode_string, constructor->barcode );
		constructor->regexp = 0;
		constructor->regexp = newRegularExpression( barcode_string );
		free( barcode_string );
	}
	// file5
	constructor->file5 = 0;
	char* filename5 = malloc( sizeof( char ) * MAX_FILENAME );
	strcpy( filename5, output5 );
	strcat( filename5, "/" );
	strcat( filename5, basename5 );
    if( noBarcode ) {}
    else if( strlen( barcode ) == 0 ) { strcat( filename5, ".nobarcode" ); }
    else { strcat( filename5, "." ); strcat( filename5, barcode ); }
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
    if( ! noBarcode ) {}
    else if( strlen( barcode ) == 0 ) { strcat( filename3, ".nobarcode" ); }
    else { strcat( filename3, "." ); strcat( filename3, barcode ); }
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
	freeRegularExpression( barcode->regexp );
	free( barcode );
}

int sequence_compare( struct RegularExpression* regexp, char* sequence ) {
	if( matchRegularExpression( regexp, sequence ) == 1 ) return 0; // match
	else return 1; // didn't match
}

void searchAndWrite( struct Barcode* barcodes, struct SequenceReader* reader5, struct SequenceReader* reader3, FILE* notfound5, FILE* notfound3, int removeBarcode ) {
	struct Barcode* pointer = barcodes;
	while( pointer != NULL ) {
		if( sequence_compare( pointer->regexp, reader5->currentSequence ) == 0 ) {
			if( removeBarcode ) {
				int length = strlen( pointer->barcode );
				if( length > 0 ) {
					if( strlen( reader5->currentSequence ) > length ) memmove( reader5->currentSequence, reader5->currentSequence + length, strlen( reader5->currentSequence ) );
					if( strlen( reader5->currentQV       ) > length ) memmove( reader5->currentQV,       reader5->currentQV       + length, strlen( reader5->currentQV       ) );
				}
			}
			printCurrentSequence( reader5, pointer->writer5 );
			printCurrentSequence( reader3, pointer->writer3 );
			return; // break
		}
		pointer = pointer->next; // next
	}
	if( notfound5 != NULL ) printCurrentSequence( reader5, notfound5 );
	if( notfound3 != NULL ) printCurrentSequence( reader3, notfound3 );
}

void searchAndWrite2( struct Barcode* barcodes, struct SequenceReader* reader5, struct SequenceReader* reader3, FILE* notfound5, FILE* notfound3, int removeBarcode ) {
	struct Barcode* pointer = barcodes;
	while( pointer != NULL ) {
		if( sequence_compare( pointer->regexp, reader3->currentSequence ) == 0 ) {
			if( removeBarcode ) {
				int length = strlen( pointer->barcode );
				if( length > 0 ) {
					if( strlen( reader3->currentSequence ) > length ) strcpy( reader3->currentSequence, reader3->currentSequence + length );
					if( strlen( reader3->currentQV       ) > length ) strcpy( reader3->currentQV,       reader3->currentQV       + length );
				}
			}
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
	struct Interface* interface = newInterface( "b:B:d:e:f:ghM:N:rRt", argc, argv );
	if( getBoolean( interface, 'h' ) || interface->argument_size < 4 ) {
		freeInterface( interface ); // free interface
		help();
		return 0;
	}
    int gzip = getBoolean( interface, 'g' );
    int removeBarcode = getBoolean( interface, 'r' );
    int noBarcode = getBoolean( interface, 'R' );
	char* input5 = 0;
	input5 = interface->arguments[ 0 ];
	char* input3 = 0;
	input3 = interface->arguments[ 1 ];
	char* output5 = 0;
	output5 = interface->arguments[ 2 ];
	mkdir( output5, 0755 );
	char* output3 = 0;
	output3 = interface->arguments[ 3 ];
	mkdir( output3, 0755 );
	struct SequenceReader* reader5 = newSequenceReader( input5 );
	struct SequenceReader* reader3 = newSequenceReader( input3 );
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
	if( getBoolean( interface, 'B' ) ) { // barcode file
		char* barcode_file   = getString( interface, 'B' );
		char* delimiters     = getString( interface, 'd' );
		char delim = strlen( delimiters ) > 0 ? delimiters[ 0 ] : '	'; // split default is '\t'
		fprintf( stderr, "Reading '%s'\n", barcode_file );
		int barcode_index  = -1;
		int filename_index = -1;
		int regexp_index   = -1;
		char* barcode_label  = getString( interface, 'b' );
		char* filename_label = getString( interface, 'f' );
		char* regexp_label   = getString( interface, 'e' );
		struct SequenceReader* reader = newSequenceReader( barcode_file );
		if( readNextTEXT( reader ) ) { // read first label line
			struct StringVector* tokens = split( reader->currentLine, delim );
			for( int i = 0; hasMoreStringVector( tokens ); i++ ) {
				char* token = nextStringVector( tokens );
				if( strcmp( barcode_label, token  ) == 0 ) barcode_index  = i;
				if( strcmp( filename_label, token ) == 0 ) filename_index = i;
				if( strcmp( regexp_label, token   ) == 0 ) regexp_index   = i;
			}
			if( barcode_index  >= 0 ) fprintf( stderr, "barcode column at %d\n",  barcode_index );
			if( filename_index >= 0 ) fprintf( stderr, "filename column at %d\n", filename_index );
			if( regexp_index   >= 0 ) fprintf( stderr, "regexp column at %d\n",   regexp_index );
			freeStringVector( tokens );
		}
		while( readNextSequence( reader ) ) { // read content
			struct StringVector* tokens = split( reader->currentLine, '	' ); // split by '\t'
			char* barcode  = ( barcode_index  >= 0 ) ? getStringVector( tokens, barcode_index  ) : 0;
			char* regexp   = ( regexp_index   >= 0 ) ? getStringVector( tokens, regexp_index   ) : 0;
			int ok = 1;
			if( filename_index >= 0 ) {
				struct RegularExpression* file_regexp = newRegularExpression( getStringVector( tokens, filename_index ) );
				if( sequence_compare( file_regexp, input5 ) != 0 ) ok = 0;
				freeRegularExpression( file_regexp );
			}
			if( ok == 1 ) barcodes = addBarcode( barcodes, barcode, regexp, basename5, basename3, output5, output3, gzip, noBarcode );
			freeStringVector( tokens );
		}
		freeSequenceReader( reader );
	} else if( getBoolean( interface, 'b' ) ) { // barcode found
		struct StringVector* barcode_vectors = split( getString( interface, 'b' ), ',' ); // split by ','
		struct StringVector* regexp_vectors  = split( getString( interface, 'e' ), ',' ); // split by ','
        if( getStringVectorSize( barcode_vectors ) > 1 ) noBarcode = 0;
		while( hasMoreStringVector( barcode_vectors ) ) {
			char* barcode = nextStringVector( barcode_vectors );
			char* regexp = 0; // regular expression character
			if( hasMoreStringVector( regexp_vectors ) ) regexp = nextStringVector( regexp_vectors );
			barcodes = addBarcode( barcodes, barcode, regexp, basename5, basename3, output5, output3, gzip, noBarcode );
		}
		freeStringVector( barcode_vectors );
		freeStringVector( regexp_vectors  );
	}
	// read files
	if( getBoolean( interface, 't' ) ) { // barcode at 3' ends
		while( readNextSequence( reader5 ) ) {
			readNextSequence( reader3 );
			searchAndWrite2( barcodes, reader5, reader3, notfound5, notfound3, removeBarcode );
		}
	} else { // barcode at 5' ends
		while( readNextSequence( reader5 ) ) {
			readNextSequence( reader3 );
			searchAndWrite( barcodes, reader5, reader3, notfound5, notfound3, removeBarcode );
		}
	}
	if( notfound5 != NULL ) closeHandler( notfound5, getString( interface, 'M' ) );
	if( notfound3 != NULL ) closeHandler( notfound3, getString( interface, 'N' ) );
	freeBarcode( barcodes );
	freeSequenceReader( reader5 );
	freeSequenceReader( reader3 );
	freeInterface( interface );
	return 0;
}
