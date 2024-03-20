#include"top.h"

void A_IO_L3_in(A_t4 *A, hls::stream<A_t4> &fifo_A_local_out){
#pragma HLS INLINE off

	A_t4 mem_data;

	for(int i = 0; i < 4; i++){
		mem_data = A[i];
		fifo_A_local_out.write(mem_data);
	}
}

void A_IO_L2_in(int idx, hls::stream<A_t4> &fifo_A_in, hls::stream<A_t4> &fifo_A_inter_out, hls::stream<A_t4> &fifo_A_intra_out){
#pragma HLS INLINE off
    A_t4 in_A;

    for(int i = idx; i < 4; i++){
#pragma PIPELINE II=1
    	if(i == idx){
    		in_A = fifo_A_in.read();
    		fifo_A_intra_out.write(in_A);
    	}else{
    		in_A = fifo_A_in.read();
    		fifo_A_inter_out.write(in_A);
    	}
    }
}

void A_IO_L2_in_boundary(hls::stream<A_t4> &fifo_A_in, hls::stream<A_t1> &fifo_A_local_out){
#pragma HLS INLINE off
	A_t4 local_A = fifo_A_in.read();;
	A_t1 data_in;

	for(int i = 0; i < 4; i++){
#pragma PIPELINE II=1
		data_in = local_A(31,0);
		local_A = local_A >> 32;
		fifo_A_local_out.write(data_in);
	}
}

void A_IO_L1_in(int idx, hls::stream<A_t4> &fifo_A_in, hls::stream<A_t1> &fifo_A_local_out){
	A_t4 local_A = fifo_A_in.read();
	A_t1 data_in;
	for(int i = 0; i < 4; i++){
#pragma PIPELINE II=1
		data_in = local_A(31,0);
		local_A = local_A >> 32;
		fifo_A_local_out.write(data_in);
	}
}

void PE(int idx, hls::stream<A_t1> &fifo_A_local_in, hls::stream<C_t1> &fifo_C_local_out){
#pragma HLS INLINE off
	A_t1 data_A_in;
	B_t1 data_B_in = idx;
	C_t1 partial_sum = 0;

	for(int i = 0; i < 4; i++){
#pragma PIPELINE II=1
		data_A_in = fifo_A_local_in.read();
		partial_sum += data_A_in * data_B_in;
	}
	fifo_C_local_out.write(partial_sum);
}


void C_drain_IO_L1_out(int idx, hls::stream<C_t1> &fifo_C_local_in, hls::stream<C_t1> &fifo_C_inter_in, hls::stream<C_t1> &fifo_C_drain_out){
#pragma HLS INLINE off
	C_t1 data_in;

	for(int i = idx; i < 4; i++){
#pragma PIPELINE II=1
		if(i == idx){
			 data_in = fifo_C_local_in.read();
		}else{
			data_in = fifo_C_inter_in.read();
		}
		fifo_C_drain_out.write(data_in);
	}
}


void C_drain_IO_L1_out_boundary(hls::stream<C_t1> &fifo_C_local_in, hls::stream<C_t1> &fifo_C_drain_out){
#pragma HLS INLINE off
	C_t1 data_in = fifo_C_local_in.read();
	fifo_C_drain_out.write(data_in);
}

void C_drain_IO_L2_out(hls::stream<C_t1> &fifo_C_drain_in, hls::stream<C_t4> &fifo_C_drain_out){
#pragma HLS INLINE off
	C_t4 data_out = 0;
	C_t1 data_in = fifo_C_drain_in.read();
	data_out(31,0) = data_in;

	for(int i = 1; i < 4; i++){
		data_in = fifo_C_drain_in.read();
		data_out = data_out << 32;
		data_out(31, 0) = data_in;
	}
	fifo_C_drain_out.write(data_out);
}

void C_drain_IO_L3_out(hls::stream<C_t4> &fifo_C_drain_out, C_t4 *C){
#pragma HLS INLINE off
	C_t4 data_in = fifo_C_drain_out.read();
	C[0] = data_in;
}

