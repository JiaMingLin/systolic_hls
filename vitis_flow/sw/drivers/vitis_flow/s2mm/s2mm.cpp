#include <chrono>
#include <cinttypes>
#include <cstring>
#include <iostream>

#include "s2mm.h"
#include "s2mm_address_map.h"
#include "s2mm_error_codes.h"

#include "xlnx_hw_device_interface.h"

using namespace XLNX;




static const uint32_t IS_INITIALISED_MAGIC_NUMBER = 0x9265CED1;


S2MM::S2MM()
{
	m_initialisedMagicNumber = 0;

	m_cuIndex = 0xFFFFFFFF;
	m_cuAddress = 0;

	// compute unit
	m_pDeviceInterface = nullptr;

	// buffer
	m_readBufferMemTopologyIndex = 0;
	m_writeBufferMemTopologyIndex = 0;

	// m_bUsingHostBank = false;
	// m_pHostBankBufferDescriptor = nullptr;
	// m_hostBankMemoryTopologyIndex = 0;
	// m_hostBankBufferVirtualAddress = nullptr;

	m_pReadBufferDescriptor = nullptr;
	m_pWriteBufferDescriptor = nullptr;

	m_readBufferHostVirtualAddress = nullptr;
	m_readBufferHWAddress = INVALID_HW_BUFFER_ADDRESS;

	m_writeBufferHostVirtualAddress = nullptr;
	m_writeBufferHWAddress = INVALID_HW_BUFFER_ADDRESS;

	m_bNeedToSetupBuffers = true;

	// ring buffer
	// m_lastReadHeadIndex = 0;
	// m_lastReadTailIndex = 0;

    // m_lastWriteHeadIndex = 0;
    // m_lastWriteTailIndex = 0;



	// m_bKeepRunning		= false;
	// m_bVerboseTracing	= false;
	// m_bYield			= false;

	// m_pPricingInterface = &m_hostPricingEngine;

	// ResetDMAStats();

	// m_maxDMAChunkSize = 0; //0 = DMA all new elements in one go (i.e. no maximum)

	// m_hwEmulationPollDelaySeconds = HW_EMU_POLL_DELAY_DEFAULT_SECONDS;
}

S2MM::~S2MM()
{
	Uninitialise();
}


uint32_t S2MM::Initialise(DeviceInterface* pDeviceInterface, const char* cuName)
{
	uint32_t retval = XLNX_OK;
	// bool bHostBank;

	m_initialisedMagicNumber = 0;	//will be set to magic number if we successfully initialise...

	m_pDeviceInterface = pDeviceInterface;


	retval = m_pDeviceInterface->GetCUAddress(cuName, &m_cuAddress);
	if (retval != XLNX_OK)
	{
		retval = S2MM_ERROR_CU_NAME_NOT_FOUND;
	}


	if (retval == XLNX_OK)
	{
		retval = m_pDeviceInterface->GetCUIndex(cuName, &m_cuIndex);
		if (retval != XLNX_OK)
		{
			retval = S2MM_ERROR_CU_NAME_NOT_FOUND;
		}
	}


	if (retval == XLNX_OK)
	{
		retval = m_pDeviceInterface->GetArgumentMemTopologyIndex(cuName, KERNEL_MEMORY_ARGUMENT_READ_BUFFER_INDEX, &m_writeBufferMemTopologyIndex);
		if (retval != XLNX_OK)
		{
			retval = S2MM_ERROR_CU_NAME_NOT_FOUND;
		}
	}




	// if (retval == XLNX_OK)
	// {
	// 	retval = m_pDeviceInterface->TopologyIndexIsHostBank(m_readBufferMemTopologyIndex, &bHostBank);
	// 	if (retval == XLNX_OK)
	// 	{
	// 		if (bHostBank)
	// 		{
	// 			//NOTE - we are assuming both read and write buffers are connected to the same bank...
	// 			//      i.e. either both are connected to a CARD RAM bank, or both are connected to HOST BANK
	// 			m_hostBankMemoryTopologyIndex = m_readBufferMemTopologyIndex;
	// 			m_bUsingHostBank = true;
	// 		}
	// 	}
	// }



    // and the write buffer
	if (retval == XLNX_OK)
	{
		retval = m_pDeviceInterface->GetArgumentMemTopologyIndex(cuName, KERNEL_MEMORY_ARGUMENT_WRITE_BUFFER_INDEX, &m_writeBufferMemTopologyIndex);
		if(retval != XLNX_OK)
		{
			retval = S2MM_ERROR_CU_NAME_NOT_FOUND;
		}
	}




	// if (retval == XLNX_OK)
	// {
	// 	retval = m_pDeviceInterface->TopologyIndexIsHostBank(m_writeBufferMemTopologyIndex, &bHostBank);
	// 	if (retval == XLNX_OK)
	// 	{
	// 		if (bHostBank)
	// 		{
	// 			//NOTE - we are assuming both read and write buffers are connected to the same bank...
	// 			//      i.e. either both are connected to a CARD RAM bank, or both are connected to HOST BANK
	// 			m_hostBankMemoryTopologyIndex = m_writeBufferMemTopologyIndex;
	// 			m_bUsingHostBank = true;
	// 		}
	// 	}
	// }




	//NOTE - ordinarily we would set up the DMA buffer here
	//		 But to aid multi-process watch commands being run, we will
	//		 defer setting up the buffer until the first time the Sync method is called.




	if (retval == XLNX_OK)
	{
		m_initialisedMagicNumber = IS_INITIALISED_MAGIC_NUMBER;
	}





	// if (retval == XLNX_OK)
	// {
	// 	//NOTE - keep the following after the setting of the IS_INITIALISED magic number...
	// 	InitialiseRingIndexes();
	// }


	return retval;
}





uint32_t S2MM::Uninitialise(void)
{
	uint32_t retval = XLNX_OK;

	// retval = StopProcessingThread();

	if (retval == XLNX_OK)
	{
		retval = CleanupBufferObjects();
	}



	return retval;
}




