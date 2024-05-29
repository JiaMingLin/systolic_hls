#include"top.h"

extern "C" void multiplicationTop(axiWordStream_t &input, axiWordStream_t &output){

#pragma HLS INTERFACE axis port=input
#pragma HLS INTERFACE axis port=output
#pragma HLS INTERFACE ap_ctrl_none port=return

    static axiWordStream_t intermediateStream("intermediateStream");
#pragma HLS STREAM variable=intermediateStream

#pragma HLS DATAFLOW disable_start_propagation

    mulOne(input, intermediateStream);
    mulTwo(intermediateStream, output);

}

void mulOne(axiWordStream_t &input, axiWordStream_t &output){
#pragma HLS PIPELINE II=1 style=flp

    static axiWord_t readWord;
    const ap_uint<DATA_WIDTH> const1 = 10;
    
    if (!input.empty()){
        // read from stream
        readWord = input.read();
        ap_uint<DATA_WIDTH> first_data = readWord.range(DATA_WIDTH-1,0);
        // calculate
        for(int i = 0 ; i < NUM_WORD; i++){
            readWord.range((i+1)*DATA_WIDTH-1, i*DATA_WIDTH) = readWord.range((i+1)*DATA_WIDTH-1, i*DATA_WIDTH) * first_data;
        }

        // write to stream
        output.write(readWord);
    }
    return;
}

void mulTwo(axiWordStream_t &input, axiWordStream_t &output){
#pragma HLS PIPELINE II=1 style=flp

    static axiWord_t readWord;
    const ap_uint<DATA_WIDTH> const1 = 10;
    const ap_uint<DATA_WIDTH> const2 = 50;

    if (!input.empty()){
        // read from stream
        readWord = input.read();

        // calculate
        for(int i = 0 ; i < NUM_WORD; i++){
            readWord.range((i+1)*DATA_WIDTH-1, i*DATA_WIDTH) = readWord.range((i+1)*DATA_WIDTH-1, i*DATA_WIDTH) * readWord.range((i+1)*DATA_WIDTH-1, i*DATA_WIDTH);
        }

        // write to stream
        output.write(readWord);
    }
    return;
}