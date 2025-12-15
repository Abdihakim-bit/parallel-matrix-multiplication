#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>
#include <vector>

using namespace std;

const unsigned int NUM_ROWS_A = 1000;
const unsigned int NUM_COLS_A = 1000;
const unsigned int NUM_ROWS_B = NUM_COLS_A;
const unsigned int NUM_COLS_B = 1000;
unsigned int NUM_THREADS = 4; // You can adjust this value based on your system by using std::thread::hardware_concurrency()

void InitArray(int**& Matrix, unsigned int Rows, unsigned int Cols) {
    Matrix = new int* [Rows];
    if (Matrix == NULL) {
        exit(2);
    }
    for (unsigned int i = 0; i < Rows; i++) {
        Matrix[i] = new int[Cols];
        if (Matrix[i] == NULL) {
            exit(2);
        }
        for (unsigned int j = 0; j < Cols; j++) {
            Matrix[i][j] = rand() % 100 + 1;
        }
    }
}

void DisplayArray(int**& Matrix, unsigned int Rows, unsigned int Cols) {
    for (unsigned int i = 0; i < Rows; i++) {
        for (unsigned int j = 0; j < Cols; j++) {
            cout << " [" << Matrix[i][j] << "] ";
        }
        cout << endl;
    }
}

//Sequential Matrix Multiplication
void sequential_matrix_multiply(int** Matrix_A, unsigned int num_rows_a, unsigned int num_cols_a,
    int** Matrix_B, unsigned int num_rows_b, unsigned int num_cols_b,
    int** Result) {
    for (unsigned int i = 0; i < num_rows_a; i++) {
        for (unsigned int j = 0; j < num_cols_b; j++) {
            int sum = 0;
            for (unsigned int k = 0; k < num_cols_a; k++) {
                sum += Matrix_A[i][k] * Matrix_B[k][j];
            }
            Result[i][j] = sum;
        }
    }
}

//Matrix Multiplication from start_row till end_row
void  parallel_helper(int** Matrix_A, unsigned int start_row, unsigned int end_row, unsigned int num_cols_a,
    int** Matrix_B, unsigned int num_rows_b, unsigned int num_cols_b,
    int** Result) {
    for (unsigned int i = start_row; i < end_row; i++) {
        for (unsigned int j = 0; j < num_cols_b; j++) {
            int sum = 0;
            for (unsigned int k = 0; k < num_cols_a; k++) {
                sum += Matrix_A[i][k] * Matrix_B[k][j];
            }
            Result[i][j] = sum;
        }
    }
}

//Divide rows among the threads and call helper function to do the multiplication
void parallel_matrix_multiply(int** Matrix_A, unsigned int num_rows_a, unsigned int num_cols_a,
    int** Matrix_B, unsigned int num_rows_b, unsigned int num_cols_b,
    int** Result) {
    vector<thread> threads(NUM_THREADS);
    unsigned int start_row = 0;
    unsigned int rows_per_thread = num_rows_a / NUM_THREADS;

    for (unsigned int i = 0; i < NUM_THREADS; i++) {
        unsigned int end_row = start_row + rows_per_thread + (i < num_rows_a % NUM_THREADS ? 1 : 0);
        threads[i] = thread(parallel_helper, Matrix_A, start_row, end_row, num_cols_a,
            Matrix_B, num_rows_b, num_cols_b, Result);
        start_row = end_row;
    }

    for (unsigned int i = 0; i < threads.size(); i++) {
        threads[i].join();
    }
}

//Compare the two result matrices
bool compare_matrices(int** MatrixA, int** MatrixB, unsigned int rows, unsigned int cols) {
    for (unsigned int i = 0; i < rows; i++) {
        for (unsigned int j = 0; j < cols; j++) {
            if (MatrixA[i][j] != MatrixB[i][j]) {
                cout << "Mismatch at [" << i << "][" << j << "]: "
                    << MatrixA[i][j] << " != " << MatrixB[i][j] << endl;
                return false;
            }
        }
    }
    return true;
}

int main()
{
    int** MatrixA = nullptr;
    int** MatrixB = nullptr;
    int** Result = nullptr;
    int** ParallelResult = nullptr;

    //Allocate data for the resulting arrays
    Result = new int* [NUM_ROWS_A];
    ParallelResult = new int* [NUM_ROWS_A];
    for (unsigned int i = 0; i < NUM_ROWS_A; i++) {
        Result[i] = new int[NUM_COLS_B];
        ParallelResult[i] = new int[NUM_COLS_B];
    }

    InitArray(MatrixA, NUM_ROWS_A, NUM_COLS_A);
    InitArray(MatrixB, NUM_ROWS_B, NUM_COLS_B);

    cout << "Evaluating Sequential Task" << endl;    
    chrono::duration<double> Seq_Time(0), Par_Time(0);

    auto startTime = chrono::high_resolution_clock::now();
    sequential_matrix_multiply(MatrixA, NUM_ROWS_A, NUM_COLS_A,
        MatrixB, NUM_ROWS_B, NUM_COLS_B, Result);
    Seq_Time = chrono::high_resolution_clock::now() - startTime;

    cout << "Evaluating Parallel Task" << endl;

    startTime = chrono::high_resolution_clock::now();
    parallel_matrix_multiply(MatrixA, NUM_ROWS_A, NUM_COLS_A,
        MatrixB, NUM_ROWS_B, NUM_COLS_B, ParallelResult);
    Par_Time = chrono::high_resolution_clock::now() - startTime;

    /*
    DisplayArray(MatrixA, NUM_ROWS_A, NUM_COLS_A);
    cout << endl;
    DisplayArray(MatrixB, NUM_ROWS_B, NUM_COLS_B);
    cout << endl;
    DisplayArray(Result, NUM_ROWS_A, NUM_COLS_B);
    cout << endl;
    DisplayArray(ParallelResult, NUM_ROWS_A, NUM_COLS_B);
    cout << endl;
    */

    cout << (compare_matrices(Result, ParallelResult, NUM_ROWS_A, NUM_COLS_B) ? "Results match!" : "Results do NOT match!") << endl;

    cout << "FINAL RESULTS" << endl;
    cout << "=============" << endl;
    cout << "Sequential Processing took: " << Seq_Time.count() * 1000 << "ms" << endl;
    cout << "Parallel Processing took: " << Par_Time.count() * 1000 << "ms" << endl;
    cout << "Speedup: " << Seq_Time / Par_Time << "\n";
    cout << "Efficiency: " << (Seq_Time / Par_Time) / NUM_THREADS << "\n";

    // Free allocated memory
    for (unsigned int i = 0; i < NUM_ROWS_A; i++) {
        delete[] MatrixA[i];
        delete[] Result[i];
        delete[] ParallelResult[i];
    }
    for (unsigned int i = 0; i < NUM_ROWS_B; i++) {
        delete[] MatrixB[i];
    }
    delete[] MatrixA;
    delete[] MatrixB;
    delete[] Result;
    delete[] ParallelResult;

    return 0;
}