#include "kernel_kernel.h"
/* Module Definition */
void A_IO_L3_in(hls::stream<A_t4> &fifo_A_in, hls::stream<A_t4> &fifo_A_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  // array
  // io_L3
  for (ap_uint<3> c3 = 0; c3 <= 3; c3 += 1) {
  #pragma HLS PIPELINE II=1
    // io_L2
    // access_coalesce
    // access_serialize
    {
      A_t4 in_data;
      A_t4 out_data;
      in_data = fifo_A_in.read();
      out_data = in_data;
      fifo_A_local_out.write(out_data);
    }
  }
}
/* Module Definition */

/* Module Definition */
void A_IO_L3_in_serialize(A_t16 *A, hls::stream<A_t4> &fifo_A_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  A_t4 fifo_data;
  A_t16 mem_data;
  for (ap_uint<1> i = 0; i < 1; i++) {
  #pragma HLS PIPELINE II=1
    mem_data = A[i];
    for (ap_uint<3> p = 0; p < 4; p++) {
      fifo_data = mem_data(127, 0);
      mem_data = mem_data >> 128;
      fifo_A_local_out.write(fifo_data);
    }
  }
}
/* Module Definition */

/* Module Definition */
void A_IO_L2_in_intra_trans(int idx, A_t4 local_A[1][1], hls::stream<float> &fifo_A_local_out, bool intra_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  ap_uint<32> data_split[4];
  #pragma HLS ARRAY_PARTITION variable=data_split complete
  /* Variable Declaration */

  if (!intra_trans_en) return;


  // array
  // io_L3
  // io_L2
  // io_L1
  // pe
  for (ap_uint<3> c5 = 0; c5 <= 3; c5 += 1) {
  #pragma HLS PIPELINE II=1
    // hls_pipeline
    {
      A_t4 in_data;
      A_t1 out_data;
      in_data = local_A[0][c5 / 4];
      for (ap_uint<3> n = 0; n < 4; n++) {
      #pragma HLS UNROLL
        data_split[n] = in_data(31, 0);
        in_data = in_data >> 32;
      }
      int split_idx = (c5) % 4;
      union {unsigned int ui; float ut;} u;
      u.ui = (unsigned int)data_split[split_idx];
      out_data = u.ut;
      fifo_A_local_out.write(out_data);
    }
  }
}
/* Module Definition */

/* Module Definition */
void A_IO_L2_in_inter_trans(int idx, A_t4 local_A[1][1], hls::stream<A_t4> &fifo_A_in, hls::stream<A_t4> &fifo_A_out, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!inter_trans_en) return;

  for (ap_uint<3> c3 = p0; c3 <= 3; c3 += 1) {
  #pragma HLS PIPELINE II=1
    // io_L2
    if (c3 == p0) {
      // access_coalesce
      {
        A_t4 in_data;
        A_t4 out_data;
        in_data = fifo_A_in.read();
        out_data = in_data;
        local_A[0][0] = out_data;
      }
    } else {
      // access_coalesce
      // hls_pipeline
      {
        A_t4 in_data;
        A_t4 out_data;
        in_data = fifo_A_in.read();
        out_data = in_data;
        fifo_A_out.write(out_data);
      }
    }
  }
}
/* Module Definition */

/* Module Definition */
void A_IO_L2_in_inter_trans_boundary(int idx, A_t4 local_A[1][1], hls::stream<A_t4> &fifo_A_in, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!inter_trans_en) return;

  for (ap_uint<3> c3 = p0; c3 <= 3; c3 += 1)
  #pragma HLS PIPELINE II=1
    if (c3 == p0) {
      // io_L2
      // access_coalesce
      {
        A_t4 in_data;
        A_t4 out_data;
        in_data = fifo_A_in.read();
        out_data = in_data;
        local_A[0][0] = out_data;
      }
    }
}
/* Module Definition */

/* Module Definition */
void A_IO_L2_in(int idx, hls::stream<A_t4> &fifo_A_in, hls::stream<A_t4> &fifo_A_out, hls::stream<float> &fifo_A_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  A_t4 local_A_ping[1][1];
  #pragma HLS RESOURCE variable=local_A_ping core=RAM_2P_BRAM
  A_t4 local_A_pong[1][1];
  #pragma HLS RESOURCE variable=local_A_pong core=RAM_2P_BRAM
  bool arb = 0;
  bool inter_trans_en = 1;
  bool intra_trans_en = 0;
  /* Variable Declaration */

  {
    // array
    // io_L3
    {
      if (arb == 0) {
        A_IO_L2_in_inter_trans(
          /* module id */ idx, 
          /* array */ local_A_pong, 
          /* fifo */ fifo_A_in, 
          /* fifo */ fifo_A_out, 
          /* enable */ inter_trans_en
        );
        A_IO_L2_in_intra_trans(
          /* module id */ idx, 
          /* array */ local_A_ping, 
          /* fifo */ fifo_A_local_out, 
          /* enable */ intra_trans_en
        );
      } else {
        A_IO_L2_in_inter_trans(
          /* module id */ idx, 
          /* array */ local_A_ping, 
          /* fifo */ fifo_A_in, 
          /* fifo */ fifo_A_out, 
          /* enable */ inter_trans_en
        );
        A_IO_L2_in_intra_trans(
          /* module id */ idx, 
          /* array */ local_A_pong, 
          /* fifo */ fifo_A_local_out, 
          /* enable */ intra_trans_en
        );
      }
      intra_trans_en = 1;
      arb = !arb;
    }
    if (arb == 0) {
      A_IO_L2_in_intra_trans(
        /* module id */ idx, 
        /* array */ local_A_ping, 
        /* fifo */ fifo_A_local_out, 
        /* enable */ intra_trans_en
      );
    } else {
      A_IO_L2_in_intra_trans(
        /* module id */ idx, 
        /* array */ local_A_pong, 
        /* fifo */ fifo_A_local_out, 
        /* enable */ intra_trans_en
      );
    }
  }
}
/* Module Definition */

/* Module Definition */
void A_IO_L2_in_boundary(int idx, hls::stream<A_t4> &fifo_A_in, hls::stream<float> &fifo_A_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  A_t4 local_A_ping[1][1];
  #pragma HLS RESOURCE variable=local_A_ping core=RAM_2P_BRAM
  A_t4 local_A_pong[1][1];
  #pragma HLS RESOURCE variable=local_A_pong core=RAM_2P_BRAM
  bool arb = 0;
  bool inter_trans_en = 1;
  bool intra_trans_en = 0;
  /* Variable Declaration */

  {
    // array
    // io_L3
    {
      if (arb == 0) {
        A_IO_L2_in_inter_trans_boundary(
          /* module id */ idx, 
          /* array */ local_A_pong, 
          /* fifo */ fifo_A_in, 
          /* enable */ inter_trans_en
        );
        A_IO_L2_in_intra_trans(
          /* module id */ idx, 
          /* array */ local_A_ping, 
          /* fifo */ fifo_A_local_out, 
          /* enable */ intra_trans_en
        );
      } else {
        A_IO_L2_in_inter_trans_boundary(
          /* module id */ idx, 
          /* array */ local_A_ping, 
          /* fifo */ fifo_A_in, 
          /* enable */ inter_trans_en
        );
        A_IO_L2_in_intra_trans(
          /* module id */ idx, 
          /* array */ local_A_pong, 
          /* fifo */ fifo_A_local_out, 
          /* enable */ intra_trans_en
        );
      }
      intra_trans_en = 1;
      arb = !arb;
    }
    if (arb == 0) {
      A_IO_L2_in_intra_trans(
        /* module id */ idx, 
        /* array */ local_A_ping, 
        /* fifo */ fifo_A_local_out, 
        /* enable */ intra_trans_en
      );
    } else {
      A_IO_L2_in_intra_trans(
        /* module id */ idx, 
        /* array */ local_A_pong, 
        /* fifo */ fifo_A_local_out, 
        /* enable */ intra_trans_en
      );
    }
  }
}
/* Module Definition */

