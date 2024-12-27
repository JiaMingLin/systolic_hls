#include"top.h"

void writeData(axis_data_stream_t &input, m_axi_word_t* output)
{
    static int counter = 0;
    axis_data_bus axis_bus;
    m_axi_word_t m_axi_bus;

    while(1){
        #pragma HLS PIPELINE II=1
        if (!input.empty()){
            // read from stream
            axis_bus = input.read();
            m_axi_bus.range(OUT_WORD_WIDTH-1, 0) = axis_bus.data;

            output[counter] = m_axi_bus;
            if(axis_bus.last) break;
            counter++;
        }
    }
    return;
}

void s2mmTop(axis_data_stream_t &input, m_axi_word_t* output){
    #pragma HLS INTERFACE mode=m_axi  port=output depth=128
    #pragma HLS INTERFACE mode=axis port=input
    
    #pragma HLS INTERFACE s_axilite port=return

    writeData(input, output);
    
}
