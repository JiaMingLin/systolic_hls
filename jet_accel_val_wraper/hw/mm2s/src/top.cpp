#include"top.h"

void mm2sTop(m_axi_word_t* input, axis_data_stream_t &output, int length, int &status){
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
        axis_data_bus bus;
        bus.data = input[i].range(IN_WORD_WIDTH-1, 0);   // input bitwidth = BUS_WIDTH, extract the lower IN_WORD_WIDTH bits
        bus.last = (i == (length-1))? 1: 0;
        output.write(bus);
    }
    status = length - i;
}