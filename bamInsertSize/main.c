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

void help( void ) {
    fprintf( stdout, "\n" );
	fprintf( stdout, "Program: Print out insert size distribution of paired-end SAM/BAM files.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: bamInsertSize [Options] INPUT > OUTPUT\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       INPUT  SAM/BAM input file(s)\n" );
	fprintf( stdout, "       OUTPUT insert size distribution with first column is length and second column is count\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options: -A  Print out an average insert length to a specified file with filename of input files in the first column\n" );
	fprintf( stdout, "         -h  Show help menu\n" );
	fprintf( stdout, "         -i  take inner distance (default='take outer distance')\n" );
	fprintf( stdout, "         -l  maximum length (default=1000)\n" );
	fprintf( stdout, "         -M  Print out median insert length to a specified file with filename of input files in the first column\n" );
	fprintf( stdout, "         -O  Output file (default='STDOUT')\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Note: Input file doesn't have to be sorted by IDs, since positions are stored in a hashtable with IDs until the other pair is matched.\n" );
	fprintf( stdout, "      Program skips secondary alignments and non properly mapped sequences.\n" );
	fprintf( stdout, "      Average insert length is calculated with the actual length, not with min/max cutoffed length values.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: 2013/05/27  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2013/04/17  Added option to print out median insert length to a specified file.\n" );
	fprintf( stdout, "         2013/03/27  Added option to print out average insert length to a specified file.\n" );
	fprintf( stdout, "\n" );
	exit( 0 );
}

