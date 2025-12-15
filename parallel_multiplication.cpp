#include <iostream>
#include <cmath>
#include <chrono>
using namespace std;

const unsigned int NUM_ROWS_A = 1000;
const unsigned int NUM_COLS_A = 1000;
const unsigned int NUM_ROWS_B = NUM_COLS_A;
const unsigned int NUM_COLS_B = 1000;

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

int main()
{
    int** MatrixA = nullptr;
    int** MatrixB = nullptr;
    int** Result = nullptr;

    //Allocate data for the resulting arrays
    Result = new int* [NUM_ROWS_A];
    for (unsigned int i = 0; i < NUM_ROWS_A; i++) {
        Result[i] = new int[NUM_COLS_B];
    }

    InitArray(MatrixA, NUM_ROWS_A, NUM_COLS_A);
    InitArray(MatrixB, NUM_ROWS_B, NUM_COLS_B);

    cout << "Evaluating Sequential Task" << endl;
    chrono::duration<double> Seq_Time(0);
    auto startTime = chrono::high_resolution_clock::now();
    sequential_matrix_multiply(MatrixA, NUM_ROWS_A, NUM_COLS_A,
        MatrixB, NUM_ROWS_B, NUM_COLS_B,
        Result);
    Seq_Time = chrono::high_resolution_clock::now() - startTime;

    /*
    DisplayArray(MatrixA, NUM_ROWS_A, NUM_COLS_A);
    cout << endl << endl;
    DisplayArray(MatrixB, NUM_ROWS_B, NUM_COLS_B);
    cout << endl << endl;
    DisplayArray(Result, NUM_ROWS_A, NUM_COLS_B);
    */

    cout << "FINAL RESULTS" << endl;
    cout << "=============" << endl;
    cout << "Sequential Processing took: " << Seq_Time.count() * 1000 << " ms" << endl;

	// Free allocated memory
    for (unsigned int i = 0; i < NUM_ROWS_A; i++) {
        delete[] MatrixA[i];
        delete[] Result[i];
	}
	delete[] MatrixA;
	delete[] Result;
    for (unsigned int i = 0; i < NUM_ROWS_B; i++) {
		delete[] MatrixB[i];
	}
	delete[] MatrixB;

    return 0;
}