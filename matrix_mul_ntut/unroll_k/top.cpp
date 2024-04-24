#include"top.h"

void top(DTYPE A[N][M], DTYPE B[M][P], DTYPE AB[N][P]){
#pragma HLS ARRAY_RESHAPE dim=1 type=complete variable=B
#pragma HLS ARRAY_RESHAPE dim=2 type=complete variable=A


	for(int i = 0; i < N; i++){
		for(int j = 0; j < P; j++){
#pragma HLS PIPELINE II=1
			int ABij = 0;
			for(int k = 0; k < M; k++){
				ABij += A[i][k] * B[k][j];
			}
			AB[i][j] = ABij;
		}
	}
}
