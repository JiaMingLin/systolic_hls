#include <iostream>
#include <cstdlib>
#include <ctime>
#include "top.h"

int main() {
    // Initialize random seed
    std::srand(std::time(nullptr));

    // Define input and output arrays
    DTYPE A[N][M];
    DTYPE B[M][P];
    DTYPE AB_software[N][P];

    // Generate random input values for A and B
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            A[i][j] = 1; // Random value between 0 and 99
        }
    }

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < P; j++) {
            B[i][j] = 1; // Random value between 0 and 99
        }
    }


    // Perform matrix multiplication on the software
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < P; j++) {
            AB_software[i][j] = 0;
            for (int k = 0; k < M; k++) {
                AB_software[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    // Call the hardware function
    DTYPE AB_hardware[N][P];
    top(A, B, AB_hardware);

    // Check if the hardware result matches the software result
    bool success = true;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < P; j++) {
            if (AB_software[i][j] != AB_hardware[i][j]) {
                std::cout << "Mismatch: AB_software[" << i << "][" << j << "] = " << AB_software[i][j] << " != AB_hardware[" << i << "][" << j << "] = " << AB_hardware[i][j] << std::endl;
                success = false;
            }
        }
    }

    return success ? 0 : 1;
}