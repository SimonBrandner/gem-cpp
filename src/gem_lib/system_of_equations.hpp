#include "eliminable_matrix.hpp"
#include "matrix.hpp"

#ifndef SYSTEM_OF_EQUATIONS_H
#define SYSTEM_OF_EQUATIONS_H

template <typename T>
Matrix<T> solve_system_of_equations(Matrix<T> map, Matrix<T> right_side) {
	EliminableMatrix<T> eliminable_matrix =
		map.right_join(right_side).get_eliminable();

	eliminable_matrix.perform_gem();
	eliminable_matrix.perform_jem();
	eliminable_matrix.normalize_rows_based_on_diagonal();

	return eliminable_matrix.extract_column_range(map.get_number_of_columns());
}

#endif
