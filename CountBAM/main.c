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
	fprintf( stdout, "Program: Count line and size of BAM files.\n" );
	fprintf( stdout, "Author: Akira Hasegawa (ah3q@gsc.riken.jp)\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Usage: CountBAM INPUT [INPUT2 ..] > OUTPUT\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "       INPUT   Input BAM file\n" );
	fprintf( stdout, "       OUTPUT  Line count and file size of BAM files.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Output: test/input.bam   2102975 B\n" );
	fprintf( stdout, "        test/input.bam   7249 line\n" );
	fprintf( stdout, "        test/input.bam   27 head line\n" );
	fprintf( stdout, "        test/input.bam   7222 BAM line\n" );
	fprintf( stdout, "        test/input.bam   6920 primary alignment\n" );
	fprintf( stdout, "        test/input.bam   302 secondary alignment\n" );
	fprintf( stdout, "        test/input.bam   0 unmapped sequence\n" );
	fprintf( stdout, "        test/input.bam   3207 positive strand\n" );
	fprintf( stdout, "        test/input.bam   4015 negative strand\n" );
	fprintf( stdout, "        test/input.bam   6920 sequence\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Output(paired-end): input.bam   373224586 B\n" );
	fprintf( stdout, "                    input.bam   10205566 line\n" );
	fprintf( stdout, "                    input.bam   0 head line\n" );
	fprintf( stdout, "                    input.bam   6195295 sequence line\n" );
	fprintf( stdout, "                    input.bam   981206 primary alignment\n" );
	fprintf( stdout, "                    input.bam   147097 secondary alignment\n" );
	fprintf( stdout, "                    input.bam   0 unmapped sequence\n" );
	fprintf( stdout, "                    input.bam   3327744 positive strand\n" );
	fprintf( stdout, "                    input.bam   2867551 negative strand\n" );
	fprintf( stdout, "                    input.bam   1128303 properly aligned\n" );
	fprintf( stdout, "                    input.bam   5066992 inadequately aligned\n" );
	fprintf( stdout, "                    input.bam   4010271 read2 sequence line\n" );
	fprintf( stdout, "                    input.bam   981206 read2 primary alignment\n" );
	fprintf( stdout, "                    input.bam   147097 read2 secondary alignment\n" );
	fprintf( stdout, "                    input.bam   0 read2 unmapped sequence\n" );
	fprintf( stdout, "                    input.bam   1901701 read2 positive strand\n" );
	fprintf( stdout, "                    input.bam   2108570 read2 negative strand\n" );
	fprintf( stdout, "                    input.bam   1128303 read2 properly aligned\n" );
	fprintf( stdout, "                    input.bam   2881968 read2 inadequately aligned\n" );
	fprintf( stdout, "                    input.bam   981206 sequence\n" );
	fprintf( stdout, "                    input.bam   981206 read2 sequence\n" );
	fprintf( stdout, "                    input.bam   981206 read1 primary alignment\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Note: This program is used by MOIRAI to count sequences in BAM file.\n" );
	fprintf( stdout, "      samtools is used to open and read BAM file.\n" );
	fprintf( stdout, "\n" );
	fprintf( stdout, "Updates: 2013/05/14  'sequence' added to enable MOIRAI to track numbers.\n" );
	fprintf( stdout, "         2011/11/22  Outputs statistical counts of BAM file.\n" );
	fprintf( stdout, "\n" );
}

