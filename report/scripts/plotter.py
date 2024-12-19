import matplotlib.pyplot as plt
import matplotlib
import numpy as np

EXPONENT = -9
X_LABEL = "Matrix size"
MARKER_SIZE = 10


def n_cubed(n: int) -> float:
    return 10**EXPONENT * n**3


def load_file(path: str, y_index: int) -> tuple[list[int], list[float]]:
    x_values = []
    y_values = []
    with open(path) as file:
        for line in file.readlines():
            line_parts = line.split(", ")
            x_values.append(int(line_parts[0]))
            y_values.append(float(line_parts[y_index]))

    return (x_values, y_values)


def plot_file(path: str, y_index: int, label: str, color: str) -> tuple[int, int]:
    x_values, y_values = load_file(path, y_index)
    plt.plot(
        x_values,
        y_values,
        marker=".",
        markersize=MARKER_SIZE,
        linestyle="-",
        color=color,
        label=label,
    )

    return (x_values[0], x_values[-1])


def plot_reference(start: int, stop: int, color: str):
    n_values = np.linspace(start, stop, int((stop - start) / 50))

    plt.plot(
        n_values,
        n_cubed(n_values),
        marker=".",
        markersize=MARKER_SIZE,
        linestyle="--",
        color=color,
        label=f"$10^{{{EXPONENT}}} n^3$",
    )


def draw_plot(y_label: str, file_path: str):
    plt.xlabel(X_LABEL)
    plt.ylabel(y_label)
    plt.legend()
    plt.grid(True)

    plt.gcf().set_size_inches(10, 10)
    plt.savefig(f"{file_path}.svg", format="svg", transparent=True, dpi=1000)
    plt.show()


def init_plot():
    plt.figure(figsize=(10, 10))


def corner_start_stop(start_stops: list[tuple[int, int]] = []) -> tuple[int, int]:
    lower_start = None
    biggest_stop = None
    for start, stop in start_stops:
        if lower_start is None or start < lower_start:
            lower_start = start
        if biggest_stop is None or stop > biggest_stop:
            biggest_stop = stop

    if lower_start is None or biggest_stop is None:
        raise Exception("No lowest or biggest stop")
    return (lower_start, biggest_stop)


def plot_system_stability(file_path: str):
    init_plot()
    start_stop: list[tuple[int, int]] = []

    start_stop.append(plot_file("../data/hilbert-parallel", 1, "Residue", "blue"))
    start_stop.append(plot_file("../data/hilbert-parallel", 2, "Error", "green"))

    draw_plot("Residue/error", file_path)


def plot_system_time_complexity(file_path: str):
    init_plot()
    start_stop: list[tuple[int, int]] = []

    start_stop.append(plot_file("../data/hilbert-sequential", 3, "Sequential", "blue"))
    start_stop.append(plot_file("../data/hilbert-parallel", 3, "Parallel", "green"))

    plot_reference(*corner_start_stop(start_stop), "red")
    draw_plot("Time", file_path)


def plot_determinant_time_complexity(file_path: str):
    init_plot()
    start_stop: list[tuple[int, int]] = []

    start_stop.append(
        plot_file("../data/determinant-definition", 1, "From definition", "green")
    )
    start_stop.append(
        plot_file("../data/determinant-elimination", 1, "Sequential GEM", "blue")
    )
    start_stop.append(
        plot_file("../data/determinant-parallel", 1, "Parallel GEM", "cyan")
    )

    draw_plot("Time", file_path)


if __name__ == "__main__":
    font = {"weight": "normal", "size": 22}
    matplotlib.rc("font", **font)

    plot_system_time_complexity("../images/system_complexity")
    plot_system_stability("../images/system_stability")
    plot_determinant_time_complexity("../images/determinant_complexity")
