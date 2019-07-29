#include "utility.h"

void extract( char* input, char* output, int start, int end ) {
	if( start < 0 || end < 0 ) {
		strcpy( output, "" );
		return;
	}
	int length = end - start;
	strncpy( output, input + start, length );
	output[ length ] = '\0';
}

int getIndex( char* path, char c, int start ) {
	for( int index = start; index < strlen( path ); index++ ) if( path[ index ] == c ) return index;
	return -1;
}

int getLastIndex( char* path, char c, int end ) {
	for( int index = end; index >= 0; index-- ) if( path[ index ] == c ) return index;
	return -1;
}

char* getBasename( char* path, char* output ) {
	int index = getLastIndex( path, '/', strlen( path ) );
	if( index > -1 ) path += index + 1;
	index = getLastIndex( path, '.', strlen( path ) );
	if( index < 0 ) index = strlen( path );
	strncpy( output, path, index );
	output[ index ] = '\0';
	return output;
}

char* getSuffix( char* path, char* output ) {
	int index = getLastIndex( path, '/', strlen( path ) );
	if( index > -1 ) path += index + 1;
	index = getLastIndex( path, '.', strlen( path ) );
	if( index < 0 ) index = strlen( path ) - 1;
	strcpy( output, path + index + 1 );
	return output;
}

char* getFilename( char* path, char* output ) {
	int index = getLastIndex( path, '/', strlen( path ) );
	strcpy( output, path + index + 1 );
	return output;
}

char* getDirectory( char* path, char* output ) {
	int index = getLastIndex( path, '/', strlen( path ) );
	if( index < 0 ) index = 0;
	strncpy( output, path, index );
	output[ index ] = '\0';
	return output;
}

// replace '\n' with '\0'
void chomp( char* line ) {
	int length = strlen( line );
	for( int i = 0; i < length; i++ ) {
		if( line[ i ] == '\n' ) line[ i ] = '\0';
		else if( line[ i ] == '\r' ) line[ i ] = '\0';
	}
}

// simply subtract by 33
int octalToQV( char c ) { return (int) ( c - 33 ); }

// Doesn't go beyond Q of 60
char QVToOctal( int qv ) { return (char) ( qv + 33 ); }

// Doesn't go beyond Q of 60
char QVToSolexaOctal( int qv ) { return (char) ( qv + 64 ); }

// http://maq.sourceforge.net/fastq.shtml
// http://maq.sourceforge.net/qual.shtml
// http://mng.iop.kcl.ac.uk/site/node/292
// So, in summary there are now 3 different types of fastq files:
// 1. Standard fastq:   ASCII( Qphred + 33 )
// 2. Illumina pre v1.3 :  ASCII( Qsolexa + 64 )
// 3. Illumina post v1.3: ASCII( Qphred + 64 )
// We've been using v1.3 since 2009/02/26 so there shouldn'tbe any problem
// ';' (-5) '<' (-4) '=' (-3) '>' (-2) '?' (-1) '@' (0) 'A' (1) 'B' (2) 'C' (3) 'D' (4) 'E' (5) 'F' (6) 'G' (7) 'H' (8) 'I' (9) 'J' (10) 'K' (11)
// 'L' (12) 'M' (13) 'N' (14) 'O' (15) 'P' (16) 'Q' (17) 'R' (18) 'S' (19) 'T' (20) 'U' (21) 'V' (22) 'W' (23) 'X' (24) 'Y' (25) 'Z' (26) '[' (27)
// '\' (28) ']' (29) '^' (30) '_' (31) '`' (32) 'a' (33) 'b' (34) 'c' (35) 'd' (36) 'e' (37) 'f' (38) 'g' (39) 'h' (40) 'i' (41) 'j' (42) 'k' (43)
// 'l' (44) 'm' (45) 'n' (46) 'o' (47) 'p' (48) 'q' (49) 'r' (50) 's' (51) 't' (52) 'u' (5) 'v' (54) 'w' (55) 'x' (56) 'y' (57) 'z' (58) '{' (59) '|' (60)
int solexaOctalToQV( char c ) { return (int) c - 64; }

// Solexa quality is NOT Phred quality. The Solexa quality and Phred quality are asymptotically identical. If the error probability of a base is $e, the Solexa quality $sQ is:
// $sQ = -10 * log($e / (1 - $e)) / log(10);
// Solexa quality $sQ can be converted to Phred quality $Q with this formula:
// $Q = 10 * log(1 + 10 ** ($sQ / 10.0)) / log(10);
int solexaQVToPhredQV( char c ) { return (int) ( 10.0 * log( 1 + pow( 10.0, ( (int) c - 64 ) / 10.0 ) ) / log( 10.0 ) ); }
