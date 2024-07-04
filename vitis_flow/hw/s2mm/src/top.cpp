#include"top.h"

void writeData(axiDataStream_t &input, axiWord_t* output)
{
    static int counter = 0;
    axiDataBus bus;
    while(1){
        #pragma HLS PIPELINE II=1
        if (!input.empty()){
            // read from stream
            bus = input.read();

            output[counter] = bus.data;
            if(bus.last) break;
            counter++;
        }
    }
    
    return;
}

void s2mmTop(axiDataStream_t &input, axiWord_t* output){
    #pragma HLS INTERFACE mode=m_axi  port=output depth=128
    #pragma HLS INTERFACE mode=axis port=input depth=128
    
    #pragma HLS INTERFACE s_axilite port=return

    writeData(input, output);
    
}
