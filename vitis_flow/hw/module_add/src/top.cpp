#include"top.h"

extern "C" void addTop(
    axiBlockStream_t &input, axiBlockStream_t &output, 
    axiBlock_t *hbmCache){

#pragma HLS INTERFACE mode=axis port=input
#pragma HLS INTERFACE mode=axis port=output
#pragma HLS INTERFACE mode=m_axi port=hbmCache offset=slave depth=128
#pragma HLS INTERFACE ap_ctrl_none port=return

    static axiBlockStream_t intermediateStream("intermediateStream");
#pragma HLS STREAM variable=intermediateStream

#pragma HLS DATAFLOW disable_start_propagation
    int hbmPtr = 0; axiBlock_t retrievedData;
    hbmRetriever(hbmPtr, hbmCache, retrievedData);
    addHBM(retrievedData, input, intermediateStream);
    addConst(intermediateStream, output);

}

void hbmRetriever(int hbmPtr, axiBlock_t *hbmCache, axiBlock_t &retrievedData){
    retrievedData = hbmCache[(hbmPtr++)%HBM_CACHE_SIZE];
}

void addHBM(axiBlock_t retrievedData, axiBlockStream_t &input, axiBlockStream_t &output){
#pragma HLS PIPELINE II=1 style=flp

    static axiBlock_t readWord;
    
    if (!input.empty()){
        // read from stream
        readWord = input.read();
    
        // calculate
        for(int i = 0 ; i < BLOCK_SIZE; i++){
            // printf("input: %d, HBM: %d ", (int)readWord.range((i+1)*DATA_WIDTH-1, i*DATA_WIDTH), (int)retrievedData.range((i+1)*DATA_WIDTH-1, i*DATA_WIDTH));
                 readWord.range((i+1)*DATA_WIDTH-1, i*DATA_WIDTH) = 
                 readWord.range((i+1)*DATA_WIDTH-1, i*DATA_WIDTH) + 
            retrievedData.range((i+1)*DATA_WIDTH-1, i*DATA_WIDTH);
        }
        // printf("\n");

        // write to stream
        output.write(readWord);
    }
    return;
}

void addConst(axiBlockStream_t &input, axiBlockStream_t &output){
#pragma HLS PIPELINE II=1 style=flp

    static axiBlock_t readWord;
    const ap_uint<DATA_WIDTH> const1 = 10;
    const ap_uint<DATA_WIDTH> const2 = 50;

    if (!input.empty()){
        // read from stream
        readWord = input.read();

        // calculate
        LOOP_ADD_CONST_1: for(int i = 0 ; i < BLOCK_SIZE; i+=2){
            readWord.range((i+1)*DATA_WIDTH-1, i*DATA_WIDTH) = readWord.range((i+1)*DATA_WIDTH-1, i*DATA_WIDTH) + const1;
        }
        LOOP_ADD_CONST_2: for(int i = 1 ; i < BLOCK_SIZE; i+=2){
            readWord.range((i+1)*DATA_WIDTH-1, i*DATA_WIDTH) = readWord.range((i+1)*DATA_WIDTH-1, i*DATA_WIDTH) + const2;
        }

        // write to stream
        output.write(readWord);
    }
    return;
}