/* Module Definition */
void B_IO_L3_in(hls::stream<B_t4> &fifo_B_in, hls::stream<B_t4> &fifo_B_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  // array
  // io_L3
  for (ap_uint<3> c3 = 0; c3 <= 3; c3 += 1) {
  #pragma HLS PIPELINE II=1
    // io_L2
    // access_coalesce
    // access_serialize
    {
      B_t4 in_data;
      B_t4 out_data;
      in_data = fifo_B_in.read();
      out_data = in_data;
      fifo_B_local_out.write(out_data);
    }
  }
}
/* Module Definition */

/* Module Definition */
void B_IO_L3_in_serialize(B_t16 *B, hls::stream<B_t4> &fifo_B_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  B_t4 fifo_data;
  B_t16 mem_data;
  for (ap_uint<1> i = 0; i < 1; i++) {
  #pragma HLS PIPELINE II=1
    mem_data = B[i];
    for (ap_uint<3> p = 0; p < 4; p++) {
      fifo_data = mem_data(127, 0);
      mem_data = mem_data >> 128;
      fifo_B_local_out.write(fifo_data);
    }
  }
}
/* Module Definition */

/* Module Definition */
void B_IO_L2_in_intra_trans(int idx, B_t4 local_B[1][1], hls::stream<float> &fifo_B_local_out, bool intra_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  ap_uint<32> data_split[4];
  #pragma HLS ARRAY_PARTITION variable=data_split complete
  /* Variable Declaration */

  if (!intra_trans_en) return;


  // array
  // io_L3
  // io_L2
  // io_L1
  // pe
  for (ap_uint<3> c5 = 0; c5 <= 3; c5 += 1) {
  #pragma HLS PIPELINE II=1
    // hls_pipeline
    {
      B_t4 in_data;
      B_t1 out_data;
      in_data = local_B[0][c5 / 4];
      for (ap_uint<3> n = 0; n < 4; n++) {
      #pragma HLS UNROLL
        data_split[n] = in_data(31, 0);
        in_data = in_data >> 32;
      }
      int split_idx = (c5) % 4;
      union {unsigned int ui; float ut;} u;
      u.ui = (unsigned int)data_split[split_idx];
      out_data = u.ut;
      fifo_B_local_out.write(out_data);
    }
  }
}
/* Module Definition */

/* Module Definition */
void B_IO_L2_in_inter_trans(int idx, B_t4 local_B[1][1], hls::stream<B_t4> &fifo_B_in, hls::stream<B_t4> &fifo_B_out, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!inter_trans_en) return;

  for (ap_uint<3> c3 = p0; c3 <= 3; c3 += 1) {
  #pragma HLS PIPELINE II=1
    // io_L2
    if (c3 == p0) {
      // access_coalesce
      {
        B_t4 in_data;
        B_t4 out_data;
        in_data = fifo_B_in.read();
        out_data = in_data;
        local_B[0][0] = out_data;
      }
    } else {
      // access_coalesce
      // hls_pipeline
      {
        B_t4 in_data;
        B_t4 out_data;
        in_data = fifo_B_in.read();
        out_data = in_data;
        fifo_B_out.write(out_data);
      }
    }
  }
}
/* Module Definition */

/* Module Definition */
void B_IO_L2_in_inter_trans_boundary(int idx, B_t4 local_B[1][1], hls::stream<B_t4> &fifo_B_in, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!inter_trans_en) return;

  for (ap_uint<3> c3 = p0; c3 <= 3; c3 += 1)
  #pragma HLS PIPELINE II=1
    if (c3 == p0) {
      // io_L2
      // access_coalesce
      {
        B_t4 in_data;
        B_t4 out_data;
        in_data = fifo_B_in.read();
        out_data = in_data;
        local_B[0][0] = out_data;
      }
    }
}
/* Module Definition */

/* Module Definition */
void B_IO_L2_in(int idx, hls::stream<B_t4> &fifo_B_in, hls::stream<B_t4> &fifo_B_out, hls::stream<float> &fifo_B_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  B_t4 local_B_ping[1][1];
  #pragma HLS RESOURCE variable=local_B_ping core=RAM_2P_BRAM
  B_t4 local_B_pong[1][1];
  #pragma HLS RESOURCE variable=local_B_pong core=RAM_2P_BRAM
  bool arb = 0;
  bool inter_trans_en = 1;
  bool intra_trans_en = 0;
  /* Variable Declaration */

  {
    // array
    // io_L3
    {
      if (arb == 0) {
        B_IO_L2_in_inter_trans(
          /* module id */ idx, 
          /* array */ local_B_pong, 
          /* fifo */ fifo_B_in, 
          /* fifo */ fifo_B_out, 
          /* enable */ inter_trans_en
        );
        B_IO_L2_in_intra_trans(
          /* module id */ idx, 
          /* array */ local_B_ping, 
          /* fifo */ fifo_B_local_out, 
          /* enable */ intra_trans_en
        );
      } else {
        B_IO_L2_in_inter_trans(
          /* module id */ idx, 
          /* array */ local_B_ping, 
          /* fifo */ fifo_B_in, 
          /* fifo */ fifo_B_out, 
          /* enable */ inter_trans_en
        );
        B_IO_L2_in_intra_trans(
          /* module id */ idx, 
          /* array */ local_B_pong, 
          /* fifo */ fifo_B_local_out, 
          /* enable */ intra_trans_en
        );
      }
      intra_trans_en = 1;
      arb = !arb;
    }
    if (arb == 0) {
      B_IO_L2_in_intra_trans(
        /* module id */ idx, 
        /* array */ local_B_ping, 
        /* fifo */ fifo_B_local_out, 
        /* enable */ intra_trans_en
      );
    } else {
      B_IO_L2_in_intra_trans(
        /* module id */ idx, 
        /* array */ local_B_pong, 
        /* fifo */ fifo_B_local_out, 
        /* enable */ intra_trans_en
      );
    }
  }
}
/* Module Definition */

