#include <vector>

#ifndef MATRIX_H
#define MATRIX_H

template <typename T> class Matrix {
	private:
	const size_t number_of_rows;
	const size_t number_of_columns;
	const std::vector<T> data;

	public:
	Matrix(
		std::vector<T> data, size_t number_of_rows, size_t number_of_columns
	);

	size_t get_number_of_rows();
	size_t get_number_of_columns();

	Matrix<T> right_join(Matrix<T> &rhs);
	T &at(size_t row, size_t column);
};

#endif
