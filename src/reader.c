#include "reader.h"

/* Returns -1 if EOF reached. Else returns the number of characters and
   Positions the file right before reading the non-whitespace */
int skip_whitespace(FILE *infile) {
    int c, i = -1;
    do {
        c = fgetc(infile);
        if (c == EOF)
            return -1;
        i++;
    } while (isspace(c));
    ungetc(c, infile);
    return i;
}

/* Returns 0 on success, -1 on errors */
int read_instruction(FILE *infile, FILE *outfile, Table *table) {

    // First, determine if this is a DEF, EVAL or PRINT instruction
    // Or maybe we reach the end of the file
    char line_type[6];
    if (fscanf(infile, "%5s", line_type) != 1)
        return -1;

    // Now determine the operation

    if (skip_whitespace(infile) < 1) {
        // Less than one whitespace character after DEF, EVAL or PRINT
        // Or a newline is read
        fprintf(outfile, "Syntax error: Instruction must begin with DEF, EVAL or PRINT");
        return -1;
    }

    if (strcmp(line_type, "DEF") == 0) {
        char name[101];
        int rows, columns;
        
        // fscanf() handles any amount of whitespace between DEF, name, rows and columns
        if (fscanf(infile, "%100s %d %d", name, &rows, &columns) != 3) {
            fprintf(outfile, "Syntax error: Proper DEF header: name (max 100 characters), rows, columns");
            return -1;
        }

        if (rows < 1 || columns < 1) {
            fprintf(outfile, "Value error: Matrix dimensions must be positive");
            return -1;
        }

        // Skip any whitespace to find '['
        skip_whitespace(infile);
        if (fgetc(infile) != '[') {
            fprintf(outfile, "Syntax error: Matrix values must be contained by [...]");
            return -1;
        }

        Matrix *m = create_matrix(name, rows, columns);
        if (m == NULL) {
            fprintf(outfile, "Memory error");
            return -1;
        }
        int register_matrix = table_add(table, m);
        if (register_matrix == -1) {
            fprintf(outfile, "Memory error");
            return -1;
        }
        if (register_matrix == 1) {
            fprintf(outfile, "Value error: Matrix %s redefined", name);
            return -1;
        }

        double value;
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < columns; c++) {
                // Read the number
                if (fscanf(infile, "%lf", &value) != 1) {
                    fprintf(outfile, "Syntax error: Expected a number");
                return -1;
                }
                matrix_set(m, r, c, value);

                if (skip_whitespace(infile) == -1) {
                    fprintf(outfile, "Syntax error: Could not parse numbers");
                    return -1;
                }

                int next = fgetc(infile);
                if (c < columns - 1) {
                    if (next != ',') {
                        fprintf(outfile, "\nSyntax error: Row ended too early (expected ',')");
                        return -1;
                    }
                }
                else {
                    // We are at the end of a row.
                    // If it's the last row, 'next' SHOULD be ']' or maybe a trailing ';'
                    // If it's NOT the last row, 'next' MUST be ';'
                    if (r < rows - 1) {
                        if (next != ';') {
                            fprintf(outfile, "\nSyntax error: Expected ';' at end of row");
                            return -1;
                        }
                    }
                    else {
                        // Last row: handle ']' or optional ';' then ']'
                        if (next == ';') {
                            skip_whitespace(infile);
                            next = fgetc(infile); // Move on to see if ']' follows
                        }
                        if (next != ']') {
                            fprintf(outfile, "\nSyntax error: Expected ']' to close matrix");
                            return -1;
                        }
                    }
                }
            }
        }
    }








    else if (strcmp(line_type, "EVAL") == 0) {
        char target_name[101];
        char op[10]; // To store "ADD", "SUB", "MUL", or "TRANS"
        char op1_name[101], op2_name[101];

        // 1. Read Target Name and the Operation
        if (fscanf(infile, "%100s %9s", target_name, op) != 2) {
            fprintf(outfile, "Syntax error: EVAL format begins with EVAL [new matrix] [operation]");
            return -1;
        }

        if (find_matrix(table, target_name)) {
            fprintf(outfile, "Name error: Matrix %s redefined", target_name);
            return -1;
        }

        // 2. Fetch Operand 1
        if (fscanf(infile, "%100s", op1_name) != 1) {
            fprintf(outfile, "Syntax error: EVAL missing input matrix");
            return -1;
        }
        Matrix *A = find_matrix(table, op1_name);
        if (A == NULL) {
            fprintf(outfile, "Name error: Unknown matrix %s", op1_name);
            return -1;
        }

        Matrix *result = NULL;

        // 3. Branching Logic based on Operation
        if (strcmp(op, "TRANS") == 0) {
            // TRANSpose only needs one matrix
            result = matrix_transpose(A, target_name);
        }
        else {
            // ADD, SUB, and MUL all need a second matrix
            if (fscanf(infile, "%100s", op2_name) != 1) {
                fprintf(outfile, "Syntax error: Missing required second matrix");
                return -1;
            }
            Matrix *B = find_matrix(table, op2_name);
            if (B == NULL) {
                fprintf(outfile, "Name error: Unknown matrix %s", op2_name);
                return -1;
            }

            if (strcmp(op, "ADD") == 0) {
                if (A->rows != B->rows || A->cols != B->cols) {
                    fprintf(outfile, "Matrix error: ADD dimension mismatch for matrices %s and %s", op1_name, op2_name);
                return -1;
                }
                result = matrix_add(A, B, target_name);
            } 
            else if (strcmp(op, "SUB") == 0) {
                if (A->rows != B->rows || A->cols != B->cols) {
                    fprintf(outfile, "Matrix error: SUB dimension mismatch for matrices %s and %s", op1_name, op2_name);
                    return -1;
                }
                result = matrix_sub(A, B, target_name);
            } 
            else if (strcmp(op, "MUL") == 0) {
                // Matrix multiplication rule: Cols of A must equal Rows of B
                if (A->cols != B->rows) {
                    fprintf(outfile, "Matrix error: MUL dimension mismatch for matrices %s and %s", op1_name, op2_name);
                    return -1;
                }
                result = matrix_multiply(A, B, target_name);
            } 
            else {
                fprintf(outfile, "Syntax error: Unknown EVAL operation %s", op);
                return -1;
            }
        }

        // 4. Store the result in the table
        if (result) {
            if (table_add(table, result)) {
                // Earlier, we already checked for redefinition
                fprintf(outfile, "Memory error");
                return -1;
            }
        }
        else {
            fprintf(outfile, "Memory error");
            return -1;
        }
    }







    else if (strcmp(line_type, "PRINT") == 0) {
        char name[101];

        // 1. Read the name of the matrix to print
        if (fscanf(infile, "%100s", name) != 1) {
            fprintf(outfile, "\nSyntax error: Expected matrix name for PRINT");
            return -1;
        }

        // 2. Look up the matrix in your table
        Matrix *m = find_matrix(table, name);
        if (m == NULL) {
            fprintf(outfile, "\nName error: Undefined matrix: %s", name);
            return -1;
        }

        // 3. Print the matrix in the [1, 2; 3, 4] format
        fprintf(outfile, "%s = [", m->name);
    
        for (int r = 0; r < m->rows; r++) {
            for (int c = 0; c < m->cols; c++) {
                // Get the value (using your matrix_get or direct access)
                double val = matrix_get(m, r, c);
            
                // Print the value. %.2f limits decimals, or use %g for "smart" formatting
                fprintf(outfile, "%g", val);

                // Logic for delimiters
                if (c < m->cols - 1) {
                    fprintf(outfile, ", "); // Comma between elements in a row
                } else if (r < m->rows - 1) {
                    fprintf(outfile, "; "); // Semicolon between rows
                }
            }
        }
        fprintf(outfile, "]\n");
    }









    else {
        fprintf(outfile, "\nUnknown command");
        return -1;
    }

    return 0;
}