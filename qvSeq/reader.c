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

#include "reader.h"

/******************** READER ********************/

struct SequenceReader* newSequenceReader( char* path ) {
	struct SequenceReader* constructor = 0;
	constructor = malloc( sizeof( struct SequenceReader ) );
	// reader status
	constructor->report = REPORT_COUNT;
	constructor->sequenceCount = 0;
	// filename information
	constructor->path = 0;
	constructor->path = malloc( sizeof( char ) * MAX_FILENAME);
	strcpy( constructor->path, path );
	constructor->basename = 0;
	constructor->basename = malloc( sizeof( char ) * MAX_FILENAME );
	getFilename( path, constructor->basename );
	// line information
	constructor->previousLine = 0;
	constructor->previousLine = malloc( sizeof( char ) * MAX_LINE );
	strcpy( constructor->previousLine, "" );
	constructor->currentLine = 0;
	constructor->currentLine = malloc( sizeof( char ) * MAX_LINE );
	strcpy( constructor->currentLine, "" );
	// actual information
	constructor->currentID = 0;
	constructor->currentID = malloc( sizeof( char ) * MAX_LINE );
	constructor->currentID2 = 0;
	constructor->currentID2 = malloc( sizeof( char ) * MAX_LINE );
	constructor->currentSequence = 0;
	constructor->currentSequence = malloc( sizeof( char ) * MAXIMUM_SEQUENCE_LENGTH );
	constructor->currentQV = 0;
	constructor->currentQV = malloc( sizeof( char ) * MAXIMUM_SEQUENCE_LENGTH );
	// SAM format
	constructor->flag = 0;
	constructor->rname = 0;
	constructor->rname = malloc( sizeof( char ) * MAX_LINE );
	constructor->pos   = 0;
	constructor->mapq  = 0;
	constructor->cigar = 0;
	constructor->cigar = malloc( sizeof( char ) * MAX_LINE );
	constructor->rnext = 0;
	constructor->rnext = malloc( sizeof( char ) * MAX_LINE );
	constructor->pnext = 0;
	constructor->tlen = 0;
	constructor->option = 0;
	constructor->option = malloc( sizeof( char ) * MAX_LINE );
	// count format
	constructor->count = 0;
	// filehandler information
	constructor->type = FASTQ; // default
	constructor->reader = &readNextFASTQ; // default
	constructor->print = &printCurrentFASTQ; // default
	constructor->filehandler = openReader( path );
	constructor->headers = 0;
	constructor->headers = newStringVector();
	// remove .gz or .bz2 suffix from a basename
	int length = strlen( constructor->basename );
	if( strcmp( ".gz", constructor->basename + ( length - 3 ) ) == 0 ) constructor->basename[ length - 3 ] = '\0';
	else if( strcmp( ".bz2", constructor->basename + ( length - 4 ) ) == 0 ) constructor->basename[ length - 4 ] = '\0';
	length = strlen( constructor->basename );
	// handle file format by the name
	if( strcmp( ".fa", constructor->basename + ( length - 3 ) ) == 0 ) {
		constructor->type = FASTA;
		constructor->basename[ length - 3 ] = '\0';
	} else if( strcmp( ".fq", constructor->basename + ( length - 3 ) ) == 0 ) {
		constructor->type = FASTQ;
		constructor->basename[ length - 3 ] = '\0';
	} else if( strcmp( ".sam", constructor->basename + ( length - 4 ) ) == 0 ) {
		constructor->type = SAM;
		constructor->basename[ length - 4 ] = '\0';
	} else if( strcmp( ".bam", constructor->basename + ( length - 4 ) ) == 0 ) {
		constructor->type = SAM;
		constructor->basename[ length - 4 ] = '\0';
	} else if( strcmp( ".txt", constructor->basename + ( length - 4 ) ) == 0 ) {
		constructor->type = TEXT;
		constructor->basename[ length - 4 ] = '\0';
	} else if( strcmp( ".text", constructor->basename + ( length - 5 ) ) == 0 ) {
		constructor->type = TEXT;
		constructor->basename[ length - 5 ] = '\0';
	} else if( strcmp( ".uniq", constructor->basename + ( length - 5 ) ) == 0 ) {
		constructor->type = UNIQ;
		constructor->basename[ length - 5 ] = '\0';
	} else { // set file format by looking at the first line
		intializeSequenceReader( constructor );
	}
	// set reader and printer
	if( constructor->type == FASTA ) {
		constructor->reader = &readNextFASTA;
		constructor->print  = &printCurrentFASTA;
	} else if( constructor->type == FASTQ ) {
		constructor->reader = &readNextFASTQ;
		constructor->print  = &printCurrentFASTQ;
	} else if( constructor->type == SAM ) {
		constructor->reader = &readNextSAM;
		constructor->print  = &printCurrentSAM;
	} else if( constructor->type == UNIQ ) {
		constructor->reader = &readNextUNIQ;
		constructor->print  = &printCurrentUNIQ;
	} else if( constructor->type == TEXT ) {
		constructor->reader = &readNextTEXT;
		constructor->print  = &printCurrentTEXT;
	}
	return constructor;
}

