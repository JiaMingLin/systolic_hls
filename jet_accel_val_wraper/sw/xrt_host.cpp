#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fstream>
#include <sstream>

#include <ap_fixed.h>
#include <ap_int.h>
#include <xrt/xrt_bo.h>
#include <experimental/xrt_xclbin.h>
#include <experimental/xrt_ip.h>
#include <xrt/xrt_device.h>
#include <xrt/xrt_kernel.h>

#include <random>

#define BUS_WIDTH 512
#define IN_DATA_WIDTH 16
#define OUT_DATA_WIDTH 32
#define IN_FRAC_BITS 10
#define OUT_FRAC_BITS 20

#define IN_WORD_LEN 16
#define OUT_WORD_LEN 5

typedef ap_fixed<IN_DATA_WIDTH, IN_DATA_WIDTH - IN_FRAC_BITS> in_data_t;
typedef ap_fixed<OUT_DATA_WIDTH, OUT_DATA_WIDTH - OUT_FRAC_BITS> out_data_t;

void wait_for_enter(const std::string &msg) {
    std::cout << msg << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

bool load_test_data(std::vector<std::vector<float>>& input_data, 
                   std::vector<std::vector<float>>& predict_data,
                   const std::string& input_file,
                   const std::string& predict_file) {
    std::ifstream fin(input_file);
    std::ifstream fpr(predict_file);
    
    if (!fin.is_open() || !fpr.is_open()) {
        std::cout << "Can not open input file or predict file" << std::endl;
        return false;
    }

    std::string iline, pline;
    while (std::getline(fin, iline) && std::getline(fpr, pline)) {
        // 處理輸入資料
        std::vector<float> in;
        std::istringstream iss(iline);
        float val;
        while (iss >> val) {
            in.push_back(val);
        }
        if (in.size() != IN_WORD_LEN) {
            std::cout << "Input data dimension does not match expected" << std::endl;
            return false;
        }
        input_data.push_back(in);

        // 處理預測資料
        std::vector<float> pr;
        std::istringstream pss(pline);
        while (pss >> val) {
            pr.push_back(val);
        }
        if (pr.size() != OUT_WORD_LEN) {
            std::cout << "Predict data dimension does not match expected" << std::endl;
            return false;
        }
        predict_data.push_back(pr);
    }

    fin.close();
    fpr.close();
    return true;
}

int main(int argc, char** argv) {
	
    ap_uint<BUS_WIDTH> *data_in, *data_out;

    std::vector<std::vector<float>> input_data;
    std::vector<std::vector<float>> ground_truth;
    std::cout << "Load the test golden data" << std::endl;

    if (!load_test_data(input_data, ground_truth, 
                        "tb_input_features.dat",
                        "tb_output_predictions.dat")) {
        std::cout << "Can not load test data" << std::endl;
        return -1;
    }

    // choose device and initialize device
	std::string binaryFile = "./accel_val_wrapper.xclbin";
    int device_index = 0;
    std::cout << "Open the device" << device_index << std::endl;
    auto device = xrt::device(device_index);
    std::cout << "Load the xclbin " << binaryFile << std::endl;
    auto uuid = device.load_xclbin(binaryFile);

    std::cout << "Initialize kernek: mm2sTop:mm2s at uuid = " << uuid << std::endl;
    auto mm2s_krnl = xrt::kernel(device, uuid, "mm2sTop", 
				xrt::kernel::cu_access_mode::exclusive); //
    std::cout << "Initialize kernek: s2mm at uuid = " << uuid << std::endl;
    auto s2mm_krnl = xrt::kernel(device, uuid, "s2mmTop", 
				xrt::kernel::cu_access_mode::exclusive); // 

    std::cout << "Allocate Buffer in Global Memory\n";
    const size_t data_size = input_data.size();
    size_t data_size_bytes = sizeof(ap_uint<BUS_WIDTH>) * data_size;
    auto input_buffer = xrt::bo(device, data_size_bytes, mm2s_krnl.group_id(0));
    auto output_buffer = xrt::bo(device, data_size_bytes, s2mm_krnl.group_id(1));

    std::cout << "Map the Buffer Object to Host \n";
    auto input_bo_map = input_buffer.map<ap_uint<BUS_WIDTH>*>();
    auto output_bo_map = output_buffer.map<ap_uint<BUS_WIDTH>*>();

    std::cout << "Initialize data on Host Memory\n";
    for(int i = 0; i < data_size; i++){
        ap_uint<BUS_WIDTH> input_bus;
        for(int j = 0; j < IN_WORD_LEN; j++){
            input_bus.range((j+1) * IN_DATA_WIDTH - 1, j * IN_DATA_WIDTH) 
            = (ap_uint<IN_DATA_WIDTH>)(in_data_t(input_data[i][j]) * (1 << IN_FRAC_BITS));
        }
        input_bo_map[i] = input_bus;
    }
    std::cout << "Synchronize data from Host Memory to Device Memory\n";
    input_buffer.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    std::cout << "Execution of MM2S\n";
    int status = 1;
    auto mm2s_run = xrt::run(mm2s_krnl);
    mm2s_run.set_arg(0, input_buffer);
    mm2s_run.set_arg(2, data_size);

    auto s2mm_run = xrt::run(s2mm_krnl);
    s2mm_run.set_arg(1, output_buffer);
    std::cout << "Reading status before run = " << status << std::endl;
    // wait_for_enter("Pressing enter to continue...\n");
    mm2s_run.start();
    s2mm_run.start();
    s2mm_run.wait();

    std::cout << "Reading CU mapped register\n";
    // auto mm2s_ip = xrt::ip(device, uuid, "mm2sTop:{mm2s}");
    status = (int)mm2s_krnl.read_register(0x24);
    std::cout << "Reading status after run = " << status << std::endl;

    std::cout << "Sychronizing output buffer\n";
    std::vector<std::vector<out_data_t>> hw_output_data;
    output_buffer.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
    for(int i = 0; i < data_size; i++){
        std::vector<out_data_t> hw_output_row;
        for(int j = 0; j < OUT_WORD_LEN; j++){
            hw_output_row.push_back(out_data_t(output_bo_map[i].range((j+1) * OUT_DATA_WIDTH - 1, j * OUT_DATA_WIDTH))/(1 << OUT_FRAC_BITS));
        }
        hw_output_data.push_back(hw_output_row);
    }

    std::cout << "Validating the hardware output data" << std::endl;
    // row of hw_output_data is a vector of fixed point data, e.g. [0.1, 0.3, 0.01, 0.4, 0.2]
    // row of ground_truth is a vector of float data, e.g. [0., 0., 1., 0., 0.]
    // compare the argmax of both vector, if they are the same, the test pass
    int err = 0;
    for(int i = 0; i < data_size; i++){
        int hw_argmax = std::max_element(hw_output_data[i].begin(), hw_output_data[i].end()) - hw_output_data[i].begin();
        int gt_argmax = std::max_element(ground_truth[i].begin(), ground_truth[i].end()) - ground_truth[i].begin();
        if(hw_argmax != gt_argmax){
            err++;
        }
    }
    // print the error rate in numbers and percentage
    std::cout << "Error rate: " << err << "/" << data_size << " = " << (float)err/data_size << std::endl;

    return err;
}
