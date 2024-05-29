#include"top.h"

void mm2sTop(axiWord_t* input, axiWordStream_t &output, int length){
    #pragma HLS INTERFACE mode=m_axi port=input offset=slave
    #pragma HLS INTERFACE mode=axis port=output
    
    #pragma HLS INTERFACE s_axilite port=length
    #pragma HLS INTERFACE ap_none port=length
    #pragma HLS INTERFACE s_axilite port=return

    for(int i = 0; i < length; i++){
        #pragma HLS loop_tripcount min=4096 max=4096 avg=4096
        #pragma HLS PIPELINE II=1
        output.write(input[i]);
    }
}