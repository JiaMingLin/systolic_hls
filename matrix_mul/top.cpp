#include"top.h"

void A_IO_L3_in_serialize(A_t4 *A, hls::stream<A_t4> &fifo_A_local_out){

	A_t4 mem_data;

	for(int i = 0; i < 4; i++){
		mem_data = A[i];
		fifo_A_local_out.write(mem_data);
	}

}

void A_IO_L3_in(hls::stream<A_t4> &fifo_A_in, hls::stream<A_t4> &fifo_A_local_out){

}


void A_IO_L2_in(int idx, hls::stream<A_t4> &fifo_A_in, hls::stream<A_t4> &fifo_A_out, hls::stream<A_t1> &fifo_A_local_out){

    A_t4 local_A;

    // inter PE
    for(int i = 0; i < 4; i++){
#pragma PIPELINE II=1
    	if(i = idx){
    		local_A = fifo_A_in.read();
    	}else{
    		fifo_A_out.write(temp_A);
    	}
    }

    // intra PE
    A_t1 data_in;
    for(int i = 0; i < 4; i++){
#pragma PIPELINE II=1
    	data_in = local_A(31,0);
    	local_A = local_A >> 32;
    	fifo_A_local_out.write(data_in);
    }
}

void A_IO_L2_in_boundary(hls::stream<A_t4> &fifo_A_in, hls::stream<A_t1> &fifo_A_local_out){

	A_t4 local_A;
	A_t1 data_in;

	for(int i = 0; i < 4; i++){
#pragma PIPELINE II=1
		local_A = fifo_A_in.read();
		data_in = local_A(31,0);
		local_A = local_A >> 32;
		fifo_A_local_out.write(data_in);
	}
}

void PE(int idx, hls::stream<A_t1> &fifo_A_local_in, hls::steam<C_t1> &fifo_C_local_out){
	A_t1 data_A_in;
	B_t1 data_B_in = idx;
	C_t1 partial_sum;

	for(int i = 0; i < 4; i++){
		data_A_in = fifo_A_local_in.read();
		partial_sum += data_A_in * data_B_in;

		if(i == 3){
			fifo_C_local_out.write(partial_sum);
		}
	}
}


void C_drain_IO_L1(int idx, hls::steam<C_t1> &fifo_C_local_in, hls::stream<c_t1> &fifo_C_inter_in, hls::steam<C_t1> &fifo_C_drain_out){
	C_t1 data_in;

	for(int i = idx; i < 4; i++){
		if(i == idx){
			 data_in = fifo_C_local_in.read();
		}else{
			data_in = fifo_C_inter_in.read();
		}
		fifo_C_drain_out.write(data_in);
	}
}


void C_drain_IO_L1_boundary(int idx, hls::steam<C_t1> &fifo_C_local_in, hls::stream<C_t1> &fifo_C_drain_out){

	C_t1 data_in = fifo_C_local_in.read();
	fifo_C_drain_out.write(data_in);
}

void C_drain_IO_L2_out(hls::steam<C_t1> &fifo_C_drain_in, hls::stream<C_t4> &fifo_C_drain_out){

	C_t1 data_in;
	C_t4 data_out;
	for(int i = 0; i < 4; i++){
		data_in = fifo_C_drain_in.read();
		data_out(31, 0);
		data_out << 32;
	}
	fifo_C_drain_out.write(data_out);
}

void C_drain_IO_L3_out(hls::stream<C_t4> &fifo_C_drain_out, C_t4 *C){

	C_t4 data_in = fifo_C_drain_out.read();
	C[0] = data_in;
}

void kernel0(A_t4 *A, B_t4 *B, C_t4 *C){

}
