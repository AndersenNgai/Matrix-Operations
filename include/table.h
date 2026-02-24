#ifndef TABLE_H
#define TABLE_H

#include <stdio.h>

typedef struct {
    Matrix **items;
    size_t size;
    size_t capacity;
} MatrixTable;

/* ===== table ===== */
void table_init(MatrixTable *t);
void table_free(MatrixTable *t);
Matrix *table_find(MatrixTable *t, const char *name);
int table_add(MatrixTable *t, Matrix *m);

#endif