int main ( int argc, char* argv[] ) {
	if( argc == 2 && strcmp( argv[ 1 ], "-h" ) == 0 ) {
        help();
        return 0;
    }
	for( int i = 1; i < argc; i++ ) {
		long int lineCount           = 0;
		long int headCount           = 0;
		long int sequenceCount       = 0;
		long int sequenceCount2      = 0; // read2 only
		long int unmappedSeq         = 0;
		long int unmappedSeq2        = 0; // read2 only
		long int primaryAlign        = 0;
		long int primaryAlign2       = 0; // read2 only
		long int secondaryAlign      = 0;
		long int secondaryAlign2     = 0; // read2 only
		long int positiveStrand      = 0;
		long int positiveStrand2     = 0; // read2 only
		long int negativeStrand      = 0;
		long int negativeStrand2     = 0; // read2 only
		long int properlyAligned     = 0;
		long int properlyAligned2    = 0; // read2 only
		long int inadequateAlign     = 0;
		long int inadequateAlign2    = 0; // read2 only
		long int read1PrimaryAlign   = 0;
		long int read2PrimaryAlign   = 0; // read2 only
		long int read1SecondaryAlign = 0;
		long int read2SecondaryAlign = 0; // read2 only
		char line[ MAX_LINE ];
		char temp[ MAX_LINE ];
		FILE* reader = openReader( argv[ i ] );
		int paired_flag = 0;
		while( fgets( line, MAX_LINE, reader ) ) {
			lineCount++;
			if( line[ 0 ] == '@' ) { headCount++; continue; }
			int flag = 0;
			int column = 0;
			int start = 0;
			for( int index = 0; index < strlen( line ); index++ ) {
				if( line[ index ] == '\t' ) { // look for first tab
					if( column == 0 ) { // ID
						start = index + 1;
					} else if( column == 1 ) { // flag
						int size = index - start;
						strncpy( temp, line + start, size );
						temp[ size ] = '\0';
						flag = atoi( temp );
						start = index + 1;
						break;
					}
					column++;
				}
			}
			if( ( flag & 1 ) > 0 ) { // template having multiple segments in squencing 0x1
				paired_flag = 1;
				if( ( flag & 4 ) > 0 ) { // segment unmapped
					if( ( flag & 64 ) > 0 ) unmappedSeq++; // first sequence
					else if( ( flag & 128 ) > 0 ) unmappedSeq2++; // second sequence
				} else {
					if( ( flag & 64 ) > 0 ) { // first sequence
						if( ( flag & 16 ) > 0 ) { negativeStrand++; } // SEQ being reverse complemented
						else { positiveStrand++; } // forward strand
					} else if( ( flag & 128 ) > 0 ) { // second sequence
						if( ( flag & 16 ) > 0 ) { negativeStrand2++; } // SEQ being reverse complemented
						else { positiveStrand2++; } // forward strand
					}
					if( ( flag & 2 ) > 0 ) { // each segment properly aligned according to the aligner 0x2
						if( ( flag & 64 ) > 0 ) { // first sequence
							properlyAligned++; // number of properly mapped read1 tags
							if( ( flag & 256 ) > 0 ) { read1SecondaryAlign++; secondaryAlign++; } // secondary alignment 0x100
							else { read1PrimaryAlign++; primaryAlign++; } // primary alignment of read1
						} else if ( ( flag & 128 ) > 0 ) { // second sequence
							properlyAligned2++; // number of properly mapped read2 tags
							if( ( flag & 256 ) > 0 ) { read2SecondaryAlign++; secondaryAlign2++; } // secondary alignment 0x100
							else { read2PrimaryAlign++; primaryAlign2++; } // primary alignment of read2
						}
					} else { // not properly aligned
						if( ( flag & 64 ) > 0 ) {
							inadequateAlign++; // first sequence
							if( ( flag & 256 ) > 0 ) read1SecondaryAlign++; // secondary alignment 0x100
							else read1PrimaryAlign++; // primary alignment of read1
						} else if( ( flag & 128 ) > 0 ) {
							inadequateAlign2++; // second sequence
							if( ( flag & 256 ) > 0 ) read2SecondaryAlign++; // secondary alignment 0x100
							else read2PrimaryAlign++; // primary alignment of read2
						}
					}
				}
				if( ( flag & 64 ) > 0 ) sequenceCount++; // first sequence
				else if( ( flag & 128 ) > 0 ) sequenceCount2++; // second sequence
			} else { // single end
				if( ( flag & 4 ) > 0 ) { unmappedSeq++; } // segment unmapped
				else { // mapped
					if( ( flag & 16 ) > 0 ) { negativeStrand++; } // SEQ being reverse complemented
					else { positiveStrand++; } // forward strand
					if( ( flag & 256 ) > 0 ) secondaryAlign++; // secondary alignment 0x100
					else primaryAlign++; // primary alignment
				}
				sequenceCount++;
			}
		}
		closeHandler( reader, argv[ i ] );
		if( paired_flag == 1 ) { // paired-end
			fprintf( stdout, "%s\t%ld B\n", argv[ i ], getFileSize( argv[ i ] ) );
			fprintf( stdout, "%s\t%ld line\n",          argv[ i ], lineCount );
			fprintf( stdout, "%s\t%ld head line\n",     argv[ i ], headCount );
			fprintf( stdout, "%s\t%ld BAM line\n", argv[ i ], sequenceCount );
			fprintf( stdout, "%s\t%ld primary alignment\n",   argv[ i ], primaryAlign );
			fprintf( stdout, "%s\t%ld secondary alignment\n", argv[ i ], secondaryAlign );
			fprintf( stdout, "%s\t%ld unmapped sequence\n",   argv[ i ], unmappedSeq );
			fprintf( stdout, "%s\t%ld positive strand\n", argv[ i ], positiveStrand );
			fprintf( stdout, "%s\t%ld negative strand\n", argv[ i ], negativeStrand );
			fprintf( stdout, "%s\t%ld properly aligned\n",      argv[ i ], properlyAligned );
			fprintf( stdout, "%s\t%ld inadequately aligned\n",  argv[ i ], inadequateAlign );
			fprintf( stdout, "%s\t%ld read2 BAM line\n", argv[ i ], sequenceCount2  );
			fprintf( stdout, "%s\t%ld read2 primary alignment\n", argv[ i ], primaryAlign2   );
			fprintf( stdout, "%s\t%ld read2 secondary alignment\n", argv[ i ], secondaryAlign2 );
			fprintf( stdout, "%s\t%ld read2 unmapped sequence\n", argv[ i ], unmappedSeq2    );
			fprintf( stdout, "%s\t%ld read2 positive strand\n", argv[ i ], positiveStrand2 );
			fprintf( stdout, "%s\t%ld read2 negative strand\n", argv[ i ], negativeStrand2 );
			fprintf( stdout, "%s\t%ld read2 properly aligned\n", argv[ i ], properlyAligned2 );
			fprintf( stdout, "%s\t%ld read2 inadequately aligned\n", argv[ i ], inadequateAlign2 );
			fprintf( stdout, "%s\t%ld sequence\n", argv[ i ], primaryAlign );
			fprintf( stdout, "%s\t%ld read2 sequence\n", argv[ i ], primaryAlign2 );
			fprintf( stdout, "%s\t%ld read1 primary alignment\n", argv[ i ], read1PrimaryAlign );
			
		} else { // single end
			fprintf( stdout, "%s\t%ld B\n", argv[ i ], getFileSize( argv[ i ] ) );
			fprintf( stdout, "%s\t%ld line\n",          argv[ i ], lineCount );
			fprintf( stdout, "%s\t%ld head line\n",     argv[ i ], headCount );
			fprintf( stdout, "%s\t%ld BAM line\n", argv[ i ], sequenceCount );
			fprintf( stdout, "%s\t%ld primary alignment\n",   argv[ i ], primaryAlign );
			fprintf( stdout, "%s\t%ld secondary alignment\n", argv[ i ], secondaryAlign );
			fprintf( stdout, "%s\t%ld unmapped sequence\n",   argv[ i ], unmappedSeq );
			fprintf( stdout, "%s\t%ld positive strand\n", argv[ i ], positiveStrand );
			fprintf( stdout, "%s\t%ld negative strand\n", argv[ i ], negativeStrand );
			fprintf( stdout, "%s\t%ld sequence\n",   argv[ i ], primaryAlign );
		}
	}
	return 0;
}