void freeSequenceReader( struct SequenceReader* reader ) {
	fprintf( stderr, "Sequences read = %ld\n", reader->sequenceCount );
	closeHandler( reader->filehandler, reader->path );
	freeStringVector( reader->headers );
	free( reader->path );
	free( reader->basename );
	free( reader->previousLine );
	free( reader->currentLine );
	free( reader->currentSequence );
	free( reader->currentQV );
	free( reader->currentID );
	free( reader->currentID2 );
	free( reader->rname );
	free( reader->cigar );
	free( reader->rnext );
	free( reader->option );
	free( reader );
}

void removeReadInformation( struct SequenceReader* reader ) {
	for( int i = 0; i < strlen( reader->currentID );  i++ ) if( reader->currentID[ i ]  == '#' || reader->currentID[ i ]  == ' ' ) reader->currentID[ i ]  = '\0';
	for( int i = 0; i < strlen( reader->currentID2 ); i++ ) if( reader->currentID2[ i ] == '#' || reader->currentID2[ i ] == ' ' ) reader->currentID2[ i ] = '\0';
}

void intializeSequenceReader( struct SequenceReader* reader ) {
	char* line = readNextLine( reader );
	if( line[ 0 ] == '>' ) { // FASTA starts with '>'
		strcpy( reader->previousLine, line ); // remember in previous line storage
		reader->type = FASTA; // set type
		return;
	}
	if( line[ 0 ] == '@' ) { // FASTQ
		if( ( line[ 3 ] == ' ' ) || ( line[ 3 ] == '\t' ) ) { // probably SAM starting with @HD, @SQ, @RG, @PG, @CO
			addStringVector( reader->headers, line ); // remember header lines
		} else { // FASTQ
			strcpy( reader->previousLine, line ); // remember in previous line storage
			reader->type = FASTQ; // set type
			return;
		}
		line = readNextLine( reader ); // read next line
		while( line != NULL ) { // read until end of header lines
			if( line[ 0 ] != '@' ) { // found end of header lines
				reader->type = SAM; // set type
				strcpy( reader->previousLine, line ); // remember in previous line storage
				return; // done
			}
			line = readNextLine( reader ); // read next line
		}
	} else if( countChar( line, '\t' ) == 1 ) { // sequence	count
		reader-> type = UNIQ; // unique count
		strcpy( reader->previousLine, line ); // remember in previous line storage
		return; // done
	}
	int count = countChar( line, '\t' ); // count number of tags
	if( count >= 10 ) { // SAM file format has more than 10 columns
		reader->type = SAM; // set type
		strcpy( reader->previousLine, line ); // remember in previous line storage
		return; // done
	}
	reader->type = TEXT; // set to default type
	strcpy( reader->previousLine, line ); // remember in previous line storage
}

