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

#ifndef SAM_READER
#define SAM_READER
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include "io.h"
#include "reader.h"
#include "regexp.h"
#include "vector.h"

/******************** STRUCT ********************/

// Structure used to read a FASTQ or FASTA file
struct SAMReader {
	struct SequenceReader* sequenceReader; // reader
	int size; // number of mappings
	struct SAMData* rootData; // stored lines
	struct SAMData* currentData; // stored lines
};

struct SAMData {
	char* qname; // 1st sequence ID
	int flag; // 2nd column
	char* rname; // 3rd column
	int pos; // 4th column
	int mapq; // 5th column
	char* cigar; // 6th column
	char* rnext; // 7th column
	int pnext; // 8th column
	int tlen; // 9th column
	char* seq; // 10th sequence string
	char* qual; // 11th quality value string
	char* option; // 12th column (can be multiple)
	struct SAMData* next; // next data
};

/******************** BAM READER ********************/

// create BAM/SAM reader
struct SAMReader* newSAMReader( char* path );

// free BAM/SAM reader
void freeSAMReader( struct SAMReader* reader );

// create new bam data from storage
struct SAMData* newSAMData();

// free BAM/SAM data
void freeSAMData( struct SAMData* data );

// print SAM data
void printSAMData( struct SAMReader* data, FILE* writer );

// copy bam data from reader
void copySAMData( struct SequenceReader* reader, struct SAMData* data );

// read next SAM sequence called from readNextSequence
int readNextSAMs( struct SAMReader* reader );

#endif
