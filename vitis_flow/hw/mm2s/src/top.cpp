#include"top.h"

void mm2sTop(axiWord_t* input, axiDataStream_t &output, int length, int &status){
    #pragma HLS INTERFACE mode=m_axi port=input offset=slave depth=128
    #pragma HLS INTERFACE mode=axis port=output
    
    #pragma HLS INTERFACE s_axilite port=length
    #pragma HLS INTERFACE s_axilite port=status
    #pragma HLS INTERFACE ap_none port=length
    #pragma HLS INTERFACE s_axilite port=return
    int i;
    for(i = 0; i < length; i++){
        #pragma HLS loop_tripcount min=64 max=64 avg=64
        #pragma HLS PIPELINE II=1
        axiDataBus bus;
        bus.data = input[i];
        bus.last = (i == (length-1))? 1: 0;
        output.write(bus);
    }
    status = length - i;

}