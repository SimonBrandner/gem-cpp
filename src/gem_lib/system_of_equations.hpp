#include "eliminable_matrix.hpp"
#include "matrix.hpp"

#include <stdexcept>

#ifndef SYSTEM_OF_EQUATIONS_H
#define SYSTEM_OF_EQUATIONS_H

template <typename T>
Matrix<T>
solve_system_of_equations(Matrix<T> map, Matrix<T> right_side, bool parallel) {
	if (map.get_number_of_rows() != map.get_number_of_columns()) {
		throw std::runtime_error(
			"Cannot solve a system of equations with a non-square matrix!"
		);
	}

	EliminableMatrix<T> eliminable_matrix =
		map.right_join(right_side).get_eliminable();

	eliminable_matrix.perform_gem(parallel);
	eliminable_matrix.perform_jem(parallel);
	eliminable_matrix.normalize_rows_based_on_diagonal(parallel);

	return eliminable_matrix.extract_column_range(map.get_number_of_columns());
}

template <typename T>
Matrix<T> solve_system_of_equations(Matrix<T> map, Matrix<T> right_side) {
	return solve_system_of_equations(map, right_side, true);
}

template <typename T>
double get_residue(Matrix<T> map, Matrix<T> right_side, Matrix<T> solution) {
	return abs(right_side - map * solution);
}

template <typename T>
double get_error(Matrix<T> exact_solution, Matrix<T> computed_solution) {
	return abs(exact_solution - computed_solution);
}

#endif
