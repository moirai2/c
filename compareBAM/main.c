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
	fprintf( stdout, "Program: Compare two BAM sorted by IDs and split into categories.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: compareBAM [Options] BAM1 BAM2\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       BAM1  BAM/SAM format file to compare\n" );
	fprintf( stdout, "       BAM2  BAM/SAM format file to compare\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options: -h  Show help menu\n" );
	fprintf( stdout, "         -a  Alone sequences only found in BAM1.\n" );
	fprintf( stdout, "         -A  Alone sequences only found in BAM2.\n" );
	fprintf( stdout, "         -b  BAM1 sequences found in both BAM1 and BAM2.\n" );
	fprintf( stdout, "         -B  BAM2 sequences found in both BAM1 and BAM2.\n" );
	fprintf( stdout, "         -d  BAM1 sequences found in both BAM1 and BAM2, but mapped to different locations. NOT IMPLEMENTED YET.\n" );
	fprintf( stdout, "         -D  BAM2 sequences found in both BAM1 and BAM2, but mapped to different locations. NOT IMPLEMENTED YET.\n" );
	fprintf( stdout, "         -o  only show differences in summary.\n" );
	fprintf( stdout, "         -O  Output summary in text format with '>' and '<'.\n" );
	fprintf( stdout, "         -H  Output summary in HTML table format.\n" );
	fprintf( stdout, "         -s  BAM1 sequences found in both BAM1 and BAM2, and mapped to same locations. NOT IMPLEMENTED YET.\n" );
	fprintf( stdout, "         -S  BAM2 sequences found in both BAM1 and BAM2, and mapped to same locations. NOT IMPLEMENTED YET.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Note: Very simple program for quality control.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: 2013/05/27  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2012/05/11  Print outs differences in separate files.\n" );
	fprintf( stdout, "\n" );
	exit( 0 );
}

int readNextAlignment( struct SequenceReader* reader ) {
	readNextSequence( reader ); // read read
	if( ! hasCurrentSequence( reader ) ) return 0;
	while( ( reader->flag & 4 ) > 0 ) { // unmapped..
		readNextSequence( reader ); // read read1
		if( ! hasCurrentSequence( reader ) ) return 0;
	}
	return 1;
}

