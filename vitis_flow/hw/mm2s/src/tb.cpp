#include "top.h"

int main(){
    // generate init data
    int length = 64;
    axiWord_t input[length];
    for(int i = 0; i < length; i++){
        axiWord_t data;
        for(int j = 0; j < BUS_WIDTH/DATA_WIDTH; j++){
            data.range((j+1)*DATA_WIDTH-1, j*DATA_WIDTH) = rand();
        }
        input[i] = data;
    }

    // output is stream
    axiDataStream_t output_stream;

    // hw call
    int status = 1;
    mm2sTop(input, output_stream, length, status);
    printf("status %d \n", status);
    // validate
    int err = 0;
    for(int i = 0; i < length; i++){
        axiDataBus bus = output_stream.read();
        axiWord_t output_data = bus.data;
        bool last = bus.last;
        if(output_data != input[i]) err++;
        if((i == (length-1)) && !last) err++;
    }
    err+=status;

    return err;
}