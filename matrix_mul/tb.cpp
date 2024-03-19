#include "top.h"

const float X = 1000;

int main(){
	// init test data 4x4 array in float

	float arr[4][4];
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
//			arr[i][j] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/X));
			arr[i][j] = i*4 + j;
		}
	}

	// packing row as ap_uint<128> for hw input
	A_t4 hw_arr[4];
	for(int i = 0; i < 4; i++){
		A_t4 temp = 0;
		for(int j = 3; j >=0; j--){
			temp(31, 0) = (A_t1)arr[i][j];
			if(j > 0) temp = temp << 32;
		}
		hw_arr[i] = temp;
	}

	// compute by hardware
	C_t4 hw_arr_out[1];
	kernel0(hw_arr, hw_arr_out);
	C_t4 hw_out = hw_arr_out[0];

	C_t1 hw_out_split[4];
	for(int i = 3; i >= 0; i--){
		hw_out_split[i] = hw_out(31,0);
		hw_out = hw_out >> 32;
	}

	// software compute
	C_t1 sw_out[4];
	C_t1 temp;
	for(int i = 0; i < 4; i++){
		temp = 0;
		for(int j = 0; j < 4; j++){
//			printf("arr[%d][%d] * %d = %.4f\n", i,j,i,arr[i][j] * i);
			temp += arr[i][j] * i;
		}
		sw_out[i] = temp;
	}

	int err = 0;
	// check
	for(int i = 0; i < 4; i++){
		printf("software output: %.3f; hardware output: %.3f\n", sw_out[i], hw_out_split[i]);
		if(hw_out_split[i] != sw_out[i]) err++;
	}

	return err;
}
