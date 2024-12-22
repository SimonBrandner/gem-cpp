#include "matrix.hpp"

#include <cstddef>
#include <functional>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <thread>
#include <vector>

#ifndef ELIMINABLE_MATRIX_H
#define ELIMINABLE_MATRIX_H

template <typename T>
Matrix<T> solve_system_of_equations(Matrix<T> map, Matrix<T> right_side);

template <typename T> class EliminableMatrix : public Matrix<T> {
	friend Matrix<T>;
	friend Matrix<T>
	solve_system_of_equations<T>(Matrix<T> map, Matrix<T> right_side);

	private:
	std::vector<size_t> row_order; // Keeps track of the row order for pivoting

	// Swaps two rows in the row_order vector
	void swap_rows(size_t row_a_index, size_t row_b_index) {
		std::swap(this->row_order[row_a_index], this->row_order[row_b_index]);
	}

	// Adds a multiple of one row to another row
	void add_row_multiple(size_t source, size_t target, T multiplicator) {
		for (size_t i = 0; i < this->number_of_columns; ++i) {
			this->at(target, i) += multiplicator * this->at(source, i);
		}
	}

	// Multiplies a row by a given multiplicator
	void multiply_row(size_t row, T multiplicator) {
		for (size_t i = 0; i < this->number_of_columns; ++i) {
			this->at(row, i) *= multiplicator;
		}
	}

	// Eliminates a row using another row based on a specific column
	void eliminate_row(size_t row, size_t by, size_t column) {
		this->add_row_multiple(
			by, row, -this->at(row, column) / this->at(by, column)
		);
	}

	// Eliminates multiple rows sequentially
	void eliminate_rows(
		size_t by_row, size_t based_on_column, size_t start_row, size_t end_row
	) {
		for (size_t row = start_row; row < end_row; ++row) {
			this->eliminate_row(row, by_row, based_on_column);
		}
	}

	// Eliminates multiple rows in parallel using multiple threads
	void eliminate_rows_in_parallel(
		size_t by_row, size_t based_on_column, size_t start_row, size_t end_row
	) {
		const size_t number_of_rows = end_row - start_row;
		const size_t number_of_threads = std::thread::hardware_concurrency();
		const size_t chunk_size = number_of_rows / number_of_threads;

		std::vector<std::thread> threads;
		threads.reserve(number_of_threads);

		for (size_t thread_index = 0; thread_index < number_of_threads;
			 ++thread_index) {
			size_t chunk_start_row = start_row + thread_index * chunk_size;
			// Ensure we do not exceed matrix bounds
			size_t chunk_end_row = (thread_index == number_of_threads - 1)
									   ? end_row
									   : chunk_start_row + chunk_size;
			threads.emplace_back(std::bind(
				&EliminableMatrix<T>::eliminate_rows,
				this,
				by_row,
				based_on_column,
				chunk_start_row,
				chunk_end_row
			));
		}

		for (auto &thread : threads) {
			thread.join();
		}
	}

	// Pivots the matrix to bring the highest value in the column to the
	// diagonal
	void pivot(size_t column) {
		std::optional<std::pair<size_t, T>> row_with_highest_value;
		for (size_t row = column; row < this->number_of_rows; ++row) {
			auto val = std::abs(this->at(row, column));
			if (!row_with_highest_value.has_value() ||
				val > row_with_highest_value->second) {
				row_with_highest_value = std::make_pair(row, val);
			}
		}

		if (!row_with_highest_value.has_value()) {
			throw std::runtime_error("No pivot!");
		}

		this->swap_rows(column, row_with_highest_value->first);
	}

	EliminableMatrix<T>(const Matrix<T> &matrix) : Matrix<T>(matrix) {
		this->row_order = std::vector<size_t>(this->number_of_rows);
		std::iota(this->row_order.begin(), this->row_order.end(), 0);
	}

	// Performs Gaussian Elimination Method (GEM) on the matrix
	void perform_gem(bool parallel = true) {
		for (size_t column = 0; column < this->number_of_rows; ++column) {
			this->pivot(column);
			if (this->at(column, column) != 0) {
				if (parallel) {
					this->eliminate_rows_in_parallel(
						column, column, column + 1, this->number_of_rows
					);
				} else {
					this->eliminate_rows(
						column, column, column + 1, this->number_of_rows
					);
				}
			}
		}
	}

	// Performs Jordan Elimination Method (JEM) on the matrix
	void perform_jem(bool parallel = true) {
		for (size_t row = 1; row < this->number_of_rows; ++row) {
			if (this->at(row, row) != 0) {
				if (parallel) {
					this->eliminate_rows_in_parallel(row, row, 0, row);
				} else {
					this->eliminate_rows(row, row, 0, row);
				}
			}
		}
	}

	// Normalizes rows based on the diagonal elements
	void normalize_rows_based_on_diagonal(bool parallel = true) {
		if (!parallel) {
			for (size_t row = 0; row < this->number_of_rows; ++row) {
				if (this->at(row, row) != 0) {
					this->multiply_row(row, 1. / this->at(row, row));
				}
			}
			return;
		}

		const size_t number_of_threads = std::thread::hardware_concurrency();
		const size_t chunk_size = this->number_of_rows / number_of_threads;

		std::vector<std::thread> threads;
		threads.reserve(number_of_threads);

		for (size_t thread_index = 0; thread_index < number_of_threads;
			 ++thread_index) {
			size_t start_row = thread_index * chunk_size;
			// Ensure we do not exceed matrix bounds
			size_t end_row = (thread_index == number_of_threads - 1)
								 ? this->number_of_rows
								 : start_row + chunk_size;
			threads.emplace_back(
				[this](size_t start_row, size_t end_row) {
					for (size_t row = start_row; row < end_row; ++row) {
						if (this->at(row, row) != 0) {
							this->multiply_row(row, 1. / this->at(row, row));
						}
					}
				},
				start_row,
				end_row
			);
		}

		for (auto &thread : threads) {
			thread.join();
		}
	}

	T &at(size_t row, size_t column) {
		return this->data[row * this->number_of_columns + column];
	}
};

#endif
