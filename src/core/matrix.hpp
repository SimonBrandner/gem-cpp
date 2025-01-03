#include "./permutations.hpp"

#include <cmath>
#include <fstream>
#include <iostream>
#include <ostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#ifndef MATRIX_H
#define MATRIX_H

enum class DeterminantMethod {
	Elimination,
	ParallelElimination,
	Definition,
};

template <typename T> class Matrix;
template <typename T> class EliminableMatrix;

template <typename T>
Matrix<T>
solve_system_of_equations(Matrix<T> map, Matrix<T> right_side, bool parallel);

template <typename T> class Matrix {
	friend Matrix<T> solve_system_of_equations<T>(
		Matrix<T> map, Matrix<T> right_side, bool parallel
	);

	private:
	// Convert the matrix to an eliminable matrix for Gaussian elimination
	EliminableMatrix<T> get_eliminable() const {
		return EliminableMatrix<T>(*this);
	}

	// Join two matrices horizontally
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

	// Extract a range of columns from the matrix
	Matrix<T> extract_column_range(size_t start) const {
		return this->extract_column_range(start, this->number_of_columns);
	}

	// Extract a range of columns from the matrix with specified end
	Matrix<T> extract_column_range(size_t start, size_t end) const {
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
	// Generate a random matrix with specified size and value range
	static Matrix<T> random(size_t size, T min, T max) {
		return Matrix::random(size, size, min, max);
	}

	// Generate a random matrix with specified dimensions and value range
	static Matrix<T>
	random(size_t number_of_rows, size_t number_of_columns, T min, T max) {
		std::random_device rd;
		std::mt19937 gen(rd());

		std::vector<T> data(number_of_rows * number_of_columns);
		std::uniform_real_distribution<T> dist(min, max);
		for (size_t i = 0; i < data.size(); ++i) {
			data[i] = dist(gen);
		}

		return Matrix<T>(data, number_of_rows, number_of_columns);
	}

	// Generate an identity matrix of specified size
	static Matrix<T> identity(const size_t size) {
		std::vector<T> data(size * size, 0);
		for (size_t i = 0; i < size; ++i) {
			data[i * size + i] = 1;
		}
		return Matrix<T>(data, size, size);
	}

	// Generate a matrix filled with ones of specified size
	static Matrix<T> ones(const size_t size) {
		return Matrix::ones(size, size);
	}

	// Generate a matrix filled with ones with specified dimensions
	static Matrix<T>
	ones(const size_t number_of_rows, const size_t number_of_columns) {
		std::vector<T> data(number_of_rows * number_of_columns, 1);
		return Matrix<T>(data, number_of_rows, number_of_columns);
	}

	// Generate a Hilbert matrix of specified size
	static Matrix<T> hilbert(const size_t size) {
		std::vector<T> data(size * size);
		for (int row = 0; row < size; ++row) {
			for (int column = 0; column < size; ++column) {
				data[row * size + column] = 1.0 / (row + column + 1.0);
			}
		}
		return Matrix<T>(data, size, size);
	}

	// Load a matrix from a file
	static Matrix<T> from_file(const std::string &file_path) {
		std::ifstream file(file_path);

		std::vector<T> data;
		size_t number_of_rows = 0;
		size_t number_of_columns = 0;

		std::string line;
		while (std::getline(file, line)) {
			std::stringstream line_stream(line);

			size_t row_length = 0;
			T value;
			while (line_stream >> value) {
				data.push_back(value);
				++row_length;
			}

			if (number_of_columns != 0 && number_of_columns != row_length) {
				throw std::runtime_error(
					"Row lengths do not match in matrix file!"
				);
			}
			number_of_columns = row_length;
			++number_of_rows;
		}

		file.close();
		return Matrix<T>(data, number_of_rows, number_of_columns);
	}

	// Constructor for the Matrix class
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

	// Get the number of rows in the matrix
	const size_t get_number_of_rows() const { return this->number_of_rows; }

	// Get the number of columns in the matrix
	const size_t get_number_of_columns() const {
		return this->number_of_columns;
	}

	// Calculate the product of the diagonal elements
	const double get_diagonal_product() const {
		double product = 1;
		for (size_t position = 0; position < this->get_number_of_rows();
			 ++position) {
			product *= this->at(position, position);
		}
		return product;
	}

	const double get_determinant(
		DeterminantMethod method = DeterminantMethod::ParallelElimination
	) const {
		if (this->number_of_rows != this->number_of_columns) {
			throw std::runtime_error(
				"Cannot compute determinant of a non-square matrix!"
			);
		}

		switch (method) {
		case DeterminantMethod::Definition: {
			std::vector<std::pair<std::vector<size_t>, int>> permutations =
				generate_permutations(this->number_of_rows);

			double determinant = 0;
			for (const auto &permutation : permutations) {
				double product = permutation.second;
				for (size_t i = 0; i < this->number_of_rows; ++i) {
					product *= this->at(i, permutation.first[i]);
				}
				determinant += product;
			}
			return determinant;
		}
		case DeterminantMethod::Elimination: {
			EliminableMatrix<T> eliminable_matrix = this->get_eliminable();
			eliminable_matrix.perform_gem(false);
			return eliminable_matrix.get_diagonal_product();
		}
		case DeterminantMethod::ParallelElimination: {
			EliminableMatrix<T> eliminable_matrix = this->get_eliminable();
			eliminable_matrix.perform_gem(true);
			return eliminable_matrix.get_diagonal_product();
		}
		default:
			throw std::runtime_error("Unknown determinant method!");
		}
	}

	Matrix<T> get_inverse(bool parallel = true) const {
		if (this->number_of_rows != this->number_of_columns) {
			throw std::runtime_error("Cannot invert a non-square matrix!");
		}

		return solve_system_of_equations(
			*this, Matrix<T>::identity(this->number_of_rows), parallel
		);
	}

	const T &at(size_t row, size_t column) const {
		return this->data[row * this->number_of_columns + column];
	}

	void save_to_file(const std::string &path) const {
		std::ofstream file(path);

		for (size_t row = 0; row < this->number_of_rows; ++row) {
			for (size_t column = 0; column < this->number_of_columns;
				 ++column) {
				file << this->at(row, column);
				if (column < this->number_of_columns - 1) {
					file << " ";
				}
			}
			if (row < this->number_of_rows - 1) {
				file << std::endl;
			}
		}

		file.close();
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
