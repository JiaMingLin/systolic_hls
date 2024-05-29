#include "top.h"

int main(){
    
    axiWordStream_t input_stream, output_stream;

    // generate random input in array
    int length = 64;
    data_t input_data[length][NUM_WORD];
    data_t sw_output[length][NUM_WORD];
    data_t hw_output[length][NUM_WORD];
    for(int i = 0; i < length; i++){
        for(int j = 0; j < NUM_WORD; j++){
            input_data[i][j] = rand();
        }
    }

    // software compute
    for(int i = 0; i < length; i++){
        for(int j = 0; j < NUM_WORD; j++){
            input_data[i][j] = input_data[i][j] + 10;
        }
    }

    for(int i = 0; i < length; i++){
        for(int j = 0; j < NUM_WORD; j++){
            if(j % 2 == 0) sw_output[i][j] = input_data[i][j] + 10;
            if(j % 2 == 1) sw_output[i][j] = input_data[i][j] + 50;
        }
    }

    // hardware compute
    for(int i = 0; i < length; i++){
        axiWord_t input_line, output_line;
        for(int j = 0; j < NUM_WORD; j++){
            input_line.range((j+1)*DATA_WIDTH-1, j*DATA_WIDTH) = input_data[i][j];
        }

        input_stream.write(input_line);
        addConstantTop(input_stream, output_stream);
        output_line = output_stream.read();
        for(int j = 0; j < NUM_WORD; j++){
            hw_output[i][j] = output_line.range((j+1)*DATA_WIDTH-1, j*DATA_WIDTH);
        }
    }
    

    // validate
    int err = 0;
    for(int i = 0; i < length; i++){
        for(int j = 0; j < NUM_WORD; j++){
            if(hw_output[i][j] != sw_output[i][j]) err++;
        }
    }
    
    return err;
}