#include<ap_int.h>
#include<hls_stream.h>

#define BUS_WIDTH 512
#define DATA_WIDTH 32
#define OUT_WORD_LEN 5
#define OUT_WORD_WIDTH DATA_WIDTH*OUT_WORD_LEN

typedef ap_uint<BUS_WIDTH> m_axi_word_t;
typedef ap_uint<OUT_WORD_WIDTH> axis_word_t;

typedef struct axis_data_bus
{
    axis_word_t data;
    bool last;
} axis_data_bus;

typedef hls::stream<axis_data_bus> axis_data_stream_t;

void s2mmTop(axis_data_stream_t &input, m_axi_word_t* output);
