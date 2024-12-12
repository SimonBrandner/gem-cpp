#include "matrix.hpp"

#include <algorithm>
#include <cstddef>
#include <execution>
#include <optional>
#include <ostream>
#include <stdexcept>

#ifndef SYSTEM_OF_EQUATIONS_H
#define SYSTEM_OF_EQUATIONS_H

template <typename T> class SystemOfEquations {
	friend std::ostream &operator<<(
		std::ostream &stream, const SystemOfEquations<T> &system_of_equations
	) {
		stream << system_of_equations.augmented_matrix;
		return stream;
	}

	private:
	size_t map_number_of_columns;
	size_t right_side_number_of_columns;

	Matrix<T> augmented_matrix;
	std::optional<Matrix<T>> result;

	std::vector<size_t> row_order;

	void swap_rows(size_t row_a_index, size_t row_b_index) {
		auto tmp = this->row_order[row_a_index];
		this->row_order[row_a_index] = this->row_order[row_b_index];
		this->row_order[row_b_index] = tmp;
	}

	void add_row_multiple(size_t from_index, size_t to_index, T multiplicator) {
		for (size_t i = 0; i < this->get_number_of_columns(); ++i) {
			this->at(to_index, i) =
				this->at(to_index, i) + multiplicator * this->at(from_index, i);
		}
	}

	void multiply_row(size_t row, T multiplicator) {
		for (size_t i = 0; i < this->get_number_of_columns(); ++i) {
			this->at(row, i) *= multiplicator;
		}
	}

	void pivot(size_t column) {
		std::optional<std::pair<size_t, T>> row_with_highest_value;
		for (size_t row = column; row < this->get_number_of_rows(); ++row) {
			auto val = std::abs(this->at(row, column));
			if (!row_with_highest_value.has_value()) {
				row_with_highest_value =
					std::make_optional(std::make_pair(row, val));
				continue;
			}
			if (val > row_with_highest_value.value().second) {
				row_with_highest_value.value().first = row;
				row_with_highest_value.value().second = val;
			}
		}

		if (!row_with_highest_value.has_value()) {
			throw std::runtime_error("No pivot!");
		}

		this->swap_rows(column, row_with_highest_value.value().first);
	}

	void perform_gem() {
		for (size_t column = 0; column < this->get_number_of_rows(); ++column) {
			this->pivot(column);
			if (this->at(column, column) == 0) {
				continue;
			}

			std::vector<size_t> indices(
				this->map_number_of_columns - column - 1
			);
			std::iota(indices.begin(), indices.end(), column + 1);
			std::for_each(
				std::execution::par,
				indices.begin(),
				indices.end(),
				[this, column](size_t row) {
					this->add_row_multiple(
						column,
						row,
						-this->at(row, column) / this->at(column, column)
					);
				}
			);

			/*for (size_t row = column + 1; row < this->map_number_of_columns;*/
			/*	 ++row) {*/
			/*	if (this->at(column, column) != 0) {*/
			/*		this->add_row_multiple(*/
			/*			column,*/
			/*			row,*/
			/*			-this->at(row, column) / this->at(column, column)*/
			/*		);*/
			/*	}*/
			/*}*/
		}
	}

	void perform_jem() {
		for (size_t row = 1; row < this->get_number_of_rows(); ++row) {
			if (this->at(row, row) == 0) {
				continue;
			}

			std::vector<size_t> indices(row);
			std::iota(indices.begin(), indices.end(), 0);
			std::for_each(
				std::execution::par,
				indices.begin(),
				indices.end(),
				[this, row](size_t row_to_eliminate) {
					this->add_row_multiple(
						row,
						row_to_eliminate,
						-this->at(row_to_eliminate, row) / this->at(row, row)
					);
				}
			);

			/*for (size_t row_to_eliminate = 0; row_to_eliminate < row;*/
			/*	 ++row_to_eliminate) {*/
			/*	if (this->at(row, row) != 0) {*/
			/*		this->add_row_multiple(*/
			/*			row,*/
			/*			row_to_eliminate,*/
			/*			-this->at(row_to_eliminate, row) / this->at(row, row)*/
			/*		);*/
			/*	}*/
			/*}*/
		}
	}

	void normalize_rows_based_on_diagonal() {
		std::vector<size_t> indices(this->get_number_of_rows());
		std::iota(indices.begin(), indices.end(), 0);

		std::for_each(
			std::execution::par,
			indices.begin(),
			indices.end(),
			[this](size_t row) {
				if (this->at(row, row) != 0) {
					this->multiply_row(row, 1. / this->at(row, row));
				}
			}
		);
	}

	void extract_solution() {
		std::vector<T> right_side_data;
		right_side_data.reserve(
			this->right_side_number_of_columns * this->get_number_of_rows()
		);
		for (size_t row = 0; row < this->get_number_of_rows(); ++row) {
			for (size_t column = this->map_number_of_columns;
				 column < this->get_number_of_columns();
				 ++column) {
				right_side_data.push_back(this->at(row, column));
			}
		}
		this->result = Matrix(
			right_side_data,
			this->get_number_of_rows(),
			this->right_side_number_of_columns
		);
	}

	T &at(size_t row, size_t column) {
		return this->augmented_matrix.at(this->row_order[row], column);
	}

	public:
	SystemOfEquations(Matrix<T> &map_matrix, Matrix<T> &right_side_matrix) {
		// We could use a member initializer list here but I find this a little
		// better since it's a bit more explicit
		this->augmented_matrix = map_matrix.right_join(right_side_matrix);
		this->map_number_of_columns = map_matrix.get_number_of_columns();
		this->right_side_number_of_columns =
			right_side_matrix.get_number_of_columns();

		this->result = {};

		this->row_order =
			std::vector<size_t>(this->augmented_matrix.get_number_of_rows());
		for (size_t row = 0; row < this->row_order.size(); ++row) {
			this->row_order[row] = row;
		}
	}

	const size_t get_number_of_rows() const {
		return this->augmented_matrix.get_number_of_rows();
	}

	const size_t get_number_of_columns() const {
		return this->augmented_matrix.get_number_of_columns();
	}

	const size_t get_map_number_of_columns() const {
		return this->map_number_of_columns;
	}

	const size_t get_right_side_number_of_columns() const {
		return this->right_side_number_of_columns;
	}

	const T &at(size_t row, size_t column) const {
		return this->augmented_matrix.at(this->row_order[row], column);
	}

	Matrix<T> solve() {
		if (this->result.has_value()) {
			return result.value();
		}

		this->perform_gem();
		this->perform_jem();
		this->normalize_rows_based_on_diagonal();
		this->extract_solution();

		if (!this->result.has_value()) {
			throw std::runtime_error("No solution!");
		}
		return this->result.value();
	}
};

#endif
