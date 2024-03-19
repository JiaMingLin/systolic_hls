#include "top.h"

const float X = 1000;

int main(){
	// init test data 4x4 array in float

	float arr[4][4];
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			arr[i][j] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/X));
		}
	}

	// packing row as ap_uint<128> for hw input
	A_t4 hw_arr[4];
	for(int i = 0; i < 4; i++){
		A_t4 temp;
		for(int j = 3; j >=0; j--){
			temp(31, 0) = (A_t1)arr[i][j];
			temp = temp << 32;
		}
		hw_arr[i] = temp;
	}

	// compute by hardware
	C_t4 hw_arr_out[1];
	kernel0(hw_arr, hw_arr_out);
	C_t4 hw_out = hw_arr_out[0];

	C_t1 hw_out_split[4];
	for(int i = 3; i <= 0; i--){
		hw_out_split[i] = (C_t1)hw_out(31,0);
		hw_out = hw_out >> 32;
	}

	// software compute
	C_t1 sw_out[4];
	for(int i = 0; i < 4; i++){
		sw_out[i] = 0;
		for(int j = 0; j < 4; i++){
			sw_out[i] += arr[i][j] * i;
		}
	}

	int err = 0;
	// check
	for(int i = 0; i < 4; i++){
		printf("software output: %f; hardware output: %f", hw_out_split[i], sw_out[i]);
		if(hw_out_split[i] != sw_out[i]) err++;
	}

	return err;
}