/******************** IO ********************/

FILE* openReader( char* filename ) {
	char command[ 1000 ];
	FILE* handler = 0;
	if( strcmp( filename, "-" ) == 0 ) return stdin;
	else if( strcmp( filename, "stdin" ) == 0 ) return stdin;
	else if( strcmp( filename, "STDIN" ) == 0 ) return stdin;
	else if( strlen( filename ) == 0 ) return stdin;
	else if( strcmp( ".bam", filename + ( strlen( filename ) - 4 ) ) == 0 ) {
		strcpy( command, "" );
		strcat( command, "samtools view " );
		strcat( command, filename );
		if( ! ( handler = popen( command, "r" ) ) ) {
			fprintf( stderr, "Can't open BAM file \"%s\".\n", filename );
			exit( -1 );
		}
	} else if( strcmp( ".gz", filename + ( strlen( filename ) - 3 ) ) == 0 ) {
		strcpy( command, "" );
		strcat( command, "gzip -cd " );
		strcat( command, filename );
		if( ! ( handler = popen( command, "r" ) ) ) {
			strcpy( command, "" );
			strcat( command, "zcat -d " );
			strcat( command, filename );
			if( ! ( handler = popen( command, "r" ) ) ) {
				fprintf( stderr, "Can't open gzip input file \"%s\".\n", filename );
				exit( -1 );
			}
		}
	} else if( strcmp( ".bz2", filename + ( strlen( filename ) - 4 ) ) == 0 ) {
		strcpy( command, "" );
		strcat( command, "bzip2 -cd " );
		strcat( command, filename );
		if( ! ( handler = popen( command, "r" ) ) ) {
			strcpy( command, "" );
			strcat( command, "zcat -d " );
			strcat( command, filename );
			if( ! ( handler = popen( command, "r" ) ) ) {
				fprintf( stderr, "Can't open bzip2 input file \"%s\".\n", filename );
				exit( -1 );
			}
		}
	} else {
		handler = fopen( filename, "r" );
	}
	return handler;
}

void closeHandler( FILE* file, char* filename ) {
	if( file == NULL ) return;
	if( strcmp( ".gz", filename + ( strlen( filename ) - 3 ) ) == 0 ) pclose( file );
	else if( strcmp( ".bz2", filename + ( strlen( filename ) - 4 ) ) == 0 ) pclose( file );
	else fclose( file );
}

long int getFileSize( char* filename ) {
	FILE* reader = fopen( filename, "rb" );
	fseek( reader, 0, SEEK_END );
	long int fsize = ftell( reader );
	fclose( reader );
	return fsize;
}
