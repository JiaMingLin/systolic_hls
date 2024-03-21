#include <ap_int.h>
#include <hls_stream.h>

#define min(x,y) ((x < y) ? x : y)
#define max(x,y) ((x > y) ? x : y)

/* Data Type */
typedef float A_t1;
typedef float B_t1;
typedef float C_t1;
typedef ap_uint<512> A_t16;
typedef ap_uint<128> A_t4;
typedef ap_uint<512> B_t16;
typedef ap_uint<128> B_t4;
typedef ap_uint<512> C_t16;
/* Data Type */

/* Helper Function */
inline void host_serialize_A(float *A_to, float *A_from){
  /* Variable Declaration */
  unsigned int cnt = 0;
  /* Variable Declaration */

  // array
  // io_L3
  for (int c3 = 0; c3 <= 3; c3 += 1) {
    // io_L2
    for (int c5 = 0; c5 <= 3; c5 += 1)
      A_to[cnt++] = A_from[c3 * 4 + c5];
  }
}
/* Helper Function */

/* Helper Function */
inline void host_serialize_B(float *B_to, float *B_from){
  /* Variable Declaration */
  unsigned int cnt = 0;
  /* Variable Declaration */

  // array
  // io_L3
  for (int c3 = 0; c3 <= 3; c3 += 1) {
    // io_L2
    for (int c5 = 0; c5 <= 3; c5 += 1)
      B_to[cnt++] = B_from[c3 * 4 + c5];
  }
}
/* Helper Function */

/* Helper Function */
inline void host_deserialize_C(float *C_to, float *C_from){
  /* Variable Declaration */
  unsigned int cnt = 0;
  /* Variable Declaration */

  // array
  // io_L3
  for (int c3 = 0; c3 <= 3; c3 += 1) {
    // io_L2
    for (int c4 = 0; c4 <= 3; c4 += 1) {
      // io_L1
      // pe
      C_to[c4 * 4 + c3] = C_from[cnt++];
    }
  }
}
/* Helper Function */

void kernel0(A_t16 *A, B_t16 *B, C_t16 *C);
void A_IO_L2_in_intra_trans(int idx, A_t4 local_A[1][1], hls::stream<float> &fifo_A_local_out, bool intra_trans_en);
void A_IO_L2_in_inter_trans(int idx, A_t4 local_A[1][1], hls::stream<A_t4> &fifo_A_in, hls::stream<A_t4> &fifo_A_out, bool inter_trans_en);
void A_IO_L2_in_inter_trans_boundary(int idx, A_t4 local_A[1][1], hls::stream<A_t4> &fifo_A_in, bool inter_trans_en);
void B_IO_L2_in_intra_trans(int idx, B_t4 local_B[1][1], hls::stream<float> &fifo_B_local_out, bool intra_trans_en);
void B_IO_L2_in_inter_trans(int idx, B_t4 local_B[1][1], hls::stream<B_t4> &fifo_B_in, hls::stream<B_t4> &fifo_B_out, bool inter_trans_en);
void B_IO_L2_in_inter_trans_boundary(int idx, B_t4 local_B[1][1], hls::stream<B_t4> &fifo_B_in, bool inter_trans_en);
void PE_wrapper(int idx, int idy, hls::stream<float> &fifo_A_in, hls::stream<float> &fifo_A_out, hls::stream<float> &fifo_B_in, hls::stream<float> &fifo_B_out, hls::stream<float> &fifo_C_drain_out);
void C_drain_IO_L1_out_intra_trans(int idx, int idy, float local_C[1][1], hls::stream<float> &fifo_C_drain_local_in);
void C_drain_IO_L1_out_inter_trans(int idx, int idy, float local_C[1][1], hls::stream<float> &fifo_C_drain_in, hls::stream<float> &fifo_C_drain_out);
void C_drain_IO_L1_out_inter_trans_boundary(int idx, int idy, float local_C[1][1], hls::stream<float> &fifo_C_drain_out);
void C_drain_IO_L1_out_wrapper(int idx, int idy, hls::stream<float> &fifo_C_drain_in, hls::stream<float> &fifo_C_drain_out, hls::stream<float> &fifo_C_drain_local_in);
void C_drain_IO_L1_out_boundary_wrapper(int idx, int idy, hls::stream<float> &fifo_C_drain_out, hls::stream<float> &fifo_C_drain_local_in);
