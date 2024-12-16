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

enum class Command { Generate, Solve, Complexity, Determinant };
enum class ComplexityTask { SystemOfEquations, MatrixEquation, Determinant };
enum class SystemMethod { Parallel, Sequential };
enum class MatrixType { Random };

Command string_to_command(const std::string &string_command) {
	static const std::unordered_map<std::string, Command> command_map = {
		{"generate", Command::Generate},
		{"solve", Command::Solve},
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
	};

	auto it = type_map.find(string_type);
	if (it != type_map.end()) {
		return it->second;
	}
	throw std::runtime_error("Unknown matrix type: " + string_type);
}

void solve_random_system(size_t size, bool parallel) {
	solve_system_of_equations(
		Matrix<FLOAT_TYPE>::random(size, MIN, MAX),
		Matrix<FLOAT_TYPE>::random(size, 1, MIN, MAX)
	);
}

void solve_random_matrix_equation(size_t size, bool parallel) {
	solve_system_of_equations(
		Matrix<FLOAT_TYPE>::random(size, MIN, MAX),
		Matrix<FLOAT_TYPE>::random(size, MIN, MAX)
	);
}

void compute_random_determinant(size_t size, DeterminantMethod method) {
	auto matrix = Matrix<FLOAT_TYPE>::random(size, MIN, MAX);
	matrix.get_determinant(method);
}

void handle_complexity_task(
	ComplexityTask task,
	const std::string &method,
	const size_t start_size,
	const size_t step_size,
	const size_t stop_size
) {
	std::function<void(size_t)> task_function;
	switch (task) {
	case ComplexityTask::SystemOfEquations: {
		task_function = [method](size_t i) {
			solve_random_system(i, string_to_parallel(method));
		};
		break;
	}
	case ComplexityTask::MatrixEquation: {
		task_function = [method](size_t i) {
			solve_random_matrix_equation(i, string_to_parallel(method));
		};
		break;
	}
	case ComplexityTask::Determinant: {
		task_function = [method](size_t i) {
			compute_random_determinant(i, string_to_determinant_method(method));
		};
		break;
	}
	}

	for (size_t i = start_size; i < stop_size; i += step_size) {
		auto start = std::chrono::high_resolution_clock::now();
		task_function(i);
		std::chrono::duration<double> elapsed =
			std::chrono::high_resolution_clock::now() - start;
		std::cout << "(" << i << ", " << elapsed.count() << ")"
				  << "," << std::endl;
	}
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		throw std::runtime_error(NOT_ENOUGH_ARGS);
	}

	Command command = string_to_command(argv[1]);

	switch (command) {
	case Command::Generate: {
		if (argc < 8) {
			throw std::runtime_error(NOT_ENOUGH_ARGS);
		}

		MatrixType type = string_to_matrix_type(argv[2]);
		size_t number_of_columns = std::stoi(argv[3]);
		size_t number_of_rows = std::stoi(argv[4]);
		FLOAT_TYPE min = std::stod(argv[5]);
		FLOAT_TYPE max = std::stod(argv[6]);
		std::string file_path = argv[7];

		switch (type) {
		case MatrixType::Random: {
			Matrix<FLOAT_TYPE>::random(
				number_of_rows, number_of_columns, min, max
			)
				.save_to_file(file_path);
			break;
		}
		}

		break;
	}
	case Command::Solve: {
		break;
	}
	case Command::Determinant: {
		break;
	}
	case Command::Complexity: {
		if (argc < 7) {
			throw std::runtime_error(NOT_ENOUGH_ARGS);
		}

		ComplexityTask task = string_to_complexity_task(argv[2]);
		std::string method = argv[3];
		size_t start_size = std::stoi(argv[4]);
		size_t step_size = std::stoi(argv[5]);
		size_t stop_size = std::stoi(argv[6]);

		handle_complexity_task(task, method, start_size, step_size, stop_size);
		break;
	}
	}

	return 0;
}