int hasCurrentSequence( struct SequenceReader* reader ) {
	return strlen( reader->currentID ) > 0;
}

char* readNextLine( struct SequenceReader* reader ) {
	if( strlen( reader->previousLine ) > 0 ) { // if there is previous line
		strcpy( reader->currentLine, reader->previousLine ); // copy previous => current
		strcpy( reader->previousLine, "" ); // erase previous
		return reader->currentLine; // return copied current line
	} else { // no previous line found
		if( reader->filehandler == NULL ) return NULL;
		while( fgets( reader->currentLine, MAX_LINE, reader->filehandler ) ) { // read from file
			chomp( reader->currentLine ); // remove '¥n'
			return reader->currentLine; // return current line
		}
		return NULL; // end of line
	}
}

int readNextSequence( struct SequenceReader* reader ) {
	int result = reader->reader( reader ); // use different reader
	if( result ) {
			reader->sequenceCount++; // increment sequence count
		if( reader->sequenceCount >= reader->report ) { fprintf( stderr, "%ld sequences processed.\n", reader->report ); reader->report += REPORT_COUNT; } // increment
	}
	return result;
}

int readNextTEXT( struct SequenceReader* reader ) {
	reader->count = 1;
	char* line = readNextLine( reader ); // read next line
	if( line == NULL ) return 0;
	else return 1;
}

int readNextFASTA( struct SequenceReader* reader ) {
	reader->count = 1;
	strcpy( reader->currentID, "" ); // reset
	strcpy( reader->currentID2, "" ); // reset
	strcpy( reader->currentSequence, "" ); // reset
	strcpy( reader->currentQV, "" ); // reset
	int status = 0; // 1 means sequence
	char* line = readNextLine( reader ); // read next line
	while( line != NULL ) { // while line is readable
		if( line[ 0 ] == '>' ) { // sequence ID
			if( status == 1 ) { // it's been read
				strcpy( reader->previousLine, line ); // remember current line
				return 1; // read OK
			} else { // first time it hits '>'
				int index = 1; // index
				while( line[ index ] == ' ' || line[ index ] == '\t' ) index++; // skip spaces before ID
				strcpy( reader->currentID, line + index ); // remember current sequence ID
				strcpy( reader->currentID2, reader->currentID ); // remember current QC ID
				strcpy( reader->currentSequence, "" ); // set current sequence to empty
				status = 1; // '>' been hit
			}
		} else if( status == 1 ) { // '>' has been hit
			strcat( reader->currentSequence, line ); // concat sequence after current sequence
		}
		line = readNextLine( reader ); // read next line
	}
	if( status == 1 ) return 1; // last read
	else return 0; // not read at all...
}

int readNextFASTQ( struct SequenceReader* reader ) {
	reader->count = 1;
	strcpy( reader->currentID, "" ); // reset
	strcpy( reader->currentID2, "" ); // reset
	strcpy( reader->currentSequence, "" ); // reset
	strcpy( reader->currentQV, "" ); // reset
	// 0 = sequence ID, 1 = sequence line, 2 = QV ID, 3 = QV line
	int status = 0;
	char* line = readNextLine( reader ); // read next line
	while( line != NULL ) { // while line is readable
		if( status == 0 && line[ 0 ] == '@' ) { // sequence ID
			int index = 1; // ID start index
			while( line[ index ] == ' ' || line[ index ] == '\t' ) index++; // skip spaces before ID
			strcpy( reader->currentID, line + index ); // remember sequence ID
			status = 1; // sequence ID line found
		} else if( status == 1 ) { // sequence ID found
			strcpy( reader->currentSequence, line ); // remember sequence line
			status = 2;
		} else if( status == 2 && line[ 0 ] == '+' ) { // QV ID
			int index = 1; // ID start index
			while( line[ index ] == ' ' || line[ index ] == '\t' ) index++; // skip spaces before ID
			strcpy( reader->currentID2, line + index ); // remember QC ID
			status = 3; // QC ID line found
		} else if( status == 3 ) { // QC ID found
			strcpy( reader->currentQV, line ); // remember QC line
			return 1; // OK
		}
		line = readNextLine( reader ); // read next line
	}
	return 0; // NO OK
}

