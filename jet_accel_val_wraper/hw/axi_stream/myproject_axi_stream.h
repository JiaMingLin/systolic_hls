#ifndef MYPROJECT_AXI_H_
#define MYPROJECT_AXI_H_

#include <iostream>
#include <ap_int.h>
#include<hls_stream.h>
// #ifdef __SYNTHESIS__
#include "myproject.h"
// #endif

static const unsigned N_IN = 16;
static const unsigned N_OUT = 5;
static const unsigned IN_FRAC_BITS = 10;
static const unsigned OUT_FRAC_BITS = 20;
typedef ap_fixed<16, 16 - IN_FRAC_BITS, AP_TRN, AP_WRAP, 0> T_in;
typedef ap_fixed<32, 32 - OUT_FRAC_BITS, AP_TRN, AP_WRAP, 0> T_out;

static const unsigned IN_DATA_WIDTH = T_in::width;
static const unsigned OUT_DATA_WIDTH = T_out::width;
// a data bus
#define IN_BUS_WIDTH N_IN*IN_DATA_WIDTH
// typedef ap_uint<IN_BUS_WIDTH> axiInBus;
typedef struct axis_in_data_bus{
    ap_uint<IN_BUS_WIDTH> data;
    bool last;
}axis_in_data_bus;

#define OUT_BUS_WIDTH N_OUT*OUT_DATA_WIDTH
// typedef ap_uint<OUT_BUS_WIDTH> axiOutBus;
typedef struct axis_out_data_bus{
    ap_uint<OUT_BUS_WIDTH> data;
    bool last;
}axis_out_data_bus;

// stream of words
typedef hls::stream<axis_in_data_bus> axis_in_stream_t;
typedef hls::stream<axis_out_data_bus> axis_out_stream_t;

typedef ap_fixed<IN_DATA_WIDTH+IN_FRAC_BITS,IN_DATA_WIDTH> in_extended_t;
typedef ap_fixed<OUT_DATA_WIDTH+OUT_FRAC_BITS,OUT_DATA_WIDTH> out_extended_t;

void myproject_axi_stream(axis_in_stream_t &in, axis_out_stream_t &out);
#endif