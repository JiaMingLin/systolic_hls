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
    axiWordStream_t output_stream;

    // hw call
    mm2sTop(input, output_stream, length);

    // validate
    int err = 0;
    for(int i = 0; i < length; i++){
        axiWord_t output_data = output_stream.read();
        if(output_data != input[i]) err++;
    }

    return err;
}