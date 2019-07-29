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

#include "sam_reader.h"

/******************** BAM READER ********************/

struct SAMReader* newSAMReader( char* path ) {
	struct SAMReader* constructor = 0;
	constructor = malloc( sizeof( struct SAMReader ) );
	constructor->sequenceReader = newSequenceReader( path );
	constructor->rootData = newSAMData();
	constructor->currentData = 0;
	return constructor;
}

void freeSAMReader( struct SAMReader* reader ) {
	freeSequenceReader( reader->sequenceReader );
	freeSAMData( reader->rootData );
	free( reader );
}

struct SAMData* newSAMData() {
	struct SAMData* constructor = 0;
	constructor = malloc( sizeof( struct SAMData ) );
	constructor->qname  = 0;
	constructor->flag   = 0;
	constructor->rname  = 0;
	constructor->pos    = 0;
	constructor->mapq   = 0;
	constructor->cigar  = 0;
	constructor->rnext  = 0;
	constructor->pnext  = 0;
	constructor->tlen   = 0;
	constructor->seq    = 0;
	constructor->qual   = 0;
	constructor->option = 0;
	constructor->qname  = malloc( sizeof( char ) * ( MAXIMUM_LINE_LENGTH + 1 ) );
	constructor->rname  = malloc( sizeof( char ) * ( MAXIMUM_LINE_LENGTH + 1 ) );
	constructor->cigar  = malloc( sizeof( char ) * ( MAXIMUM_LINE_LENGTH + 1 ) );
	constructor->rnext  = malloc( sizeof( char ) * ( MAXIMUM_LINE_LENGTH + 1 ) );
	constructor->seq    = malloc( sizeof( char ) * ( MAXIMUM_SEQUENCE_LENGTH + 1 ) );
	constructor->qual   = malloc( sizeof( char ) * ( MAXIMUM_SEQUENCE_LENGTH + 1 ) );
	constructor->option = malloc( sizeof( char ) * ( MAXIMUM_LINE_LENGTH + 1 ) );
	return constructor;
}

void copySAMData( struct SequenceReader* reader, struct SAMData* data ) {
	strcpy( data->qname, reader->currentID );
	data->flag = reader->flag;
	strcpy( data->rname, reader->rname );
	data->pos = reader->pos;
	data->mapq = reader->mapq;
	strcpy( data->cigar, reader->cigar );
	strcpy( data->rnext, reader->rnext );
	data->pnext = reader->pnext;
	data->tlen = reader->tlen;
	strcpy( data->seq, reader->currentSequence );
	strcpy( data->qual, reader->currentQV );
	strcpy( data->option, reader->option );
}

void freeSAMData( struct SAMData* data ) {
	free( data->qname  );
	free( data->rname  );
	free( data->cigar  );
	free( data->rnext  );
	free( data->seq    );
	free( data->qual   );
	free( data->option );
	if( data->next != NULL ) freeSAMData( data->next );
	free( data );
}

void printSAMData( struct SAMReader* reader, FILE* writer ) {
	struct SAMData* data = reader->rootData;
	for( int i = 0; i < reader->size; i++ ) {
		if( data == NULL ) continue;
		fprintf( writer, "%s\t%d\t%s\t%d\t%d\t%s\t%s\t%d\t%d\t%s\t%s\t%s\n", data->qname, data->flag, data->rname, data->pos, data->mapq, data->cigar, data->rnext, data->pnext, data->tlen, data->seq, data->qual, data->option );
		data = data->next;
	}
}

int readNextSAMs( struct SAMReader* reader ) {
	char* previousID = 0;
	reader->size = 0;
	while( readNextSequence( reader->sequenceReader ) ) {
		if( previousID == 0 ) { // first time
			copySAMData( reader->sequenceReader, reader->rootData ); // copy
			previousID = reader->rootData->qname; // remember
			reader->currentData = reader->rootData; // next
			reader->size++; // increment count
			continue;
		} else if( strcmp( previousID, reader->sequenceReader->currentID ) != 0 ) { // IDs do not match
			strcpy( reader->sequenceReader->previousLine, reader->sequenceReader->currentLine ); // remember
			return reader->size;
		}
		if( reader->currentData->next == NULL ) reader->currentData->next = newSAMData();
		copySAMData( reader->sequenceReader, reader->currentData->next ); // coyp data
		reader->currentData = reader->currentData->next; // set to next data
		reader->size++; // increment count
	}
	return reader->size; // return read line
}