int readNextSAM( struct SequenceReader* reader ) {
	strcpy( reader->currentID, "" ); // reset
	strcpy( reader->currentID2, "" ); // reset
	strcpy( reader->currentSequence, "" ); // reset
	strcpy( reader->currentQV, "" ); // reset
	strcpy( reader->rname, "" ); // reset
	strcpy( reader->cigar, "" ); // reset
	strcpy( reader->rnext, "" ); // reset
	reader->flag  = 0;
	reader->pos   = 0;
	reader->mapq  = 0;
	reader->pnext = 0;
	reader->tlen  = 0;
	reader->count = 1;
	char temp[ MAX_LINE ];
	char* line = readNextLine( reader ); // read next line
	if( line == NULL ) return 0;
	while( line[ 0 ] == '@' ) {
		addStringVector( reader->headers, line ); // remember header lines
		line = readNextLine( reader ); // read next line
		if( line == NULL ) return 0;
	}
	if( countChar( line, '\t' ) < 10 ) return 0; // not SAM format?
	int index = 0;
	// QNAME
	int index2 = getIndex( line, '\t', index );
	extract( line, reader->currentID, index, index2 );
	// FLAG
	index = index2 + 1;
	index2 = getIndex( line, '\t', index );
	extract( line, temp, index, index2 );
	reader->flag = atoi( temp );
	// RNAME
	index = index2 + 1;
	index2 = getIndex( line, '\t', index );
	extract( line, reader->rname, index, index2 );
	// POS
	index = index2 + 1;
	index2 = getIndex( line, '\t', index );
	extract( line, temp, index, index2 );
	reader->pos = atoi( temp );
	// MAPQ
	index = index2 + 1;
	index2 = getIndex( line, '\t', index );
	extract( line, temp, index, index2 );
	reader->mapq = atoi( temp );
	// CIGAR
	index = index2 + 1;
	index2 = getIndex( line, '\t', index );
	extract( line, reader->cigar, index, index2 );
	// RNEXT
	index = index2 + 1;
	index2 = getIndex( line, '\t', index );
	extract( line, reader->rnext, index, index2 );
	// PNEXT
	index = index2 + 1;
	index2 = getIndex( line, '\t', index );
	extract( line, temp, index, index2 );
	reader->pnext = atoi( temp );
	// TLEN
	index = index2 + 1;
	index2 = getIndex( line, '\t', index );
	extract( line, temp, index, index2 );
	reader->tlen = atoi( temp );
	// SEQ
	index = index2 + 1;
	index2 = getIndex( line, '\t', index );
	extract( line, reader->currentSequence, index, index2 );
	// QUAL
	index = index2 + 1;
	index2 = getIndex( line, '\t', index );
	extract( line, reader->currentQV, index, index2 );
	// OPTION
	index = index2 + 1;
	extract( line, reader->option, index, strlen( line ) );
	return 1;
}

int readNextUNIQ( struct SequenceReader* reader ) {
	strcpy( reader->currentID, "" ); // reset
	strcpy( reader->currentID2, "" ); // reset
	strcpy( reader->currentSequence, "" ); // reset
	strcpy( reader->currentQV, "" ); // reset
	char temp[ MAX_LINE ];
	char* line = readNextLine( reader ); // read next line
	if( line == NULL ) return 0;
	if( countChar( line, '\t' ) < 1 ) return 0; // not SAM format?
	int index = 0;
	// SEQUENCE
	int index2 = getIndex( line, '\t', index );
	extract( line, reader->currentSequence, index, index2 );
	// COUNT
	index = index2 + 1;
	extract( line, temp, index, strlen( line ) );
	reader->count = atoi( temp );
	// ID
	strcpy( reader->currentID, reader->currentSequence );
	strcat( reader->currentID, "_" );
	strcat( reader->currentID, temp );
	return 1;
}