uint32_t S2MM::CheckIsInitialised(void)
{
	uint32_t retval = XLNX_OK;

	if (m_initialisedMagicNumber != IS_INITIALISED_MAGIC_NUMBER)
	{
		retval = S2MM_ERROR_NOT_INITIALISED;
	}

	return retval;
}


void S2MM::IsInitialised(bool* pbIsInitialised)
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





uint32_t S2MM::GetCUIndex(uint32_t* pCUIndex)
{
	uint32_t retval = XLNX_OK;

	retval = CheckIsInitialised();

	if (retval == XLNX_OK)
	{
		*pCUIndex = m_cuIndex;
	}

	return retval;
}




uint32_t S2MM::GetCUAddress(uint64_t* pCUAddress)
{
	uint32_t retval = XLNX_OK;

	retval = CheckIsInitialised();

	if (retval == XLNX_OK)
	{
		*pCUAddress = m_cuAddress;
	}

	return retval;
}




uint32_t S2MM::GetReadBufferCUMemTopologyIndex(uint32_t* pMemTopologyIndex)
{
	uint32_t retval = XLNX_OK;

	retval = CheckIsInitialised();

	if (retval == XLNX_OK)
	{
		*pMemTopologyIndex = m_readBufferMemTopologyIndex;
	}

	return retval;
}




uint32_t S2MM::GetWriteBufferCUMemTopologyIndex(uint32_t* pMemTopologyIndex)
{
	uint32_t retval = XLNX_OK;

	retval = CheckIsInitialised();

	if (retval == XLNX_OK)
	{
		*pMemTopologyIndex = m_writeBufferMemTopologyIndex;
	}

	return retval;
}




// uint32_t S2MM::InitialiseRingIndexes(void)
// {
// 	uint32_t retval = XLNX_OK;
// 	uint32_t readHeadIndex;
// 	uint32_t readTailIndex;
// 	uint32_t writeHeadIndex;
// 	uint32_t writeTailIndex;

// 	retval = GetHWRingReadBufferIndexes(&readHeadIndex, &readTailIndex);

// 	if (retval == XLNX_OK)
// 	{
// 		retval = GetHWRingWriteBufferIndexes(&writeHeadIndex, &writeTailIndex);
// 	}


// 	if (retval == XLNX_OK)
// 	{
// 		m_lastReadHeadIndex = readHeadIndex;
// 		m_lastReadTailIndex = readTailIndex;

// 		m_lastWriteHeadIndex = writeHeadIndex;
// 		m_lastWriteTailIndex = writeTailIndex;
// 	}

// 	return retval;
// }




uint32_t S2MM::StartHWKernel(void)
{
	uint32_t retval = XLNX_OK;
	uint32_t value;
	uint32_t mask;

	const uint32_t AP_START		= (1 << 0);
	const uint32_t AP_CONTINUE	= (1 << 7);

	retval = CheckIsInitialised();



	if (retval == XLNX_OK)
	{
		retval = SetupBuffers();
	}


	if (retval == XLNX_OK)
	{
		//NOTE - This Kernel is NOT a free running kernel.  This means that it must be manually started running from the host.
		//		 To do this, we set the AP_START bit in the kernel control register...
		//
		//       A typical compute kernel will run once (i.e perform one iteration), then stop and wait until it is started
		//       again from the host.  However once we start this kernel, we want it to continue running without further intervention.
		//		 To achieve this, we also set the AP_CONTINUE bit.

		// value = AP_START | AP_CONTINUE;
		value = AP_START;
		mask = value;

		retval = WriteRegWithMask32(S2MM_KERNEL_CONTROL_OFFSET, value, mask);
	}

	return retval;
}


// uint32_t S2MM::IsHWKernelRunning(bool* pbIsRunning)
// {
// 	uint32_t retval = XLNX_OK;
// 	uint32_t value;

// 	retval = CheckIsInitialised();

// 	if (retval == XLNX_OK)
// 	{
// 		retval = ReadReg32(S2MM_KERNEL_CONTROL_OFFSET, &value);
// 	}

// 	if (retval == XLNX_OK)
// 	{
// 		if (value & 0x01)
// 		{
// 			*pbIsRunning = true;
// 		}
// 		else
// 		{
// 			*pbIsRunning = false;
// 		}
// 	}

// 	return retval;
// }


uint32_t S2MM::ReadReg32(uint64_t offset, uint32_t* value)
{
	uint32_t retval = XLNX_OK;
	uint64_t address;


	address = m_cuAddress + offset;

	retval = m_pDeviceInterface->ReadReg32(address, value);

	if (retval != XLNX_OK)
	{
		retval = S2MM_ERROR_IO_FAILED;
	}

	return retval;
}


uint32_t S2MM::WriteReg32(uint64_t offset, uint32_t value)
{
	uint32_t retval = XLNX_OK;
	uint64_t address;


	address = m_cuAddress + offset;

	retval = m_pDeviceInterface->WriteReg32(address, value);


	if (retval != XLNX_OK)
	{
		retval = S2MM_ERROR_IO_FAILED;
	}

	return retval;
}




uint32_t S2MM::WriteRegWithMask32(uint64_t offset, uint32_t value, uint32_t mask)
{
	uint32_t retval = XLNX_OK;
	uint64_t address;


	address = m_cuAddress + offset;

	retval = m_pDeviceInterface->WriteRegWithMask32(address, value, mask);

	if (retval != XLNX_OK)
	{
		retval = S2MM_ERROR_IO_FAILED;
	}

	return retval;
}



uint32_t S2MM::BlockReadReg32(uint64_t offset, uint32_t* buffer, uint32_t numWords)
{
	uint32_t retval = XLNX_OK;
	uint64_t address;


	address = m_cuAddress + offset;

	retval = m_pDeviceInterface->BlockReadReg32(address, buffer, numWords);


	if (retval != XLNX_OK)
	{
		retval = S2MM_ERROR_IO_FAILED;
	}

	return retval;
}





