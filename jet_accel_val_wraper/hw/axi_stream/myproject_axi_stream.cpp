#include "myproject_axi_stream.h"

/*
    This function is a wrapper for the myproject function.
    It reads the input stream, processes the data, and writes the output stream.
    Bus width of input is IN_DATA_WIDTH * N_IN, and output is OUT_DATA_WIDTH * N_OUT.
    The input and output data type is T_in and T_out respectively.
    e.g. IN_DATA_WIDTH = 16, N_IN = 16, OUT_DATA_WIDTH = 32, N_OUT = 5.
    The input bus width is 16 * 16 = 256, and the output bus width is 32 * 5 = 160. 
*/
void myproject_axi_stream(axis_in_stream_t &in, axis_out_stream_t &out) {

    #pragma HLS INTERFACE axis port=in
    #pragma HLS INTERFACE axis port=out
    #pragma HLS INTERFACE ap_ctrl_none port=return

    #pragma HLS DATAFLOW disable_start_propagation
    bool is_last = false;
    T_in in_local[N_IN];
    T_out out_local[N_OUT];

    if (!in.empty()) {
        axis_in_data_bus in_bus = in.read();
        
        // decode input
        for (unsigned i = 0; i < N_IN; i++) {
            #pragma HLS UNROLL
            ap_uint<IN_DATA_WIDTH> temp = in_bus.data.range(IN_DATA_WIDTH*(i+1)-1, IN_DATA_WIDTH*i);
            in_extended_t in_extended = in_extended_t(temp);  // extend to fixed point with wider width
            in_local[i] = T_in(in_extended >> IN_FRAC_BITS);
            // in_local[i].range(IN_DATA_WIDTH-1, 0) = in_bus.data.range(IN_DATA_WIDTH*(i+1)-1, IN_DATA_WIDTH*i);
            // std::cout << "in_local " << in_local[i].to_string(10).c_str() << std::endl;
        }

// #ifdef __SYNTHESIS__
        myproject(in_local, out_local);
// #else
        // for (unsigned k = 0; k < N_OUT; k++) {
        //     #pragma HLS UNROLL
        //     out_local[k] = T_out(in_local[k]);
        //     std::cout << "out_local " << out_local[k].to_string(10).c_str() << std::endl;
        // }
// #endif
        axis_out_data_bus out_bus;
        // encode output
        for (unsigned j = 0; j < N_OUT; j++) {
            #pragma HLS UNROLL
            out_extended_t out_extended = out_extended_t(out_local[j]);
            ap_uint<OUT_DATA_WIDTH> out_encoded = ap_uint<OUT_DATA_WIDTH>(out_extended << OUT_FRAC_BITS);
            out_bus.data.range(OUT_DATA_WIDTH*(j+1)-1, OUT_DATA_WIDTH*j) = out_encoded;
        }
        out_bus.last = in_bus.last;
        out.write(out_bus);
    }
}