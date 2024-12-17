#include "../gem_lib/matrix.hpp"
#include "../gem_lib/system_of_equations.hpp"

#include <chrono>
#include <functional>
#include <iostream>
#include <stdexcept>

/*
 * The code in here could certainly be improved but since argument parsing was
 * not the focus of the semestral work, we leave it as is. An argument parsing
 * library like Rust's clap would do wonders here. (But at that point it is
 * somewhat tempting to use Rust for the whole thing)
 */

#define FLOAT_TYPE double

constexpr double MIN = 100;
constexpr double MAX = -100;
constexpr char NOT_ENOUGH_ARGS[] = "Not enough arguments!";

enum class Command { Generate, Solve, Invert, Complexity, Determinant };
enum class ComplexityTask { SystemOfEquations, MatrixEquation, Determinant };
enum class SystemMethod { Parallel, Sequential };
enum class MatrixType { Random, Identity, Ones, Hilbert };

Command string_to_command(const std::string &string_command) {
	static const std::unordered_map<std::string, Command> command_map = {
		{"generate", Command::Generate},
		{"solve", Command::Solve},
		{"invert", Command::Invert},
		{"determinant", Command::Determinant},
		{"complexity", Command::Complexity}
	};

	auto it = command_map.find(string_command);
	if (it != command_map.end()) {
		return it->second;
	}
	throw std::runtime_error("Unknown command: " + string_command);
}

ComplexityTask string_to_complexity_task(const std::string &string_task) {
	static const std::unordered_map<std::string, ComplexityTask> task_map = {
		{"determinant", ComplexityTask::Determinant},
		{"system", ComplexityTask::SystemOfEquations},
		{"equation", ComplexityTask::MatrixEquation}
	};

	auto it = task_map.find(string_task);
	if (it != task_map.end()) {
		return it->second;
	}
	throw std::runtime_error(
		"Unknown task for complexity measurement: " + string_task
	);
}

bool string_to_parallel(const std::string &string_method) {
	static const std::unordered_map<std::string, bool> method_map = {
		{"parallel", true}, {"sequential", false}
	};

	auto it = method_map.find(string_method);
	if (it != method_map.end()) {
		return it->second;
	}
	throw std::runtime_error("Unknown command: " + string_method);
}

DeterminantMethod string_to_determinant_method(const std::string &string_method
) {
	static const std::unordered_map<std::string, DeterminantMethod> method_map =
		{{"parallel-elimination", DeterminantMethod::ParallelElimination},
		 {"elimination", DeterminantMethod::Elimination},
		 {"definition", DeterminantMethod::Definition}};

	auto it = method_map.find(string_method);
	if (it != method_map.end()) {
		return it->second;
	}
	throw std::runtime_error(
		"Unknown method for solving systems: " + string_method
	);
}

MatrixType string_to_matrix_type(const std::string &string_type) {
	static const std::unordered_map<std::string, MatrixType> type_map = {
		{"random", MatrixType::Random},
		{"ones", MatrixType::Ones},
		{"identity", MatrixType::Identity},
		{"hilbert", MatrixType::Hilbert},
	};

	auto it = type_map.find(string_type);
	if (it != type_map.end()) {
		return it->second;
	}
	throw std::runtime_error("Unknown matrix type: " + string_type);
}

Matrix<FLOAT_TYPE> get_matrix_of_type(MatrixType matrix_type, size_t size) {
	switch (matrix_type) {
	case MatrixType::Random: {
		return Matrix<FLOAT_TYPE>::random(size, MIN, MAX);
	}
	case MatrixType::Hilbert: {
		return Matrix<FLOAT_TYPE>::hilbert(size);
	}
	default: {
		throw std::runtime_error("We do not support getting this matrix type");
	}
	}
}

Matrix<FLOAT_TYPE> get_solution_for_matrix_type(
	MatrixType matrix_type, size_t number_of_rows, size_t number_of_columns
) {
	switch (matrix_type) {
	case MatrixType::Random: {
		return Matrix<FLOAT_TYPE>::random(
			number_of_rows, number_of_columns, MIN, MAX
		);
	}
	case MatrixType::Hilbert: {
		return Matrix<FLOAT_TYPE>::ones(number_of_rows, number_of_columns);
	}
	default: {
		throw std::runtime_error("We do not support getting this matrix type");
	}
	}
}

Matrix<FLOAT_TYPE>
get_solution_for_matrix_type(MatrixType matrix_type, size_t size) {
	return get_solution_for_matrix_type(matrix_type, size);
}

void solve_system(MatrixType matrix_type, size_t size, bool parallel) {
	auto map = get_matrix_of_type(matrix_type, size);
	auto expected_solution = get_solution_for_matrix_type(matrix_type, size, 1);
	auto right_side = map * expected_solution;

	auto computed_solution = solve_system_of_equations(map, right_side);

	auto residue = get_residue(map, right_side, computed_solution);
	auto error = get_error(expected_solution, computed_solution);

	std::cout << residue << ", " << error << ", ";
}

void solve_matrix_equation(MatrixType matrix_type, size_t size, bool parallel) {
	auto map = get_matrix_of_type(matrix_type, size);
	auto expected_solution = get_solution_for_matrix_type(matrix_type, size);
	auto right_side = map * expected_solution;

	auto computed_solution = solve_system_of_equations(map, right_side);
	auto residue = get_residue(map, right_side, computed_solution);
	auto error = get_error(expected_solution, computed_solution);

	std::cout << residue << ", " << error << ", ";
}

