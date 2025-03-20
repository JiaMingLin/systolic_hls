#include <iostream>
#include <cstdlib> // For rand() and srand()
#include <cassert> // For assert()
#include "top.h"

#define N 3
#define M 3
#define P 2

// Function to fill a matrix with random data
void fill_random_data(DTYPE matrix[N][M]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            matrix[i][j] = rand() % 10; // Random numbers between 0 and 9
        }
    }
}

// Software method for matrix multiplication (for validation)
void software_matrix_multiply(DTYPE A[N][M], DTYPE B[M][P], DTYPE C[N][P]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < P; j++) {
            C[i][j] = 0;
            for (int k = 0; k < M; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main() {
    srand(time(0)); // Seed for random number generation

    DTYPE A[N][M], B[M][P];
    DTYPE AB[N][P], AB_ref[N][P];

    // Generate random data for matrices A and B
    fill_random_data(A);
    fill_random_data(B);

    // Call the top function
    top(A, B, AB);

    // Call the software method
    software_matrix_multiply(A, B, AB_ref);

    // Validate the output of the 
    // top function against the software method
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < P; j++) {
            assert(AB[i][j] == AB_ref[i][j]);
        }
    }
    std::cout << "Test passed" << std::endl;
    return 0;
}