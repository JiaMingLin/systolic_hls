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
        
        for (unsigned i = 0; i < N_IN; i++) {
            #pragma HLS UNROLL
            ap_uint<IN_DATA_WIDTH> temp = in_bus.data.range(IN_DATA_WIDTH*(i+1)-1, IN_DATA_WIDTH*i);
            in_local[i] = (T_in)temp >> IN_FRAC_BITS;
        }

        myproject(in_local, out_local);

        axis_out_data_bus out_bus;
        for (unsigned j = 0; j < N_OUT; j++) {
            #pragma HLS UNROLL
            ap_uint<OUT_DATA_WIDTH> temp = (ap_uint<OUT_DATA_WIDTH>)(out_local[j] << OUT_FRAC_BITS);
            out_bus.data.range(OUT_DATA_WIDTH*(j+1)-1, OUT_DATA_WIDTH*j) = temp;
        }
        out_bus.last = in_bus.last;
        out.write(out_bus);
    }
}