int main ( int argc, char* argv[] ) {
	struct Interface* interface = newInterface( "a:A:b:B:d:D:hH:oO:s:S:", argc, argv );
	if( getBoolean( interface, 'h' ) || ( interface->argument_size < 2 ) ) {
		freeInterface( interface ); // free interface
		help();
		return 0;
	}
	// int only_show_difference = ( getBoolean( interface, 'o' ) ) ? 1 : 0;
	char* input_file1 = getArgument( interface, 0 );
	char* input_file2 = getArgument( interface, 1 );
	long int countBoth   = 0;
	long int countAlone1 = 0;
	long int countAlone2 = 0;
	long int countDiff   = 0;
	long int countSame   = 0;
	struct SequenceReader* reader1 = newSequenceReader( input_file1 );
	struct SequenceReader* reader2 = newSequenceReader( input_file2 );
	FILE* writerAlone1     = 0;
	FILE* writerAlone2     = 0;
	FILE* writerBoth1      = 0;
	FILE* writerBoth2      = 0;
	FILE* writerDifferent1 = 0;
	FILE* writerDifferent2 = 0;
	FILE* writerSame1      = 0;
	FILE* writerSame2      = 0;
	FILE* writerOutput     = 0;
	FILE* writerHTML       = 0;
	if( getBoolean( interface, 'a' ) ) writerAlone1     = openWriter( getString( interface, 'a' ) );
	if( getBoolean( interface, 'A' ) ) writerAlone2     = openWriter( getString( interface, 'A' ) );
	if( getBoolean( interface, 'b' ) ) writerBoth1      = openWriter( getString( interface, 'b' ) );
	if( getBoolean( interface, 'B' ) ) writerBoth2      = openWriter( getString( interface, 'B' ) );
	if( getBoolean( interface, 'd' ) ) writerDifferent1 = openWriter( getString( interface, 'd' ) );
	if( getBoolean( interface, 'D' ) ) writerDifferent2 = openWriter( getString( interface, 'D' ) );
	if( getBoolean( interface, 's' ) ) writerSame1      = openWriter( getString( interface, 's' ) );
	if( getBoolean( interface, 'S' ) ) writerSame2      = openWriter( getString( interface, 'S' ) );
	if( getBoolean( interface, 'O' ) ) writerOutput     = openWriter( getString( interface, 'O' ) );
	if( getBoolean( interface, 'H' ) ) writerHTML       = openWriter( getString( interface, 'H' ) );
	readNextAlignment( reader1 ); // read read1
	readNextAlignment( reader2 ); // read read2
	char id1[ 1000 ]; // temporary ID holder
	char id2[ 1000 ]; // temporary ID holder
	if( writerHTML ) fprintf( writerHTML, "<TABLE BORDER=1>\n" );
	if( writerHTML ) fprintf( writerHTML, "<TR><TH>ID</TH><TH>%s</TH><TH>%s</TH></TR>\n", input_file1, input_file2 );
	while( hasCurrentSequence( reader1 ) && hasCurrentSequence( reader2 ) ) {
		strcpy( id1, reader1->currentID );
		strcpy( id2, reader2->currentID );
		int compare = strcmp( id1, id2 );
		if( compare < 0 ) { // less
			if( writerHTML ) fprintf( writerHTML, "<TR><TH>%s</TH><TD>", id1 );
			while( hasCurrentSequence( reader1 ) && ( strcmp( reader1->currentID, id2 ) < 0 ) ) {
				countAlone1++;
				if( writerAlone1 ) printCurrentSequence( reader1, writerAlone1 ); // print 5prime
				if( writerOutput ) { fprintf( writerOutput, "< " ); printCurrentSequence( reader1, writerOutput ); }
				if( writerHTML ) fprintf( writerHTML, "%s:%d..%d", reader1->rname, getStartLocation( reader1 ), getEndLocation( reader1 ) );
				readNextAlignment( reader1 );
			}
			if( writerHTML ) fprintf( writerHTML, "</TD><TD></TD></TR>\n" );
		} else if( compare == 0 ) { // same
			countBoth++;
			// if( writerHTML ) fprintf( writerHTML, "<TR><TH>%s</TH><TD>", id2 );
			while( hasCurrentSequence( reader1 ) && strcmp( reader1->currentID, id1 ) == 0 ) {
				if( writerBoth1 ) printCurrentSequence( reader1, writerBoth1 ); // print 5prime
				// if( writerHTML ) fprintf( writerHTML, "%s:%d..%d", reader1->rname, getStartLocation( reader1 ), getEndLocation( reader1 ) );
				readNextAlignment( reader1 ); // read read1
			}
			// if( writerHTML ) fprintf( writerHTML, "</TD><TD>" );
			while( hasCurrentSequence( reader2 ) && strcmp( reader2->currentID, id2 ) == 0 ) {
				if( writerBoth2 ) printCurrentSequence( reader2, writerBoth2 ); // print 5prime
				// if( writerHTML ) fprintf( writerHTML, "%s:%d..%d", reader2->rname, getStartLocation( reader2 ), getEndLocation( reader2 ) );
				readNextAlignment( reader2 ); // read read2
			}
			// if( writerHTML ) fprintf( writerHTML, "</TD></TR>\n" );
		} else if( compare > 0 ) { // more
			if( writerHTML ) fprintf( writerHTML, "<TR><TH>%s</TH><TD></TD><TD>", id2 );
			while( hasCurrentSequence( reader2 ) && ( strcmp( id1 ,reader2->currentID ) > 0 ) ) {
				countAlone2++;
				if( writerAlone2 ) printCurrentSequence( reader2, writerAlone2 ); // print 5prime
				if( writerOutput ) { fprintf( writerOutput, "> " ); printCurrentSequence( reader2, writerOutput ); }
				if( writerHTML ) fprintf( writerHTML, "%s:%d..%d", reader2->rname, getStartLocation( reader1 ), getEndLocation( reader1 ) );
				readNextAlignment( reader2 );
			}
			if( writerHTML ) fprintf( writerHTML, "</TD></TR>\n" );
		}
	}
	while( hasCurrentSequence( reader1 ) ) {
		countAlone1++;
		if( writerAlone1 ) printCurrentSequence( reader1, writerAlone1 ); // print 5prime
		if( writerOutput ) { fprintf( writerOutput, "< " ); printCurrentSequence( reader1, writerOutput ); }
		if( writerHTML ) fprintf( writerHTML, "<TR><TH>%s</TH><TD>%s:%d..%d</TD><TD></TD></TR>\n", reader1->currentID, reader1->rname, getStartLocation( reader1 ), getEndLocation( reader1 ) );
		readNextAlignment( reader1 );
	}
	while( hasCurrentSequence( reader2 ) ) {
		countAlone2++;
		if( writerAlone2 ) printCurrentSequence( reader2, writerAlone2 ); // print 5prime
		if( writerOutput ) { fprintf( writerOutput, "> " ); printCurrentSequence( reader2, writerOutput ); }
		if( writerHTML ) fprintf( writerHTML, "<TR><TH>%s</TH><TD></TD><TD>%s:%d..%d</TD></TR>\n", reader2->currentID, reader2->rname, getStartLocation( reader2 ), getEndLocation( reader2 ) );
		readNextAlignment( reader2 );
	}
	if( writerHTML ) fprintf( writerHTML, "</TABLE>\n" );
	if( writerAlone1     ) closeHandler( writerAlone1,     getString( interface, 'a' ) );
	if( writerAlone2     ) closeHandler( writerAlone2,     getString( interface, 'A' ) );
	if( writerBoth1      ) closeHandler( writerBoth1,      getString( interface, 'b' ) );
	if( writerBoth2      ) closeHandler( writerBoth2,      getString( interface, 'B' ) );
	if( writerDifferent1 ) closeHandler( writerDifferent1, getString( interface, 'd' ) );
	if( writerDifferent2 ) closeHandler( writerDifferent2, getString( interface, 'D' ) );
	if( writerSame1      ) closeHandler( writerSame1,      getString( interface, 's' ) );
	if( writerSame2      ) closeHandler( writerSame2,      getString( interface, 'S' ) );
	if( writerOutput     ) closeHandler( writerOutput,     getString( interface, 'O' ) );
	if( writerHTML       ) closeHandler( writerHTML,       getString( interface, 'H' ) );
	fprintf( stderr, "alone1 = %ld\n", countAlone1 );
	fprintf( stderr, "both = %ld\n", countBoth );
	fprintf( stderr, "alone2 = %ld\n", countAlone2 );
	fprintf( stderr, "diff = %ld\n", countDiff );
	fprintf( stderr, "same = %ld\n", countSame );
	freeSequenceReader( reader1 );
	freeSequenceReader( reader2 );
	freeInterface( interface );
	return 0;
}