void kernel0(A_t4 *A, C_t4 *C){

#pragma HLS DATAFLOW
	hls::stream<A_t4> fifo_A_t4_L3_out;
	hls::stream<A_t4> fifo_A_t4_L2_inter_0;
	hls::stream<A_t4> fifo_A_t4_L2_inter_1;
	hls::stream<A_t4> fifo_A_t4_L2_inter_2;

	hls::stream<A_t4> fifo_A_t4_L2_intra_0;
	hls::stream<A_t4> fifo_A_t4_L2_intra_1;
	hls::stream<A_t4> fifo_A_t4_L2_intra_2;
	hls::stream<A_t4> fifo_A_t4_L2_intra_3;

	hls::stream<A_t1> fifo_A_t1_L1_local_0;
	hls::stream<A_t1> fifo_A_t1_L1_local_1;
	hls::stream<A_t1> fifo_A_t1_L1_local_2;
	hls::stream<A_t1> fifo_A_t1_L1_local_3;

	hls::stream<C_t1> fifo_C_t1_L1_local_0;
	hls::stream<C_t1> fifo_C_t1_L1_local_1;
	hls::stream<C_t1> fifo_C_t1_L1_local_2;
	hls::stream<C_t1> fifo_C_t1_L1_local_3;

	hls::stream<C_t1> fifo_C_t1_L1_inter_0;
	hls::stream<C_t1> fifo_C_t1_L1_inter_1;
	hls::stream<C_t1> fifo_C_t1_L1_inter_2;
	hls::stream<C_t1> fifo_C_t1_L1_inter_3;

	hls::stream<C_t4> fifo_C_t4_L2_out;

	A_IO_L3_in(A, fifo_A_t4_L3_out);
	A_IO_L2_in(0, fifo_A_t4_L3_out, fifo_A_t4_L2_inter_0, fifo_A_t4_L2_intra_0);
	A_IO_L1_in(0, fifo_A_t4_L2_intra_0, fifo_A_t1_L1_local_0);

	A_IO_L2_in(1, fifo_A_t4_L2_inter_0, fifo_A_t4_L2_inter_1, fifo_A_t4_L2_intra_1);
	A_IO_L1_in(1, fifo_A_t4_L2_intra_1, fifo_A_t1_L1_local_1);

	A_IO_L2_in(2, fifo_A_t4_L2_inter_1, fifo_A_t4_L2_inter_2, fifo_A_t4_L2_intra_2);
	A_IO_L1_in(2, fifo_A_t4_L2_intra_2, fifo_A_t1_L1_local_2);
	A_IO_L2_in_boundary(fifo_A_t4_L2_inter_2, fifo_A_t1_L1_local_3);

	PE(0, fifo_A_t1_L1_local_0, fifo_C_t1_L1_local_0);
	PE(1, fifo_A_t1_L1_local_1, fifo_C_t1_L1_local_1);
	PE(2, fifo_A_t1_L1_local_2, fifo_C_t1_L1_local_2);
	PE(3, fifo_A_t1_L1_local_3, fifo_C_t1_L1_local_3);

	C_drain_IO_L1_out_boundary(fifo_C_t1_L1_local_3, fifo_C_t1_L1_inter_3);
	C_drain_IO_L1_out(2, fifo_C_t1_L1_local_2, fifo_C_t1_L1_inter_3, fifo_C_t1_L1_inter_2);
	C_drain_IO_L1_out(1, fifo_C_t1_L1_local_1, fifo_C_t1_L1_inter_2, fifo_C_t1_L1_inter_1);
	C_drain_IO_L1_out(0, fifo_C_t1_L1_local_0, fifo_C_t1_L1_inter_1, fifo_C_t1_L1_inter_0);

	C_drain_IO_L2_out(fifo_C_t1_L1_inter_0, fifo_C_t4_L2_out);
	C_drain_IO_L3_out(fifo_C_t4_L2_out, C);

}