uint32_t S2MM::BlockReadMem32(uint64_t address, uint32_t* buffer, uint32_t numWords)
{
	uint32_t retval = XLNX_OK;

	retval = m_pDeviceInterface->BlockReadMem32(address, buffer, numWords);

	if (retval != XLNX_OK)
	{
		retval = S2MM_ERROR_IO_FAILED;
	}

	return retval;
}








uint32_t S2MM::SetupBufferObjects(void)
{
	uint32_t retval = XLNX_OK;

	// if (m_bUsingHostBank)
	// {
	// 	retval = SetupBufferObjectsFromHostBank();
	// }
	// else
	// {
	// 	retval = SetupBufferObjectsFromCardRAM();
	// }

	retval = SetupBufferObjectsFromCardRAM();

	return retval;
}





uint32_t S2MM::CleanupBufferObjects(void)
{
	uint32_t retval = XLNX_OK;

	// if (m_bUsingHostBank)
	// {
	// 	CleanupBufferObjectsFromHostBank();
	// }
	// else
	// {
	// 	CleanupBufferObjectsFromCardRAM();
	// }

	CleanupBufferObjectsFromCardRAM();

	return retval;
}










// uint32_t S2MM::SetupBufferObjectsFromHostBank(void)
// {
// 	uint32_t retval = XLNX_OK;

// 	uint32_t totalBytesToAllocate;
// 	uint64_t bufferHWAddress;

// 	uint32_t readBufferSize;
// 	uint32_t writeBufferSize;



// 	//When using the HOST BANK, XRT currently seems to treat it as one large buffer


// 	readBufferSize = RING_SIZE * READ_ELEMENT_SIZE;
// 	writeBufferSize = RING_SIZE * WRITE_ELEMENT_SIZE;

// 	totalBytesToAllocate = readBufferSize + writeBufferSize;


// 	m_pHostBankBufferDescriptor = m_pDeviceInterface->AllocateBufferHostOnly(totalBytesToAllocate, m_hostBankMemoryTopologyIndex);
// 	if (m_pHostBankBufferDescriptor == nullptr)
// 	{
// 		retval = S2MM_ERROR_FAILED_TO_ALLOCATE_BUFFER_OBJECT;
// 	}


// 	if (retval == XLNX_OK)
// 	{
// 		bufferHWAddress = m_pDeviceInterface->GetDeviceBufferAddress(m_pHostBankBufferDescriptor);
// 	}


// 	if (retval == XLNX_OK)
// 	{
// 		retval = m_pDeviceInterface->MapBufferToUserspace(m_pHostBankBufferDescriptor, DeviceInterface::BufferMapType::READ_WRITE, &m_hostBankBufferVirtualAddress);
// 		if (retval != XLNX_OK)
// 		{
// 			retval = S2MM_ERROR_FAILED_TO_MAP_BUFFER_OBJECT;
// 		}
// 	}

// 	//Set up the HOST VIRTUAL ADDRESSES...
// 	if (retval == XLNX_OK)
// 	{
// 		uint8_t* address = (uint8_t*)m_hostBankBufferVirtualAddress;

// 		m_readBufferHostVirtualAddress = address;
// 		address += readBufferSize;

// 		m_writeBufferHostVirtualAddress = address;
// 		address += writeBufferSize;
// 	}



// 	//Set up the HW ADDRESSES...
// 	if (retval == XLNX_OK)
// 	{
// 		uint64_t address = bufferHWAddress;

// 		m_readBufferHWAddress = address;
// 		address += readBufferSize;

// 		m_writeBufferHWAddress = address;
// 		address += writeBufferSize;
// 	}


// 	//Tell the kernel about the buffer addresses...
// 	if (retval == XLNX_OK)
// 	{
// 		retval = SetHWReadBufferAddress(m_readBufferHWAddress);
// 	}

// 	if (retval == XLNX_OK)
// 	{
// 		retval = SetHWWriteBufferAddress(m_writeBufferHWAddress);
// 	}

// 	return retval;
// }








uint32_t S2MM::SetupBufferObjectsFromCardRAM(void)
{
	uint32_t retval = XLNX_OK;

	// The HW kernel is connected to CARD RAM.  This means we need to allocate a BUFFER PAIR (i.e. HOST + CARD)
	std::cout << (int)m_readBufferMemTopologyIndex << "\n";
	m_pReadBufferDescriptor = m_pDeviceInterface->AllocateBufferPair(DATA_SIZE * READ_ELEMENT_SIZE, m_readBufferMemTopologyIndex);

	if (m_pReadBufferDescriptor == nullptr)
	{
		retval = S2MM_ERROR_FAILED_TO_ALLOCATE_BUFFER_OBJECT;
	}


	// if (retval == XLNX_OK)
	// {
	// 	m_pWriteBufferDescriptor = m_pDeviceInterface->AllocateBufferPair(DATA_SIZE * READ_ELEMENT_SIZE, m_writeBufferMemTopologyIndex);
	// 	if (m_pWriteBufferDescriptor == nullptr)
	// 	{
	// 		retval = S2MM_ERROR_FAILED_TO_ALLOCATE_BUFFER_OBJECT;
	// 	}
	// }




	//Retrieve the HW address of the buffers in CARD RAM
	// if (retval == XLNX_OK)
	// {      
	// 	m_readBufferHWAddress = m_pDeviceInterface->GetDeviceBufferAddress(m_pReadBufferDescriptor);
	// }


	if (retval == XLNX_OK)
	{
		m_writeBufferHWAddress = m_pDeviceInterface->GetDeviceBufferAddress(m_pWriteBufferDescriptor);
	}





	//Retrieve the virtual address of the HOST BUFFERS for host to READ the results
	if (retval == XLNX_OK)
	{
		retval = m_pDeviceInterface->MapBufferToUserspace(m_pReadBufferDescriptor, DeviceInterface::BufferMapType::READ_ONLY, &m_readBufferHostVirtualAddress);
		if (retval != XLNX_OK)
		{    
			retval = S2MM_ERROR_FAILED_TO_MAP_BUFFER_OBJECT;
		}
	}

	//Retrieve the virtual address of the HOST BUFFERS for host to write the inputs
	// if (retval == XLNX_OK)
	// {
	// 	retval = m_pDeviceInterface->MapBufferToUserspace(m_pWriteBufferDescriptor, DeviceInterface::BufferMapType::READ_ONLY, &m_writeBufferHostVirtualAddress);
	// 	if (retval != XLNX_OK)
	// 	{
	// 		retval = S2MM_ERROR_FAILED_TO_MAP_BUFFER_OBJECT;
	// 	}
	// }



	//Tell the kernel about the buffer addresses...
	// if (retval == XLNX_OK)
	// { 
	// 	retval = SetHWReadBufferAddress(m_readBufferHWAddress);
	// }

	// if (retval == XLNX_OK)
	// {
	// 	retval = SetHWWriteBufferAddress(m_writeBufferHWAddress);
	// }

	return retval;
}







