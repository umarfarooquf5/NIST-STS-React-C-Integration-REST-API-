#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>
#include "matrix.h"
#include "defs.h"

#define MATRIX_FORWARD_ELIMINATION 0
#define MATRIX_BACKWARD_ELIMINATION 1

int computeRank(int M, int Q, BitSequence **matrix)
{
	int i, rank, m = MIN(M, Q);

	for (i = 0; i < m - 1; i++)
	{
		if (matrix[i][i] == 1)
			perform_elementary_row_operations(MATRIX_FORWARD_ELIMINATION, i, M, Q, matrix);
		else if (find_unit_element_and_swap(MATRIX_FORWARD_ELIMINATION, i, M, Q, matrix) == 1)
			perform_elementary_row_operations(MATRIX_FORWARD_ELIMINATION, i, M, Q, matrix);
	}

	for (i = m - 1; i > 0; i--)
	{
		if (matrix[i][i] == 1)
			perform_elementary_row_operations(MATRIX_BACKWARD_ELIMINATION, i, M, Q, matrix);
		else if (find_unit_element_and_swap(MATRIX_BACKWARD_ELIMINATION, i, M, Q, matrix) == 1)
			perform_elementary_row_operations(MATRIX_BACKWARD_ELIMINATION, i, M, Q, matrix);
	}

	rank = determine_rank(m, M, Q, matrix);
	return rank;
}

void perform_elementary_row_operations(int flag, int i, int M, int Q, BitSequence **A)
{
	int j, k;
	if (flag == MATRIX_FORWARD_ELIMINATION)
	{
		for (j = i + 1; j < M; j++)
			if (A[j][i] == 1)
				for (k = i; k < Q; k++)
					A[j][k] = (A[j][k] + A[i][k]) % 2;
	}
	else
	{
		for (j = i - 1; j >= 0; j--)
			if (A[j][i] == 1)
				for (k = 0; k < Q; k++)
					A[j][k] = (A[j][k] + A[i][k]) % 2;
	}
}

int find_unit_element_and_swap(int flag, int i, int M, int Q, BitSequence **A)
{
	int index, row_op = 0;
	if (flag == MATRIX_FORWARD_ELIMINATION)
	{
		index = i + 1;
		while ((index < M) && (A[index][i] == 0))
			index++;
		if (index < M)
			row_op = swap_rows(i, index, Q, A);
	}
	else
	{
		index = i - 1;
		while ((index >= 0) && (A[index][i] == 0))
			index--;
		if (index >= 0)
			row_op = swap_rows(i, index, Q, A);
	}
	return row_op;
}

int swap_rows(int i, int index, int Q, BitSequence **A)
{
	for (int p = 0; p < Q; p++)
	{
		BitSequence temp = A[i][p];
		A[i][p] = A[index][p];
		A[index][p] = temp;
	}
	return 1;
}

int determine_rank(int m, int M, int Q, BitSequence **A)
{
	int rank = m;
	for (int i = 0; i < M; i++)
	{
		int allZeroes = 1;
		for (int j = 0; j < Q; j++)
		{
			if (A[i][j] == 1)
			{
				allZeroes = 0;
				break;
			}
		}
		if (allZeroes)
			rank--;
	}
	return rank;
}

BitSequence **create_matrix(int M, int Q)
{
	BitSequence **matrix = (BitSequence **)calloc(M, sizeof(BitSequence *));
	if (!matrix)
		throw std::runtime_error("Insufficient memory available.");
	for (int i = 0; i < M; i++)
	{
		matrix[i] = (BitSequence *)calloc(Q, sizeof(BitSequence));
		if (!matrix[i])
			throw std::runtime_error("Insufficient memory for matrix.");
	}
	return matrix;
}

void def_matrix(int M, int Q, BitSequence **m, int k, BitSequence *epsilon)
{
	for (int i = 0; i < M; i++)
		for (int j = 0; j < Q; j++)
			m[i][j] = epsilon[k * (M * Q) + j + i * M];
}

void delete_matrix(int M, BitSequence **matrix)
{
	for (int i = 0; i < M; i++)
		free(matrix[i]);
	free(matrix);
}
