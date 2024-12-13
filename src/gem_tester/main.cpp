#include "../gem_lib/matrix.hpp"
#include "../gem_lib/system_of_equations.hpp"

#include <iostream>
#include <random>

#define MIN 100
#define MAX -100
#define EPSILON 0.01
#define FLOAT_TYPE double

Matrix<FLOAT_TYPE> random_matrix(
	size_t number_of_rows,
	size_t number_of_columns,
	FLOAT_TYPE min,
	FLOAT_TYPE max
) {
	std::random_device rd;
	std::mt19937 gen(rd());

	std::vector<FLOAT_TYPE> data(number_of_rows * number_of_columns);
	for (size_t i = 0; i < data.size(); ++i) {
		std::uniform_real_distribution<FLOAT_TYPE> dist(min, max);
		data[i] = dist(gen);
	}

	return Matrix<FLOAT_TYPE>(data, number_of_rows, number_of_columns);
}

void test_solve(Matrix<FLOAT_TYPE> map, Matrix<FLOAT_TYPE> expected_solution) {
	auto expected_right_side = map * expected_solution;

	auto solution = solve_system_of_equations(map, expected_right_side);

	auto right_side = map * solution;
	auto difference = expected_right_side - right_side;
	auto distance = abs(difference);

	if (distance < EPSILON) {
		std::cout << "Solved correctly" << std::endl;
	} else {
		std::cout << "Failed to solve correctly with distance: " << distance
				  << std::endl;
	}
}

void test_system(size_t size) {
	std::cout << "Random big test" << std::endl;
	test_solve(
		random_matrix(size, size, MIN, MAX), random_matrix(size, 1, MIN, MAX)
	);
}

void test_matrix_equation(size_t size) {
	std::cout << "Random big test matrix equation" << std::endl;
	test_solve(
		random_matrix(size, size, MIN, MAX), random_matrix(size, size, MIN, MAX)
	);
}

void test_determinant(size_t size) {
	auto matrix = random_matrix(size, size, MIN, MAX);
	std::cout << "Computing determinant for:" << matrix << std::endl;
	std::cout << "Determinant: " << matrix.get_determinant() << std::endl;
}

int main(void) {
	test_system(1000);
	/*test_random_big_matrix_equation(1000);*/
	/*test_determinant(5);*/

	return 0;
}
