#include<ap_int.h>

#define N 16
#define M 16
#define P 16
#define DATA_WIDTH 32

typedef ap_uint<DATA_WIDTH> DTYPE;
typedef ap_uint<DATA_WIDTH> PSUM;

void top(DTYPE A[N][M], DTYPE B[M][P], DTYPE AB[N][P]);
PSUM pe(DTYPE a, DTYPE b, PSUM c);
