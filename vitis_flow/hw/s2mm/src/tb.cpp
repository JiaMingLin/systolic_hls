#include "top.h"

int main(){
    // initial data
    const int DATA_SIZE = 64;
    axiDataStream_t data_stream;
    axiWord_t input[DATA_SIZE];
    axiWord_t output[DATA_SIZE];
    for(int i = 0; i < DATA_SIZE; i++){
        axiWord_t data = 0;
        axiDataBus bus;
        for(int j = 0; j < BUS_WIDTH/DATA_WIDTH; j++){
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
        if(input[i] != output[i]) err++;
    }

    return err;
}
