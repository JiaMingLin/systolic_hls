#include "top.h"

int main(){
    
    axiDataStream_t input_stream, output_stream;

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
        data_t multiplier = input_data[i][0];
        for(int j = 0; j < NUM_WORD; j++){
            sw_output[i][j] = input_data[i][j] * multiplier;
        }
    }

    for(int i = 0; i < length; i++){
        for(int j = 0; j < NUM_WORD; j++){
            input_data[i][j] = input_data[i][j] * input_data[i][j];
        }
    }

    

    // hardware compute
    bool last = 0;
    for(int i = 0; i < length; i++){
        axiDataBus bus_in;
        for(int j = 0; j < NUM_WORD; j++){
            bus_in.data.range((j+1)*DATA_WIDTH-1, j*DATA_WIDTH) = input_data[i][j];
        }
        bus_in.last = (i == length-1)? 1: 0;

        input_stream.write(bus_in);

        multiplicationTop(input_stream, output_stream);

        axiDataBus bus_out = output_stream.read();
        for(int j = 0; j < NUM_WORD; j++){
            hw_output[i][j] = bus_out.data.range((j+1)*DATA_WIDTH-1, j*DATA_WIDTH);
        }
        
        if(i == (length - 1)) last = bus_out.last;
    }
    
    // validate
    int err = 0;
    for(int i = 0; i < length; i++){
        for(int j = 0; j < NUM_WORD; j++){
            if(hw_output[i][j] != sw_output[i][j]) err++;
        }
    }

    if(!last) err++;
    
    return err;
}