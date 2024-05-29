#include "mm2s.h"
#include "mm2s_address_map.h"

using namespace XLNX;

MM2S::MM2S(){
    m_pDeviceInterface = nullptr;
    m_cuAddress = 0;
    m_cuIndex = 0;
    m_initialisedMagicNumber = 0;
}

MM2S::~MM2S(){

}

MM2S::Initialise(DeviceInterface* pDeviceInterface, const char* cuName){
    uint32_t retval = XLNX_OK;
	bool bHostBank;

    m_initialisedMagicNumber = 0;	//will be set to magic number if we successfully initialise...

	m_pDeviceInterface = pDeviceInterface;

    retval = m_pDeviceInterface->GetCUAddress(cuName, &m_cuAddress);
	if (retval != XLNX_OK)
	{
		retval = XLNX_ORDER_BOOK_DATA_MOVER_ERROR_CU_NAME_NOT_FOUND;
	}

    if (retval == XLNX_OK)
	{
		retval = m_pDeviceInterface->GetCUIndex(cuName, &m_cuIndex);
		if (retval != XLNX_OK)
		{
			retval = XLNX_ORDER_BOOK_DATA_MOVER_ERROR_CU_NAME_NOT_FOUND;
		}
	}

    if (retval == XLNX_OK)
	{
		retval = m_pDeviceInterface->GetArgumentMemTopologyIndex(cuName, KERNEL_MEMORY_ARGUMENT_READ_BUFFER_INDEX, &m_readBufferMemTopologyIndex);
		if (retval != XLNX_OK)
		{
			retval = XLNX_ORDER_BOOK_DATA_MOVER_ERROR_CU_NAME_NOT_FOUND;
		}
	}

    if (retval == XLNX_OK)
	{
		retval = m_pDeviceInterface->TopologyIndexIsHostBank(m_readBufferMemTopologyIndex, &bHostBank);
		if (retval == XLNX_OK)
		{
			if (bHostBank)
			{
				//NOTE - we are assuming both read and write buffers are connected to the same bank...
				//      i.e. either both are connected to a CARD RAM bank, or both are connected to HOST BANK
				m_hostBankMemoryTopologyIndex = m_readBufferMemTopologyIndex;
				m_bUsingHostBank = true;
			}
		}
	}

    // and the write buffer
	if (retval == XLNX_OK)
	{
		retval = m_pDeviceInterface->GetArgumentMemTopologyIndex(cuName, KERNEL_MEMORY_ARGUMENT_WRITE_BUFFER_INDEX, &m_writeBufferMemTopologyIndex);
		if(retval != XLNX_OK)
		{
			retval = XLNX_ORDER_BOOK_DATA_MOVER_ERROR_CU_NAME_NOT_FOUND;
		}
	}




	if (retval == XLNX_OK)
	{
		retval = m_pDeviceInterface->TopologyIndexIsHostBank(m_writeBufferMemTopologyIndex, &bHostBank);
		if (retval == XLNX_OK)
		{
			if (bHostBank)
			{
				//NOTE - we are assuming both read and write buffers are connected to the same bank...
				//      i.e. either both are connected to a CARD RAM bank, or both are connected to HOST BANK
				m_hostBankMemoryTopologyIndex = m_writeBufferMemTopologyIndex;
				m_bUsingHostBank = true;
			}
		}
	}

    if (retval == XLNX_OK)
	{
		m_initialisedMagicNumber = IS_INITIALISED_MAGIC_NUMBER;
	}

    if (retval == XLNX_OK)
	{
		//NOTE - keep the following after the setting of the IS_INITIALISED magic number...
		InitialiseRingIndexes();
	}
}