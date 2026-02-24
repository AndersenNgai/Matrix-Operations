#include <stdio.h>
#include <string.h>
#include <matrix.h>
#include <reader.h>

// gcc src/main.c src/matrix.c src/reader.c -o matrix -I include

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: matrix input.txt output.txt\n");
        return 1;
    }
    if (strcmp(argv[1], argv[2]) == 0) {
        fprintf(stderr, "Same input and output file\n");
        return 2;
    }

    FILE *infile  = fopen(argv[1], "r");
    if (infile == NULL) {
        fprintf(stderr, "Could not read file %s\n", argv[1]);
        return 3;
    }
    FILE *outfile = fopen(argv[2], "w");
    if (outfile == NULL) {
        fclose(infile);
        fprintf(stderr, "Cannot write to file %s\n", argv[2]);
        return 4;
    }

    Table table;
    table_init(&table);

    // Read the infile
    int res;
    do {
        res = read_instruction(infile, outfile, &table);
    } while (res != -1);

    free_table(&table);
    fclose(infile);
    fclose(outfile);
    return 0;
}