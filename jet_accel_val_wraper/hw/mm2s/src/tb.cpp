#include "top.h"

int main(){
    // generate init data
    int length = 64;
    m_axi_word_t input[length];
    for(int i = 0; i < length; i++){
        m_axi_word_t data;
        for(int j = 0; j < IN_WORD_LEN; j++){
            data.range((j+1)*IN_DATA_WIDTH-1, j*IN_DATA_WIDTH) = rand();
        }
        input[i] = data;
    }

    // output is stream
    axis_data_stream_t output_stream;

    // hw call
    int status = 1;
    mm2sTop(input, output_stream, length, status);
    printf("status %d \n", status);
    // validate
    int err = 0;
    for(int i = 0; i < length; i++){
        axis_data_bus bus = output_stream.read();
        axis_word_t output_data = bus.data;
        bool last = bus.last;
        if(output_data != input[i].range(IN_WORD_WIDTH-1, 0)) err++;
        if((i == (length-1)) && !last) err++;
    }
    err+=status;

    return err;
}