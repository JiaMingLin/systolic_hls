#include<ap_int.h>
#include<hls_stream.h>

#define BUS_WIDTH 512
#define DATA_WIDTH 32
#define NUM_WORD BUS_WIDTH/DATA_WIDTH

typedef ap_uint<DATA_WIDTH> data_t;
typedef ap_uint<BUS_WIDTH> axiWord_t;
typedef hls::stream<axiWord_t> axiWordStream_t;

extern "C" void multiplicationTop(axiWordStream_t &input, axiWordStream_t &output);
void mulOne(axiWordStream_t &input, axiWordStream_t &output);
void mulTwo(axiWordStream_t &input, axiWordStream_t &output);