// uint32_t S2MM::SetHWReadBufferAddress(uint64_t address)
// {
// 	uint32_t retval = XLNX_OK;
// 	uint32_t upperWord;
// 	uint32_t lowerWord;

// 	upperWord = (uint32_t)((address >> 32) & 0xFFFFFFFF);
// 	lowerWord = (uint32_t)(address & 0xFFFFFFFF);


// 	retval = WriteReg32(S2MM_READ_BUFFER_ADDRESS_LOWER_WORD_OFFSET, lowerWord);


// 	if (retval == XLNX_OK)
// 	{
// 		retval = WriteReg32(S2MM_READ_BUFFER_ADDRESS_UPPER_WORD_OFFSET, upperWord);
// 	}

// 	return retval;
// }






// uint32_t S2MM::SetHWWriteBufferAddress(uint64_t address)
// {
// 	uint32_t retval = XLNX_OK;
// 	uint32_t upperWord;
// 	uint32_t lowerWord;

// 	upperWord = (uint32_t)((address >> 32) & 0xFFFFFFFF);
// 	lowerWord = (uint32_t)(address & 0xFFFFFFFF);


// 	retval = WriteReg32(S2MM_WRITE_BUFFER_ADDRESS_LOWER_WORD_OFFSET, lowerWord);


// 	if (retval == XLNX_OK)
// 	{
// 		retval = WriteReg32(S2MM_WRITE_BUFFER_ADDRESS_UPPER_WORD_OFFSET, upperWord);
// 	}

// 	return retval;
// }



// uint32_t S2MM::CleanupBufferObjectsFromHostBank(void)
// {
// 	uint32_t retval = XLNX_OK;

// 	if (m_hostBankBufferVirtualAddress != nullptr)
// 	{
// 		m_pDeviceInterface->UnmapBufferFromUserspace(m_pHostBankBufferDescriptor, m_hostBankBufferVirtualAddress);
// 		m_hostBankBufferVirtualAddress = nullptr;
// 	}

// 	if (m_pHostBankBufferDescriptor != nullptr)
// 	{
// 		m_pDeviceInterface->FreeBufferHostOnly(m_pHostBankBufferDescriptor);
// 		m_pHostBankBufferDescriptor = nullptr;
// 	}

// 	m_readBufferHWAddress = INVALID_HW_BUFFER_ADDRESS;
// 	m_writeBufferHWAddress = INVALID_HW_BUFFER_ADDRESS;

// 	m_readBufferHostVirtualAddress = nullptr;
// 	m_writeBufferHostVirtualAddress = nullptr;

// 	return retval;
// }







uint32_t S2MM::CleanupBufferObjectsFromCardRAM(void)
{
	uint32_t retval = XLNX_OK;

	if (m_readBufferHostVirtualAddress != nullptr)
	{
		m_pDeviceInterface->UnmapBufferFromUserspace(m_pReadBufferDescriptor, m_readBufferHostVirtualAddress);
		m_readBufferHostVirtualAddress = nullptr;
	}


	if (m_writeBufferHostVirtualAddress != nullptr)
	{
		m_pDeviceInterface->UnmapBufferFromUserspace(m_pWriteBufferDescriptor, m_writeBufferHostVirtualAddress);
		m_writeBufferHostVirtualAddress = nullptr;
	}



	if (m_pReadBufferDescriptor != nullptr)
	{
		m_pDeviceInterface->FreeBufferPair(m_pReadBufferDescriptor);
		m_pReadBufferDescriptor = nullptr;
	}

	if (m_pWriteBufferDescriptor != nullptr)
	{
		m_pDeviceInterface->FreeBufferPair(m_pWriteBufferDescriptor);
		m_pWriteBufferDescriptor = nullptr;
	}

	m_readBufferHWAddress = INVALID_HW_BUFFER_ADDRESS;
	m_writeBufferHWAddress = INVALID_HW_BUFFER_ADDRESS;


	return retval;
}



uint32_t S2MM::SetupBuffers(void)
{
	uint32_t retval = XLNX_OK;

	if (m_bNeedToSetupBuffers)
	{
		retval = SetupBufferObjects();
		m_bNeedToSetupBuffers = false;
	}

	return retval;
}



void* S2MM::GetReadBufferHostVirtualAddress(void)
{
	return m_readBufferHostVirtualAddress;
}


uint64_t S2MM::GetReadBufferHWAddress(void)
{
	return m_readBufferHWAddress;
}


uint32_t S2MM::SyncReadBuffer(void){
	uint32_t retval = XLNX_OK;

	retval = m_pDeviceInterface->SyncBuffer(m_pReadBufferDescriptor, DeviceInterface::SyncDirection::FROM_DEVICE);
	
	if (retval != XLNX_OK)
	{
		retval = S2MM_ERROR_IO_FAILED;
	}


}


// uint32_t S2MM::SyncReadBuffer(void)
// {
// 	uint32_t retval = XLNX_OK;

// 	retval = CheckIsInitialised();

// 	if (retval == XLNX_OK)
// 	{
// 		//If this is the first time we are performing a sync, we need to set up our buffer object...
// 		SetupBuffersIfNecessary();
// 	}


// 	if (retval == XLNX_OK)
// 	{
// 		retval = SyncReadBufferInternal();
// 	}

// 	return retval;
// }




