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

#ifndef ADVANCED_IO
#define ADVANCED_IO
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include "regexp.h"
#include "vector.h"

/******************** LIST ********************/

// Return files from specified path
// There is a need to remove created string vectors
struct StringVector* getFiles( char* path, struct StringVector* vector );

// Return files from specified path
// There is a need to remove created string vectors afterwards
struct StringVector* getFilesRecursively( char* path, int depth, struct RegularExpression* suffix, struct RegularExpression* grep, struct RegularExpression* vgrep, struct StringVector* vector );

// Return directories from specified path
// There is a need to remove created string vectors afterwards
struct StringVector* getDirectoriesRecursively( char* path, int depth, struct RegularExpression* grep, struct RegularExpression* vgrep, struct StringVector* vector );
#endif
