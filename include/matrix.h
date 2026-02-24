#ifndef MATRIX_H
#define MATRIX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    int rows;
    int cols;
    double *data;
} Matrix;

Matrix *create_matrix(const char *name, int rows, int cols);
void free_matrix(Matrix *m);
double matrix_get(const Matrix *m, int r, int c);
void matrix_set(Matrix *m, int r, int c, double value);





typedef struct {
    Matrix **items;
    size_t size;
    size_t capacity;
} Table;

void table_init(Table *t);
void free_table(Table *t);
Matrix *find_matrix(Table *t, const char *name);
int table_add(Table *t, Matrix *m);










Matrix* matrix_multiply(Matrix *A, Matrix *B, char *new_name);
Matrix* matrix_add(Matrix *A, Matrix *B, char *new_name);
Matrix* matrix_sub(Matrix *A, Matrix *B, char *new_name);
Matrix* matrix_transpose(Matrix *A, char *new_name);

#endif