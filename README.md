# Gaussian elimination in C++

A semestral work demonstrating an implementation of Gaussian elimination in C++.

## Report

The report can be found in [report/report.pdf](./report/report.pdf)

## Requirements

- C++ compiler (e.g., `clang++``)
- CMake

## Building

```bash
git clone https://github.com/SimonBrandner/gem-cpp
cd gem-cpp
mkdir build
cd build
cmake ..
make
```

## Usage

### Commands

The program supports the following commands:

1. **Generate**: Generate a matrix and save it to a file.
2. **Solve**: Solve a system of linear equations.
3. **Invert**: Invert a matrix.
4. **Determinant**: Compute the determinant of a matrix.
5. **Complexity**: Measure the complexity of matrix operations.

### Command Line Arguments

#### Generate

```sh
./gem_tester generate <matrix_type> <args...>
```

- `matrix_type`: Type of matrix to generate (`random`, `ones`, `identity`, `hilbert`).
- Additional arguments depend on the matrix type:
  - `random`: `<rows> <columns> <min> <max> <file_path>`
  - `ones`: `<rows> <columns> <file_path>`
  - `identity`: `<size> <file_path>`
  - `hilbert`: `<size> <file_path>`

#### Solve

```sh
./gem_tester solve <method> <matrix_file> <right_side_file> <solution_file>
```

- `method`: `parallel` or `sequential`
- `matrix_file`: Path to the matrix file.
- `right_side_file`: Path to the right-hand side vector file.
- `solution_file`: Path to save the solution.

#### Invert

```sh
./gem_tester invert <method> <matrix_file> <solution_file>
```

- `method`: `parallel` or `sequential`
- `matrix_file`: Path to the matrix file.
- `solution_file`: Path to save the inverted matrix.

#### Determinant

```sh
./gem_tester determinant <method> <matrix_file>
```

- `method`: `parallel-elimination`, `elimination`, or `definition`
- `matrix_file`: Path to the matrix file.

#### Complexity

```sh
./gem_tester complexity <task> <matrix_type> <method> <start_size> <step_size> <stop_size>
```

- `task`: `system`, `equation`, or `determinant`
- `matrix_type`: Type of matrix (`random`, `hilbert`)
- `method`: `parallel` or `sequential`
- `start_size`: Initial size of the matrix.
- `step_size`: Increment size for each step.
- `stop_size`: Final size of the matrix.

## Examples

### Generate a Random Matrix

```sh
./gem_tester generate random 10 10 -100 100 random_matrix.txt
```

### Solve a System of Equations

```sh
./gem_tester solve parallel matrix.txt right_side.txt solution.txt
```

### Invert a Matrix

```sh
./gem_tester invert sequential matrix.txt inverted_matrix.txt
```

### Compute the Determinant

```sh
./gem_tester determinant elimination matrix.txt
```

### Measure Time Complexity

```sh
./gem_tester complexity system random parallel 100 10 1000
```
