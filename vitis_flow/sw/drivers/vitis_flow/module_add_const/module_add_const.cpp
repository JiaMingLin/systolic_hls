#include "module_add_const.h"
#include "module_add_const_address_map.h"

using namespace XLNX;

static const uint32_t IS_INITIALISED_MAGIC_NUMBER = 0xD284583A;


AddConstant::AddConstant(){
    m_pDeviceInterface = nullptr;
	m_cuAddress = 0;
	m_cuIndex = 0;
	m_initialisedMagicNumber = 0;
}

AddConstant::~AddConstant(){

}

uint32_t AddConstant::Initialise(DeviceInterface* pDeviceInterface, const char* cuName){

    uint32_t retval = XLNX_OK;

    m_initialisedMagicNumber = 0;	//will be set to magic number if we successfully initialise...

    m_pDeviceInterface = pDeviceInterface;

    retval = m_pDeviceInterface->GetCUAddress(cuName, &m_cuAddress);

    if (retval != XLNX_OK)
    {
        retval = ADD_CONST_ERROR_CU_NAME_NOT_FOUND;
    }

    if (retval == XLNX_OK)
    {
        retval = m_pDeviceInterface->GetCUIndex(cuName, &m_cuIndex);

        if (retval != XLNX_OK)
        {
            retval = ADD_CONST_ERROR_CU_NAME_NOT_FOUND;
        }
    }
}