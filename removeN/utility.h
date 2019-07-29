#ifndef UTILITY
#define UTILITY
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

void chomp( char* line );
int octalToQV( char c );
char QVToOctal( int qv );
int solexaOctalToQV( char c );
char QVToSolexaOctal( int qv );
int solexaQVToPhredQV( char c );

void extract( char* input, char* output, int start, int end );
int getlastIndex( char* path, char c );
char* getFilename( char* path, char* output );
char* getDirectory( char* path, char* output );
char* getBasename( char* path, char* output );
char* getSuffix( char* path, char* output );
int getIndex( char* path, char c, int index );
#endif