/* Module Definition */
void B_IO_L2_in_boundary(int idx, hls::stream<B_t4> &fifo_B_in, hls::stream<float> &fifo_B_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  B_t4 local_B_ping[1][1];
  #pragma HLS RESOURCE variable=local_B_ping core=RAM_2P_BRAM
  B_t4 local_B_pong[1][1];
  #pragma HLS RESOURCE variable=local_B_pong core=RAM_2P_BRAM
  bool arb = 0;
  bool inter_trans_en = 1;
  bool intra_trans_en = 0;
  /* Variable Declaration */

  {
    // array
    // io_L3
    {
      if (arb == 0) {
        B_IO_L2_in_inter_trans_boundary(
          /* module id */ idx, 
          /* array */ local_B_pong, 
          /* fifo */ fifo_B_in, 
          /* enable */ inter_trans_en
        );
        B_IO_L2_in_intra_trans(
          /* module id */ idx, 
          /* array */ local_B_ping, 
          /* fifo */ fifo_B_local_out, 
          /* enable */ intra_trans_en
        );
      } else {
        B_IO_L2_in_inter_trans_boundary(
          /* module id */ idx, 
          /* array */ local_B_ping, 
          /* fifo */ fifo_B_in, 
          /* enable */ inter_trans_en
        );
        B_IO_L2_in_intra_trans(
          /* module id */ idx, 
          /* array */ local_B_pong, 
          /* fifo */ fifo_B_local_out, 
          /* enable */ intra_trans_en
        );
      }
      intra_trans_en = 1;
      arb = !arb;
    }
    if (arb == 0) {
      B_IO_L2_in_intra_trans(
        /* module id */ idx, 
        /* array */ local_B_ping, 
        /* fifo */ fifo_B_local_out, 
        /* enable */ intra_trans_en
      );
    } else {
      B_IO_L2_in_intra_trans(
        /* module id */ idx, 
        /* array */ local_B_pong, 
        /* fifo */ fifo_B_local_out, 
        /* enable */ intra_trans_en
      );
    }
  }
}
/* Module Definition */

/* Module Definition */
void PE(int idx, int idy, hls::stream<float> &fifo_A_in, hls::stream<float> &fifo_A_out, hls::stream<float> &fifo_B_in, hls::stream<float> &fifo_B_out, hls::stream<float> &fifo_C_drain_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx, p1 = idy; // module id
  A_t1 local_A[1][1];
  #pragma HLS ARRAY_PARTITION variable=local_A dim=0 complete
  B_t1 local_B[1][1];
  #pragma HLS ARRAY_PARTITION variable=local_B dim=0 complete
  C_t1 local_C[1][1];
  #pragma HLS ARRAY_PARTITION variable=local_C dim=0 complete
  /* Variable Declaration */

  // array
  // pe
  for (ap_uint<3> c5 = 0; c5 <= 3; c5 += 1) {
  #pragma HLS PIPELINE II=1
    {
      local_A[0][0] = fifo_A_in.read();
      local_B[0][0] = fifo_B_in.read();
      local_C[0][0] = (local_C[0][0] + (local_A[0][0] * local_B[0][0]));
      if (c5 == 3)
        fifo_C_drain_out.write(local_C[0][0]);
      fifo_B_out.write(local_B[0][0]);
      fifo_A_out.write(local_A[0][0]);
    }
  }
}
/* Module Definition */

/* Module Definition */
void PE_wrapper(int idx, int idy, hls::stream<float> &fifo_A_in, hls::stream<float> &fifo_A_out, hls::stream<float> &fifo_B_in, hls::stream<float> &fifo_B_out, hls::stream<float> &fifo_C_drain_out)
 {
  PE(
    /* module id */ idx, 
    /* module id */ idy, 
    /* fifo */ fifo_A_in, 
    /* fifo */ fifo_A_out, 
    /* fifo */ fifo_B_in, 
    /* fifo */ fifo_B_out, 
    /* fifo */ fifo_C_drain_out);
}
/* Module Definition */

/* Module Definition */
void A_PE_dummy_in(int idx, int idy, hls::stream<float> &fifo_A_in) {
  /* Variable Declaration */
  int p0 = idx, p1 = idy; // module id
  /* Variable Declaration */

  // array
  // pe
  for (ap_uint<3> c5 = 0; c5 <= 3; c5 += 1) {
  #pragma HLS PIPELINE II=1
    A_t1 fifo_data;
    fifo_data = fifo_A_in.read();
  }
}
/* Module Definition */

/* Module Definition */
void B_PE_dummy_in(int idx, int idy, hls::stream<float> &fifo_B_in) {
  /* Variable Declaration */
  int p0 = idx, p1 = idy; // module id
  /* Variable Declaration */

  // array
  // pe
  for (ap_uint<3> c5 = 0; c5 <= 3; c5 += 1) {
  #pragma HLS PIPELINE II=1
    B_t1 fifo_data;
    fifo_data = fifo_B_in.read();
  }
}
/* Module Definition */

/* Module Definition */
void C_drain_IO_L1_out_intra_trans(int idx, int idy, float local_C[1][1], hls::stream<float> &fifo_C_drain_local_in) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx, p1 = idy; // module id
  /* Variable Declaration */


  // io_L1
  // pe
  // hls_pipeline
  // hls_pipeline
  {
    C_t1 in_data;
    C_t1 out_data;
    in_data = fifo_C_drain_local_in.read();
    out_data = in_data;
    local_C[0][0] = out_data;
  }
}
/* Module Definition */

/* Module Definition */
void C_drain_IO_L1_out_inter_trans(int idx, int idy, float local_C[1][1], hls::stream<float> &fifo_C_drain_in, hls::stream<float> &fifo_C_drain_out) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx, p1 = idy; // module id
  /* Variable Declaration */

  for (ap_uint<3> c4 = p1; c4 <= 3; c4 += 1) {
  #pragma HLS PIPELINE II=1
    // io_L1
    if (c4 == p1) {
      // access_coalesce
      {
        C_t1 in_data;
        C_t1 out_data;
        in_data = local_C[0][0];
        out_data = in_data;
        fifo_C_drain_out.write(out_data);
      }
    } else {
      // access_coalesce
      // hls_pipeline
      {
        C_t1 in_data;
        C_t1 out_data;
        in_data = fifo_C_drain_in.read();
        out_data = in_data;
        fifo_C_drain_out.write(out_data);
      }
    }
  }
}
/* Module Definition */

/* Module Definition */
void C_drain_IO_L1_out_inter_trans_boundary(int idx, int idy, float local_C[1][1], hls::stream<float> &fifo_C_drain_out) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx, p1 = idy; // module id
  /* Variable Declaration */

  for (ap_uint<3> c4 = p1; c4 <= 3; c4 += 1)
  #pragma HLS PIPELINE II=1
    if (c4 == p1) {
      // io_L1
      // access_coalesce
      {
        C_t1 in_data;
        C_t1 out_data;
        in_data = local_C[0][0];
        out_data = in_data;
        fifo_C_drain_out.write(out_data);
      }
    }
}
/* Module Definition */

/* Module Definition */
void C_drain_IO_L1_out(int idx, int idy, hls::stream<float> &fifo_C_drain_in, hls::stream<float> &fifo_C_drain_out, hls::stream<float> &fifo_C_drain_local_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx, p1 = idy; // module id
  C_t1 local_C[1][1];
  #pragma HLS ARRAY_PARTITION variable=local_C dim=0 complete
  /* Variable Declaration */

  // array
  // io_L3
  // io_L2
  C_drain_IO_L1_out_intra_trans(
    /* module id */ idx, 
    /* module id */ idy, 
    /* array */ local_C, 
    /* fifo */ fifo_C_drain_local_in
  );
  C_drain_IO_L1_out_inter_trans(
    /* module id */ idx, 
    /* module id */ idy, 
    /* array */ local_C, 
    /* fifo */ fifo_C_drain_in, 
    /* fifo */ fifo_C_drain_out
  );
}
/* Module Definition */

/* Module Definition */
void C_drain_IO_L1_out_wrapper(int idx, int idy, hls::stream<float> &fifo_C_drain_in, hls::stream<float> &fifo_C_drain_out, hls::stream<float> &fifo_C_drain_local_in)
 {
  C_drain_IO_L1_out(
    /* module id */ idx, 
    /* module id */ idy, 
    /* fifo */ fifo_C_drain_in, 
    /* fifo */ fifo_C_drain_out, 
    /* fifo */ fifo_C_drain_local_in);
}
/* Module Definition */

