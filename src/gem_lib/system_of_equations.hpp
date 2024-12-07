#include "matrix.hpp"

#include <optional>

#ifndef SYSTEM_OF_EQUATIONS_H
#define SYSTEM_OF_EQUATIONS_H

template <typename T> class SystemOfEquations {
	private:
	const size_t number_of_rows;
	/*
	 *
	 */
	const size_t map_number_of_columns_of_map;
	/*
	 * The number of right side vectors
	 */
	const size_t right_side_number_of_columns;

	Matrix<T> augmented_matrix;
	std::optional<Matrix<T>> result;

	const std::vector<size_t> row_order;

	T &at(size_t row, size_t column);

	size_t get_number_of_columns();

	void swap_rows(size_t row_a_index, size_t row_b_index);
	void add_row_multiple(size_t from_index, size_t to_index, T scalar);
	void multiply_row(size_t row_index);

	void pivot(size_t column);

	public:
	SystemOfEquations(Matrix<T> &map_matrix, Matrix<T> &right_side_matrix);
	Matrix<T> perform_gem();
};

#endif
