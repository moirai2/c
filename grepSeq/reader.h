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

#ifndef READER
#define READER
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include "io.h"
#include "regexp.h"
#include "vector.h"

/******************** DEFINE ********************/

#define MAXIMUM_SEQUENCE_LENGTH 10000 // maximum sequence length handle by this program
#define MAXIMUM_LINE_LENGTH 200 // maximum line length for ID and others
#define REPORT_COUNT 10000000 // report current status after this count
#define FASTA 1 // fasta format
#define FASTQ 2 // fastq format
#define SAM 3 // sam/bam format
#define UNIQ 4 // sequence count format
#define TEXT 5 // sequence count format

/******************** STRUCT ********************/

// Structure used to read a FASTQ or FASTA file
struct SequenceReader {
	// reader status
	long int report; // report status of read at this sequence count
	long int sequenceCount; // number of sequences read
	// filename information
	char* path; // file path
	char* basename; // file basename
	char* previousLine; // previous line read from last interaction
	char* currentLine; // current line read by current interaction
	// actual information
	char* currentID; // sequence ID
	char* currentID2; // quality ID
	char* currentSequence; // sequence string
	char* currentQV; // quality value string
	// SAM format
	// 1st sequence QNAME
	int flag; // 2nd column
	char* rname; // 3rd column
	int pos; // 4th column
	int mapq; // 5th column
	char* cigar; // 6th column
	char* rnext; // 7th column
	int pnext; // 8th column
	int tlen; // 9th column
	// 10th SEQ
	// 11th QUAL
	char* option; // 12th column (can be multiple)
	// UNIQ format
	int count; // sequence count for unique format
	// filehandler information
	int type; // 1=FASTQ 2=FASTA 3=SAM
	FILE* filehandler; // file reader
	int (*reader)( struct SequenceReader* reader ); // function used to read a file
	void (*print)( struct SequenceReader* reader, FILE* writer ); // function used to read a file
	struct StringVector* headers; // remember header lines of SAM/BAM
};

/******************** READER ********************/

// constructor
struct SequenceReader* newSequenceReader( char* path );

// deconstructor
void freeSequenceReader( struct SequenceReader* reader );

// remove #0/2 or #0/1 from sequence ID and quality ID
// This is required to match IDs between read1 and read2 IDs for paired ends
void removeReadInformation( struct SequenceReader* reader );

// initialize
void intializeSequenceReader( struct SequenceReader* reader );

// Check if reader correctly read sequence
int hasCurrentSequence( struct SequenceReader* reader );

// read next line
char* readNextLine( struct SequenceReader* reader );

// read next sequence
int readNextSequence( struct SequenceReader* reader );

// read next sequence called from readNextSequence
int readNextFASTA( struct SequenceReader* reader );

// read next FASTQ sequence called from readNextSequence
int readNextFASTQ( struct SequenceReader* reader );

// read next SAM sequence called from readNextSequence
int readNextSAM( struct SequenceReader* reader );

// read next TEXT line called from readNextSequence
int readNextTEXT( struct SequenceReader* reader );

// read next UNIQ sequence called from readNextSequence
int readNextUNIQ( struct SequenceReader* reader );

// print current sequence
void printCurrentSequence( struct SequenceReader* reader, FILE* writer );

// print current sequence in FASTA format called from printCurrentSequence
void printCurrentFASTA( struct SequenceReader* reader, FILE* writer );

// print current sequene in FASTA format called from printCurrentSequence
void printCurrentFASTQ( struct SequenceReader* reader, FILE* writer );

// print current sequene in SAM format called from printCurrentSequence
void printCurrentSAM( struct SequenceReader* reader, FILE* writer );

// print current line
void printCurrentTEXT( struct SequenceReader* reader, FILE* writer );

// print current sequene in UNIQ format called from printCurrentSequence
void printCurrentUNIQ( struct SequenceReader* reader, FILE* writer );

// print SAM header lines
void printSAMHeaders( struct SequenceReader* reader, FILE* writer );

// return lower/minimum location for SAM/BAM
int getLowerLocation( struct SequenceReader* reader );

// return upper/maximum location for SAM/BAM
int getUpperLocation( struct SequenceReader* reader );

// return start location for SAM/BAM taking in account of reverse complement
int getStartLocation( struct SequenceReader* reader );

// return end location for SAM/BAM taking in account of reverse complement
int getEndLocation( struct SequenceReader* reader );

/******************** UTILITY ********************/

// return list of chromosome names
struct StringVector* getChromosomes( struct SequenceReader* reader );

// Return chromosome length from BAM/SAM header lines
int getChromosomeLength( struct SequenceReader* reader, char* chromosome );

// return length span by cigar line
int getCigarLength( char* cigar );

// Return 1 if sequence IDs are same, 0 if they are different
// This ignores read information like #0/2 #1/2
int checkSequenceIDs( char* sequenceID, char* sequenceID2 );

#endif
