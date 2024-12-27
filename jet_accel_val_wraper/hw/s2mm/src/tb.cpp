#include "top.h"

int main(){
    // initial data
    const int DATA_SIZE = 32;
    axis_data_stream_t data_stream;
    axis_word_t input[DATA_SIZE];
    m_axi_word_t output[DATA_SIZE];
    for(int i = 0; i < DATA_SIZE; i++){
        axis_word_t data = 0;
        axis_data_bus bus;
        for(int j = 0; j < OUT_WORD_LEN; j++){
            data.range((j+1) * DATA_WIDTH - 1, j * DATA_WIDTH) = rand();
        }
        input[i] = data;
        bus.data = data;
        bus.last = (i == DATA_SIZE-1)? 1:0;

        data_stream.write(bus);
    }

    // call hardware kernel
    s2mmTop(data_stream, output);

    // check
    int err = 0;
    for(int i = 0; i < DATA_SIZE; i++){
        if(input[i] != output[i].range(OUT_WORD_WIDTH-1, 0)) {
            err++;
            std::cout << "Error at index " << i << std::endl;
            std::cout << "Input:  " << input[i].to_string(2) << std::endl;
            std::cout << "Output: " << output[i].range(OUT_WORD_WIDTH-1, 0).to_string(2) << std::endl;
        }
    }

    return err;
}
