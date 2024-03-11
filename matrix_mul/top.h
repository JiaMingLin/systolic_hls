#include <ap_int.h>
#include <hls_stream.h>

#define min(x,y) ((x < y) ? x : y)
#define max(x,y) ((x > y) ? x : y)

/* Data Type */

typedef ap_uint<512> A_t16;
typedef ap_uint<128> A_t4;
typedef float        A_t1;
typedef ap_uint<512> B_t16;
typedef ap_uint<128> B_t4;
typedef float        B_t1;
typedef ap_uint<512> C_t16;
typedef ap_uint<128> C_t4;
typedef float        C_t1;
/* Data Type */

void kernel0(A_t4 *A, B_t4 *B, C_t4 *C);
void A_IO_L3_in(hls::stream<A_t4> &fifo_A_in, hls::stream<A_t4> &fifo_A_local_out);
void A_IO_L3_in_serialize(A_t4 *A, hls::stream<A_t4> &fifo_A_local_out);

//void A_IO_L2_in_intra_trans(int idx, A_t4 local_A[1][1], hls::stream<float> &fifo_A_local_out, bool intra_trans_en);
//void A_IO_L2_in_inter_trans(int idx, A_t4 local_A[1][1], hls::stream<A_t4> &fifo_A_in, hls::stream<A_t4> &fifo_A_out, bool inter_trans_en);
//void A_IO_L2_in_inter_trans_boundary(int idx, A_t4 local_A[1][1], hls::stream<A_t4> &fifo_A_in, bool inter_trans_en);

void A_IO_L2_in(int idx, hls::stream<A_t4> &fifo_A_in, hls::stream<A_t4> &fifo_A_out, hls::stream<float> &fifo_A_local_out);
void A_IO_L2_in_boundary(int idx, hls::stream<A_t4> &fifo_A_in, hls::stream<float> &fifo_A_local_out);

void C_IO_L2_out(int idx, float local_c, hls::stream<float> &c_out);