/* Module Definition */
void C_drain_IO_L1_out_boundary(int idx, int idy, hls::stream<float> &fifo_C_drain_out, hls::stream<float> &fifo_C_drain_local_in) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx, p1 = idy; // module id
  C_t1 local_C[1][1];
  #pragma HLS ARRAY_PARTITION variable=local_C dim=0 complete
  /* Variable Declaration */

  // array
  // io_L3
  // io_L2
  C_drain_IO_L1_out_intra_trans(
    /* module id */ idx, 
    /* module id */ idy, 
    /* array */ local_C, 
    /* fifo */ fifo_C_drain_local_in
  );
  C_drain_IO_L1_out_inter_trans_boundary(
    /* module id */ idx, 
    /* module id */ idy, 
    /* array */ local_C, 
    /* fifo */ fifo_C_drain_out
  );
}
/* Module Definition */

/* Module Definition */
void C_drain_IO_L1_out_boundary_wrapper(int idx, int idy, hls::stream<float> &fifo_C_drain_out, hls::stream<float> &fifo_C_drain_local_in)
 {
  C_drain_IO_L1_out_boundary(
    /* module id */ idx, 
    /* module id */ idy, 
    /* fifo */ fifo_C_drain_out, 
    /* fifo */ fifo_C_drain_local_in);
}
/* Module Definition */

/* Module Definition */
void C_drain_IO_L2_out(int idx, hls::stream<float> &fifo_C_drain_in, hls::stream<float> &fifo_C_drain_out, hls::stream<float> &fifo_C_drain_local_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  // array
  // io_L3
  for (ap_uint<3> c3 = p0; c3 <= 3; c3 += 1) {
    // io_L2
    if (c3 == p0) {
      for (ap_uint<3> c4 = 0; c4 <= 3; c4 += 1) {
      #pragma HLS PIPELINE II=1
        // io_L1
        // access_coalesce
        {
          C_t1 in_data;
          C_t1 out_data;
          in_data = fifo_C_drain_local_in.read();
          out_data = in_data;
          fifo_C_drain_out.write(out_data);
        }
      }
    } else {
      for (ap_uint<3> c4 = 0; c4 <= 3; c4 += 1) {
      #pragma HLS PIPELINE II=1
        // io_L1
        // access_coalesce
        {
          C_t1 in_data;
          C_t1 out_data;
          in_data = fifo_C_drain_in.read();
          out_data = in_data;
          fifo_C_drain_out.write(out_data);
        }
      }
    }
  }
}
/* Module Definition */

/* Module Definition */
void C_drain_IO_L2_out_boundary(int idx, hls::stream<float> &fifo_C_drain_out, hls::stream<float> &fifo_C_drain_local_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  // array
  // io_L3
  for (ap_uint<3> c3 = p0; c3 <= 3; c3 += 1)
    if (c3 == p0) {
      // io_L2
      for (ap_uint<3> c4 = 0; c4 <= 3; c4 += 1) {
      #pragma HLS PIPELINE II=1
        // io_L1
        // access_coalesce
        {
          C_t1 in_data;
          C_t1 out_data;
          in_data = fifo_C_drain_local_in.read();
          out_data = in_data;
          fifo_C_drain_out.write(out_data);
        }
      }
    }
}
/* Module Definition */

/* Module Definition */
void C_drain_IO_L3_out(hls::stream<float> &fifo_C_drain_out, hls::stream<float> &fifo_C_drain_local_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  // array
  // io_L3
  for (ap_uint<3> c3 = 0; c3 <= 3; c3 += 1) {
    // io_L2
    for (ap_uint<3> c4 = 0; c4 <= 3; c4 += 1) {
    #pragma HLS PIPELINE II=1
      // io_L1
      // access_coalesce
      // access_serialize
      {
        C_t1 in_data;
        C_t1 out_data;
        in_data = fifo_C_drain_local_in.read();
        out_data = in_data;
        fifo_C_drain_out.write(out_data);
      }
    }
  }
}
/* Module Definition */

/* Module Definition */
void C_drain_IO_L3_out_serialize(C_t16 *C, hls::stream<float> &fifo_C_drain_local_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  for (ap_uint<1> i = 0; i < 1; i++) {
  #pragma HLS PIPELINE II=1
    C_t1 fifo_data;
    C_t16 mem_data;
    ap_uint<32> mem_data_split[16];
    #pragma HLS ARRAY_PARTITION variable=mem_data_split complete
    for (ap_uint<5> p = 0; p < 16; p++) {
      fifo_data = fifo_C_drain_local_in.read();
      union {unsigned int ui; float ut;} u;
      u.ut = fifo_data;
      mem_data_split[p] = ap_uint<32>(u.ui);
    }
    mem_data = (mem_data_split[15], mem_data_split[14], mem_data_split[13], mem_data_split[12], mem_data_split[11], mem_data_split[10], mem_data_split[9], mem_data_split[8], mem_data_split[7], mem_data_split[6], mem_data_split[5], mem_data_split[4], mem_data_split[3], mem_data_split[2], mem_data_split[1], mem_data_split[0]);
    C[i] = mem_data;
  }
}
/* Module Definition */

