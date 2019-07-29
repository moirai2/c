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
	fprintf( stdout, "Program: Split sequences into multiple files by barcode supplied by users.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: splitByBarcode -t -b BARCODE -r REGEXP -N NOMATCH -O OUTPUT INPUT\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       INPUT    Input raw FASTQ/FASTA sequences with barcode sequence at the beginning (default='STDIN')\n" );
	fprintf( stdout, "       OUTPUT   Directory to store extracted sequences (default='.')\n" );
	fprintf( stdout, "       BARCODE  barcode sequences to split by (default='ignore barcode').\n" );
	fprintf( stdout, "       REGEXP   regular expression of barcode used to matching.  Make sure order matches with barcode (default is '^barcode').\n" );
	fprintf( stdout, "       NOMATCH  A file to store no-match sequences (default='none').\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options: -b  barcode sequences separated by ',' when barcode file is used barcode column label.\n" );
	fprintf( stdout, "         -B  Barcode file used for splitting (default='none').\n" );
	fprintf( stdout, "         -e  regular expression of barcode sequences separated by ',' (default is '^barcode').\n" );
	fprintf( stdout, "         -f  filename information used to associate filename and barcodes (default='none') with a barcode file.\n" );
	fprintf( stdout, "         -g  gzip splitted output files (default='none').\n" );
	fprintf( stdout, "         -h  Show help menu\n" );
	fprintf( stdout, "         -n  Basename of output files (default='name of first file').\n" );
	fprintf( stdout, "         -N  A file to store no-match sequences (default='none').\n" );
	fprintf( stdout, "         -O  Directory to store extracted sequences (default='.').\n" );
	fprintf( stdout, "         -t  Trim off/remove barcode sequence (default='keep original sequence').\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Note: Input can be gzipped or bzipped since program can detect zip state through filename.\n" );
	fprintf( stdout, "      Barcode sequence will be added between filename and suffix (for example, 'input.fq' => 'input.ACA.fq').\n" );
	fprintf( stdout, "      If barcode is null, all sequences will be passed without any modification.\n" );
	fprintf( stdout, "      If regexp is null, regexp will be constructed with '^barcode' string.\n" );
	fprintf( stdout, "      If barcode is at the end of sequence, use regular expression like 'ACTA$' for -r option.\n" );
	fprintf( stdout, "      Barcode file is a tab delimed file with barcode, filename, regexp information with column labels.\n" );
	fprintf( stdout, "      Specify column labels of a barcode file through -b, -r, -f information.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Example: splitByBarcode -b ACTA,TTTA input1.fq\n" );
	fprintf( stdout, "         splitByBarcode -b ACTA,TTTA -r 'ACTA$,TTTA$' -N nomatch.fq -O outdir input1.fq\n" );
	fprintf( stdout, "         splitByBarcode -B barcode.txt -b barcode -r regexp -f filename -N nomatch.fq -O outdir input1.fq\n" );\
	fprintf( stdout, "\n" );
    fprintf( stdout, "Updates: 2017/01/06  Enabled use of barcode file from PipelineBarcodeGenome.pl without label when no options -b,-r,-f are set.\n" );
    fprintf( stdout, "         2016/06/07  Use basename of a file when -n is not specified.\n" );
    fprintf( stdout, "         2013/05/30  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2013/03/07  Added the regular expression options.\n" );
	fprintf( stdout, "\n" );
	exit( 0 );
}

struct Barcode* addBarcode( struct Barcode* original, char* basename, char* barcode, char* regexp, char* directory, int gzip ) {
	struct Barcode* constructor = 0;
	constructor = malloc( sizeof( struct Barcode ) );
	// barcode
	constructor->barcode = 0;
	constructor->barcode = malloc( sizeof( char ) * strlen( barcode ) + 1 );
	strcpy( constructor->barcode, barcode );
	// regular expression
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
	// file
	constructor->file = 0;
	char* filename = malloc( sizeof( char ) * MAX_FILENAME );
	strcpy( filename, directory );
	strcat( filename, "/" );
	strcat( filename, basename );
	strcat( filename, "." );
	if( strlen( barcode ) == 0 ) strcat( filename, "nobarcode" ); // no barcode information...
	else strcat( filename, barcode ); // barcode
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
	freeRegularExpression( barcode->regexp );
	free( barcode );
}

int sequence_compare( struct RegularExpression* regexp, char* sequence ) {
	if( matchRegularExpression( regexp, sequence ) == 1 ) return 0; // match
	else return 1; // didn't match
}

