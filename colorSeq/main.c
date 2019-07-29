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
	fprintf( stdout, "Program: Creates an (average) color table representation of sequences.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: colorSeq INPUT > OUTPUT\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       INPUT  FASTQ/FASTA/BAM/SAM file/directory\n" );
	fprintf( stdout, "       OUTPUT Table with color values per bases\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options: -h  Show help menu\n" );
	fprintf( stdout, "         -O  Output chromosome and primary and secondary counts (default=STDOUT)\n" );
	fprintf( stdout, "         -s  Output results by X rows (default=600)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Outputs: 0,143,15	95,15,63	0,143,15	159,143,0\n" );
	fprintf( stdout, "         15,159,0	63,15,79	15,79,15	159,143,0\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Note: Very simple program for quality control.\n" );
	fprintf( stdout, "      Results are feed to Graph for PNG image.\n" );
	fprintf( stdout, "      A = green (0,255,0)\n" );
	fprintf( stdout, "      C = blue (0,0,255)\n" );
	fprintf( stdout, "      G = yellow (255,255,0)\n" );
	fprintf( stdout, "      T = red (255,0,0)\n" );
	fprintf( stdout, "      If sequences are sorted beforehands, this program creates a beautiful graph.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: 2013/05/27  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2011/12/17  Print outs table.\n" );
	fprintf( stdout, "\n" );
	exit( 0 );
}

// main function
int main ( int argc, char* argv[] ) {
	struct Interface* interface = newInterface( "hO:s:", argc, argv );
	if( getBoolean( interface, 'h' ) ) {
		freeInterface( interface ); // free interface
		help();
		return 0;
	}
	
	// handle options
	int output_size = ( getBoolean( interface, 's' ) ) ? getInt( interface, 's' ) : 600;
	char* output_file = ( getBoolean( interface, 'O' ) ) ? getString( interface, 'O' ) : "stdout";
	FILE* writer = openWriter( output_file );
	
	// handle input files
	struct StringVector* files = newStringVector();
	for( int i = 0; i < interface->argument_size; i++ ) getFiles( interface->arguments[ i ], files );
	
	// go through once and calculate total count and maximum sequence length
	int total_count = 0;
	int maximum_sequence_length = 0;
	while( hasMoreStringVector( files ) ) {
		char* input_file = nextStringVector( files );
		struct SequenceReader* reader = newSequenceReader( input_file );
		while( readNextSequence( reader ) ) {
			int length = strlen( reader->currentSequence );
			if( length > maximum_sequence_length ) maximum_sequence_length = length;
			total_count += reader->count;
		}
		freeSequenceReader( reader );
	}
	resetStringVectorIndex( files );
	
	// create new distribution
	struct IntDistribution* distribution = newIntDistribution( maximum_sequence_length, 5 );
	double merge_size = (double) total_count / output_size;
	if( merge_size < 1 ) merge_size = 1;
	
	int total = 0;
	int index = 0;
	while( hasMoreStringVector( files ) ) {
		char* input_file = nextStringVector( files );
		struct SequenceReader* reader = newSequenceReader( input_file );
		while( readNextSequence( reader ) ) {
			char* sequence = reader->currentSequence;
			int count = reader->count;
			while( count > 0 ) {
				int remain = 0;
				if( count + total >= (int) merge_size ) { // count goes beyond boundary
					int temp = count;
					count = (int) merge_size - total; // up to this
					remain = temp - count; // remember remain count
				}
				int length = strlen( sequence );
				for( int i = 0; i < length; i++ ) {
					if( toupper( sequence[ i ] ) == 'A' ) distribution->array[ i ][ 0 ] += count;
					else if( toupper( sequence[ i ] ) == 'C' ) distribution->array[ i ][ 1 ] += count;
					else if( toupper( sequence[ i ] ) == 'G' ) distribution->array[ i ][ 2 ] += count;
					else if( toupper( sequence[ i ] ) == 'T' ) distribution->array[ i ][ 3 ] += count;
					else if( toupper( sequence[ i ] ) == 'N' ) distribution->array[ i ][ 4 ] += count;
				}
				total += count;
				if( total >= (int) merge_size ) {
					printColorValue( distribution, index, total, writer );
					total = 0;
					index++;
					resetIntDistribution( distribution );
				}
				count = remain;
			}
		}
		freeSequenceReader( reader );
	}
	// if( total > 0 ) printColorValue( distribution, index, total, writer );
	freeIntDistribution( distribution );
	freeStringVector( files );
	closeHandler( writer, output_file );
	freeInterface( interface );
	return 0;
}

void printColorValue( struct IntDistribution* distribution, int index, int total, FILE* writer ) {
	int length = getMaximumLength( distribution );
	for( int i = 0; i < length; i++ ) {
		double red   = 0;
		double green = 0;
		double blue  = 0;
		double value = calculateColorValue( distribution->array[ i ][ 0 ], total ); // A
		green += value; // green
		value =  calculateColorValue( distribution->array[ i ][ 1 ], total ); // C
		blue += value; // blue
		value =  calculateColorValue( distribution->array[ i ][ 2 ], total ); // G
		red   += value; // yellow
		green += value; // yellow
		value =  calculateColorValue( distribution->array[ i ][ 3 ], total ); // T
		red += value; // red
		value =  calculateColorValue( distribution->array[ i ][ 4 ], total ); // N
		red  += value; // purple
		blue += value; // purple
		red   *= 255;
		green *= 255;
		blue  *= 255;
		if( red   > 255 )   red = 255;
		if( blue  > 255 )  blue = 255;
		if( green > 255 ) green = 255;
		if( i > 0 ) fprintf( writer, "\t" );
		fprintf( writer, "%d,%d,%d", (int) red, (int) green, (int) blue );
	}
	fprintf( writer, "\n" );
}

double calculateColorValue( int value1, int value2 ) {
	double value = (double) value1 / value2;
	return value * value;
}
