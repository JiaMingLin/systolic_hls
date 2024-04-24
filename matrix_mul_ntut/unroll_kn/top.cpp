#include"top.h"

void top(DTYPE A[N][M], DTYPE B[M][P], DTYPE AB[N][P]){
#pragma HLS ARRAY_RESHAPE dim=1 type=complete variable=B
#pragma HLS ARRAY_PARTITION dim=2 type=complete variable=B

#pragma HLS ARRAY_RESHAPE dim=2 type=complete variable=A

#pragma HLS ARRAY_RESHAPE dim=2 type=complete variable=AB

	for(int i = 0; i < N; i++){
#pragma HLS PIPELINE II=1
		int ABi[P] = {0};

		for(int j = 0; j < P; j++){

			for(int k = 0; k < M; k++){
				ABi[j] += A[i][k] * B[k][j];
			}

			AB[i][j] = ABi[j];
		}

	}
}
