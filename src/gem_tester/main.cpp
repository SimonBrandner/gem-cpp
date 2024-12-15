#include "../gem_lib/matrix.hpp"
#include "../gem_lib/system_of_equations.hpp"

#include <iostream>

#define MIN 100
#define MAX -100
#define EPSILON 0.01
#define FLOAT_TYPE double

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
	test_solve(
		Matrix<FLOAT_TYPE>::random(size, MIN, MAX),
		Matrix<FLOAT_TYPE>::random(size, 1, MIN, MAX)
	);
}

void test_matrix_equation(size_t size) {
	std::cout << "Random big test matrix equation" << std::endl;
	test_solve(
		Matrix<FLOAT_TYPE>::random(size, MIN, MAX),
		Matrix<FLOAT_TYPE>::random(size, MIN, MAX)
	);
}

void test_determinant(size_t size) {
	auto matrix = Matrix<FLOAT_TYPE>::random(size, MIN, MAX);
	std::cout << "Determinant by definition: "
			  << matrix.get_determinant(DeterminantMethod::Definition)
			  << std::endl;
	std::cout << "Determinant by elimination: "
			  << matrix.get_determinant(DeterminantMethod::Elimination)
			  << std::endl;
	std::cout << "Determinant by parallel elimination: "
			  << matrix.get_determinant(DeterminantMethod::ParallelElimination)
			  << std::endl;
}

int main(void) {
	test_system(1000);
	/*test_random_big_matrix_equation(1000);*/
	/*test_determinant(5);*/

	return 0;
}