void kernel0(A_t16 *A, B_t16 *B, C_t16 *C)
{
#pragma HLS INTERFACE m_axi port=A offset=slave bundle=gmem_A
#pragma HLS INTERFACE m_axi port=B offset=slave bundle=gmem_B
#pragma HLS INTERFACE m_axi port=C offset=slave bundle=gmem_C
#pragma HLS INTERFACE s_axilite port=A bundle=control
#pragma HLS INTERFACE s_axilite port=B bundle=control
#pragma HLS INTERFACE s_axilite port=C bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

#pragma HLS DATAFLOW

  /* FIFO Declaration */
  /* A_IO_L3_in_serialize fifo */ hls::stream<A_t4> fifo_A_A_IO_L3_in_serialize;
  #pragma HLS STREAM variable=fifo_A_A_IO_L3_in_serialize depth=2
  /* B_IO_L3_in_serialize fifo */ hls::stream<B_t4> fifo_B_B_IO_L3_in_serialize;
  #pragma HLS STREAM variable=fifo_B_B_IO_L3_in_serialize depth=2
  /* C_drain_IO_L3_out_serialize fifo */ hls::stream<float> fifo_C_drain_C_drain_IO_L3_out_serialize;
  #pragma HLS STREAM variable=fifo_C_drain_C_drain_IO_L3_out_serialize depth=2
  /* A_IO_L2_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L2_in_0;
  #pragma HLS STREAM variable=fifo_A_A_IO_L2_in_0 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L2_in_0 core=FIFO_SRL
  /* A_IO_L2_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L2_in_1;
  #pragma HLS STREAM variable=fifo_A_A_IO_L2_in_1 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L2_in_1 core=FIFO_SRL
  /* A_IO_L2_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L2_in_2;
  #pragma HLS STREAM variable=fifo_A_A_IO_L2_in_2 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L2_in_2 core=FIFO_SRL
  /* A_IO_L2_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L2_in_3;
  #pragma HLS STREAM variable=fifo_A_A_IO_L2_in_3 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L2_in_3 core=FIFO_SRL
  /* A_IO_L2_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L2_in_4;
  #pragma HLS STREAM variable=fifo_A_A_IO_L2_in_4 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L2_in_4 core=FIFO_SRL
  /* B_IO_L2_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L2_in_0;
  #pragma HLS STREAM variable=fifo_B_B_IO_L2_in_0 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L2_in_0 core=FIFO_SRL
  /* B_IO_L2_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L2_in_1;
  #pragma HLS STREAM variable=fifo_B_B_IO_L2_in_1 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L2_in_1 core=FIFO_SRL
  /* B_IO_L2_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L2_in_2;
  #pragma HLS STREAM variable=fifo_B_B_IO_L2_in_2 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L2_in_2 core=FIFO_SRL
  /* B_IO_L2_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L2_in_3;
  #pragma HLS STREAM variable=fifo_B_B_IO_L2_in_3 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L2_in_3 core=FIFO_SRL
  /* B_IO_L2_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L2_in_4;
  #pragma HLS STREAM variable=fifo_B_B_IO_L2_in_4 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L2_in_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_0_0;
  #pragma HLS STREAM variable=fifo_A_PE_0_0 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_0_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_0_1;
  #pragma HLS STREAM variable=fifo_A_PE_0_1 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_0_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_0_2;
  #pragma HLS STREAM variable=fifo_A_PE_0_2 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_0_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_0_3;
  #pragma HLS STREAM variable=fifo_A_PE_0_3 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_0_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_0_4;
  #pragma HLS STREAM variable=fifo_A_PE_0_4 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_0_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_1_0;
  #pragma HLS STREAM variable=fifo_A_PE_1_0 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_1_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_1_1;
  #pragma HLS STREAM variable=fifo_A_PE_1_1 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_1_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_1_2;
  #pragma HLS STREAM variable=fifo_A_PE_1_2 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_1_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_1_3;
  #pragma HLS STREAM variable=fifo_A_PE_1_3 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_1_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_1_4;
  #pragma HLS STREAM variable=fifo_A_PE_1_4 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_1_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_2_0;
  #pragma HLS STREAM variable=fifo_A_PE_2_0 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_2_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_2_1;
  #pragma HLS STREAM variable=fifo_A_PE_2_1 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_2_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_2_2;
  #pragma HLS STREAM variable=fifo_A_PE_2_2 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_2_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_2_3;
  #pragma HLS STREAM variable=fifo_A_PE_2_3 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_2_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_2_4;
  #pragma HLS STREAM variable=fifo_A_PE_2_4 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_2_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_3_0;
  #pragma HLS STREAM variable=fifo_A_PE_3_0 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_3_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_3_1;
  #pragma HLS STREAM variable=fifo_A_PE_3_1 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_3_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_3_2;
  #pragma HLS STREAM variable=fifo_A_PE_3_2 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_3_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_3_3;
  #pragma HLS STREAM variable=fifo_A_PE_3_3 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_3_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_3_4;
  #pragma HLS STREAM variable=fifo_A_PE_3_4 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_3_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_0_0;
  #pragma HLS STREAM variable=fifo_B_PE_0_0 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_0_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_1_0;
  #pragma HLS STREAM variable=fifo_B_PE_1_0 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_1_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_2_0;
  #pragma HLS STREAM variable=fifo_B_PE_2_0 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_2_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_3_0;
  #pragma HLS STREAM variable=fifo_B_PE_3_0 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_3_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_4_0;
  #pragma HLS STREAM variable=fifo_B_PE_4_0 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_4_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_0_1;
  #pragma HLS STREAM variable=fifo_B_PE_0_1 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_0_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_1_1;
  #pragma HLS STREAM variable=fifo_B_PE_1_1 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_1_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_2_1;
  #pragma HLS STREAM variable=fifo_B_PE_2_1 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_2_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_3_1;
  #pragma HLS STREAM variable=fifo_B_PE_3_1 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_3_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_4_1;
  #pragma HLS STREAM variable=fifo_B_PE_4_1 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_4_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_0_2;
  #pragma HLS STREAM variable=fifo_B_PE_0_2 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_0_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_1_2;
  #pragma HLS STREAM variable=fifo_B_PE_1_2 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_1_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_2_2;
  #pragma HLS STREAM variable=fifo_B_PE_2_2 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_2_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_3_2;
  #pragma HLS STREAM variable=fifo_B_PE_3_2 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_3_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_4_2;
  #pragma HLS STREAM variable=fifo_B_PE_4_2 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_4_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_0_3;
  #pragma HLS STREAM variable=fifo_B_PE_0_3 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_0_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_1_3;
  #pragma HLS STREAM variable=fifo_B_PE_1_3 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_1_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_2_3;
  #pragma HLS STREAM variable=fifo_B_PE_2_3 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_2_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_3_3;
  #pragma HLS STREAM variable=fifo_B_PE_3_3 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_3_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_4_3;
  #pragma HLS STREAM variable=fifo_B_PE_4_3 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_4_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_C_drain_PE_0_0;
  #pragma HLS STREAM variable=fifo_C_drain_PE_0_0 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_PE_0_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_C_drain_PE_1_0;
  #pragma HLS STREAM variable=fifo_C_drain_PE_1_0 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_PE_1_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_C_drain_PE_2_0;
  #pragma HLS STREAM variable=fifo_C_drain_PE_2_0 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_PE_2_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_C_drain_PE_3_0;
  #pragma HLS STREAM variable=fifo_C_drain_PE_3_0 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_PE_3_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_C_drain_PE_0_1;
  #pragma HLS STREAM variable=fifo_C_drain_PE_0_1 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_PE_0_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_C_drain_PE_1_1;
  #pragma HLS STREAM variable=fifo_C_drain_PE_1_1 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_PE_1_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_C_drain_PE_2_1;
  #pragma HLS STREAM variable=fifo_C_drain_PE_2_1 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_PE_2_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_C_drain_PE_3_1;
  #pragma HLS STREAM variable=fifo_C_drain_PE_3_1 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_PE_3_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_C_drain_PE_0_2;
  #pragma HLS STREAM variable=fifo_C_drain_PE_0_2 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_PE_0_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_C_drain_PE_1_2;
  #pragma HLS STREAM variable=fifo_C_drain_PE_1_2 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_PE_1_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_C_drain_PE_2_2;
  #pragma HLS STREAM variable=fifo_C_drain_PE_2_2 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_PE_2_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_C_drain_PE_3_2;
  #pragma HLS STREAM variable=fifo_C_drain_PE_3_2 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_PE_3_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_C_drain_PE_0_3;
  #pragma HLS STREAM variable=fifo_C_drain_PE_0_3 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_PE_0_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_C_drain_PE_1_3;
  #pragma HLS STREAM variable=fifo_C_drain_PE_1_3 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_PE_1_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_C_drain_PE_2_3;
  #pragma HLS STREAM variable=fifo_C_drain_PE_2_3 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_PE_2_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_C_drain_PE_3_3;
  #pragma HLS STREAM variable=fifo_C_drain_PE_3_3 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_PE_3_3 core=FIFO_SRL
  /* C_drain_IO_L1_out fifo */ hls::stream<float> fifo_C_drain_C_drain_IO_L1_out_0_0;
  #pragma HLS STREAM variable=fifo_C_drain_C_drain_IO_L1_out_0_0 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_C_drain_IO_L1_out_0_0 core=FIFO_SRL
  /* C_drain_IO_L1_out fifo */ hls::stream<float> fifo_C_drain_C_drain_IO_L1_out_0_1;
  #pragma HLS STREAM variable=fifo_C_drain_C_drain_IO_L1_out_0_1 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_C_drain_IO_L1_out_0_1 core=FIFO_SRL
  /* C_drain_IO_L1_out fifo */ hls::stream<float> fifo_C_drain_C_drain_IO_L1_out_0_2;
  #pragma HLS STREAM variable=fifo_C_drain_C_drain_IO_L1_out_0_2 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_C_drain_IO_L1_out_0_2 core=FIFO_SRL
  /* C_drain_IO_L1_out fifo */ hls::stream<float> fifo_C_drain_C_drain_IO_L1_out_0_3;
  #pragma HLS STREAM variable=fifo_C_drain_C_drain_IO_L1_out_0_3 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_C_drain_IO_L1_out_0_3 core=FIFO_SRL
  /* C_drain_IO_L1_out fifo */ hls::stream<float> fifo_C_drain_C_drain_IO_L1_out_0_4;
  #pragma HLS STREAM variable=fifo_C_drain_C_drain_IO_L1_out_0_4 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_C_drain_IO_L1_out_0_4 core=FIFO_SRL
  /* C_drain_IO_L1_out fifo */ hls::stream<float> fifo_C_drain_C_drain_IO_L1_out_1_0;
  #pragma HLS STREAM variable=fifo_C_drain_C_drain_IO_L1_out_1_0 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_C_drain_IO_L1_out_1_0 core=FIFO_SRL
  /* C_drain_IO_L1_out fifo */ hls::stream<float> fifo_C_drain_C_drain_IO_L1_out_1_1;
  #pragma HLS STREAM variable=fifo_C_drain_C_drain_IO_L1_out_1_1 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_C_drain_IO_L1_out_1_1 core=FIFO_SRL
  /* C_drain_IO_L1_out fifo */ hls::stream<float> fifo_C_drain_C_drain_IO_L1_out_1_2;
  #pragma HLS STREAM variable=fifo_C_drain_C_drain_IO_L1_out_1_2 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_C_drain_IO_L1_out_1_2 core=FIFO_SRL
  /* C_drain_IO_L1_out fifo */ hls::stream<float> fifo_C_drain_C_drain_IO_L1_out_1_3;
  #pragma HLS STREAM variable=fifo_C_drain_C_drain_IO_L1_out_1_3 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_C_drain_IO_L1_out_1_3 core=FIFO_SRL
  /* C_drain_IO_L1_out fifo */ hls::stream<float> fifo_C_drain_C_drain_IO_L1_out_1_4;
  #pragma HLS STREAM variable=fifo_C_drain_C_drain_IO_L1_out_1_4 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_C_drain_IO_L1_out_1_4 core=FIFO_SRL
  /* C_drain_IO_L1_out fifo */ hls::stream<float> fifo_C_drain_C_drain_IO_L1_out_2_0;
  #pragma HLS STREAM variable=fifo_C_drain_C_drain_IO_L1_out_2_0 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_C_drain_IO_L1_out_2_0 core=FIFO_SRL
  /* C_drain_IO_L1_out fifo */ hls::stream<float> fifo_C_drain_C_drain_IO_L1_out_2_1;
  #pragma HLS STREAM variable=fifo_C_drain_C_drain_IO_L1_out_2_1 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_C_drain_IO_L1_out_2_1 core=FIFO_SRL
  /* C_drain_IO_L1_out fifo */ hls::stream<float> fifo_C_drain_C_drain_IO_L1_out_2_2;
  #pragma HLS STREAM variable=fifo_C_drain_C_drain_IO_L1_out_2_2 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_C_drain_IO_L1_out_2_2 core=FIFO_SRL
  /* C_drain_IO_L1_out fifo */ hls::stream<float> fifo_C_drain_C_drain_IO_L1_out_2_3;
  #pragma HLS STREAM variable=fifo_C_drain_C_drain_IO_L1_out_2_3 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_C_drain_IO_L1_out_2_3 core=FIFO_SRL
  /* C_drain_IO_L1_out fifo */ hls::stream<float> fifo_C_drain_C_drain_IO_L1_out_2_4;
  #pragma HLS STREAM variable=fifo_C_drain_C_drain_IO_L1_out_2_4 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_C_drain_IO_L1_out_2_4 core=FIFO_SRL
  /* C_drain_IO_L1_out fifo */ hls::stream<float> fifo_C_drain_C_drain_IO_L1_out_3_0;
  #pragma HLS STREAM variable=fifo_C_drain_C_drain_IO_L1_out_3_0 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_C_drain_IO_L1_out_3_0 core=FIFO_SRL
  /* C_drain_IO_L1_out fifo */ hls::stream<float> fifo_C_drain_C_drain_IO_L1_out_3_1;
  #pragma HLS STREAM variable=fifo_C_drain_C_drain_IO_L1_out_3_1 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_C_drain_IO_L1_out_3_1 core=FIFO_SRL
  /* C_drain_IO_L1_out fifo */ hls::stream<float> fifo_C_drain_C_drain_IO_L1_out_3_2;
  #pragma HLS STREAM variable=fifo_C_drain_C_drain_IO_L1_out_3_2 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_C_drain_IO_L1_out_3_2 core=FIFO_SRL
  /* C_drain_IO_L1_out fifo */ hls::stream<float> fifo_C_drain_C_drain_IO_L1_out_3_3;
  #pragma HLS STREAM variable=fifo_C_drain_C_drain_IO_L1_out_3_3 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_C_drain_IO_L1_out_3_3 core=FIFO_SRL
  /* C_drain_IO_L1_out fifo */ hls::stream<float> fifo_C_drain_C_drain_IO_L1_out_3_4;
  #pragma HLS STREAM variable=fifo_C_drain_C_drain_IO_L1_out_3_4 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_C_drain_IO_L1_out_3_4 core=FIFO_SRL
  /* C_drain_IO_L2_out fifo */ hls::stream<float> fifo_C_drain_C_drain_IO_L2_out_0;
  #pragma HLS STREAM variable=fifo_C_drain_C_drain_IO_L2_out_0 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_C_drain_IO_L2_out_0 core=FIFO_SRL
  /* C_drain_IO_L2_out fifo */ hls::stream<float> fifo_C_drain_C_drain_IO_L2_out_1;
  #pragma HLS STREAM variable=fifo_C_drain_C_drain_IO_L2_out_1 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_C_drain_IO_L2_out_1 core=FIFO_SRL
  /* C_drain_IO_L2_out fifo */ hls::stream<float> fifo_C_drain_C_drain_IO_L2_out_2;
  #pragma HLS STREAM variable=fifo_C_drain_C_drain_IO_L2_out_2 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_C_drain_IO_L2_out_2 core=FIFO_SRL
  /* C_drain_IO_L2_out fifo */ hls::stream<float> fifo_C_drain_C_drain_IO_L2_out_3;
  #pragma HLS STREAM variable=fifo_C_drain_C_drain_IO_L2_out_3 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_C_drain_IO_L2_out_3 core=FIFO_SRL
  /* C_drain_IO_L2_out fifo */ hls::stream<float> fifo_C_drain_C_drain_IO_L2_out_4;
  #pragma HLS STREAM variable=fifo_C_drain_C_drain_IO_L2_out_4 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_C_drain_IO_L2_out_4 core=FIFO_SRL
  /* FIFO Declaration */

  /* Module Call */
  A_IO_L3_in_serialize(
    /* array */ A,
    /* fifo */ fifo_A_A_IO_L3_in_serialize
  );
  /* Module Call */

  /* Module Call */
  A_IO_L3_in(
    /* fifo */ fifo_A_A_IO_L3_in_serialize,
    /* fifo */ fifo_A_A_IO_L2_in_0
  );
  /* Module Call */

  /* Module Call */
  A_IO_L2_in(
    /* module id */ 0,
    /* fifo */ fifo_A_A_IO_L2_in_0,
    /* fifo */ fifo_A_A_IO_L2_in_1,
    /* fifo */ fifo_A_PE_0_0
  );
  /* Module Call */

  /* Module Call */
  A_IO_L2_in(
    /* module id */ 1,
    /* fifo */ fifo_A_A_IO_L2_in_1,
    /* fifo */ fifo_A_A_IO_L2_in_2,
    /* fifo */ fifo_A_PE_1_0
  );
  /* Module Call */

  /* Module Call */
  A_IO_L2_in(
    /* module id */ 2,
    /* fifo */ fifo_A_A_IO_L2_in_2,
    /* fifo */ fifo_A_A_IO_L2_in_3,
    /* fifo */ fifo_A_PE_2_0
  );
  /* Module Call */

  /* Module Call */
  A_IO_L2_in_boundary(
    /* module id */ 3,
    /* fifo */ fifo_A_A_IO_L2_in_3,
    /* fifo */ fifo_A_PE_3_0
  );
  /* Module Call */

  /* Module Call */
  B_IO_L3_in_serialize(
    /* array */ B,
    /* fifo */ fifo_B_B_IO_L3_in_serialize
  );
  /* Module Call */

  /* Module Call */
  B_IO_L3_in(
    /* fifo */ fifo_B_B_IO_L3_in_serialize,
    /* fifo */ fifo_B_B_IO_L2_in_0
  );
  /* Module Call */

  /* Module Call */
  B_IO_L2_in(
    /* module id */ 0,
    /* fifo */ fifo_B_B_IO_L2_in_0,
    /* fifo */ fifo_B_B_IO_L2_in_1,
    /* fifo */ fifo_B_PE_0_0
  );
  /* Module Call */

  /* Module Call */
  B_IO_L2_in(
    /* module id */ 1,
    /* fifo */ fifo_B_B_IO_L2_in_1,
    /* fifo */ fifo_B_B_IO_L2_in_2,
    /* fifo */ fifo_B_PE_0_1
  );
  /* Module Call */

  /* Module Call */
  B_IO_L2_in(
    /* module id */ 2,
    /* fifo */ fifo_B_B_IO_L2_in_2,
    /* fifo */ fifo_B_B_IO_L2_in_3,
    /* fifo */ fifo_B_PE_0_2
  );
  /* Module Call */

  /* Module Call */
  B_IO_L2_in_boundary(
    /* module id */ 3,
    /* fifo */ fifo_B_B_IO_L2_in_3,
    /* fifo */ fifo_B_PE_0_3
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 0,
    /* module id */ 0,
    /* fifo */ fifo_A_PE_0_0,
    /* fifo */ fifo_A_PE_0_1,
    /* fifo */ fifo_B_PE_0_0,
    /* fifo */ fifo_B_PE_1_0,
    /* fifo */ fifo_C_drain_PE_0_0
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 0,
    /* module id */ 1,
    /* fifo */ fifo_A_PE_0_1,
    /* fifo */ fifo_A_PE_0_2,
    /* fifo */ fifo_B_PE_0_1,
    /* fifo */ fifo_B_PE_1_1,
    /* fifo */ fifo_C_drain_PE_0_1
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 0,
    /* module id */ 2,
    /* fifo */ fifo_A_PE_0_2,
    /* fifo */ fifo_A_PE_0_3,
    /* fifo */ fifo_B_PE_0_2,
    /* fifo */ fifo_B_PE_1_2,
    /* fifo */ fifo_C_drain_PE_0_2
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 0,
    /* module id */ 3,
    /* fifo */ fifo_A_PE_0_3,
    /* fifo */ fifo_A_PE_0_4,
    /* fifo */ fifo_B_PE_0_3,
    /* fifo */ fifo_B_PE_1_3,
    /* fifo */ fifo_C_drain_PE_0_3
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 1,
    /* module id */ 0,
    /* fifo */ fifo_A_PE_1_0,
    /* fifo */ fifo_A_PE_1_1,
    /* fifo */ fifo_B_PE_1_0,
    /* fifo */ fifo_B_PE_2_0,
    /* fifo */ fifo_C_drain_PE_1_0
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 1,
    /* module id */ 1,
    /* fifo */ fifo_A_PE_1_1,
    /* fifo */ fifo_A_PE_1_2,
    /* fifo */ fifo_B_PE_1_1,
    /* fifo */ fifo_B_PE_2_1,
    /* fifo */ fifo_C_drain_PE_1_1
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 1,
    /* module id */ 2,
    /* fifo */ fifo_A_PE_1_2,
    /* fifo */ fifo_A_PE_1_3,
    /* fifo */ fifo_B_PE_1_2,
    /* fifo */ fifo_B_PE_2_2,
    /* fifo */ fifo_C_drain_PE_1_2
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 1,
    /* module id */ 3,
    /* fifo */ fifo_A_PE_1_3,
    /* fifo */ fifo_A_PE_1_4,
    /* fifo */ fifo_B_PE_1_3,
    /* fifo */ fifo_B_PE_2_3,
    /* fifo */ fifo_C_drain_PE_1_3
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 2,
    /* module id */ 0,
    /* fifo */ fifo_A_PE_2_0,
    /* fifo */ fifo_A_PE_2_1,
    /* fifo */ fifo_B_PE_2_0,
    /* fifo */ fifo_B_PE_3_0,
    /* fifo */ fifo_C_drain_PE_2_0
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 2,
    /* module id */ 1,
    /* fifo */ fifo_A_PE_2_1,
    /* fifo */ fifo_A_PE_2_2,
    /* fifo */ fifo_B_PE_2_1,
    /* fifo */ fifo_B_PE_3_1,
    /* fifo */ fifo_C_drain_PE_2_1
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 2,
    /* module id */ 2,
    /* fifo */ fifo_A_PE_2_2,
    /* fifo */ fifo_A_PE_2_3,
    /* fifo */ fifo_B_PE_2_2,
    /* fifo */ fifo_B_PE_3_2,
    /* fifo */ fifo_C_drain_PE_2_2
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 2,
    /* module id */ 3,
    /* fifo */ fifo_A_PE_2_3,
    /* fifo */ fifo_A_PE_2_4,
    /* fifo */ fifo_B_PE_2_3,
    /* fifo */ fifo_B_PE_3_3,
    /* fifo */ fifo_C_drain_PE_2_3
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 3,
    /* module id */ 0,
    /* fifo */ fifo_A_PE_3_0,
    /* fifo */ fifo_A_PE_3_1,
    /* fifo */ fifo_B_PE_3_0,
    /* fifo */ fifo_B_PE_4_0,
    /* fifo */ fifo_C_drain_PE_3_0
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 3,
    /* module id */ 1,
    /* fifo */ fifo_A_PE_3_1,
    /* fifo */ fifo_A_PE_3_2,
    /* fifo */ fifo_B_PE_3_1,
    /* fifo */ fifo_B_PE_4_1,
    /* fifo */ fifo_C_drain_PE_3_1
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 3,
    /* module id */ 2,
    /* fifo */ fifo_A_PE_3_2,
    /* fifo */ fifo_A_PE_3_3,
    /* fifo */ fifo_B_PE_3_2,
    /* fifo */ fifo_B_PE_4_2,
    /* fifo */ fifo_C_drain_PE_3_2
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 3,
    /* module id */ 3,
    /* fifo */ fifo_A_PE_3_3,
    /* fifo */ fifo_A_PE_3_4,
    /* fifo */ fifo_B_PE_3_3,
    /* fifo */ fifo_B_PE_4_3,
    /* fifo */ fifo_C_drain_PE_3_3
  );
  /* Module Call */

  /* Module Call */
  A_PE_dummy_in(
    /* module id */ 0,
    /* module id */ 3,
    /* fifo */ fifo_A_PE_0_4
  );
  /* Module Call */

  /* Module Call */
  A_PE_dummy_in(
    /* module id */ 1,
    /* module id */ 3,
    /* fifo */ fifo_A_PE_1_4
  );
  /* Module Call */

  /* Module Call */
  A_PE_dummy_in(
    /* module id */ 2,
    /* module id */ 3,
    /* fifo */ fifo_A_PE_2_4
  );
  /* Module Call */

  /* Module Call */
  A_PE_dummy_in(
    /* module id */ 3,
    /* module id */ 3,
    /* fifo */ fifo_A_PE_3_4
  );
  /* Module Call */

  /* Module Call */
  B_PE_dummy_in(
    /* module id */ 3,
    /* module id */ 0,
    /* fifo */ fifo_B_PE_4_0
  );
  /* Module Call */

  /* Module Call */
  B_PE_dummy_in(
    /* module id */ 3,
    /* module id */ 1,
    /* fifo */ fifo_B_PE_4_1
  );
  /* Module Call */

  /* Module Call */
  B_PE_dummy_in(
    /* module id */ 3,
    /* module id */ 2,
    /* fifo */ fifo_B_PE_4_2
  );
  /* Module Call */

  /* Module Call */
  B_PE_dummy_in(
    /* module id */ 3,
    /* module id */ 3,
    /* fifo */ fifo_B_PE_4_3
  );
  /* Module Call */

  /* Module Call */
  C_drain_IO_L1_out_boundary_wrapper(
    /* module id */ 0,
    /* module id */ 3,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_0_3,
    /* fifo */ fifo_C_drain_PE_3_0
  );
  /* Module Call */

  /* Module Call */
  C_drain_IO_L1_out_wrapper(
    /* module id */ 0,
    /* module id */ 2,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_0_3,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_0_2,
    /* fifo */ fifo_C_drain_PE_2_0
  );
  /* Module Call */

  /* Module Call */
  C_drain_IO_L1_out_wrapper(
    /* module id */ 0,
    /* module id */ 1,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_0_2,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_0_1,
    /* fifo */ fifo_C_drain_PE_1_0
  );
  /* Module Call */

  /* Module Call */
  C_drain_IO_L1_out_wrapper(
    /* module id */ 0,
    /* module id */ 0,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_0_1,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_0_0,
    /* fifo */ fifo_C_drain_PE_0_0
  );
  /* Module Call */

  /* Module Call */
  C_drain_IO_L1_out_boundary_wrapper(
    /* module id */ 1,
    /* module id */ 3,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_1_3,
    /* fifo */ fifo_C_drain_PE_3_1
  );
  /* Module Call */

  /* Module Call */
  C_drain_IO_L1_out_wrapper(
    /* module id */ 1,
    /* module id */ 2,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_1_3,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_1_2,
    /* fifo */ fifo_C_drain_PE_2_1
  );
  /* Module Call */

  /* Module Call */
  C_drain_IO_L1_out_wrapper(
    /* module id */ 1,
    /* module id */ 1,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_1_2,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_1_1,
    /* fifo */ fifo_C_drain_PE_1_1
  );
  /* Module Call */

  /* Module Call */
  C_drain_IO_L1_out_wrapper(
    /* module id */ 1,
    /* module id */ 0,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_1_1,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_1_0,
    /* fifo */ fifo_C_drain_PE_0_1
  );
  /* Module Call */

  /* Module Call */
  C_drain_IO_L1_out_boundary_wrapper(
    /* module id */ 2,
    /* module id */ 3,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_2_3,
    /* fifo */ fifo_C_drain_PE_3_2
  );
  /* Module Call */

  /* Module Call */
  C_drain_IO_L1_out_wrapper(
    /* module id */ 2,
    /* module id */ 2,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_2_3,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_2_2,
    /* fifo */ fifo_C_drain_PE_2_2
  );
  /* Module Call */

  /* Module Call */
  C_drain_IO_L1_out_wrapper(
    /* module id */ 2,
    /* module id */ 1,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_2_2,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_2_1,
    /* fifo */ fifo_C_drain_PE_1_2
  );
  /* Module Call */

  /* Module Call */
  C_drain_IO_L1_out_wrapper(
    /* module id */ 2,
    /* module id */ 0,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_2_1,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_2_0,
    /* fifo */ fifo_C_drain_PE_0_2
  );
  /* Module Call */

  /* Module Call */
  C_drain_IO_L1_out_boundary_wrapper(
    /* module id */ 3,
    /* module id */ 3,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_3_3,
    /* fifo */ fifo_C_drain_PE_3_3
  );
  /* Module Call */

  /* Module Call */
  C_drain_IO_L1_out_wrapper(
    /* module id */ 3,
    /* module id */ 2,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_3_3,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_3_2,
    /* fifo */ fifo_C_drain_PE_2_3
  );
  /* Module Call */

  /* Module Call */
  C_drain_IO_L1_out_wrapper(
    /* module id */ 3,
    /* module id */ 1,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_3_2,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_3_1,
    /* fifo */ fifo_C_drain_PE_1_3
  );
  /* Module Call */

  /* Module Call */
  C_drain_IO_L1_out_wrapper(
    /* module id */ 3,
    /* module id */ 0,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_3_1,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_3_0,
    /* fifo */ fifo_C_drain_PE_0_3
  );
  /* Module Call */

  /* Module Call */
  C_drain_IO_L2_out_boundary(
    /* module id */ 3,
    /* fifo */ fifo_C_drain_C_drain_IO_L2_out_3,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_3_0
  );
  /* Module Call */

  /* Module Call */
  C_drain_IO_L2_out(
    /* module id */ 2,
    /* fifo */ fifo_C_drain_C_drain_IO_L2_out_3,
    /* fifo */ fifo_C_drain_C_drain_IO_L2_out_2,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_2_0
  );
  /* Module Call */

  /* Module Call */
  C_drain_IO_L2_out(
    /* module id */ 1,
    /* fifo */ fifo_C_drain_C_drain_IO_L2_out_2,
    /* fifo */ fifo_C_drain_C_drain_IO_L2_out_1,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_1_0
  );
  /* Module Call */

  /* Module Call */
  C_drain_IO_L2_out(
    /* module id */ 0,
    /* fifo */ fifo_C_drain_C_drain_IO_L2_out_1,
    /* fifo */ fifo_C_drain_C_drain_IO_L2_out_0,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_0_0
  );
  /* Module Call */

  /* Module Call */
  C_drain_IO_L3_out(
    /* fifo */ fifo_C_drain_C_drain_IO_L3_out_serialize,
    /* fifo */ fifo_C_drain_C_drain_IO_L2_out_0
  );
  /* Module Call */

  /* Module Call */
  C_drain_IO_L3_out_serialize(
    /* array */ C,
    /* fifo */ fifo_C_drain_C_drain_IO_L3_out_serialize
  );
  /* Module Call */

}