void printCurrentSequence( struct SequenceReader* reader, FILE* writer ) { reader->print( reader, writer ); }

void printSAMHeaders( struct SequenceReader* reader, FILE* writer ) {
	resetStringVectorIndex( reader->headers ); // go to beginning
	while( hasMoreStringVector( reader->headers ) ) { // continue until end of header line
		char* header = nextStringVector( reader->headers ); // get next header line
		fprintf( writer, "%s\n", header ); // print header line
	}
}

void printCurrentFASTA( struct SequenceReader* reader, FILE* writer ) {
	if( writer == NULL ) return; // nothing to print out
	if( strlen( reader->currentID ) == 0 ) return; // No ID, so don't print
	fprintf( writer, ">%s\n", reader->currentID ); // print ID line
	fprintf( writer, "%s\n", reader->currentSequence ); // print sequence line
}

void printCurrentFASTQ( struct SequenceReader* reader, FILE* writer ) {
	if( writer == NULL ) return; // there is no output.
	if( strlen( reader->currentID ) == 0 ) return; // If there is no ID, don't print
	fprintf( writer, "@%s\n", reader->currentID ); // print current ID
	fprintf( writer, "%s\n", reader->currentSequence ); // assumes there is a sequence
	fprintf( writer, "+%s\n", reader->currentID2 ); // ID2 can be empty
	if( strlen( reader->currentQV ) == 0 ) { // no quality values found
		int length = strlen( reader->currentSequence ); // length of sequence
		for( int i = 0; i < length; i++ ) fprintf( writer, "%c", '/' ); // default quality values
		fprintf( writer, "\n" ); // line return
	} else fprintf( writer, "%s\n", reader->currentQV ); // print out quality values
}

void printCurrentSAM( struct SequenceReader* reader, FILE* writer ) {
	if( writer == NULL ) return; // nothing to print out
	if( strlen( reader->currentID ) == 0 ) return; // no ID, so don't print
	fprintf( writer, "%s\t%d\t%s\t%d\t%d\t%s\t%s\t%d\t%d\t%s\t%s\t%s\n", reader->currentID, reader->flag, reader->rname, reader->pos, reader->mapq, reader->cigar, reader->rnext, reader->pnext, reader->tlen, reader->currentSequence, reader->currentQV, reader->option );
	return;
}

void printCurrentUNIQ( struct SequenceReader* reader, FILE* writer ) {
	if( writer == NULL ) return; // nothing to print out
	if( strlen( reader->currentID ) == 0 ) return; // no ID, so don't print
	fprintf( writer, "%s\t%d\n", reader->currentID, reader->count );
	return;
}

void printCurrentTEXT( struct SequenceReader* reader, FILE* writer ) {
	if( writer == NULL ) return; // nothing to print out
	if( strlen( reader->currentLine ) == 0 ) return; // no ID, so don't print
	fprintf( writer, "%s\n", reader->currentLine );
	return;
}

int getLowerLocation( struct SequenceReader* reader ) { return reader->pos; }

int getUpperLocation( struct SequenceReader* reader ) { return reader->pos + getCigarLength( reader->cigar ) - 1; }

int getStartLocation( struct SequenceReader* reader ) {
	if( ( reader->flag & 16 ) > 0 ) return getLowerLocation( reader );
	else return getUpperLocation( reader );
}

int getEndLocation( struct SequenceReader* reader ) {
	if( ( reader->flag & 16 ) > 0 ) return getUpperLocation( reader );
	else return getLowerLocation( reader );
}

/******************** UTILITY ********************/

