#include<ap_int.h>

#define N 32
#define M 32
#define P 32
#define DATA_WIDTH 16

typedef ap_uint<DATA_WIDTH> DTYPE;

void top(DTYPE A[N][M], DTYPE B[M][P], DTYPE AB[N][P]);
