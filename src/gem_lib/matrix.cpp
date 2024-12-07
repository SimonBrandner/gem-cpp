#include "matrix.hpp"

#include <exception>
#include <expected>

template <typename T>
Matrix<T>::Matrix(
	std::vector<T> data, size_t number_of_rows, size_t number_of_columns
) {
	if (data.size() != number_of_rows * number_of_columns) {
		throw std::exception();
	}

	// We could use a member initializer list here but I find this a little
	// better since it's a bit more explicit
	this->data = data;
	this->number_of_rows = number_of_rows;
	this->number_of_columns = number_of_columns;
}

template <typename T> size_t Matrix<T>::get_number_of_columns() {
	return this->number_of_columns;
}

template <typename T> size_t Matrix<T>::get_number_of_rows() {
	return this->number_of_rows;
}
template <typename T> T &Matrix<T>::at(size_t row, size_t column) {
	return this->data[row * this->number_of_columns + column];
}

template <typename T> Matrix<T> Matrix<T>::right_join(Matrix<T> &rhs) {
	if (this->number_of_rows != rhs.number_of_rows) {
		throw std::exception();
	}

	std::vector<T> new_data = std::vector(
		this->number_of_rows * (this->number_of_columns + rhs.number_of_columns)
	);

	for (size_t row = 0; row < this->number_of_rows; ++row) {
		for (size_t column; column < this->number_of_columns; ++column) {
			new_data.push_back(this->at(row, column));
		}
		for (size_t column; column < rhs.number_of_columns; ++column) {
			new_data.push_back(rhs->at(row, column));
		}
	}

	return Matrix<T>(new_data);
}
