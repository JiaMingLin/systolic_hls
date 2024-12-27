#include<ap_int.h>
#include<hls_stream.h>

#define BUS_WIDTH 512
#define IN_DATA_WIDTH 16
#define IN_WORD_LEN 16
#define IN_WORD_WIDTH IN_DATA_WIDTH*IN_WORD_LEN

typedef ap_uint<BUS_WIDTH> m_axi_word_t;
typedef ap_uint<IN_WORD_WIDTH> axis_word_t;

typedef struct axis_data_bus
{
    axis_word_t data;
    bool last;
} axis_data_bus;

typedef hls::stream<axis_data_bus> axis_data_stream_t;

void mm2sTop(m_axi_word_t* input, axis_data_stream_t &output, int length, int &status);