int main( int argc, char* argv[] ) {
	struct Interface* interface = newInterface( "b:B:e:f:ghN:n:O:t", argc, argv ); // options
	if( getBoolean( interface, 'h' ) || interface->argument_size == 0 ) { // help message
		freeInterface( interface ); // free interface
		help(); // print help message
		return 0; // quit
	}
	char* directory = "."; // output directory
	if( getBoolean( interface, 'O' ) ) { // output directory specified
		directory = getString( interface, 'O' ); // set output directory
		mkdir( directory, 0755 ); // change to writable
	}
	int trimBarcode = getBoolean( interface, 't' ); // trim barcode sequence if found?
	int gzip = getBoolean( interface, 'g' ); // gzipped output?
	// barcode not found sequence
	FILE* notfound = 0;
	if( getBoolean( interface, 'N' ) ) notfound = openWriter( getString( interface, 'N' ) );
	// basename
	char* filename = interface->arguments[ 0 ];
	struct SequenceReader* reader = newSequenceReader( filename );
	char* basename;
	if( getBoolean( interface, 'n' ) ) basename = getString( interface, 'n' );
    else basename = reader->basename;
	// barcodes
	struct Barcode* barcodes = 0;
	if( getBoolean( interface, 'B' ) ) { // barcode file
		char* barcode_file   = getString( interface, 'B' );
		fprintf( stderr, "Reading '%s'\n", barcode_file );
		int barcode_index  = -1;
		int filename_index = -1;
		int regexp_index   = -1;
		char* barcode_label  = getString( interface, 'b' );
		char* filename_label = getString( interface, 'f' );
		char* regexp_label   = getString( interface, 'e' );
		struct SequenceReader* reader = newSequenceReader( barcode_file );
        if(existsOption(interface,'b')||existsOption(interface,'f')||existsOption(interface,'e')){
            if( readNextTEXT( reader ) ) { // read first label line
                struct StringVector* tokens = split( reader->currentLine, '	' ); // split by '\t'
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
        }else{
            barcode_index=0;
            fprintf( stderr, "barcode column at %d\n",  barcode_index );
        }
		while( readNextTEXT( reader ) ) { // read content
			struct StringVector* tokens = split( reader->currentLine, '	' ); // split by '\t'
			char* barcode  = ( barcode_index  >= 0 ) ? getStringVector( tokens, barcode_index  ) : 0;
			char* regexp   = ( regexp_index   >= 0 ) ? getStringVector( tokens, regexp_index   ) : 0;
			int ok = 1;
			if( filename_index >= 0 ) {
				struct RegularExpression* file_regexp = newRegularExpression( getStringVector( tokens, filename_index ) );
				if( sequence_compare( file_regexp, filename ) != 0 ) ok = 0;
				freeRegularExpression( file_regexp );
			}
			if( ok == 1 ) barcodes = addBarcode( barcodes, basename, barcode, regexp, directory, gzip );
			freeStringVector( tokens );
		}
		freeSequenceReader( reader );
	} else if( getBoolean( interface, 'b' ) ) { // barcode specified
		struct StringVector* barcode_vectors = split( getString( interface, 'b' ), ',' ); // split by ','
		struct StringVector* regexp_vectors  = split( getString( interface, 'e' ), ',' ); // split by ','
		while( hasMoreStringVector( barcode_vectors ) ) {
			char* barcode = nextStringVector( barcode_vectors );
			char* regexp = 0; // regular expression character
			if( hasMoreStringVector( regexp_vectors ) ) regexp = nextStringVector( regexp_vectors );
			barcodes = addBarcode( barcodes, basename, barcode, regexp, directory, gzip );
		}
		freeStringVector( barcode_vectors );
		freeStringVector( regexp_vectors  );
	}
	fprintf( stderr, "Reading '%s'\n", interface->arguments[ 0 ] );
	while( readNextSequence( reader ) ) {
		struct Barcode* pointer = barcodes;
		int found = 0;
		while( pointer != NULL ) {
			if( sequence_compare( pointer->regexp, reader->currentSequence ) == 0 ) {
				if( trimBarcode ) {
					int barcode_length = strlen( pointer->barcode );
					int sequence_length = strlen( reader->currentSequence );
					for( int i = 0; i <= sequence_length - barcode_length; i++ ) reader->currentSequence[ i ] = reader->currentSequence[ i + barcode_length ];
					if( reader->currentQV != NULL ) for( int i = 0; i <= sequence_length - barcode_length; i++ ) reader->currentQV[ i ] = reader->currentQV[ i + barcode_length ];
				}
				printCurrentSequence( reader, pointer->writer );
				found = 1;
				break;
			}
			pointer = pointer->next; // next
		}
		if( ( found == 0 ) && ( notfound != NULL ) ) printCurrentSequence( reader, notfound );
	}
	if( notfound != NULL ) closeHandler( notfound, getString( interface, 'N' ) );
	freeSequenceReader( reader );
	freeBarcode( barcodes );
	freeInterface( interface );
	return 0;
}