struct StringVector* getChromosomes( struct SequenceReader* reader ) {
	// @SQ	SN:chr1	LN:197195432
	char temp[ 100 ];
	strcpy( temp, "^@SQ\tSN:(.+)\t" );
	struct StringVector* vector = newStringVector();
	struct RegularExpression* expression = newRegularExpression( temp );
	resetStringVectorIndex( reader->headers ); // go to beginning
	while( hasMoreStringVector( reader->headers ) ) { // continue until end of header line
		char* header = nextStringVector( reader->headers ); // get next header line
		if( matchRegularExpression( expression, header ) ) {
			getRegularExpression( expression, 1, temp );
			addStringVector( vector, temp );
		}
	}
	return vector;
}

int getChromosomeLength( struct SequenceReader* reader, char* chromosome ) {
	// @SQ	SN:chr1	LN:197195432
	char temp[ 100 ];
	strcpy( temp, "^@SQ\tSN:" );
	strcat( temp, chromosome );
	strcat( temp, "\tLN:(.+)$" );
	struct RegularExpression* expression = newRegularExpression( temp );
	resetStringVectorIndex( reader->headers ); // go to beginning
	while( hasMoreStringVector( reader->headers ) ) { // continue until end of header line
		char* header = nextStringVector( reader->headers ); // get next header line
		if( matchRegularExpression( expression, header ) ) {
			getRegularExpression( expression, 1, temp );
			return atoi( temp );
		}
	}
	freeRegularExpression( expression );
	return 0;
}

int getCigarLength( char* cigar ) {
	int length = 0;
	int size = strlen( cigar );
	int index1 = 0;
	char temp[ 100 ];
	for( int index2 = 1; index2 < size; index2++ ) {
		if( cigar[ index2 ] == 'M' ) { // M alignment match
			extract( cigar, temp, index1, index2 );
			length += atoi( temp );
			index1 = index2 + 1;
		} else if( cigar[ index2 ] == 'I' ) { // I insertion
			extract( cigar, temp, index1, index2 );
			length += atoi( temp );
			index1 = index2 + 1;
		} else if( cigar[ index2 ] == 'D' ) { // D deletion
			extract( cigar, temp, index1, index2 );
			length -= atoi( temp );
			index1 = index2 + 1;
		} else if( cigar[ index2 ] == 'N' ) { // N skipped region
			extract( cigar, temp, index1, index2 );
			length += atoi( temp );
			index1 = index2 + 1;
		} else if( cigar[ index2 ] == 'S' ) { // S soft clipping
			extract( cigar, temp, index1, index2 );
			length += atoi( temp );
			index1 = index2 + 1;
		} else if( cigar[ index2 ] == 'H' ) { // H hard clipping
			extract( cigar, temp, index1, index2 );
			length += atoi( temp );
			index1 = index2 + 1;
		} else if( cigar[ index2 ] == 'P' ) { // P padding
			extract( cigar, temp, index1, index2 );
			length += atoi( temp );
			index1 = index2 + 1;
		} else if( cigar[ index2 ] == '=' ) { // = sequence match
			extract( cigar, temp, index1, index2 );
			length += atoi( temp );
			index1 = index2 + 1;
		} else if( cigar[ index2 ] == 'X' ) { // X sequence mismatch
			extract( cigar, temp, index1, index2 );
			length += atoi( temp );
			index1 = index2 + 1;
		}
	}
	return length;
}

int checkSequenceIDs( char* sequenceID, char* sequenceID2 ) {
	int length = strlen( sequenceID );
	for( int i = 0; i < length; i++ ) {
		if( sequenceID[ i ] == sequenceID2[ i ] ) {
			if( sequenceID[ i ] == '#' && sequenceID2[ i ] == '#' ) break;
			else if( sequenceID[ i ] == ' ' && sequenceID2[ i ] == ' ' ) break;
		} else { // different
			return 0;
		}
	}
	return 1;
}
