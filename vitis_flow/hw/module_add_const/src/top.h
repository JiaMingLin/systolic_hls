#include<ap_int.h>
#include<hls_stream.h>

#define BUS_WIDTH 512
#define DATA_WIDTH 32
#define NUM_WORD BUS_WIDTH/DATA_WIDTH

typedef ap_uint<DATA_WIDTH> data_t;
typedef ap_uint<BUS_WIDTH> axiWord_t;
typedef hls::stream<axiWord_t> axiWordStream_t;

extern "C" void addConstantTop(axiWordStream_t &input, axiWordStream_t &output);
void addOne(axiWordStream_t &input, axiWordStream_t &output);
void addTwo(axiWordStream_t &input, axiWordStream_t &output);