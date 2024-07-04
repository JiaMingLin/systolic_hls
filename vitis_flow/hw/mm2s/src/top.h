#include<ap_int.h>
#include<hls_stream.h>

#define BUS_WIDTH 512
#define DATA_WIDTH 32

typedef ap_uint<BUS_WIDTH> axiWord_t;

typedef struct axiDataBus
{
    axiWord_t data;
    bool last;
} axiDataBus;

typedef hls::stream<axiDataBus> axiDataStream_t;

void mm2sTop(axiWord_t* input, axiDataStream_t &output, int length, int &status);
