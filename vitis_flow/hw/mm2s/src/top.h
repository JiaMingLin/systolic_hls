#include<ap_int.h>
#include<hls_stream.h>

#define BUS_WIDTH 512
#define DATA_WIDTH 32

typedef ap_uint<BUS_WIDTH> axiWord_t;
typedef hls::stream<axiWord_t> axiWordStream_t;

void mm2sTop(axiWord_t* input, axiWordStream_t &output, int length);
