#include <cmath>
#include <iostream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#ifndef MATRIX_H
#define MATRIX_H

template <typename T> class Matrix;
template <typename T> class EliminableMatrix;

template <typename T>
Matrix<T> solve_system_of_equations(Matrix<T> map, Matrix<T> right_side);

template <typename T> class Matrix {
	friend Matrix<T>
	solve_system_of_equations<T>(Matrix<T> map, Matrix<T> right_side);

	private:
	EliminableMatrix<T> get_eliminable() { return EliminableMatrix<T>(*this); }

	Matrix<T> right_join(const Matrix<T> &rhs) const {
		if (this->number_of_rows != rhs.get_number_of_rows()) {
			throw std::runtime_error("The number of rows does not match!");
		}

		std::vector<T> new_data;
		new_data.reserve(
			this->number_of_rows *
			(this->number_of_columns + rhs.number_of_columns)
		);

		for (size_t row = 0; row < this->number_of_rows; ++row) {
			for (size_t column = 0; column < this->number_of_columns;
				 ++column) {
				new_data.push_back(this->at(row, column));
			}
			for (size_t column = 0; column < rhs.get_number_of_columns();
				 ++column) {
				new_data.push_back(rhs.at(row, column));
			}
		}

		return Matrix<T>(
			new_data,
			this->number_of_rows,
			this->number_of_columns + rhs.get_number_of_columns()
		);
	}

	Matrix<T> extract_column_range(size_t start) {
		return this->extract_column_range(start, this->number_of_columns);
	}

	Matrix<T> extract_column_range(size_t start, size_t end) {
		std::vector<T> extracted_data;
		extracted_data.reserve((end - start) * this->get_number_of_rows());

		for (size_t row = 0; row < this->get_number_of_rows(); ++row) {
			for (size_t column = start; column < end; ++column) {
				extracted_data.push_back(this->at(row, column));
			}
		}

		return Matrix(extracted_data, this->get_number_of_rows(), end - start);
	}

	protected:
	size_t number_of_rows;
	size_t number_of_columns;
	std::vector<T> data;

	public:
	Matrix(
		std::vector<T> data, size_t number_of_rows, size_t number_of_columns
	) {
		if (data.size() != (number_of_rows * number_of_columns)) {
			throw std::runtime_error("The supplied data has the wrong size");
		}

		// We could use a member initializer list here but I find this a little
		// better since it's a bit more explicit
		this->data = data;
		this->number_of_rows = number_of_rows;
		this->number_of_columns = number_of_columns;
	}

	const size_t get_number_of_rows() const { return this->number_of_rows; }

	const size_t get_number_of_columns() const {
		return this->number_of_columns;
	}

	const T &at(size_t row, size_t column) const {
		return this->data[row * this->number_of_columns + column];
	}

	const Matrix<T> operator*(const Matrix<T> &rhs) const {
		// Matrix A is R^t -> R^r and Matrix B is R^c -> R^p but we cannot
		// compose R^c -> R^p and R^t -> R^r since p != t
		if (this->number_of_columns != rhs.number_of_rows) {
			std::stringstream error_message;
			error_message << "Cannot compose R^" << this->number_of_columns
						  << " -> R^" << this->number_of_rows << " with R^"
						  << rhs.number_of_columns << "R^" << rhs.number_of_rows
						  << "!";
			throw std::runtime_error(error_message.str());
		}
		// Matrix A is R^t -> R^r and Matrix B is R^c -> R^t. So when we compose
		// R^c -> R^t and R^t -> R^r, we get R^c -> R^r
		size_t result_number_of_rows = this->number_of_rows;
		size_t result_number_of_columns = rhs.number_of_columns;
		std::vector<T> result_data(
			result_number_of_rows * result_number_of_columns, 0
		);

		for (size_t row = 0; row < result_number_of_rows; ++row) {
			for (size_t column = 0; column < result_number_of_columns;
				 ++column) {
				result_data[row * result_number_of_columns + column] = 0;
				for (size_t i = 0; i < this->number_of_columns; ++i) {
					result_data[row * result_number_of_columns + column] +=
						this->at(row, i) * rhs.at(i, column);
				}
			}
		}

		return Matrix<T>(
			result_data, result_number_of_rows, result_number_of_columns
		);
	}

	const Matrix<T> operator-(const Matrix<T> &rhs) const {
		if (this->number_of_rows != rhs.number_of_rows ||
			this->number_of_columns != rhs.number_of_columns) {
			throw std::runtime_error(
				"Cannot subtract matrices of different sizes!"
			);
		}

		std::vector<T> result_data(
			this->number_of_rows * this->number_of_columns, 0
		);

		for (size_t i = 0; i < this->data.size(); ++i) {
			result_data[i] = this->data[i] - rhs.data[i];
		}

		return Matrix<T>(
			result_data, this->number_of_rows, this->number_of_columns
		);
	}
};

template <typename T> const double abs(const Matrix<T> &matrix) {
	double sum = 0;
	for (size_t row = 0; row < matrix.get_number_of_rows(); ++row) {
		for (size_t column = 0; column < matrix.get_number_of_columns();
			 ++column) {
			sum += pow(matrix.at(row, column), 2);
		}
	}
	return sqrt(sum);
}

template <typename T>
std::ostream &operator<<(std::ostream &stream, const Matrix<T> &matrix) {
	std::vector<size_t> column_sizes(matrix.get_number_of_columns());
	for (size_t row = 0; row < matrix.get_number_of_rows(); ++row) {
		for (size_t column = 0; column < matrix.get_number_of_columns();
			 ++column) {
			auto value_string_size =
				std::to_string(matrix.at(row, column)).size();
			if (value_string_size > column_sizes[column]) {
				column_sizes[column] = value_string_size;
			}
		}
	}

	for (size_t row = 0; row < matrix.get_number_of_rows(); ++row) {
		for (size_t column = 0; column < matrix.get_number_of_columns();
			 ++column) {
			auto value_string = std::to_string(matrix.at(row, column));
			stream << std::string(
				" ", column_sizes[column] - value_string.size()
			);
			stream << value_string;

			if (column != matrix.get_number_of_columns() - 1) {
				stream << " ";
			}
		}
		if (row != matrix.get_number_of_rows() - 1) {
			stream << std::endl;
		}
	}

	return stream;
}

#endif
