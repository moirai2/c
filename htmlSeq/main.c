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
	fprintf( stdout, "Program: Mark sequence with colors.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: htmlSeq STYLE FASTA MARKER > HTML\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       MARKER  Marker fasta file used to color sequences output from 'markSeq'\n" );
	fprintf( stdout, "       HTML  FASTA printed in HTML format\n" );
	fprintf( stdout, "       FASTA  Original FASTA file\n" );
	fprintf( stdout, "       STYLE  file showing how style associated with the marker with a tab delim\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Options -d  delim (default=none)\n" );
	fprintf( stdout, "        -f  Show filename under image\n" );
	fprintf( stdout, "        -h  Show help menu\n" );
	fprintf( stdout, "        -H  set image width (default=none)\n" );
	fprintf( stdout, "        -l  link to original image\n" );
	fprintf( stdout, "        -O  Output HTML file (default=STDOUT)\n" );
	fprintf( stdout, "        -r  root directory of HTML (default=current)\n" );
	fprintf( stdout, "        -W  set image width (default=none)\n" );
    fprintf( stdout, "\n" );
	fprintf( stdout, "Style: +	background-color: blue; color: red;\n" );
	fprintf( stdout, "       -	background-color: blue; color: white;\n" );
	fprintf( stdout, "       #	color: pink;\n" );
	fprintf( stdout, "       =	color: orange;\n" );
    fprintf( stdout, "\n" );
	fprintf( stdout, "Fasta: >SequencdA\n" );
	fprintf( stdout, "       CGAAATTCCGG\n" );
	fprintf( stdout, "       >SequenceB\n" );
	fprintf( stdout, "       AATTCCGGCGA\n" );
    fprintf( stdout, "\n" );
	fprintf( stdout, "Marker: >SequenceA\n" );
	fprintf( stdout, "        ###++++++++\n" );
	fprintf( stdout, "        >SequenceB\n" );
	fprintf( stdout, "        ++++++++###\n" );
    fprintf( stdout, "\n" );
	fprintf( stdout, "HTML: <style TYPE=\"text/css\">\n" );
	fprintf( stdout, "      <!--\n" );
	fprintf( stdout, "      span.marker35 { color: pink; }\n" );
	fprintf( stdout, "      span.marker43 { background-color: blue; color: red; }\n" );
	fprintf( stdout, "      span.marker45 { background-color: blue; color: white; }\n" );
	fprintf( stdout, "      span.marker61 { color: orange; }\n" );
	fprintf( stdout, "      -->\n" );
	fprintf( stdout, "      </style>\n" );
    fprintf( stdout, "      <PRE>\n" );
	fprintf( stdout, "      >SequenceA\n" );
	fprintf( stdout, "      <SPAN CLASS=\"marker35\">CGA</SPAN><SPAN CLASS=\"marker43\">AATTCCGG</SPAN>\n" );
	fprintf( stdout, "      >SequenceB\n" );
	fprintf( stdout, "      <SPAN CLASS=\"marker43\">AATTCCGG</SPAN><SPAN CLASS=\"marker35\">CGA</SPAN>\n" );
    fprintf( stdout, "      </PRE>\n" );
    fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: 2013/05/29  Unified help message across perl, java, and shell scripts.\n" );
	fprintf( stdout, "         2011/12/16  Outputs HTML lines coloring sequences.\n" );
	fprintf( stdout, "\n" );
	exit( 0 );
}

// <style TYPE="text/css">
// <!--
// span.marker1 { background-color: blue; color: red; }
// span.marker2 { background-color: red; color: blue; }
// -->
// </style>
// <span class="marker1">CGAAATT</SPAN>
// main function
int main ( int argc, char* argv[] ) {
	struct Interface* interface = newInterface( "hO:sz:", argc, argv );
	if( getBoolean( interface, 'h' ) || interface->argument_size < 2 ) {
		freeInterface( interface ); // free interface
		help();
		return 0;
	}
	
	// handle output file
	char* output_file = ( getBoolean( interface, 'O' ) ) ? getString( interface, 'O' ) : "stdout";
	FILE* writer = openWriter( output_file );
	
	// handle style files
	char** styles = 0;
	styles = malloc( sizeof( char* ) * 256 );
	for( int i = 0; i < 256; i++ ) styles[ i ] = 0;
	struct StringVector* style_files = newStringVector();
	getFiles( interface->arguments[ 0 ], style_files );
	char line[ MAX_LINE ];
	while( hasMoreStringVector( style_files ) ) {
		char* style_file = nextStringVector( style_files );
		FILE* reader = openReader( style_file );
		while( fgets( line, MAX_LINE, reader ) ) { // read from file
			chomp( line ); // remove '\n'
			int index = (int) line[ 0 ];
			int string_length =  strlen( line ) - 2;
			styles[ index ] = malloc( sizeof( char ) * ( string_length + 1 ) );
			strcpy( styles[ index ], line + 2 );
		}
		closeHandler( reader, style_file );
	}
	freeStringVector( style_files );
	
	// handle input files
	struct StringVector* input_files = newStringVector();
	getFiles( interface->arguments[ 1 ], input_files );
	
	// handle marker files
	if( interface->argument_size < 3 ) addArgument( interface, "stdin" );
	struct StringVector* marker_files = newStringVector();
	getFiles( interface->arguments[ 2 ], marker_files );
	
	// print style
	fprintf( writer, "<style TYPE=\"text/css\">\n" );
	fprintf( writer, "<!--\n" );
	for( int i = 0; i < 256; i++ ) {
		if( styles[ i ] == NULL ) continue;
		fprintf( writer, "span.marker%d { %s }\n", i, styles[ i ] );
	}
	fprintf( writer, "-->\n" );
	fprintf( writer, "</style>\n" );
	
	fprintf( writer, "<PRE>\n" );
	// go through input files
	while( hasMoreStringVector( input_files ) ) {
		char* input_file = nextStringVector( input_files );
		char* marker_file = nextStringVector( marker_files );
		struct SequenceReader* reader1 = newSequenceReader( input_file );
		struct SequenceReader* reader2 = newSequenceReader( marker_file );
		while( readNextSequence( reader1 ) ) {
			fprintf( writer, ">%s\n", reader1->currentID );
			readNextSequence( reader2 );
			char* sequence1 = reader1->currentSequence;
			char* sequence2 = reader2->currentSequence;
			int length = strlen( sequence1 );
			int number = -1;
			for( int i = 0; i < length; i++ ) {
				int number2 = (int) sequence2[ i ];
				if( sequence2[ i ] == ' ' ) {
					if( number > -1 ) fprintf( writer, "</SPAN>" );
					number = -1;
					fprintf( writer, "%c", sequence1[ i ] );
					continue;
				}
				if( number == number2 ) {
					fprintf( writer, "%c", sequence1[ i ] );
					continue;
				}
				if( number > -1 ) fprintf( writer, "</SPAN>" );
				fprintf( writer, "<SPAN CLASS=\"marker%d\">%c", number2, sequence1[ i ] );
				number = number2;
			}
			if( number > -1 ) fprintf( writer, "</SPAN>" );
			fprintf( writer, "\n" );
		}
		freeSequenceReader( reader1 );
		freeSequenceReader( reader2 );
	}
	fprintf( writer, "</PRE>\n" );
	
	for( int i = 0; i < 256; i++ ) free( styles[ i ] );
	free( styles );
	freeStringVector( input_files );
	freeStringVector( marker_files );
	closeHandler( writer, output_file );
	freeInterface( interface );
	return 0;
}
