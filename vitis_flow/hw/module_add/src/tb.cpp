#include "top.h"

int main(){
    
    axiBlockStream_t input_stream, output_stream;

    // generate random input in array
    int length = 64;
    data_t data_cache[HBM_CACHE_SIZE][BLOCK_SIZE];
    data_t input_data[length][BLOCK_SIZE];
    data_t sw_output[length][BLOCK_SIZE];
    data_t hw_output[length][BLOCK_SIZE];
    for(int i = 0; i < length; i++){
        for(int j = 0; j < BLOCK_SIZE; j++){
            input_data[i][j] = rand();
        }
    }
    // generate hbm cached data
    for(int i = 0; i < HBM_CACHE_SIZE; i++){
        for(int j = 0; j < BLOCK_SIZE; j++){
            data_cache[i][j] = rand();
        }
    }

    // software compute
    for(int i = 0; i < length; i++){
        for(int j = 0; j < BLOCK_SIZE; j++){
            // printf("input: %d, HBM: %d ", (int)input_data[i][j], (int)data_cache[i%HBM_CACHE_SIZE][j]);
            input_data[i][j] = input_data[i][j] + data_cache[i%HBM_CACHE_SIZE][j];
        }
    }
    // printf("\n");

    for(int i = 0; i < length; i++){
        for(int j = 0; j < BLOCK_SIZE; j++){
            if(j % 2 == 0) sw_output[i][j] = input_data[i][j] + 10;
            if(j % 2 == 1) sw_output[i][j] = input_data[i][j] + 50;
        }
    }

    // printf("=====================================================================================");
    // hardware compute
    axiBlock_t hbm_cache[HBM_CACHE_SIZE];
    for(int i = 0; i < HBM_CACHE_SIZE; i++){
        axiBlock_t cache_block;
        for(int j = 0; j < BLOCK_SIZE; j++){
            cache_block.range((j+1)*DATA_WIDTH-1, j*DATA_WIDTH) = data_cache[HBM_CACHE_SIZE][j];
        }
    }
    for(int i = 0; i < length; i++){
        axiBlock_t input_block, output_block;
        for(int j = 0; j < BLOCK_SIZE; j++){
            input_block.range((j+1)*DATA_WIDTH-1, j*DATA_WIDTH) = input_data[i][j];
        }

        input_stream.write(input_block);
        addTop(input_stream, output_stream, hbm_cache);
        output_block = output_stream.read();
        for(int j = 0; j < BLOCK_SIZE; j++){
            hw_output[i][j] = output_block.range((j+1)*DATA_WIDTH-1, j*DATA_WIDTH);
        }
    }
    

    // validate
    int err = 0;
    for(int i = 0; i < length; i++){
        for(int j = 0; j < BLOCK_SIZE; j++){
            if(hw_output[i][j] != sw_output[i][j]) err++;
        }
    }
    
    return err;
}