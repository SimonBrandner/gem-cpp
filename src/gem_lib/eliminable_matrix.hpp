#include "matrix.hpp"

#include <algorithm>
#include <cstddef>
#include <execution>
#include <numeric>
#include <optional>
#include <stdexcept>

#ifndef ELIMINABLE_MATRIX_H
#define ELIMINABLE_MATRIX_H

template <typename T>
Matrix<T> solve_system_of_equations(Matrix<T> map, Matrix<T> right_side);

template <typename T> class EliminableMatrix : public Matrix<T> {
	friend Matrix<T>;
	friend Matrix<T>
	solve_system_of_equations<T>(Matrix<T> map, Matrix<T> right_side);

	private:
	std::vector<size_t> row_order;

	void swap_rows(size_t row_a_index, size_t row_b_index) {
		auto tmp = this->row_order[row_a_index];
		this->row_order[row_a_index] = this->row_order[row_b_index];
		this->row_order[row_b_index] = tmp;
	}

	void add_row_multiple(size_t source, size_t target, T multiplicator) {
		for (size_t i = 0; i < this->number_of_columns; ++i) {
			this->at(target, i) =
				this->at(target, i) + multiplicator * this->at(source, i);
		}
	}

	void multiply_row(size_t row, T multiplicator) {
		for (size_t i = 0; i < this->number_of_columns; ++i) {
			this->at(row, i) *= multiplicator;
		}
	}

	void eliminate_row(size_t row, size_t by, size_t column) {
		this->add_row_multiple(
			by, row, -this->at(row, column) / this->at(by, column)
		);
	}

	void pivot(size_t column) {
		std::optional<std::pair<size_t, T>> row_with_highest_value;
		for (size_t row = column; row < this->number_of_rows; ++row) {
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

	EliminableMatrix<T>(const Matrix<T> &matrix) : Matrix<T>(matrix) {
		this->row_order = std::vector<size_t>(this->number_of_rows);
		std::iota(this->row_order.begin(), this->row_order.end(), 0);
	}

	void perform_gem() {
		for (size_t column = 0; column < this->number_of_rows; ++column) {
			this->pivot(column);
			if (this->at(column, column) == 0) {
				continue;
			}

			std::vector<size_t> indices(this->number_of_rows - column - 1);
			std::iota(indices.begin(), indices.end(), column + 1);
			std::for_each(
				std::execution::par,
				indices.begin(),
				indices.end(),
				[this, column](size_t row) {
					this->eliminate_row(row, column, column);
				}
			);

			/*for (size_t row = column + 1; row < this->map_number_of_columns;*/
			/*	 ++row) {*/
			/*	this->eliminate_row(row, column, column);*/
			/*}*/
		}
	}

	void perform_jem() {
		for (size_t row = 1; row < this->number_of_rows; ++row) {
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
					this->eliminate_row(row_to_eliminate, row, row);
				}
			);

			/*for (size_t row_to_eliminate = 0; row_to_eliminate < row;*/
			/*	 ++row_to_eliminate) {*/
			/*	this->eliminate_row(row_to_eliminate, row, row);*/
			/*}*/
		}
	}

	void normalize_rows_based_on_diagonal() {
		std::vector<size_t> indices(this->number_of_rows);
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

	T &at(size_t row, size_t column) {
		return this->data[row * this->number_of_columns + column];
	}
};

#endif