int main ( int argc, char* argv[] ) {
	struct Interface* interface = newInterface( "A:ihl:M:O:", argc, argv );
	if( getBoolean( interface, 'h' ) ) {
		freeInterface( interface ); // free interface
		help();
		return 0;
	}
	char* average_file = 0;
	FILE* averageWriter = 0;
	if( getBoolean( interface, 'A' ) ) {
		average_file = getString( interface, 'A' );
		if( strlen( average_file ) == 0 ) average_file = "average.txt";
		averageWriter = openWriter( average_file );
	}
	char* median_file = 0;
	FILE* medianWriter = 0;
	if( getBoolean( interface, 'M' ) ) {
		median_file = getString( interface, 'M' );
		if( strlen( median_file ) == 0 ) median_file = "median.txt";
		medianWriter = openWriter( median_file );
	}
	int max_distribution = getBoolean( interface, 'l' ) ? getInt( interface, 'l' ) : 1000;
	// handle output
	char* output_file = ( getBoolean( interface, 'O' ) ) ? getString( interface, 'O' ) : "stdout";
	FILE* writer = openWriter( output_file );
	struct StringVector* files = newStringVector();
	for( int i = 0; i < interface->argument_size; i++ ) getFiles( interface->arguments[ i ], files );
	long int notProperCount = 0;
	long int secondaryCount = 0;
	long int totalCount     = 0;
	long int selectedCount  = 0;
	long int firstCount     = 0;
	long int lastCount      = 0;
	long int distribution[ max_distribution + 1 ]; // to take care of plus and minus
	for( int i = 0; i < max_distribution + 1; i++ ) distribution[ i ] = 0;
	int inner_distance = getBoolean( interface, 'i' ) ? 1 : 0;
	int minimum_length = max_distribution;
	int maximum_length = -max_distribution;
	long double grandAverage = 0.0;
	long int grandTotal = 0;
	while( hasMoreStringVector( files ) ) {
		long double average = 0.0;
		long int total = 0;
		char* file = nextStringVector( files );
		struct SequenceReader* reader = newSequenceReader( file );
		struct StringHashtable* hashtable = newStringHashtable( 1000000 ); // default is 1 million
		while( readNextSequence( reader ) ) {
			totalCount++;
			int flag = reader->flag;
			char* id = reader->currentID;
			// 1 multimapped
			// 2 properly aligned
			// 4 unmapped
			// 8 paired segment unmapped
			// 16 reverse complement
			// 32 next segment reverse complement
			// 64 first segment
			// 128 second segment
			// 256 secondary alignment
			// 512 not passing quality control
			// 1024 PCR duplicate
			// 163 = 128 + 32 + 2 + 1 = second segment, forward, properly mapped, multimapped typeB
			// 147 = 128 + 16 + 2 + 1 = second segment, reversed, properly mapped, multimapped typeA
			//  99 =  64 + 32 + 2 + 1 = first segment, forward, properly mapped, multimapped typeA
			//  83 =  64 + 16 + 2 + 1 = first segment, reversed, properly mapped, multimapped typeB
			if( ( flag & 2   ) == 0 ) { notProperCount++; continue; } // properly mapped?
			if( ( flag & 256 )  > 0 ) { secondaryCount++; continue; } // secondary alignment?
			int position = -1;
			if( inner_distance == 1 ) { // take inner distance
				if( ( flag & 16 ) == 0 ) position = getUpperLocation( reader ) + 1; // forward
				else position = getLowerLocation( reader ) - 1; // reverse
			} else { // take outer distance
				if( ( flag & 16 ) == 0 ) position = getLowerLocation( reader ); // forward
				else position = getUpperLocation( reader ); // reverse
			}
			if( ( flag & 64 ) > 0 ) firstCount++; // read1
			if( ( flag & 128 ) > 0 ) lastCount++; // read2
			int position2 = getStringHashtableCount( hashtable, id ); // get position of previous ID
			if( position2 == 0 ) { // haven't processed yet.
				countStringHashtable( hashtable, id, position ); // remember
			} else {
				int difference = ( ( flag & 16 ) > 0 ) ? ( position - position2 + 1 ) : ( position2 - position + 1 );
				average += difference;
				total++;
				if( difference > max_distribution ) difference = max_distribution;
				else if( difference < 0 ) difference = 0;
				distribution[ difference ]++;
				if( difference < minimum_length ) minimum_length = difference;
				if( difference > maximum_length ) maximum_length = difference;
				// if( abs( reader->tlen ) != abs( difference ) ) fprintf( stderr, "%s and %d and %d\n", reader->currentID, reader->tlen, difference );
			}
			selectedCount++;
		}
		grandTotal += total;
		grandAverage += average;
		if( total > 0 ) average /= total;
		if( averageWriter != NULL ) fprintf( averageWriter, "%s\taverage insert length\t%0.3Lf\n", reader->basename, average );
		if( medianWriter  != NULL ) {
			double median = calculate_median( distribution, max_distribution + 1, total );
			fprintf(  medianWriter, "%s\tmedian insert length\t%0.3f\n",  reader->basename, median  );
		}
		freeStringHashtable( hashtable );
		freeSequenceReader( reader );
	}
	if( grandTotal > 0 ) grandAverage /= grandTotal;
	double grandMedian = calculate_median( distribution, max_distribution + 1, grandTotal );
	for( int i = minimum_length; i <= maximum_length; i++ ) fprintf( writer, "%d\t%ld\n", i, distribution[ i ] );
	fprintf( stderr, "total alignment line\t%ld\n", totalCount );
	fprintf( stderr, "not properly aligned\t%ld\n", notProperCount );
	fprintf( stderr, "secondary alignment\t%ld\n", secondaryCount );
	fprintf( stderr, "forward properly aligned\t%ld\n", firstCount );
	fprintf( stderr, "reverse properly aligned\t%ld\n", lastCount );
	fprintf( stderr, "average insert length\t%0.3Lf\n", grandAverage );
	fprintf( stderr, "median insert length\t%0.3f\n", grandMedian );
	freeStringVector( files );
	if( writer != NULL ) closeHandler( writer, output_file );
	if( averageWriter != NULL ) closeHandler( averageWriter, average_file );
	if( medianWriter  != NULL ) closeHandler(  medianWriter,  median_file );
	freeInterface( interface );
	return 0;
}

double calculate_median( long int* distribution, int size, long int total ) {
	// calculate median value
	double median = 0.0;
	int middle = (int) total / 2;
	if( total % 2 == 0 ) { // even
		int found = 0;
		for( int i = 0; i < size; i++ ) {
			middle -= distribution[ i ];
			if( middle > 0 ) continue;
			if( found ) {
				if( distribution[ i ] == 0 )continue;
				median += i;
				median /= 2;
				break;
			}
			if( middle < 0 ) {
				median = i;
				found = 1;
			}
		}
	} else { // odd
		for( int i = 0; i < size; i++ ) {
			middle -= distribution[ i ];
			if( middle > 0 ) continue;
			median = i;
			break;
		}
	}
	return median;
}
