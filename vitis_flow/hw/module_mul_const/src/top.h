#include<ap_int.h>
#include<hls_stream.h>

#define BUS_WIDTH 512
#define DATA_WIDTH 32
#define NUM_WORD BUS_WIDTH/DATA_WIDTH

typedef ap_uint<DATA_WIDTH> data_t;
typedef ap_uint<BUS_WIDTH> axiWord_t;

typedef struct axiDataBus
{
    axiWord_t data;
    bool last;
} axiDataBus;

typedef hls::stream<axiDataBus> axiDataStream_t;

extern "C" void multiplicationTop(axiDataStream_t &input, axiDataStream_t &output);
void mulOne(axiDataStream_t &input, axiDataStream_t &output);
void mulTwo(axiDataStream_t &input, axiDataStream_t &output);