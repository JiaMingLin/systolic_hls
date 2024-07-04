#include<ap_int.h>
#include<hls_stream.h>

#define BUS_WIDTH 512
#define DATA_WIDTH 32
#define BLOCK_SIZE BUS_WIDTH/DATA_WIDTH
#define HBM_CACHE_SIZE 32

typedef ap_uint<DATA_WIDTH> data_t;
typedef ap_uint<BUS_WIDTH> axiBlock_t;
typedef hls::stream<axiBlock_t> axiBlockStream_t;

extern "C" void addTop(axiBlockStream_t &input, axiBlockStream_t &output, axiBlock_t* hbmCache);
void hbmRetriever(int hbmPtr, axiBlock_t *hbmCache, axiBlock_t &retrievedData);
void addHBM(axiBlock_t retrievedData, axiBlockStream_t &input, axiBlockStream_t &output);
void addConst(axiBlockStream_t &input, axiBlockStream_t &output);