/*
#Copyright (C) 2023, Advanced Micro Devices, Inc. All rights reserved.
#SPDX-License-Identifier: X11
#*/

//#include "cmdlineparser.h"
// #include <iostream>
// #include <cstring>

// // XRT includes
// // #include "xrt/xrt_bo.h"
// #include <experimental/xrt_xclbin.h>
// // #include <experimental/xrt_ip.h>
// #include "experimental/xrt-next.h"
// // #include "xrt/xrt_device.h"
// // #include "xrt/xrt_kernel.h"
// // #include <ap_int.h>
// #include "xrt.h"

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

#include <ap_int.h>
#include <xrt/xrt_bo.h>
#include <experimental/xrt_xclbin.h>
#include <experimental/xrt_ip.h>
#include <xrt/xrt_device.h>
#include <xrt/xrt_kernel.h>


// #include "xlnx_device_manager.h"
// #include "vitis_flow.h"

#define DATA_SIZE 16
#define BUS_WIDTH 512
#define DATA_WIDTH 32

// using namespace XLNX;

// DeviceManager g_deviceManager;
// VitisFlow vitis_flow;

void wait_for_enter(const std::string &msg) {
    std::cout << msg << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void cpuCompute(ap_uint<BUS_WIDTH> *data_in, ap_uint<BUS_WIDTH> *data_out){
    
    const int add1_const = 10;
    const int add2_const = 50;
    for(int i = 0; i < DATA_SIZE; i++){
        
        ap_uint<BUS_WIDTH> data = data_in[i];

        // ADD constant
        // ADD 1
        for(int j = 0; j < BUS_WIDTH/DATA_WIDTH; j++){
            data.range((j+1) * DATA_WIDTH - 1, j * DATA_WIDTH) = data.range((j+1) * DATA_WIDTH - 1, j * DATA_WIDTH) + add1_const;
        }

        // ADD 2
        for(int j = 0; j < BUS_WIDTH/DATA_WIDTH; j+=2){
            data.range((j+1) * DATA_WIDTH - 1, j * DATA_WIDTH) = data.range((j+1) * DATA_WIDTH - 1, j * DATA_WIDTH) + add1_const;
        }
        for(int j = 1; j < BUS_WIDTH/DATA_WIDTH; j+=2){
            data.range((j+1) * DATA_WIDTH - 1, j * DATA_WIDTH) = data.range((j+1) * DATA_WIDTH - 1, j * DATA_WIDTH) + add2_const;
        }

        // MUL Constant
        // MUL 1
        ap_uint<DATA_WIDTH> first_element = data.range(DATA_WIDTH - 1, 0);
        for(int j = 0; j < BUS_WIDTH/DATA_WIDTH; j++){
            data.range((j+1) * DATA_WIDTH - 1, j * DATA_WIDTH) = data.range((j+1) * DATA_WIDTH - 1, j * DATA_WIDTH) * first_element;
        }

        // MUL 2
        for(int j = 0; j < BUS_WIDTH/DATA_WIDTH; j++){
            data.range((j+1) * DATA_WIDTH - 1, j * DATA_WIDTH) = data.range((j+1) * DATA_WIDTH - 1, j * DATA_WIDTH) * data.range((j+1) * DATA_WIDTH - 1, j * DATA_WIDTH);
        }

        data_out[i] = data;
    }
}

int main(int argc, char** argv) {
	
    ap_uint<BUS_WIDTH> *data_in, *data_out, *cpu_out;
    cpu_out = (ap_uint<BUS_WIDTH> *)malloc(BUS_WIDTH * DATA_SIZE);
    if(cpu_out == NULL){
        printf("[ERROR] CPU output buffer allocate error");
        return 1;
    }

    // choose device and initialize device
	std::string binaryFile = "./vitis_flow.xclbin";
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
    size_t data_size_bytes = sizeof(ap_uint<BUS_WIDTH>) * DATA_SIZE;
    auto input_buffer = xrt::bo(device, data_size_bytes, mm2s_krnl.group_id(0));
    auto output_buffer = xrt::bo(device, data_size_bytes, s2mm_krnl.group_id(1));

    std::cout << "Map the Buffer Object to Host \n";
    auto input_bo_map = input_buffer.map<ap_uint<BUS_WIDTH>*>();
    auto output_bo_map = output_buffer.map<ap_uint<BUS_WIDTH>*>();

    std::cout << "Initialize data on Host Memory\n";
    for(int i = 0; i < DATA_SIZE; i++){
        ap_uint<BUS_WIDTH> bus;
        for(int j = 0; j < BUS_WIDTH/DATA_WIDTH; j++){
            bus.range((j+1) * DATA_WIDTH - 1, j * DATA_WIDTH) = rand();
        }
        input_bo_map[i] = bus;
    }
    std::cout << "Synchronize data from Host Memory to Device Memory\n";
    input_buffer.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    std::cout << "Execution of MM2S\n";
    int status = 1;
    auto mm2s_run = xrt::run(mm2s_krnl);
    mm2s_run.set_arg(0, input_buffer);
    mm2s_run.set_arg(2, DATA_SIZE);

    auto s2mm_run = xrt::run(s2mm_krnl);
    s2mm_run.set_arg(1, output_buffer);
    std::cout << "Reading status before run = " << status << std::endl;
    wait_for_enter("Pressing enter to continue...\n");
    mm2s_run.start();
    s2mm_run.start();
    s2mm_run.wait();

    std::cout << "Reading CU mapped register\n";
    // auto mm2s_ip = xrt::ip(device, uuid, "mm2sTop:{mm2s}");
    status = (int)mm2s_krnl.read_register(0x24);
    std::cout << "Reading status after run = " << status << std::endl;

    std::cout << "Sychronizing output buffer\n";
    output_buffer.sync(XCL_BO_SYNC_BO_FROM_DEVICE);

    std::cout << "Software Compute \n";
    cpuCompute(input_bo_map, cpu_out);

    // validate
    int err = 0;
    for(int i = 0; i < DATA_SIZE; i++){
        ap_uint<BUS_WIDTH> output_bus = (ap_uint<BUS_WIDTH>)output_bo_map[i];
        ap_uint<BUS_WIDTH> cpu_output_row = cpu_out[i];
        for(int j = 0; j < BUS_WIDTH/DATA_WIDTH; j++){
            if((uint32_t)output_bus.range((j+1) * DATA_WIDTH - 1, j * DATA_WIDTH) != cpu_output_row.range((j+1) * DATA_WIDTH - 1, j * DATA_WIDTH)) err++;
            printf("%d ,", (int)output_bus.range((j+1) * DATA_WIDTH - 1, j * DATA_WIDTH));
        }
        printf("\n");
    }
    if(err == 0) printf("Test Pass!!\n");
    else printf("Test Failed!! ERR= %d \n", err);

    // xrtBOFree(input_buffer);
    // xrtBOFree(output_buffer);

    return err;

    // 
    /*
    // initialize the main modules
    // get CU address and memory topologies
    uint32_t init_result = vitis_flow.Initialise(pDeviceInterface);
    printf("Vitis initialization result: %d\n", init_result);

    // allocate and map data buffer 
    printf("Allocate and map data buffer...\n");
    vitis_flow.mm2s.SetupBuffers();
    vitis_flow.s2mm.SetupBuffers();

    // virtual address of the HOST BUFFERS for host to WRITE the inputs
    data_in = (ap_uint<BUS_WIDTH>*)vitis_flow.mm2s.GetWriteBufferHostVirtualAddress();
    // virtual address of the HOST BUFFERS for host to READ the results
    data_out = (ap_uint<BUS_WIDTH>*)vitis_flow.s2mm.GetReadBufferHostVirtualAddress();
    
    // fill data
    printf("Filling data...");
    for(int i = 0; i < DATA_SIZE; i++){
        ap_uint<BUS_WIDTH> bus = 0;
        for(int j = 0; j < BUS_WIDTH/DATA_WIDTH; j++){
            bus.range((j+1)*BUS_WIDTH-1, j) = rand();
        }
        data_in[i] = bus;
    }

    printf("[INFO] CPU compute start...");
    // software compute
    cpuCompute(data_in, cpu_out);
    printf("[INFO] CPU compute done!");

    // hardware compute
    // (1) sync data, (2) execute
    // Note: the stream to memory map module is not freely run, must be manually started running from the host.
    vitis_flow.mm2s.SyncWriteBuffer();
    vitis_flow.mm2s.StartHWKernel();
    vitis_flow.s2mm.StartHWKernel();

    // wait for complete
    pDeviceInterface->Wait();

    // sync output data
    vitis_flow.s2mm.SyncReadBuffer();

    // comparison hardware compute with software compute
    int err = 0;
    for(int i = 0; i < DATA_SIZE; i++){
        ap_uint<BUS_WIDTH> hw_bus = data_out[i];
        ap_uint<BUS_WIDTH> sw_bus = cpu_out[i];
        for(int j = 0; j < BUS_WIDTH/DATA_WIDTH; j++){
            if(hw_bus.range((j+1) * DATA_WIDTH, j * DATA_WIDTH) != sw_bus.range((j+1) * DATA_WIDTH, j * DATA_WIDTH)){
                err++;
            }
        }
    }

    printf("The error count = %d", err);
    if(err > 0){
        return err;
    }
    std::cout << "TEST PASSED\n";
    return 0;
    */
}