// uint32_t S2MM::SyncReadBufferInternal(void)
// {
// 	uint32_t retval = XLNX_OK;
// 	uint32_t currentHeadIndex;
// 	uint32_t currentTailIndex;
// 	uint32_t maxElementsPerSync = m_maxDMAChunkSize;
// 	uint32_t totalNewElements;
// 	uint32_t totalTransferredElements;
// 	uint32_t transferSizeInElements;
// 	uint32_t offset;
// 	uint32_t size;


// 	if (retval == XLNX_OK)
// 	{
// 		retval = GetHWRingReadBufferIndexes(&currentHeadIndex, &currentTailIndex);
// 	}


// 	if (retval == XLNX_OK)
// 	{
// 		//We only need to manually sync the buffer if we are using CARD RAM
// 		//If we are using the HOST BANK (i.e. the slave bridge), data is automatically sync'd
// 		if (m_bUsingHostBank == false)
// 		{

// 			if (currentTailIndex > m_lastReadTailIndex)
// 			{
// 				totalNewElements = currentTailIndex - m_lastReadTailIndex;
// 				totalTransferredElements = 0;

// 				while (totalTransferredElements < totalNewElements)
// 				{
// 					if (maxElementsPerSync == 0)
// 					{
// 						//transfer ALL new elements in one go...
// 						transferSizeInElements = totalNewElements;
// 					}
// 					else
// 					{
// 						//We want to limit each transfer to a maximum size...
// 						transferSizeInElements = totalNewElements - totalTransferredElements;

// 						if (transferSizeInElements > maxElementsPerSync)
// 						{
// 							transferSizeInElements = maxElementsPerSync;
// 						}
// 					}



// 					offset = (m_lastReadTailIndex + totalTransferredElements) * READ_ELEMENT_SIZE;
// 					size = transferSizeInElements * READ_ELEMENT_SIZE;

// 					DMAStatsLogTransfer(&m_dmaC2HStats, size);

// 					retval = m_pDeviceInterface->SyncBuffer(m_pReadBufferDescriptor, DeviceInterface::SyncDirection::FROM_DEVICE, size, offset);

// 					if (retval == XLNX_OK)
// 					{
// 						totalTransferredElements += transferSizeInElements;
// 					}
// 					else
// 					{
// 						retval = S2MM_ERROR_FAILED_TO_SYNC_BUFFER_OBJECT;
// 						break; //out of loop
// 					}
// 				}


// 			}
// 			else if (currentTailIndex < m_lastReadTailIndex)
// 			{
// 				//we will assume we have wrapped around...
// 				//this means we have to do two syncs...one of the latter part of the buffer and one of the early part of the buffer...


// 				totalNewElements = RING_SIZE - m_lastReadTailIndex;
// 				totalTransferredElements = 0;

// 				while (totalTransferredElements < totalNewElements)
// 				{
// 					if (maxElementsPerSync == 0)
// 					{
// 						//transfer ALL new elements in one go...
// 						transferSizeInElements = totalNewElements;
// 					}
// 					else
// 					{
// 						//We want to limit each transfer to a maximum size...
// 						transferSizeInElements = totalNewElements - totalTransferredElements;

// 						if (transferSizeInElements > maxElementsPerSync)
// 						{
// 							transferSizeInElements = maxElementsPerSync;
// 						}
// 					}



// 					DMAStatsLogWrapAround(&m_dmaC2HStats);

// 					offset = (m_lastReadTailIndex + totalTransferredElements) * READ_ELEMENT_SIZE;
// 					size = transferSizeInElements * READ_ELEMENT_SIZE;

// 					DMAStatsLogTransfer(&m_dmaC2HStats, size);

// 					retval = m_pDeviceInterface->SyncBuffer(m_pReadBufferDescriptor, DeviceInterface::SyncDirection::FROM_DEVICE, size, offset);
// 					if (retval == XLNX_OK)
// 					{
// 						totalTransferredElements += transferSizeInElements;
// 					}
// 					else
// 					{
// 						retval = S2MM_ERROR_FAILED_TO_SYNC_BUFFER_OBJECT;
// 						break; //out of loop
// 					}
// 				}





// 				//
// 				//Now for the early part of the buffer....
// 				//

// 				if (retval == XLNX_OK)
// 				{

// 					totalNewElements = currentTailIndex; //i.e. from index 0 to currentTailIndex
// 					totalTransferredElements = 0;

// 					while (totalTransferredElements < totalNewElements)
// 					{
// 						if (maxElementsPerSync == 0)
// 						{
// 							//transfer ALL new elements in one go...
// 							transferSizeInElements = totalNewElements;
// 						}
// 						else
// 						{
// 							//We want to limit each transfer to a maximum size...
// 							transferSizeInElements = totalNewElements - totalTransferredElements;

// 							if (transferSizeInElements > maxElementsPerSync)
// 							{
// 								transferSizeInElements = maxElementsPerSync;
// 							}
// 						}

// 						offset = 0 + totalTransferredElements;
// 						size = transferSizeInElements * READ_ELEMENT_SIZE;

// 						DMAStatsLogTransfer(&m_dmaC2HStats, size);

// 						if (size > 0)
// 						{
// 							retval = m_pDeviceInterface->SyncBuffer(m_pReadBufferDescriptor, DeviceInterface::SyncDirection::FROM_DEVICE, size, offset);
// 							if (retval == XLNX_OK)
// 							{
// 								totalTransferredElements += transferSizeInElements;
// 							}
// 							else
// 							{
// 								retval = S2MM_ERROR_FAILED_TO_SYNC_BUFFER_OBJECT;
// 								break; //out of loop
// 							}
// 						}
// 					}
// 				}
// 			}
// 		}

// 	}


// 	if (retval == XLNX_OK)
// 	{
// 		m_lastReadHeadIndex = currentHeadIndex;
// 		m_lastReadTailIndex = currentTailIndex;
// 	}

// 	return retval;
// }



// uint32_t S2MM::GetHWRingReadBufferIndexes(uint32_t* pHeadIndex, uint32_t* pTailIndex)
// {
// 	uint32_t retval = XLNX_OK;

