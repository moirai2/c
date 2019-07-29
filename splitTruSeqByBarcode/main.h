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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include "advanced_io.h"
#include "interface.h"
#include "io.h"
#include "reader.h"

/******************** STRUCT ********************/

// Vector of barcodes
struct Barcode {
	char* barcode;
	char* regexp;
	char* file;
	FILE* writer; // for read1
	FILE* writer2; // for read2
	struct Barcode* next;
};

/******************** MAIN ********************/

void help( void );

struct Barcode* addBarcode( struct Barcode* original, char* basename, char* barcode, char* directory, int gzip );

void freeBarcode( struct Barcode* barcodes );

void searchAndWrite( struct Barcode* barcodes, struct SequenceReader* reader1, struct SequenceReader* reader2, FILE* notfound );

void searchAndWritePairs( struct Barcode* barcodes, struct SequenceReader* reader1, struct SequenceReader* reader2, FILE* notfound, FILE* notfound2 );

struct Barcode* addBarcode( struct Barcode* barcodes, char* basename, char* barcode, char* directory, int gzip );
