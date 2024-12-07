#include "system_of_equations.hpp"
#include "matrix.hpp"

#include <cstddef>
#include <cstdlib>
#include <exception>
#include <optional>
#include <utility>
#include <vector>

template <typename T>
SystemOfEquations<T>::SystemOfEquations(
	Matrix<T> &map_matrix, Matrix<T> &right_side_matrix
) {
	if (map_matrix.get_number_of_rows() !=
		right_side_matrix.get_number_of_rows()) {
		throw std::exception();
	}

	// We could use a member initializer list here but I find this a little
	// better since it's a bit more explicit
	this->number_of_rows = map_matrix.get_number_of_rows();
	this->map_number_of_columns = map_matrix.get_number_of_columns();
	this->right_side_number_of_columns =
		right_side_matrix.get_number_of_columns();

	this->augmented_matrix = map_matrix.right_join(right_side_matrix);
	this->result = {};

	this->row_order = std::vector<size_t>(number_of_rows);
	for (size_t row = 0; row < number_of_rows; ++row) {
		this->row_order[row] = row * this->get_number_of_columns();
	}
}

template <typename T> T &SystemOfEquations<T>::at(size_t row, size_t column) {
	return this->augmented_matrix.at(this->row_order[row], column);
}

template <typename T>
void SystemOfEquations<T>::swap_rows(size_t row_a_index, size_t row_b_index) {
	auto tmp = this->row_order[row_a_index];
	this->row_order[row_a_index] = this->row_order[row_b_index];
	this->row_order[row_b_index] = tmp;
}

template <typename T>
void SystemOfEquations<T>::add_row_multiple(
	size_t from_index, size_t to_index, T multiplicator
) {
	for (size_t i = 0; i < this->get_number_of_columns(); ++i) {
		this->at(to_index, i) =
			this->at(to_index, i) + multiplicator * this->at(from_index, i);
	}
}

template <typename T> void SystemOfEquations<T>::pivot(size_t column) {
	std::optional<std::pair<size_t, T>> row_with_highest_value;
	for (size_t row = 0; row < this->number_of_rows; ++row) {
		auto val = std::abs(this->at(row, column));
		if (!row_with_highest_value.has_value()) {
			row_with_highest_value =
				std::make_optional(std::make_pair(row, val));
			continue;
		}
		if (val > row_with_highest_value.value().second) {
			row_with_highest_value.value().second = val;
		}
	}

	if (!row_with_highest_value.has_value()) {
		throw std::exception();
	}

	this->swap_rows(0, row_with_highest_value.value().second);
}

// TODO: Do we need this?
template <typename T>
void SystemOfEquations<T>::multiply_row(size_t row_index) {}

template <typename T> Matrix<T> SystemOfEquations<T>::perform_gem() {
	if (this->result.has_value()) {
		return result;
	}

	for (size_t column = 0; column < this->get_number_of_columns();
		 this->augmented_matrix.get_number_of_columns()) {
		this->pivot(column);
	}

	// Perform GEM

	return this->result;
}