// 	if (retval == XLNX_OK)
// 	{
// 		retval = ReadReg32(S2MM_RING_READ_BUFFER_HEAD_INDEX_OFFSET, pHeadIndex);
// 	}


// 	if (retval == XLNX_OK)
// 	{
// 		retval = ReadReg32(S2MM_RING_READ_BUFFER_TAIL_INDEX_OFFSET, pTailIndex);
// 	}

// 	return retval;
// }




// uint32_t S2MM::GetSWRingReadBufferIndexes(uint32_t* pHeadIndex, uint32_t* pTailIndex)
// {
// 	uint32_t retval = XLNX_OK;

// 	*pHeadIndex = m_lastReadHeadIndex;
// 	*pTailIndex = m_lastReadTailIndex;

// 	return retval;
// }






// uint32_t S2MM::SetHWRingReadHeadIndex(uint32_t headIndex)
// {
// 	uint32_t retval = XLNX_OK;

// 	retval = CheckIsInitialised();

// 	if (retval == XLNX_OK)
// 	{
// 		retval = WriteReg32(S2MM_RING_READ_BUFFER_HEAD_INDEX_OFFSET, headIndex);
// 	}

// 	return retval;
// }





// uint32_t S2MM::GetStats(Stats* pStats)
// {
// 	uint32_t retval = XLNX_OK;

// 	memset(pStats, 0, sizeof(Stats));

// 	retval = CheckIsInitialised();

// 	if (retval == XLNX_OK)
// 	{
// 		retval = ReadReg32(S2MM_TX_RESPONSE_INDEX_OFFSET, &(pStats->txResponseCount));
// 	}

// 	if (retval == XLNX_OK)
// 	{
// 		retval = ReadReg32(S2MM_NUM_RX_OP_OFFSET, &(pStats->rxOp));
// 	}

// 	return retval;
// }







// latency counters
// uint32_t S2MM::GetLatencyStats(uint32_t durationSeconds, double* max, double* min, double* sum, uint32_t* cnt, uint32_t* cyclesPre, uint32_t* cyclesPost)
// {
// 	uint32_t retval = XLNX_OK;
// 	uint32_t clockFrequencyArray[DeviceInterface::MAX_SUPPORTED_CLOCKS];
//     uint32_t clockFrequencyMHz;
// 	uint32_t numClocks;
// 	uint32_t multiplier;
//     uint32_t val;

// 	retval = CheckIsInitialised();

// 	if (retval == XLNX_OK)
// 	{
// 		retval = m_pDeviceInterface->GetClocks(clockFrequencyArray, &numClocks);
// 	}

//     if (retval == XLNX_OK)
//     {
//         clockFrequencyMHz = clockFrequencyArray[0]; //NOTE - assuming our clock is the first one in the list
//     }

// 	if (retval == XLNX_OK)
// 	{
// 		//The counters do not increment each clock cycle...so first we need to figure out a multiplication factor...
// 		retval = ReadReg32(S2MM_CYCLES_PRE_OFFSET, &val);
// 		multiplier = (uint32_t)((double)(durationSeconds * 1000000) / ((double)val / (double)clockFrequencyMHz) + 0.5);
// 	}




// 	if (retval == XLNX_OK)
// 	{
// 		retval = ReadReg32(S2MM_LATENCY_MIN_OFFSET, &val);
// 	    *min = (double)val * (double)multiplier / (double)clockFrequencyMHz;
// 	}

// 	if (retval == XLNX_OK)
// 	{
// 		retval = ReadReg32(S2MM_LATENCY_MAX_OFFSET, &val);
// 	    *max = (double)val * (double)multiplier / (double)clockFrequencyMHz;
// 	}

// 	if (retval == XLNX_OK)
// 	{
// 		retval = ReadReg32(S2MM_LATENCY_SUM_OFFSET, &val);
// 	    *sum = (double)val * (double)multiplier / (double)clockFrequencyMHz;
// 	}

// 	if (retval == XLNX_OK)
// 	{
// 		retval = ReadReg32(S2MM_LATENCY_CNT_OFFSET, cnt);
// 	}

// 	if (retval == XLNX_OK)
// 	{
// 		retval = ReadReg32(S2MM_CYCLES_PRE_OFFSET, &val);
// 		*cyclesPre = (uint32_t)(((double)val * (double)multiplier / (double)clockFrequencyMHz) + 0.5);
// 	}

// 	if (retval == XLNX_OK)
// 	{
// 		retval = ReadReg32(S2MM_CYCLES_POST_OFFSET, &val);
// 		*cyclesPost = (uint32_t)(((double)val * (double)multiplier / (double)clockFrequencyMHz) + 0.5);
// 	}

// 	return retval;
// }







// uint32_t S2MM::StartLatencyCounters(void)
// {
//     uint32_t retval = XLNX_OK;

// 	retval = CheckIsInitialised();


// 	/* toggle the bit to reset the counters */
// 	if (retval == XLNX_OK)
// 	{
// 		retval = WriteRegWithMask32(S2MM_CTRL_OFFSET, 0x04, 0x04);
// 	}

// 	if (retval == XLNX_OK)
// 	{
// 		//The following sleep is to cope with the HW needing a small delay to reset its counters...
// 		std::this_thread::sleep_for(std::chrono::milliseconds(5));
// 	}


// 	if (retval == XLNX_OK)
// 	{
// 		retval = WriteRegWithMask32(S2MM_CTRL_OFFSET, 0x00, 0x04);
// 	}



// 	/* enable the counters */
// 	if(retval == XLNX_OK)
// 	{
// 		retval = WriteRegWithMask32(S2MM_CTRL_OFFSET, 0x02, 0x02);
// 	}
// 	return 0;
// }







// uint32_t S2MM::StopLatencyCounters(void)
// {
// 	uint32_t retval = XLNX_OK;

// 	retval = CheckIsInitialised();

// 	if (retval == XLNX_OK)
// 	{
// 		retval = WriteRegWithMask32(S2MM_CTRL_OFFSET, 0x00, 0x02);
// 	}

