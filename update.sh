#!/bin/sh
# This is used to update all my C library files
# AKira Hasegawa
# 2012/04/10

Basename.pl -g "advanced_io.h" "cp ../library/[file] [path]" | Run.pl
Basename.pl -g "advanced_io.c" "cp ../library/[file] [path]" | Run.pl
Basename.pl -g "chromosome.h" "cp ../library/[file] [path]" | Run.pl
Basename.pl -g "chromosome.c" "cp ../library/[file] [path]" | Run.pl
Basename.pl -g "distribution.h" "cp ../library/[file] [path]" | Run.pl
Basename.pl -g "distribution.c" "cp ../library/[file] [path]" | Run.pl
Basename.pl -g "dotmatrix.h" "cp ../library/[file] [path]" | Run.pl
Basename.pl -g "dotmatrix.c" "cp ../library/[file] [path]" | Run.pl
Basename.pl -g "hashtable.h" "cp ../library/[file] [path]" | Run.pl
Basename.pl -g "hashtable.c" "cp ../library/[file] [path]" | Run.pl
Basename.pl -g "interface.h" "cp ../library/[file] [path]" | Run.pl
Basename.pl -g "interface.c" "cp ../library/[file] [path]" | Run.pl
Basename.pl -g "io.h" "cp ../library/[file] [path]" | Run.pl
Basename.pl -g "io.c" "cp ../library/[file] [path]" | Run.pl
Basename.pl -g "reader.h" "cp ../library/[file] [path]" | Run.pl
Basename.pl -g "reader.c" "cp ../library/[file] [path]" | Run.pl
Basename.pl -g "regexp.h" "cp ../library/[file] [path]" | Run.pl
Basename.pl -g "regexp.c" "cp ../library/[file] [path]" | Run.pl
Basename.pl -g "vector.h" "cp ../library/[file] [path]" | Run.pl
Basename.pl -g "vector.c" "cp ../library/[file] [path]" | Run.pl
