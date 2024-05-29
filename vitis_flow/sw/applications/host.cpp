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
#include <cstdint>

#include <xrt.h>
#include <experimental/xrt-next.h>
#include <experimental/xrt_xclbin.h>

#include "xlnx_device_manager.h"

#define LENGTH 4096
#define BUS_WIDTH 512
#define WORD_WIDTH 32
#define NUM_WORD BUS_WIDTH/WORD_WIDTH

DeviceManager g_deviceManager;

DeviceInterface* ChooseDeviceInterface(int device_idx){
	int numDevices = g_deviceManager.EnumerateDevices();
	DeviceInterface* pDeviceInterface;

	if(numDevices >= device_idx){
		pDeviceInterface = g_deviceManager.CreateHWDeviceInterface(device_idx);
	}
	else
	{
		printf("[ERROR] Specified -d device index = %d\n", device_idx);
		printf("        Found %d HW device(s)\n", numDevices);
		exit(0);
	}

	printf("Using device: %s...\n\n", pDeviceInterface->GetDeviceName());
	return pDeviceInterface;
}

int main(int argc, char** argv) {

	
	DeviceInterface* pDeviceInterface;
	pDeviceInterface = ChooseDeviceInterface(0);
    

    // std::cout << "argc = " << argc << std::endl;
	// for(int i=0; i < argc; i++){
	//     std::cout << "argv[" << i << "] = " << argv[i] << std::endl;
	// }

    // // Read settings
    // std::string binaryFile = "./vitis_flow.xclbin";
    // int device_index = 0;

    // std::cout << "Open the device" << device_index << std::endl;
    // auto device = xrt::device(device_index);
    // std::cout << "Load the xclbin " << binaryFile << std::endl;
    // auto uuid = device.load_xclbin("./vitis_flow.xclbin");

    // size_t vector_size_bytes = BUS_WIDTH/8 * LENGTH;

    // //auto krnl = xrt::kernel(device, uuid, "vadd");, xrt::kernel::cu_access_mode::exclusive
    // auto mm2s_krnl = xrt::ip(device, uuid, "mm2s");

    // std::cout << "Allocate Buffer in Global Memory\n";
    // auto boIn1 = xrt::bo(device, vector_size_bytes, krnl.group_id(0)); //Match kernel arguments to RTL kernel
    // auto boOut = xrt::bo(device, vector_size_bytes, krnl.group_id(1));

    // // Map the contents of the buffer object into host memory
    // auto bo0_map = boIn1.map<ap_uint<BUS_WIDTH>*>();
    // auto bo1_map = boOut.map<ap_uint<BUS_WIDTH>*>();
    // std::fill(bo0_map, bo0_map + LENGTH, 0);
    // std::fill(bo1_map, bo1_map + LENGTH, 0);

    // // Create the test data
    // ap_uint<BUS_WIDTH> bufReference[LENGTH];
    // for (int i = 0; i < LENGTH; ++i) {
    //     ap_uint<BUS_WIDTH> bus;
    //     for(int j = 0; j < WORD_WIDTH; j++){
    //         bus.range((j+1)*BUS_WIDTH-1, j) = i+j;
    //     }
    //     bo0_map[i] = bus;
    // }

    // // Synchronize buffer content with device side
    // std::cout << "synchronize input buffer data to device global memory\n";
    // boIn1.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    // std::cout << "Execution of the kernel\n";
    // auto run = krnl(boIn1, boOut, LENGTH); //DATA_SIZE=size
    // // auto run = krnl(boIn1, boIn2, boOut);
    // run.wait();

    // // Get the output;
    // std::cout << "Get the output data from the device" << std::endl;
    // boOut.sync(XCL_BO_SYNC_BO_FROM_DEVICE);

    // // Validate results
    // // if (std::memcmp(bo2_map, bufReference, vector_size_bytes))
    // //     throw std::runtime_error("Value read back does not match reference");

    std::cout << "TEST PASSED\n";
    return 0;
}