// 	return retval;
// }





void* S2MM::GetWriteBufferHostVirtualAddress(void)
{
	return m_writeBufferHostVirtualAddress;
}






uint64_t S2MM::GetWriteBufferHWAddress(void)
{
	return m_writeBufferHWAddress;
}



uint32_t S2MM::SyncWriteBuffer(void)
{
	uint32_t retval = XLNX_OK;

	retval = m_pDeviceInterface->SyncBuffer(m_pWriteBufferDescriptor, DeviceInterface::SyncDirection::TO_DEVICE);
	
	if (retval != XLNX_OK)
	{
		retval = S2MM_ERROR_IO_FAILED;
	}

	return retval;
}

// uint32_t S2MM::SyncWriteBuffer(void)
// {
// 	uint32_t retval = XLNX_OK;

// 	retval = CheckIsInitialised();

// 	if (retval == XLNX_OK)
// 	{
// 		//If this is the first time we are performing a sync, we need to set up our buffer object...
// 		retval = SetupBuffersIfNecessary();
// 	}

// 	if (retval == XLNX_OK)
// 	{
// 		retval = SyncWriteBufferInternal();
// 	}

// 	return retval;
// }


// uint32_t S2MM::SyncWriteBufferInternal(void)
// {
// 	uint32_t retval = XLNX_OK;
// 	uint32_t currentSWHeadIndex;
// 	uint32_t currentSWTailIndex;
// 	uint32_t currentHWHeadIndex;
// 	uint32_t currentHWTailIndex;
// 	uint32_t maxElementsPerSync = m_maxDMAChunkSize;
// 	uint32_t totalNewElements;
// 	uint32_t totalTransferredElements;
// 	uint32_t transferSizeInElements;
// 	uint32_t offset;
// 	uint32_t size;




// 	if (retval == XLNX_OK)
// 	{
// 		retval = GetSWRingWriteBufferIndexes(&currentSWHeadIndex, &currentSWTailIndex);
// 	}

// 	if (retval == XLNX_OK)
// 	{
// 		retval = GetHWRingWriteBufferIndexes(&currentHWHeadIndex, &currentHWTailIndex);
// 	}




// 	if (retval == XLNX_OK)
// 	{
// 		//We only need to manually sync the buffer if we are using CARD RAM
// 		//If we are using the HOST BANK (i.e. the slave bridge), data is automatically sync'd
// 		if (m_bUsingHostBank == false)
// 		{
// 			if (currentSWTailIndex > currentHWTailIndex)
// 			{

// 				totalNewElements = currentSWTailIndex - currentHWTailIndex;
// 				totalTransferredElements = 0;

// 				while (totalTransferredElements < totalNewElements)
// 				{
// 					if (maxElementsPerSync == 0)
// 					{
// 						//transfer ALL new elements in one go...
// 						transferSizeInElements = totalNewElements;
// 					}
// 					else
// 					{
// 						//We want to limit each transfer to a maximum size...
// 						transferSizeInElements = totalNewElements - totalTransferredElements;

// 						if (transferSizeInElements > maxElementsPerSync)
// 						{
// 							transferSizeInElements = maxElementsPerSync;
// 						}
// 					}



// 					offset = (currentHWTailIndex + totalTransferredElements) * WRITE_ELEMENT_SIZE;
// 					size = transferSizeInElements * WRITE_ELEMENT_SIZE;

// 					DMAStatsLogTransfer(&m_dmaH2CStats, size);


// 					retval = m_pDeviceInterface->SyncBuffer(m_pWriteBufferDescriptor, DeviceInterface::SyncDirection::TO_DEVICE, size, offset);
// 					if (retval == XLNX_OK)
// 					{
// 						totalTransferredElements += transferSizeInElements;
// 					}
// 					else
// 					{
// 						retval = S2MM_ERROR_FAILED_TO_SYNC_BUFFER_OBJECT;
// 						break; //out of loop
// 					}
// 				}
// 			}
// 			else if (currentSWTailIndex < currentHWTailIndex)
// 			{
// 				//we will assume we have wrapped around...
// 				//this means we have to do two syncs...one of the latter part of the buffer and one of the early part of the buffer...



// 				totalNewElements = RING_SIZE - currentHWTailIndex;
// 				totalTransferredElements = 0;

// 				while (totalTransferredElements < totalNewElements)
// 				{
// 					if (maxElementsPerSync == 0)
// 					{
// 						//transfer ALL new elements in one go...
// 						transferSizeInElements = totalNewElements;
// 					}
// 					else
// 					{
// 						//We want to limit each transfer to a maximum size...
// 						transferSizeInElements = totalNewElements - totalTransferredElements;

// 						if (transferSizeInElements > maxElementsPerSync)
// 						{
// 							transferSizeInElements = maxElementsPerSync;
// 						}
// 					}



// 					DMAStatsLogWrapAround(&m_dmaH2CStats);

// 					offset = (currentHWTailIndex + totalTransferredElements) * WRITE_ELEMENT_SIZE;
// 					size = transferSizeInElements * WRITE_ELEMENT_SIZE;

// 					DMAStatsLogTransfer(&m_dmaH2CStats, size);

// 					retval = m_pDeviceInterface->SyncBuffer(m_pWriteBufferDescriptor, DeviceInterface::SyncDirection::TO_DEVICE, size, offset);
// 					if (retval == XLNX_OK)
// 					{
// 						totalTransferredElements += transferSizeInElements;
// 					}
// 					else
// 					{
// 						retval = S2MM_ERROR_FAILED_TO_SYNC_BUFFER_OBJECT;
// 						break; //out of loop
// 					}
// 				}




// 				//
// 				//Now for the early part of the buffer
// 				//

// 				if (retval == XLNX_OK)
// 				{
// 					totalNewElements = currentSWTailIndex; //i.e from index 0 to currentSWTailIndex
// 					totalTransferredElements = 0;

