#include "myproject_axi_stream.h"

#include <random>


int main(){
    // generate init data
    int length = 1;
    T_in input[length][N_IN];
    axis_in_stream_t input_bo_map;
    axis_out_stream_t output_bo_map;

    // fill data
    std::random_device rd;
    // Initialize random engine and distribution
    std::mt19937 gen(rd()); // Mersenne Twister engine
    std::uniform_real_distribution<> dist(-5.0, 5.0); // Range [0.0, 1.0)

    /*
    double random_value = dist(gen);
    std::cout << "random_value " << random_value << std::endl;
    ap_fixed<16, 16 - IN_FRAC_BITS> input_value = (ap_fixed<16, 16 - IN_FRAC_BITS>)random_value;
    std::cout << "Original input: " << input_value << std::endl;
    std::cout << "Original binary: " << input_value.to_string(2) << std::endl;

    // 編碼：直接複製位元表示
    ap_uint<IN_DATA_WIDTH> encoded_input_data;
    encoded_input_data.range(IN_DATA_WIDTH-1, 0) = input_value.range(IN_DATA_WIDTH-1, 0);
    std::cout << "Encoded binary: " << encoded_input_data.to_string(2) << std::endl;

    // 解碼：直接複製位元表示
    ap_fixed<16, 16 - IN_FRAC_BITS> decoded_input_data;
    decoded_input_data.range(IN_DATA_WIDTH-1, 0) = encoded_input_data.range(IN_DATA_WIDTH-1, 0);
    std::cout << "Decoded value: " << decoded_input_data << std::endl;
    std::cout << "Decoded binary: " << decoded_input_data.to_string(2) << std::endl;
    */
    
    for(int i = 0; i < length; i++){
        for(int j = 0; j < N_IN; j++){
            double random_value = dist(gen);
            input[i][j] = random_value;
            std::cout << "input " << (float)input[i][j] << " random " << random_value << std::endl;
        }
    }

    for(int i = 0; i < length; i++){
        axis_in_data_bus input_bus;
        // encode input
        for(int j = 0; j < N_IN; j++){
            in_extended_t in_extended = in_extended_t(input[i][j]);
            ap_uint<IN_DATA_WIDTH> encoded_input_data = ap_uint<IN_DATA_WIDTH>(in_extended << IN_FRAC_BITS);
            input_bus.data.range((j+1) * IN_DATA_WIDTH - 1, j * IN_DATA_WIDTH) = encoded_input_data;
        }
        input_bus.last = (i == (length-1));
        input_bo_map.write(input_bus);

        myproject_axi_stream(input_bo_map, output_bo_map);

        // decode output
        axis_out_data_bus output_bus = output_bo_map.read();
        for(int j = 0; j < N_OUT; j++){ 
            ap_uint<OUT_DATA_WIDTH> temp = output_bus.data.range((j+1) * OUT_DATA_WIDTH - 1, j * OUT_DATA_WIDTH);
            out_extended_t out_extended = out_extended_t(temp);
            T_out output_data = T_out(out_extended >> OUT_FRAC_BITS);
            bool last = output_bus.last;
            std::cout << " output_data " << output_data.to_string(10).c_str() << std::endl;
        }
    }

    return 0;
}