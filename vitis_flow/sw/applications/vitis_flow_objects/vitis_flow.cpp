#include "vitis_flow.h"
#include "vitis_flow_error_codes.h"

using namespace XLNX;

// <Kernel_Name>:<CU_instance_name>
static const char* MM2S_CU_NAME      = "mm2sTop:mm2s";
static const char* ADD_CONST_CU_NAME = "addConstantTop:add_const";
static const char* MUL_CONST_CU_NAME = "multiplicationTop:mul_const";
static const char* S2MM_CU_NAME      = "s2mmTop:s2mm";

VitisFlow::VitisFlow(){

}

VitisFlow::~VitisFlow(){

}

uint32_t VitisFlow::Initialise(DeviceInterface* pDeviceInterface){

    uint32_t retval = XLNX_OK;

    retval = mm2s.Initialise(pDeviceInterface, MM2S_CU_NAME);
    // add_const.Initialise(pDeviceInterface, ADD_CONST_CU_NAME);
    // mul_const.Initialise(pDeviceInterface, MUL_CONST_CU_NAME);
    // printf(retval);
    // if(retval == XLNX_OK){
    //     retval = s2mm.Initialise(pDeviceInterface, S2MM_CU_NAME);
    // }
    
    return retval;
}

uint32_t VitisFlow::Uninitialise(void){
    uint32_t retval = XLNX_OK;

    // currently, the kernels using DRAM/HBM are mm2s and s2mm
    retval = mm2s.Uninitialise();
    retval = s2mm.Uninitialise();

    return retval;
}

uint32_t VitisFlow::StartMM2SDataMover(void){
    uint32_t retval = XLNX_OK;
    mm2s.StartHWKernel();
}

uint32_t VitisFlow::StartS2MMDataMover(void){
    uint32_t retval = XLNX_OK;
    s2mm.StartHWKernel();
}