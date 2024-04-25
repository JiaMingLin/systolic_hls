#include"top.h"

PSUM pe(DTYPE a, DTYPE b, PSUM c){
#pragma HLS INLINE off
	c += a*b;
	return c;
}

void top(DTYPE A[N][M], DTYPE B[M][P], DTYPE AB[N][P]){
#pragma HLS ARRAY_RESHAPE dim=2 complete variable=B
#pragma HLS ARRAY_RESHAPE dim=1 complete variable=A
#pragma HLS ARRAY_RESHAPE dim=2 complete variable=AB

#pragma HLS allocation function instances=pe limit=1024

    PSUM tmp_C[N][P] = {0};
#pragma HLS ARRAY_PARTITION dim=0 complete variable=tmp_C

	LOOP_PSUM_M:for(int m = 0; m < M; m++){
#pragma HLS PIPELINE II=1

		DTYPE tmp_A[N];
		LOOP_FETCH_A: for(int i = 0 ; i < N; i++){
			#pragma HLS UNROLL
			tmp_A[i] = A[i][m];
		}

		DTYPE tmp_B[P];
		LOOP_FETCH_B: for(int j = 0; j < P; j++){
			#pragma HLS UNROLL
			tmp_B[j] = B[m][j];
		}
		
		PE_ARRAY_I: for(int i = 0; i < N; i++){
			#pragma HLS UNROLL
			PE_ARRAY_J: for(int j = 0; j< P; j++){
				#pragma HLS UNROLL
				tmp_C[i][j] = pe(tmp_A[i], tmp_B[j], tmp_C[i][j]);
			}	
		}
	}

	OFFLOAD_I: for(int i = 0; i < N; i++){
#pragma HLS PIPELINE  II=1
		OFFLOAD_J: for(int j = 0; j < P; j++){
			AB[i][j] = tmp_C[i][j];
		}
	}
}
