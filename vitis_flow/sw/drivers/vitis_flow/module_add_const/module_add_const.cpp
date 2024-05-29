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

    if (retval == XLNX_OK)
	{
		m_initialisedMagicNumber = IS_INITIALISED_MAGIC_NUMBER;
	}

	return retval;
}


// uint32_t AddConstant::IsRunning(bool* pbIsRunning)
// {
//     uint32_t retval = XLNX_OK;
//     uint32_t offset;
//     uint32_t value;
//     uint32_t mask = 0x01;
//     uint32_t shift = 0;

//     retval = CheckIsInitialised();


//     if (retval == XLNX_OK)
//     {
//         offset = ADD_CONST_RESET_CONTROL_OFFSET;

//         retval = ReadReg32(offset, &value);
//     }


//     if (retval == XLNX_OK)
//     {
//         value = (value >> shift)& mask;

//         if (value == 0)
//         {
//             *pbIsRunning = true;
//         }
//         else
//         {
//             *pbIsRunning = false;
//         }
//     }

//     return retval;
// }

void AddConstant::IsInitialised(bool* pbIsInitialised)
{
    if (CheckIsInitialised() == XLNX_OK)
    {
        *pbIsInitialised = true;
    }
    else
    {
        *pbIsInitialised = false;
    }
}


uint32_t AddConstant::GetCUAddress(uint64_t* pCUAddress)
{
	uint32_t retval = XLNX_OK;

	retval = CheckIsInitialised();

	if (retval == XLNX_OK)
	{
		*pCUAddress = m_cuAddress;
	}

	return retval;
}


uint32_t AddConstant::GetCUIndex(uint32_t* pCUIndex)
{
	uint32_t retval = XLNX_OK;

	retval = CheckIsInitialised();

	if (retval == XLNX_OK)
	{
		*pCUIndex = m_cuIndex;
	}

	return retval;
}

uint32_t AddConstant::ReadReg32(uint64_t offset, uint32_t* value)
{
	uint32_t retval = XLNX_OK;
	uint32_t address = m_cuAddress + offset;

	retval = m_pDeviceInterface->ReadReg32(address, value);

	if (retval != XLNX_OK)
	{
		retval = ADD_CONST_ERROR_IO_FAILED;
	}

	return retval;
}


uint32_t AddConstant::WriteReg32(uint64_t offset, uint32_t value)
{
	uint32_t retval = XLNX_OK;
	uint64_t address = m_cuAddress + offset;

	retval = m_pDeviceInterface->WriteReg32(address, value);

	if (retval != XLNX_OK)
	{
		retval = ADD_CONST_ERROR_IO_FAILED;
	}

	return retval;
}


uint32_t AddConstant::WriteRegWithMask32(uint64_t offset, uint32_t value, uint32_t mask)
{
	uint32_t retval = XLNX_OK;
	uint64_t address = m_cuAddress + offset;

	retval = m_pDeviceInterface->WriteRegWithMask32(address, value, mask);

	if (retval != XLNX_OK)
	{
		retval = ADD_CONST_ERROR_IO_FAILED;
	}

	return retval;
}


uint32_t AddConstant::BlockReadReg32(uint64_t offset, uint32_t* buffer, uint32_t numWords)
{
	uint32_t retval = XLNX_OK;
	uint64_t address = m_cuAddress + offset;

	retval = m_pDeviceInterface->BlockReadReg32(address, buffer, numWords);

	if (retval != XLNX_OK)
	{
		retval = ADD_CONST_ERROR_IO_FAILED;
	}

	return retval;
}