#include"top.h"

extern "C" void addConstantTop(axiDataStream_t &input, axiDataStream_t &output){

#pragma HLS INTERFACE axis port=input
#pragma HLS INTERFACE axis port=output
#pragma HLS INTERFACE ap_ctrl_none port=return

    static axiDataStream_t intermediateStream("intermediateStream");
#pragma HLS STREAM variable=intermediateStream

#pragma HLS DATAFLOW disable_start_propagation

    addOne(input, intermediateStream);
    addTwo(intermediateStream, output);

}

void addOne(axiDataStream_t &input, axiDataStream_t &output){
#pragma HLS PIPELINE II=1 style=flp

    axiDataBus bus;
    const ap_uint<DATA_WIDTH> const1 = 10;
    
    if (!input.empty()){
        // read from stream
        bus = input.read();
    
        // calculate
        for(int i = 0 ; i < NUM_WORD; i++){
            bus.data.range((i+1)*DATA_WIDTH-1, i*DATA_WIDTH) = bus.data.range((i+1)*DATA_WIDTH-1, i*DATA_WIDTH) + const1;
        }

        // write to stream
        output.write(bus);
    }
    return;
}

void addTwo(axiDataStream_t &input, axiDataStream_t &output){
#pragma HLS PIPELINE II=1 style=flp

    axiDataBus bus;
    const ap_uint<DATA_WIDTH> const1 = 10;
    const ap_uint<DATA_WIDTH> const2 = 50;

    if (!input.empty()){
        // read from stream
        bus = input.read();

        // calculate
        LOOP_ADD_CONST_1: for(int i = 0 ; i < NUM_WORD; i+=2){
            bus.data.range((i+1)*DATA_WIDTH-1, i*DATA_WIDTH) = bus.data.range((i+1)*DATA_WIDTH-1, i*DATA_WIDTH) + const1;
        }
        LOOP_ADD_CONST_2: for(int i = 1 ; i < NUM_WORD; i+=2){
            bus.data.range((i+1)*DATA_WIDTH-1, i*DATA_WIDTH) = bus.data.range((i+1)*DATA_WIDTH-1, i*DATA_WIDTH) + const2;
        }

        // write to stream
        output.write(bus);
    }
    return;
}