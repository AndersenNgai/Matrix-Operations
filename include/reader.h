#ifndef READER_H
#define READER_H

#include <stdio.h>
#include <ctype.h>
#include "matrix.h"

int read_instruction(FILE *infile, FILE *outfile, Table *table);

#endif