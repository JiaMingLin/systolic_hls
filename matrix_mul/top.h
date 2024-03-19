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

void A_IO_L3_in(A_t4 *A, hls::stream<A_t4> &fifo_A_local_out);
void A_IO_L2_in(int idx, hls::stream<A_t4> &fifo_A_in, hls::stream<A_t4> &fifo_A_out, hls::stream<A_t1> &fifo_A_local_out);
void A_IO_L2_in_boundary(hls::stream<A_t4> &fifo_A_in, hls::stream<A_t1> &fifo_A_local_out);

void PE(int idx, hls::stream<A_t1> &fifo_A_local_in, hls::stream<C_t1> &fifo_C_local_out);

void C_drain_IO_L1_out(int idx, hls::stream<C_t1> &fifo_C_local_in, hls::stream<C_t1> &fifo_C_inter_in, hls::stream<C_t1> &fifo_C_drain_out);
void C_drain_IO_L1_out_boundary(hls::stream<C_t1> &fifo_C_local_in, hls::stream<C_t1> &fifo_C_drain_out);
void C_drain_IO_L2_out(hls::stream<C_t1> &fifo_C_drain_in, hls::stream<C_t4> &fifo_C_drain_out);
void C_drain_IO_L3_out(hls::stream<C_t4> &fifo_C_drain_out, C_t4 *C);

void kernel0(A_t4 *A, C_t4 *C);
