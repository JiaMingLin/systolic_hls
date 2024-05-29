#include"top.h"

extern "C" void addConstantTop(axiWordStream_t &input, axiWordStream_t &output){

#pragma HLS INTERFACE axis port=input
#pragma HLS INTERFACE axis port=output
#pragma HLS INTERFACE ap_ctrl_none port=return

    static axiWordStream_t intermediateStream("intermediateStream");
#pragma HLS STREAM variable=intermediateStream

#pragma HLS DATAFLOW disable_start_propagation

    addOne(input, intermediateStream);
    addTwo(intermediateStream, output);

}

void addOne(axiWordStream_t &input, axiWordStream_t &output){
#pragma HLS PIPELINE II=1 style=flp

    static axiWord_t readWord;
    const ap_uint<DATA_WIDTH> const1 = 10;
    
    if (!input.empty()){
        // read from stream
        readWord = input.read();
    
        // calculate
        for(int i = 0 ; i < NUM_WORD; i++){
            readWord.range((i+1)*DATA_WIDTH-1, i*DATA_WIDTH) = readWord.range((i+1)*DATA_WIDTH-1, i*DATA_WIDTH) + const1;
        }

        // write to stream
        output.write(readWord);
    }
    return;
}

void addTwo(axiWordStream_t &input, axiWordStream_t &output){
#pragma HLS PIPELINE II=1 style=flp

    static axiWord_t readWord;
    const ap_uint<DATA_WIDTH> const1 = 10;
    const ap_uint<DATA_WIDTH> const2 = 50;

    if (!input.empty()){
        // read from stream
        readWord = input.read();

        // calculate
        LOOP_ADD_CONST_1: for(int i = 0 ; i < NUM_WORD; i+=2){
            readWord.range((i+1)*DATA_WIDTH-1, i*DATA_WIDTH) = readWord.range((i+1)*DATA_WIDTH-1, i*DATA_WIDTH) + const1;
        }
        LOOP_ADD_CONST_2: for(int i = 1 ; i < NUM_WORD; i+=2){
            readWord.range((i+1)*DATA_WIDTH-1, i*DATA_WIDTH) = readWord.range((i+1)*DATA_WIDTH-1, i*DATA_WIDTH) + const2;
        }

        // write to stream
        output.write(readWord);
    }
    return;
}