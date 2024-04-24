#include"top.h"

void top(DTYPE A[N][M], DTYPE B[M][P], DTYPE AB[N][P]){
#pragma HLS ARRAY_RESHAPE dim=2 type=complete variable=B
#pragma HLS ARRAY_RESHAPE dim=1 type=complete variable=A
#pragma HLS ARRAY_RESHAPE dim=2 type=complete variable=AB

    DTYPE tmp[N][P] = {0};
#pragma HLS ARRAY_PARTITION dim=0 type=complete variable=tmp

	for(int m = 0; m < M; m++){
#pragma HLS PIPELINE II=1

		for(int i = 0 ; i < N; i++){
			for(int j = 0; j< P; j++){
				tmp[i][j] += A[i][m] * B[m][j];
			}
		}
	}

	for(int i = 0; i < N; i++){
#pragma HLS PIPELINE  II=1
		for(int j = 0; j < P; j++){
			AB[i][j] = tmp[i][j];
		}
	}
}
