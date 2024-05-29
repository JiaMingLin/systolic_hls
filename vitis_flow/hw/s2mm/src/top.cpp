#include"top.h"


void writeData(axiWordStream_t &input, axiWord_t* output)
{
#pragma HLS PIPELINE II=1 style=flp
    static axiWord_t readWord;
    static int counter = 0;

    if (!input.empty()){
        // read from stream
        readWord = input.read();

        output[counter] = readWord;
        counter++;
    }
    return;
}

void s2mmTop(axiWordStream_t &input, axiWord_t* output){
    #pragma HLS INTERFACE mode=m_axi port=output offset=slave
    #pragma HLS INTERFACE mode=axis port=input
    
    #pragma HLS INTERFACE s_axilite port=return

    writeData(input, output);
    
}