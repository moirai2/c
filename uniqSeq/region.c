#include "vector.h"

/******************** STRING VECTOR ********************/

struct RegionVector* newRegionVector() {
	struct RegionVector* constructor = 0;
	constructor = malloc( sizeof( struct RegionVector ) );
	constructor->current = 0;
	constructor->last    = 0;
	constructor->objects = 0;
	return constructor;
}

struct RegionVectorObject* newRegionVectorObject( int start, int end ) {
	struct RegionVectorObject* constructor = 0;
	constructor = malloc( sizeof( struct RegionVectorObject ) );
	constructor->start = 0;
	constructor->end = 0;
	constructor->next = 0;
	constructor->start = start;
	constructor->start = end;
	return constructor;
}

long int getRegionVectorSize( struct RegionVector* vector ) {
	struct RegionVectorObject* object = vector->objects;
	long int size = 0;
	while( object != NULL ) {
		size++;
		object = object->next;
	}
	return size;
}

void addRegionVector( struct RegionVector* vector, long int start, long int end ) {
	if( vector->last == NULL ) {
		vector->objects = newRegionVectorObject( start, end );
		vector->last    = vector->objects;
		vector->current = vector->objects;
	} else {
		vector->last->next = newRegionVectorObject( start, end );
		vector->last = vector->last->next;
	}
}

char* getRegionVector( struct RegionVector* vector, long int index ) {
	long int size = 0;
	struct RegionVectorObject* object = vector->objects;
	while( object != NULL ) {
		if( index == size++ ) return object->string;
		object = object->next;
	}
	return NULL;
}

void printRegionVector( struct RegionVector* vector, FILE* handler ) {
	resetRegionVectorIndex( vector );
	struct RegionVectorObject* object = vector->objects;
	while( object != NULL ) {
		fprintf( handler, "%ld - %ld\n", object->start, object->end );
		object = object->next;
	}
}

int hasMoreRegionVector( struct RegionVector* vector ) { return vector->current != NULL; }

char* nextRegionVector( struct RegionVector* vector ) {
	if( vector->current == NULL ) return NULL;
	char* string = vector->current->string;
	vector->current = vector->current->next;
	return string;
}

char* peekRegionVector( struct RegionVector* vector ) {
	if( vector->current == NULL ) return NULL;
	else return vector->current->string;
}

void resetRegionVectorIndex( struct RegionVector* vector ) { vector->current = vector->objects; }

struct RegionVectorObject* quickSort( struct RegionVectorObject* pivot ) {
	if( pivot == NULL ) return NULL;
	struct RegionVectorObject* larger_s  = 0; // start pointer
	struct RegionVectorObject* larger_p  = 0; // end pointer
	struct RegionVectorObject* equals_s  = pivot; // start pointer
	struct RegionVectorObject* equals_p  = pivot; // end pointer
	struct RegionVectorObject* smaller_s = 0; // start pointer
	struct RegionVectorObject* smaller_p = 0; // end pointer
	struct RegionVectorObject* object    = pivot->next;
	pivot->next = NULL;
	while( object != NULL ) {
		int compare = ( object->start < pivot->start );
		if( compare < 0 ) { // smaller
			if( smaller_s == NULL ) {
				smaller_s = object;
				smaller_p = object;
			} else {
				smaller_p->next = object;
				smaller_p = object;
			}
		} else if( compare > 0 ) { // bigger
			if( larger_s == NULL ) {
				larger_s = object;
				larger_p = object;
			} else {
				larger_p->next = object;
				larger_p = object;
			}
		} else { // equals
			equals_p->next = object;
			equals_p = object;
		}
		object = object->next;
	}
	struct RegionVectorObject* output_s = 0;
	struct RegionVectorObject* output_e = 0;
	if( smaller_p != NULL ) {
		smaller_p->next = NULL;
		smaller_s = quickSort( smaller_s );
		output_s = smaller_s;
		output_e = smaller_s;
		while( output_e->next != NULL ) output_e = output_e->next;
	}
	if( equals_p != NULL ) {
		equals_p->next = NULL;
		if( output_s == NULL ) {
			output_s = equals_s;
			output_e = equals_s;
		} else {
			output_e->next = equals_s;
		}
		while( output_e->next != NULL ) output_e = output_e->next;
	}
	if( larger_p != NULL ) {
		larger_p->next  = NULL;
		larger_s = quickSort( larger_s );
		if( output_s == NULL ) {
			output_s = larger_s;
			output_e = larger_s;
		} else {
			output_e->next = larger_s;
		}
		while( output_e->next != NULL ) output_e = output_e->next;
	}
	return output_s;
}

void sortRegionVector( struct RegionVector* vector ) {
	vector->objects = quickSort( vector->objects );
	vector->current = vector->objects;
	vector->last    = vector->objects;
	while( vector->last->next != NULL ) vector->last = vector->last->next;
}

void freeRegionVector( struct RegionVector* vector ) {
	struct RegionVectorObject* object = vector->objects;
	struct RegionVectorObject* next = 0;
	while( object != NULL ) {
		next = object->next;
		free( object );
		object = next;
	}
	free( vector );
}

struct RegionVector* split( char* string, char delim ) {
	struct RegionVector* constructor = newRegionVector(); // construct new string vector
	int start  = 0; // start
	int end    = 0; // end
	char* temp = 0; // temporary string
	temp = malloc( sizeof( char ) * ( strlen( string ) + 1 ) ); // allocate string memory
	for( end = 0; end < strlen( string ); end++ ) { // go through all characters
		if( string[ end ] == delim ) { // found delim
			strcpy( temp, "" ); // reset
			int length = end - start;
			strncpy( temp, string + start, length ); // copy from original string
			temp[ length ] = '\0'; // add EOF always after strncpy
			addRegionVector( constructor, temp ); // add new string
			start = end + 1;
		}
	}
	strcpy( temp, "" ); // reset
	int length = end - start;
	strncpy( temp, string + start, length ); // copy from original string
	temp[ length ] = '\0'; // add EOF always after strncpy
	addRegionVector( constructor, temp ); // add new string
	free( temp ); // free string
	return constructor;
}

struct RegionVector* loadRegionVector( struct RegionVector* vector, char* path ) {
	FILE* filehandler = openReader( path );
	if( filehandler == NULL ) return NULL;
	if( vector == NULL ) vector = newRegionVector();
	char line[ MAX_LINE ];
	while( fgets( line, MAX_LINE, filehandler ) ) {
		chomp( line );
		addRegionVector( vector, line );
	}
	closeHandler( filehandler, path );
	return vector;
}
