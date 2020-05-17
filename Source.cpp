#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <ctime>
#include <algorithm>

using namespace std;

typedef vector<vector<int>> matrix;
int n1, m1, n2, m2, threads_count;
matrix A;
matrix B;

void print(const matrix& m) {
	for (vector<int> row : m) {
		for (int el : row) {
			cout << el << '\t';
		}
		cout << '\n';
	}
}

void SimpleMultiplication(matrix& result) {
	for (int i = 0; i < n1; ++i) {
		for (int j = 0; j < m2; ++j) {
			for (int k = 0; k < m1; ++k) {
				result[i][j] += A[i][k] * B[k][j];
			}
		}
	}
	print(result);
}

void FirstOneThread(matrix& result, int first_row, int last_row) {
	for (int i = first_row; i < last_row; ++i) {
		for (int j = 0; j < n2; ++j) {
			for (int k = 0; k < m1; ++k) {
				result[i][j] += A[i][k] * B[k][j];
			}
		}
	}
}

void FirstParralelThreads() {
	vector<vector<int>> result(n1, vector<int>(m2, 0));
	vector<thread> threads;
	int count = n1 / threads_count;
	int first_row = 0, last_row = 0;

	for (int i = 0; i < threads_count - 1; ++i) {
		first_row = i * count;
		last_row = first_row + count;
		threads.push_back(thread(FirstOneThread, ref(result), first_row, last_row));
	}
	threads.push_back(thread(FirstOneThread, ref(result), last_row, n1));

	for (int i = 0; i < threads_count; ++i) {
		threads[i].join();
	}

	print(result);
}

void SecondOneThread(matrix& result, int first_col, int last_col) {
	for (int i = 0; i < n1; ++i) {
		for (int j = first_col; j < last_col; ++j) {
			for (int k = 0; k < m2; ++k) {
				result[i][j] += A[i][k] * B[k][j];
			}
		}
	}
}

void SecondParallelThreads() {
	vector<vector<int>> result(n1, vector<int>(m2, 0));
	vector<thread> threads;
	int count = m1 / threads_count;
	int first_col = 0, last_col = 0;

	for (int i = 0; i < threads_count - 1; ++i) {
		first_col = i * count;
		last_col = first_col + count;
		threads.push_back(thread(SecondOneThread, ref(result), first_col, last_col));
	}
	threads.push_back(thread(SecondOneThread, ref(result), last_col, m1));

	for (int i = 0; i < threads_count; ++i) {
		threads[i].join();
	}

	print(result);
}

void ThirdOneThread(matrix& result, int first_col, int last_col, int first_row, int last_row) {
	for (int i = first_row; i < last_row; ++i) {
		for (int j = first_col; j < last_col; ++j) {
			for (int k = 0; k < m2; ++k) {
				result[i][j] += A[i][k] * B[k][j];
			}
		}
	}
}

void ThirdParallelThreads() {
	vector<vector<int>> result(n1, vector<int>(m2, 0));
	vector<thread> threads;
	int count_rows = n1 / threads_count;
	int count_cols = m1 / threads_count;
	int first_col = 0, last_col = 0, first_row = 0, last_row = 0;

	for (int i = 0; i < threads_count - 1; ++i) {
		first_col = i * count_cols;
		last_col = first_col + count_cols;
		for (int j = 0; j < threads_count - 1; ++j) {
			first_row = j * count_rows;
			last_row = first_row + count_rows;
			threads.push_back(thread(ThirdOneThread, ref(result), first_col, last_col, first_row, last_row));
		}
	}
	for (int i = 0; i < threads_count - 1; ++i) {
		threads.push_back(thread(ThirdOneThread, ref(result), i * count_cols, (i + 1) * count_cols, last_row, n1));
		threads.push_back(thread(ThirdOneThread, ref(result), last_col, m1, i * count_rows, (i + 1) * count_rows));
	}
	threads.push_back(thread(ThirdOneThread, ref(result), last_col, m1, last_row, n1));

	for (int i = 0; i < threads.size(); ++i) {
		threads[i].join();
	}

	print(result);
}

int main() {
	ifstream fin("input.txt");
	fin >> threads_count >> n1 >> m1;
	A = vector<vector<int>>(n1, vector<int>(m1, 0));
	for (int i = 0; i < n1; ++i) {
		for (int j = 0; j < m1; ++j) {
			fin >> A[i][j];
		}
	}

	fin >> n2 >> m2;
	if (m1 != m2) {
		cerr << "Invalid matrix dimensions!";
	}
	B = vector<vector<int>>(n2, vector<int>(m2, 0));
	for (int i = 0; i < n2; ++i) {
		for (int j = 0; j < m2; ++j) {
			fin >> B[i][j];
		}
	}
	fin.close();

	vector<vector<int>> result(n1, vector<int>(m2, 0));
	unsigned int start = clock();
	SimpleMultiplication(result);
	unsigned int end = clock();
	cout << "Simple multiplication: " << end - start << "\n\n";

	start = clock();
	FirstParralelThreads();
	end = clock();
	cout << "Multithread multiplication via first method: " << end - start << "\n\n";

	start = clock();
	SecondParallelThreads();
	end = clock();
	cout << "Multithread multiplication via second method: " << end - start << "\n\n";

	start = clock();
	ThirdParallelThreads();
	end = clock();
	cout << "Multithread multiplication via third method: " << end - start << "\n\n";

	return 0;
}