// 					while (totalTransferredElements < totalNewElements)
// 					{
// 						if (maxElementsPerSync == 0)
// 						{
// 							//transfer ALL new elements in one go...
// 							transferSizeInElements = totalNewElements;
// 						}
// 						else
// 						{
// 							//We want to limit each transfer to a maximum size...
// 							transferSizeInElements = totalNewElements - totalTransferredElements;

// 							if (transferSizeInElements > maxElementsPerSync)
// 							{
// 								transferSizeInElements = maxElementsPerSync;
// 							}
// 						}

// 						offset = 0 + totalTransferredElements;
// 						size = transferSizeInElements * WRITE_ELEMENT_SIZE;

// 						DMAStatsLogTransfer(&m_dmaH2CStats, size);

// 						if (size > 0)
// 						{
// 							retval = m_pDeviceInterface->SyncBuffer(m_pWriteBufferDescriptor, DeviceInterface::SyncDirection::TO_DEVICE, size, offset);
// 							if (retval == XLNX_OK)
// 							{
// 								totalTransferredElements += transferSizeInElements;
// 							}
// 							else
// 							{
// 								retval = S2MM_ERROR_FAILED_TO_SYNC_BUFFER_OBJECT;
// 								break; //out of loop
// 							}
// 						}

// 					}

// 				}
// 			}
// 		}

// 	}



//     if (retval == XLNX_OK)
//     {
// 		//Set the HW tail to match the SW tail...this will notify HW that new data is available...
//         retval = SetHWWriteTailIndex(currentSWTailIndex);
//     }



// 	return retval;
// }



// uint32_t S2MM::GetHWRingWriteBufferIndexes(uint32_t* pHeadIndex, uint32_t* pTailIndex)
// {
// 	uint32_t retval = XLNX_OK;

// 	if (retval == XLNX_OK)
// 	{
// 		retval = ReadReg32(S2MM_RING_WRITE_BUFFER_HEAD_INDEX_OFFSET, pHeadIndex);
// 	}


// 	if (retval == XLNX_OK)
// 	{
// 		retval = ReadReg32(S2MM_RING_WRITE_BUFFER_TAIL_INDEX_OFFSET, pTailIndex);
// 	}

// 	return retval;
// }




// uint32_t S2MM::GetSWRingWriteBufferIndexes(uint32_t* pHeadIndex, uint32_t* pTailIndex)
// {
// 	uint32_t retval = XLNX_OK;

// 	*pHeadIndex = m_lastWriteHeadIndex;
// 	*pTailIndex = m_lastWriteTailIndex;

// 	return retval;
// }




// void S2MM::SetSWWriteTailIndex(uint32_t tailIndex)
// {
// 	m_lastWriteTailIndex = tailIndex;
// }





// uint32_t S2MM::SetHWWriteTailIndex(uint32_t tailIndex)
// {
// 	uint32_t retval = XLNX_OK;

// 	retval = CheckIsInitialised();

// 	if (retval == XLNX_OK)
// 	{
// 		retval = WriteReg32(S2MM_RING_WRITE_BUFFER_TAIL_INDEX_OFFSET, tailIndex);
// 	}

// 	return retval;
// }



// uint32_t S2MM::SetThrottleRate(uint32_t throttleRateClockCycles)
// {
// 	uint32_t retval = XLNX_OK;

// 	retval = CheckIsInitialised();

// 	if (retval == XLNX_OK)
// 	{
// 		retval = WriteReg32(S2MM_THROTTLE_RATE_OFFSET, throttleRateClockCycles);
// 	}

// 	return retval;
// }







// uint32_t S2MM::GetThrottleRate(uint32_t* pThrottleRateClockCycles)
// {
// 	uint32_t retval = XLNX_OK;

// 	retval = CheckIsInitialised();

// 	if (retval == XLNX_OK)
// 	{
// 		retval = ReadReg32(S2MM_THROTTLE_RATE_OFFSET, pThrottleRateClockCycles);
// 	}

// 	return retval;
// }




// uint32_t S2MM::GetThrottleStats(ThrottleStats* pStats)
// {
// 	uint32_t retval = XLNX_OK;

// 	memset(pStats, 0, sizeof(ThrottleStats));


// 	retval = CheckIsInitialised();



// 	if (retval == XLNX_OK)
// 	{
// 		retval = ReadReg32(S2MM_THROTTLE_COUNT_OFFSET, &(pStats->throttleCounter));
// 	}

// 	if (retval == XLNX_OK)
// 	{
// 		retval = ReadReg32(S2MM_TROTTLE_EVENT_OFFSET, &(pStats->throttleEvents));
// 	}

// 	return retval;
// }



// void S2MM::GetDMAStats(DMAStats* pH2CStats, DMAStats* pC2HStats)
// {
// 	*pH2CStats = m_dmaH2CStats;
// 	*pC2HStats = m_dmaC2HStats;
// }



// void S2MM::ResetDMAStats(void)
// {
// 	memset(&m_dmaH2CStats, 0, sizeof(m_dmaH2CStats));
// 	memset(&m_dmaC2HStats, 0, sizeof(m_dmaC2HStats));
// }



// void S2MM::DMAStatsLogWrapAround(DMAStats* pStats)
// {
// 	pStats->bufferWrapArounds++;
// }


// void S2MM::DMAStatsLogTransfer(DMAStats* pStats, uint32_t size)
// {
// 	pStats->totalSyncOperations++;
// 	pStats->totalBytesTransferred += size;

// 	if (size > pStats->transferHighTide)
// 	{
// 		pStats->transferHighTide = size;
// 	}
// }





// void S2MM::SetDMAChunkSize(uint32_t numElements)
// {
// 	m_maxDMAChunkSize = numElements;
// }



// void S2MM::GetDMAChunkSize(uint32_t* pNumElements)
// {
// 	*pNumElements = m_maxDMAChunkSize;
// }



// void S2MM::SetHWEmulationPollDelay(uint32_t delaySeconds)
// {
// 	m_hwEmulationPollDelaySeconds = delaySeconds;
// }


// void S2MM::GetHWEmulationPollDelay(uint32_t* pDelaySeconds)
// {
// 	*pDelaySeconds = m_hwEmulationPollDelaySeconds;
// }