void compute_determinant(
	MatrixType matrix_type, size_t size, DeterminantMethod method
) {
	get_matrix_of_type(matrix_type, size).get_determinant(method);
}

void handle_complexity_task(
	ComplexityTask task,
	MatrixType matrix_type,
	const std::string &method,
	const size_t start_size,
	const size_t step_size,
	const size_t stop_size
) {
	std::function<void(size_t)> task_function;
	switch (task) {
	case ComplexityTask::SystemOfEquations: {
		task_function = [method, matrix_type](size_t i) {
			solve_system(matrix_type, i, string_to_parallel(method));
		};
		break;
	}
	case ComplexityTask::MatrixEquation: {
		task_function = [method, matrix_type](size_t i) {
			solve_matrix_equation(matrix_type, i, string_to_parallel(method));
		};
		break;
	}
	case ComplexityTask::Determinant: {
		task_function = [method, matrix_type](size_t i) {
			compute_determinant(
				matrix_type, i, string_to_determinant_method(method)
			);
		};
		break;
	}
	}

	for (size_t i = start_size; i < stop_size; i += step_size) {
		std::cout << "(" << i << ", ";
		auto start = std::chrono::high_resolution_clock::now();
		task_function(i);
		std::chrono::duration<double> elapsed =
			std::chrono::high_resolution_clock::now() - start;
		std::cout << elapsed.count() << ")"
				  << "," << std::endl;
	}
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		throw std::runtime_error(NOT_ENOUGH_ARGS);
	}

	switch (string_to_command(argv[1])) {
	case Command::Generate: {
		switch (string_to_matrix_type(argv[2])) {
		case MatrixType::Random: {
			if (argc < 8) {
				throw std::runtime_error(NOT_ENOUGH_ARGS);
			}

			size_t number_of_rows = std::stoi(argv[3]);
			size_t number_of_columns = std::stoi(argv[4]);
			FLOAT_TYPE min = std::stod(argv[5]);
			FLOAT_TYPE max = std::stod(argv[6]);
			std::string file_path = argv[7];

			Matrix<FLOAT_TYPE>::random(
				number_of_rows, number_of_columns, min, max
			)
				.save_to_file(file_path);
			break;
		}
		case MatrixType::Ones: {
			if (argc < 6) {
				throw std::runtime_error(NOT_ENOUGH_ARGS);
			}

			size_t number_of_rows = std::stoi(argv[3]);
			size_t number_of_columns = std::stoi(argv[4]);
			std::string file_path = argv[5];

			Matrix<FLOAT_TYPE>::ones(number_of_rows, number_of_columns)
				.save_to_file(file_path);
			break;
		}
		case MatrixType::Identity: {
			if (argc < 5) {
				throw std::runtime_error(NOT_ENOUGH_ARGS);
			}

			size_t size = std::stoi(argv[3]);
			std::string file_path = argv[4];

			Matrix<FLOAT_TYPE>::identity(size).save_to_file(file_path);
			break;
		}
		case MatrixType::Hilbert: {
			if (argc < 5) {
				throw std::runtime_error(NOT_ENOUGH_ARGS);
			}

			size_t size = std::stoi(argv[3]);
			std::string file_path = argv[4];

			Matrix<FLOAT_TYPE>::hilbert(size).save_to_file(file_path);
			break;
		}
		}

		break;
	}
	case Command::Solve: {
		if (argc < 6) {
			throw std::runtime_error(NOT_ENOUGH_ARGS);
		}

		auto parallel = string_to_parallel(argv[2]);
		auto map_file_path = argv[3];
		auto right_side_file_path = argv[4];
		auto solution_file_path = argv[5];

		auto map = Matrix<FLOAT_TYPE>::from_file(map_file_path);
		auto right_side = Matrix<FLOAT_TYPE>::from_file(right_side_file_path);
		auto solution = solve_system_of_equations(map, right_side, parallel);
		solution.save_to_file(solution_file_path);

		break;
	}
	case Command::Invert: {
		if (argc < 5) {
			throw std::runtime_error(NOT_ENOUGH_ARGS);
		}

		auto parallel = string_to_parallel(argv[2]);
		auto matrix_file_path = argv[3];
		auto solution_file_path = argv[4];

		auto solution =
			Matrix<FLOAT_TYPE>::from_file(matrix_file_path).get_inverse();
		solution.save_to_file(solution_file_path);

		break;
	}
	case Command::Determinant: {
		if (argc < 4) {
			throw std::runtime_error(NOT_ENOUGH_ARGS);
		}

		auto method = string_to_determinant_method(argv[2]);
		auto file_path = argv[3];

		auto matrix = Matrix<FLOAT_TYPE>::from_file(file_path);
		auto determinant = matrix.get_determinant(method);

		std::cout << "Determinant: " << determinant << std::endl;
		break;
	}
	case Command::Complexity: {
		if (argc < 8) {
			throw std::runtime_error(NOT_ENOUGH_ARGS);
		}

		ComplexityTask task = string_to_complexity_task(argv[2]);
		MatrixType matrix_type = string_to_matrix_type(argv[3]);
		std::string method = argv[4];
		size_t start_size = std::stoi(argv[5]);
		size_t step_size = std::stoi(argv[6]);
		size_t stop_size = std::stoi(argv[7]);

		handle_complexity_task(
			task, matrix_type, method, start_size, step_size, stop_size
		);
		break;
	}
	}

	return 0;
}
