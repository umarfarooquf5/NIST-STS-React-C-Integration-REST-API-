#ifndef MATRIX_H
#define MATRIX_H

#include "defs.h"

int computeRank(int M, int Q, BitSequence **matrix);
void perform_elementary_row_operations(int flag, int i, int M, int Q, BitSequence **A);
int find_unit_element_and_swap(int flag, int i, int M, int Q, BitSequence **A);
int swap_rows(int i, int index, int Q, BitSequence **A);
int determine_rank(int m, int M, int Q, BitSequence **A);
BitSequence **create_matrix(int M, int Q);
void def_matrix(int M, int Q, BitSequence **m, int k, BitSequence *epsilon);
void delete_matrix(int M, BitSequence **matrix);

#endif // MATRIX_H
