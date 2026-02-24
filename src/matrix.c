#include "matrix.h"

/* Matrix */

Matrix *create_matrix(const char *name, int rows, int cols) {
    Matrix *m = malloc(sizeof(Matrix));
    if (m == NULL)
        return NULL;

    m->rows = rows;
    m->cols = cols;

    m->name = malloc(strlen(name) + 1);
    if (m->name == NULL) {
        free(m);
        return NULL;
    }
    strcpy(m->name, name);

    m->data = malloc(rows * cols * sizeof(double));
    if (m->data == NULL) {
        free(m->name);
        free(m);
        return NULL;
    }
    return m;
}

void free_matrix(Matrix *m) {
    if (m == NULL)
        return;
    free(m->name);
    free(m->data);
    free(m);
}

double matrix_get(const Matrix *m, int r, int c) {
    return m->data[r * m->cols + c];
}

void matrix_set(Matrix *m, int r, int c, double value) {
    m->data[r * m->cols + c] = value;
}

/* Table */

void table_init(Table *t) {
    t->items = NULL;
    t->size = 0;
    t->capacity = 0;
}

static int table_grow(Table *t) {
    size_t newcap = (t->capacity == 0) ? 8 : t->capacity * 2;

    Matrix **temp = realloc(t->items, newcap * sizeof(Matrix*));
    if (temp == NULL)
        return -1;
    t->items = temp;
    t->capacity = newcap;
    return 0;
}

Matrix *find_matrix(Table *t, const char *name) {
    for (size_t i = 0; i < t->size; i++) {
        if (strcmp(t->items[i]->name, name) == 0)
            return t->items[i];
    }
    return NULL;
}

// Add a Matrix. Returns 0 on success, 1 if already exists, -1 on memory failure
int table_add(Table *t, Matrix *m) {
    if (find_matrix(t, m->name))
        return 1;
    if (t->size == t->capacity)
        if (table_grow(t))
            return -1;

    t->items[t->size++] = m;
    return 0;
}

void free_table(Table *t) {
    for (size_t i = 0; i < t->size; i++)
        free_matrix(t->items[i]);
    free(t->items);
}
















Matrix* matrix_multiply(Matrix *A, Matrix *B, char *new_name) {
    Matrix *res = create_matrix(new_name, A->rows, B->cols);
    if (res == NULL)
        return NULL;
    for (int i = 0; i < A->rows; i++)
        for (int j = 0; j < B->cols; j++) {
            double sum = 0;
            for (int k = 0; k < A->cols; k++)
                sum += A->data[i * A->cols + k] * B->data[k * B->cols + j];
            res->data[i * res->cols + j] = sum;
        }
    return res;
}

Matrix* matrix_add(Matrix *A, Matrix *B, char *new_name) {
    Matrix *res = create_matrix(new_name, A->rows, A->cols);
    if (res == NULL)
        return NULL;
    int total_elements = A->rows * A->cols;
    for (int i = 0; i < total_elements; i++)
        res->data[i] = A->data[i] + B->data[i];
    return res;
}

Matrix* matrix_sub(Matrix *A, Matrix *B, char *new_name) {
    Matrix *res = create_matrix(new_name, A->rows, A->cols);
    if (res == NULL)
        return NULL;
    int total_elements = A->rows * A->cols;
    for (int i = 0; i < total_elements; i++)
        res->data[i] = A->data[i] - B->data[i];
    return res;
}

Matrix* matrix_transpose(Matrix *A, char *new_name) {
    Matrix *res = create_matrix(new_name, A->cols, A->rows);
    if (res == NULL)
        return NULL;
    for (int r = 0; r < A->rows; r++)
        for (int c = 0; c < A->cols; c++)
            res->data[c * res->cols + r] = A->data[r * A->cols + c];
    return res;
}