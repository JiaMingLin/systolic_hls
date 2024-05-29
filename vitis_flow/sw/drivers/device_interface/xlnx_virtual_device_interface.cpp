/*
 * Copyright 2021 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string.h> //for memset..

#include "xlnx_virtual_device_interface.h"

using namespace XLNX;



#ifndef XLNX_UNUSED_ARG
#define XLNX_UNUSED_ARG(x)		(x = x)
#endif



static const uint32_t VIRTUAL_DEVICE_BUFFER_DESCRIPTOR_MAGIC_NUMBER = 0x4BA782CF;











VirtualDeviceInterface::VirtualDeviceInterface()
{
	m_numCUs = 0;

	//set up some dummy MAC addresses...
	for (uint32_t i = 0; i < MACAddresses::NUM_SUPPORTED_MAC_ADDRESSES; i++)
	{
		for (uint32_t j = 0; j < MACAddresses::NUM_MAC_ADDRESS_BYTES; j++)
		{
			m_macAddresses.addr[i][j] = (uint8_t)((i << 4) | i);
		}

		m_macAddresses.isValid[i] = true;
	}


	//Create a dummy UUID...
	memset(&m_xuid, 0x01, sizeof(m_xuid));
	
}

VirtualDeviceInterface::~VirtualDeviceInterface()
{

}




char* VirtualDeviceInterface::GetDeviceName(void)
{
	return (char*)"Virtual Device";
}



void VirtualDeviceInterface::GetPCIDBDF(uint16_t* pDomain, uint8_t* pBus, uint8_t* pDevice, uint8_t* pFunction)
{
	*pDomain	= 0;
	*pBus		= 0;
	*pDevice	= 0;
	*pFunction	= 0;
}



MACAddresses* VirtualDeviceInterface::GetMACAddresses(void)
{
	return &m_macAddresses;
}


void VirtualDeviceInterface::Close(void)
{
	//TODO
	//TODO
	//TODO
}



uint32_t VirtualDeviceInterface::ReadReg32(uint64_t address, uint32_t* value)
{
	return InternalRead32(m_regMap, address, value);
}




uint32_t VirtualDeviceInterface::WriteReg32(uint64_t address, uint32_t value)
{
	return InternalWrite32(m_regMap, address, value);
}






uint32_t VirtualDeviceInterface::WriteRegWithMask32(uint64_t address, uint32_t value, uint32_t mask)
{
	uint32_t retval = XLNX_OK;
	uint32_t regValue;

	retval = InternalRead32(m_regMap, address, &regValue);
	
	if (retval == XLNX_OK)
	{
		regValue = regValue & ~mask;
		value = value & mask;

		regValue = regValue | value;

		retval = InternalWrite32(m_regMap, address, regValue);
	}

	return retval;
}




uint32_t VirtualDeviceInterface::BlockReadReg32(uint64_t address, uint32_t* buffer, uint32_t numWords)
{
	uint32_t retval = XLNX_OK;


	for (uint64_t i = 0; i < numWords; i++)
	{
		retval = InternalRead32(m_regMap, (address + (i * 4)), &buffer[i]);

		if (retval != XLNX_OK)
		{
			break; //out of loop 
		}
	}

	return retval;
}







uint32_t VirtualDeviceInterface::BlockWriteReg32(uint64_t address, uint32_t* buffer, uint32_t numWords)
{
	uint32_t retval = XLNX_OK;


	for (uint64_t i = 0; i < numWords; i++)
	{
		retval = InternalWrite32(m_regMap, (address + (i * 4)), buffer[i]);

		if (retval != XLNX_OK)
		{
			break; //out of loop 
		}
	}

	return retval;
}














uint32_t VirtualDeviceInterface::ReadMem32(uint64_t address, uint32_t* value)
{
	return InternalRead32(m_memMap, address, value);
}




uint32_t VirtualDeviceInterface::WriteMem32(uint64_t address, uint32_t value)
{
	return InternalWrite32(m_memMap, address, value);

}




uint32_t VirtualDeviceInterface::WriteMemWithMask32(uint64_t address, uint32_t value, uint32_t mask)
{
	uint32_t retval = XLNX_OK;
	uint32_t regValue;

	retval = InternalRead32(m_memMap, address, &regValue);

	if (retval == XLNX_OK)
	{
		regValue = regValue & ~mask;
		value = value & mask;

		regValue = regValue | value;

		retval = InternalWrite32(m_memMap, address, regValue);
	}

	return retval;
}







uint32_t VirtualDeviceInterface::BlockReadMem32(uint64_t address, uint32_t* buffer, uint32_t numWords)
{
	uint32_t retval = XLNX_OK;


	for (uint64_t i = 0; i < numWords; i++)
	{
		retval = InternalRead32(m_memMap, (address + (i * 4)), &buffer[i]);

		if (retval != XLNX_OK)
		{
			break; //out of loop 
		}
	}

	return retval;
}





uint32_t VirtualDeviceInterface::BlockWriteMem32(uint64_t address, uint32_t* buffer, uint32_t numWords)
{
	uint32_t retval = XLNX_OK;


	for (uint64_t i = 0; i < numWords; i++)
	{
		retval = InternalWrite32(m_memMap, (address + (i * 4)), buffer[i]);

		if (retval != XLNX_OK)
		{
			break; //out of loop 
		}
	}

	return retval;
}






uint32_t VirtualDeviceInterface::DownloadBitstream(char* filePath)
{
	XLNX_UNUSED_ARG(filePath);

	//TODO
	//TODO
	//TODO



	return XLNX_OK;
}










uint32_t VirtualDeviceInterface::InternalRead32(std::map<uint64_t, uint32_t>& map, uint64_t address, uint32_t* value)
{
	std::map<uint64_t, uint32_t>::iterator it;

	it = map.find(address);

	if (it != map.end())
	{
		*value = it->second;
	}
	else
	{
		*value = 0;
	}

	return XLNX_OK;
}






uint32_t VirtualDeviceInterface::InternalWrite32(std::map<uint64_t, uint32_t>& map, uint64_t address, uint32_t value)
{
	std::map<uint64_t, uint32_t>::iterator it;

	it = map.find(address);

	if (it != map.end()) //if the address already existed in the map...
	{
		//just change the associated value...
		it->second = value;
	}
	else
	{
		//otherwise insert a brand new element with the associated value...
		map.insert(std::make_pair(address, value));
	}


	return XLNX_OK;
}






uint32_t VirtualDeviceInterface::GetArgumentMemTopologyIndex(const char* cuName, uint32_t cuArgIndex, uint32_t* pTopologyIndex)
{
	uint32_t retval = XLNX_OK;

	XLNX_UNUSED_ARG(cuName);
	XLNX_UNUSED_ARG(cuArgIndex);

	*pTopologyIndex = 0;


	return retval;
}









uint32_t VirtualDeviceInterface::TopologyIndexIsHostBank(uint32_t topologyIndex, bool* pbIsHostBank)
{

	uint32_t retval = XLNX_OK;

	XLNX_UNUSED_ARG(topologyIndex);

	*pbIsHostBank = false;

	return retval;
}







BufferDescriptor* VirtualDeviceInterface::AllocateBufferPair(uint32_t sizeInBytes, uint32_t bankInfo)
{
	VirtualDeviceBufferDescriptor* pVirtualDescriptor = nullptr;
	
	XLNX_UNUSED_ARG(bankInfo);
	
	pVirtualDescriptor = new VirtualDeviceBufferDescriptor();

	if (pVirtualDescriptor != nullptr)
	{
		pVirtualDescriptor->m_deviceAddress = new uint8_t[sizeInBytes];
		pVirtualDescriptor->m_sizeInBytes = sizeInBytes;
		pVirtualDescriptor->m_magicNumber = VIRTUAL_DEVICE_BUFFER_DESCRIPTOR_MAGIC_NUMBER;
	}
	
	return (BufferDescriptor*)pVirtualDescriptor;
}










void VirtualDeviceInterface::FreeBufferPair(BufferDescriptor* pDescriptor)
{
	VirtualDeviceBufferDescriptor* pVirtualDescriptor = (VirtualDeviceBufferDescriptor*)pDescriptor;

	delete[] (pVirtualDescriptor->m_deviceAddress);
	pVirtualDescriptor->m_deviceAddress = nullptr;

	pVirtualDescriptor->m_sizeInBytes = 0;
	pVirtualDescriptor->m_magicNumber = 0;

	delete (pVirtualDescriptor);
}




uint64_t VirtualDeviceInterface::GetDeviceBufferAddress(BufferDescriptor* pDescriptor)
{
	VirtualDeviceBufferDescriptor* pVirtualDescriptor = (VirtualDeviceBufferDescriptor*)pDescriptor;
	uint64_t address;

	address = (uint64_t) (pVirtualDescriptor->m_deviceAddress);

	return address;
}




uint32_t VirtualDeviceInterface::MapBufferToUserspace(BufferDescriptor* pDescriptor, BufferMapType mapType, void** ppMapped)
{
	VirtualDeviceBufferDescriptor* pVirtualDescriptor = (VirtualDeviceBufferDescriptor*)pDescriptor;

	XLNX_UNUSED_ARG(mapType);

	*ppMapped = pVirtualDescriptor->m_deviceAddress;

	return XLNX_OK;
}


uint32_t VirtualDeviceInterface::UnmapBufferFromUserspace(BufferDescriptor* pDescriptor, void* pMapped)
{

	XLNX_UNUSED_ARG(pDescriptor);
	XLNX_UNUSED_ARG(pMapped);

	return XLNX_OK;
}




uint32_t VirtualDeviceInterface::SyncBuffer(BufferDescriptor* pDescriptor, SyncDirection direction)
{
	XLNX_UNUSED_ARG(pDescriptor);
	XLNX_UNUSED_ARG(direction);

	return XLNX_OK;
}



uint32_t VirtualDeviceInterface::SyncBuffer(BufferDescriptor* pDescriptor, SyncDirection direction, uint32_t size, uint32_t offset)
{
	XLNX_UNUSED_ARG(pDescriptor);
	XLNX_UNUSED_ARG(direction);

	XLNX_UNUSED_ARG(size);
	XLNX_UNUSED_ARG(offset);

	return XLNX_OK;
}






uint32_t VirtualDeviceInterface::GetBitstreamSectionInfo(void* info, size_t* size, enum axlf_section_kind kind, int index)
{
	uint32_t retval;

	XLNX_UNUSED_ARG(info);
	XLNX_UNUSED_ARG(kind);
	XLNX_UNUSED_ARG(index);

	*size = 0;

	retval = XLNX_DEV_INTERFACE_ERROR_NOT_SUPPORTED;

	return retval;
}







uint32_t VirtualDeviceInterface::GetCUAddress(const char* cuName, uint64_t* address)
{
	uint32_t retval = XLNX_OK;
	uint32_t index = 0;
	std::map<const char*, uint32_t>::iterator it;



	if (retval == XLNX_OK)
	{
		retval = InternalAddNewCUIfNecessary(cuName);
	}


	if (retval == XLNX_OK)
	{
		retval = InternalGetCUIndex(cuName, &index);
	}


	if(retval == XLNX_OK)
	{ 
		retval = InternalGetCUBaseAddress(index, address);
	}

	return retval;
}







uint32_t VirtualDeviceInterface::GetCUIndex(const char* cuName, uint32_t* cuIndex)
{
	uint32_t retval = XLNX_OK;
	std::map<const char*, uint32_t>::iterator it;



	if (retval == XLNX_OK)
	{

		retval = InternalAddNewCUIfNecessary(cuName);
	}


	if(retval == XLNX_OK)
	{
		retval = InternalGetCUIndex(cuName, cuIndex);
	}

	return retval;
}




uint32_t VirtualDeviceInterface::InternalAddNewCUIfNecessary(const char* cuName)
{
	uint32_t retval = XLNX_OK;
	uint32_t cuIndex;
	uint64_t cuAddress;
	std::map<const char*, uint32_t>::iterator it;


	//here we will try to retrieve the cuIndex for the given cuName.
	//if this fails...we know the cuName is not present...so we need to add it
	

	if (retval == XLNX_OK)
	{
		retval = InternalGetCUIndex(cuName, &cuIndex);
	}


	if (retval == XLNX_DEV_INTERFACE_ERROR_CU_NAME_NOT_FOUND)
	{
		cuIndex = m_numCUs;
		m_numCUs++;

		cuAddress = CU_START_ADDRESS + (CU_OFFSET * cuIndex);

		//insert brand new elements with the associated value...
		m_cuNameToIndexMap.insert(std::make_pair(cuName, cuIndex));
		m_cuIndexToAddressMap.insert(std::make_pair(cuIndex, cuAddress));

		//update our return code to indicate everything is OK...
		retval = XLNX_OK;		
	}
		

	return retval;
}




uint32_t VirtualDeviceInterface::InternalGetCUIndex(const char* cuName, uint32_t* index)
{
	uint32_t retval = XLNX_OK;
	std::map<const char*, uint32_t>::iterator it;


	if (retval == XLNX_OK)
	{
		it = m_cuNameToIndexMap.find(cuName);

		//if the index already existed in the map...
		if (it != m_cuNameToIndexMap.end())
		{
			*index = it->second;
		}
		else
		{
			retval = XLNX_DEV_INTERFACE_ERROR_CU_NAME_NOT_FOUND;
		}
	}

	return retval;
}






uint32_t VirtualDeviceInterface::InternalGetCUBaseAddress(uint32_t cuIndex, uint64_t* baseAddress)
{
	uint32_t retval = XLNX_OK;
	std::map<uint32_t, uint64_t>::iterator it;



	if (retval == XLNX_OK)
	{
		it = m_cuIndexToAddressMap.find(cuIndex);

		//if the index already existed in the map...
		if (it != m_cuIndexToAddressMap.end())
		{
			*baseAddress = it->second;
		}
		else
		{
			retval = XLNX_DEV_INTERFACE_ERROR_CU_INDEX_NOT_FOUND;
		}
	}

	return retval;
}







uint32_t VirtualDeviceInterface::ReadCUReg32(uint32_t cuIndex, uint64_t offset, uint32_t* value)
{
	uint32_t retval = XLNX_OK;
	uint64_t cuBaseAddress = 0;


	if (retval == XLNX_OK)
	{
		retval = InternalGetCUBaseAddress(cuIndex, &cuBaseAddress);
	}

	if (retval == XLNX_OK)
	{
		retval = InternalRead32(m_regMap, cuBaseAddress + offset, value);
	}

	return retval;
}







uint32_t VirtualDeviceInterface::WriteCUReg32(uint32_t cuIndex, uint64_t offset, uint32_t value)
{
	uint32_t retval = XLNX_OK;
	uint64_t cuBaseAddress = 0;


	if (retval == XLNX_OK)
	{
		retval = InternalGetCUBaseAddress(cuIndex, &cuBaseAddress);
	}

	if (retval == XLNX_OK)
	{
		retval = InternalWrite32(m_regMap, cuBaseAddress + offset, value);
	}

	return retval;
}









uint32_t VirtualDeviceInterface::WriteCURegWithMask32(uint32_t cuIndex, uint64_t offset, uint32_t value, uint32_t mask)
{
	uint32_t retval = XLNX_OK;
	uint64_t cuBaseAddress = 0;
	uint32_t regValue;

	if (retval == XLNX_OK)
	{
		retval = InternalGetCUBaseAddress(cuIndex, &cuBaseAddress);
	}

	if (retval == XLNX_OK)
	{
		retval = InternalRead32(m_regMap, cuBaseAddress + offset, &regValue);
	}
	
	if (retval == XLNX_OK)
	{
		regValue = regValue & ~mask;
		value = value & mask;

		regValue = regValue | value;

		retval = InternalWrite32(m_regMap, cuBaseAddress + offset, regValue);
	}

	return retval;
}





uint32_t VirtualDeviceInterface::BlockReadCUReg32(uint32_t cuIndex, uint64_t offset, uint32_t* buffer, uint32_t numWords)
{
	uint32_t retval = XLNX_OK;
	uint64_t cuBaseAddress = 0;


	if (retval == XLNX_OK)
	{
		retval = InternalGetCUBaseAddress(cuIndex, &cuBaseAddress);
	}

	if (retval == XLNX_OK)
	{
		for (uint64_t i = 0; i < numWords; i++)
		{
			retval = InternalRead32(m_regMap, (cuBaseAddress + offset + (i * 4)), &buffer[i]);

			if (retval != XLNX_OK)
			{
				break; //out of loop 
			}
		}
	}


	return retval;
}






uint32_t VirtualDeviceInterface::BlockWriteCUReg32(uint32_t cuIndex, uint64_t offset, uint32_t* buffer, uint32_t numWords)
{
	uint32_t retval = XLNX_OK;
	uint64_t cuBaseAddress = 0;

	if (retval == XLNX_OK)
	{
		retval = InternalGetCUBaseAddress(cuIndex, &cuBaseAddress);
	}
	
	if (retval == XLNX_OK)
	{
		for (uint64_t i = 0; i < numWords; i++)
		{
			retval = InternalWrite32(m_regMap, (cuBaseAddress + offset + (i * 4)), buffer[i]);

			if (retval != XLNX_OK)
			{
				break; //out of loop 
			}
		}
	}

	return retval;
}





// uint32_t VirtualDeviceInterface::OpenStream(const char* cuName, uint32_t cuStreamArgIndex, StreamDirection direction, StreamHandleType* streamHandle)
// {
// 	uint32_t retval = XLNX_OK;

// 	XLNX_UNUSED_ARG(cuName);
// 	XLNX_UNUSED_ARG(cuStreamArgIndex);
// 	XLNX_UNUSED_ARG(direction);

// 	*streamHandle = 0;

// 	//TODO
// 	//TODO
// 	//TODO



// 	return retval;
// }



// uint32_t VirtualDeviceInterface::CloseStream(StreamHandleType streamHandle)
// {
// 	uint32_t retval = XLNX_OK;

// 	XLNX_UNUSED_ARG(streamHandle);

// 	//TODO
// 	//TODO
// 	//TODO

// 	return retval;
// }


// uint32_t VirtualDeviceInterface::WriteToStream(StreamHandleType streamHandle, void* dataBuffer, uint32_t numBytesToWrite, uint32_t* numBytesWritten)
// {
// 	uint32_t retval = XLNX_OK;

// 	XLNX_UNUSED_ARG(streamHandle);
// 	XLNX_UNUSED_ARG(dataBuffer);
//     *numBytesWritten = numBytesToWrite;

// 	//TODO
// 	//TODO
// 	//TODO

// 	return retval;
// }




// uint32_t VirtualDeviceInterface::ReadFromStreamBlocking(StreamHandleType streamHandle, void* dataBuffer, uint32_t numBytesToRead, uint32_t* numBytesRead)
// {
// 	uint32_t retval = XLNX_OK;

// 	XLNX_UNUSED_ARG(streamHandle);
// 	XLNX_UNUSED_ARG(dataBuffer);
//     *numBytesRead = numBytesToRead;

// 	//TODO
// 	//TODO
// 	//TODO

// 	return retval;
// }

// uint32_t VirtualDeviceInterface::ReadFromStreamNonBlocking(StreamHandleType streamHandle, void* dataBuffer, uint32_t numBytesToRead, uint32_t* numBytesRead)
// {
// 	uint32_t retval = XLNX_OK;

// 	XLNX_UNUSED_ARG(streamHandle);
// 	XLNX_UNUSED_ARG(dataBuffer);
//     *numBytesRead = numBytesToRead;

// 	//TODO
// 	//TODO
// 	//TODO

// 	return retval;
// }

// uint32_t VirtualDeviceInterface::PollReadStream(StreamHandleType streamHandle, uint32_t* numBytesRead, int timeout)
// {
// 	uint32_t retval = XLNX_OK;

// 	XLNX_UNUSED_ARG(streamHandle);
// 	XLNX_UNUSED_ARG(timeout);
//     *numBytesRead = 0;

// 	//TODO
// 	//TODO
// 	//TODO

// 	return retval;
// }






// uint32_t VirtualDeviceInterface::AllocateStreamBuffer(uint64_t bufferSize, StreamBufferHandleType* pBufferHandle, void** pBufferAddress)
// {
// 	uint32_t retval = XLNX_OK;

// 	XLNX_UNUSED_ARG(bufferSize);
// 	XLNX_UNUSED_ARG(pBufferHandle);
// 	XLNX_UNUSED_ARG(pBufferAddress);

// 	return retval;
// }




// uint32_t VirtualDeviceInterface::DeallocateStreamBuffer(StreamBufferHandleType bufferHandle)
// {
// 	uint32_t retval = XLNX_OK;

// 	XLNX_UNUSED_ARG(bufferHandle);

// 	return retval;
// }




uint32_t VirtualDeviceInterface::GetClocks(uint32_t frequencyMHz[MAX_SUPPORTED_CLOCKS], uint32_t* pNumClocks)
{
	uint32_t retval = XLNX_OK;

	for (uint32_t i = 0; i < MAX_SUPPORTED_CLOCKS; i++)
	{
		frequencyMHz[i] = 300;
	}

	*pNumClocks = MAX_SUPPORTED_CLOCKS;

	return retval;
}




xuid_t* VirtualDeviceInterface::GetBitstreamUUID(void)
{
	return &m_xuid;
}



uint32_t VirtualDeviceInterface::ConvertAddressToCUIndexOffset(uint64_t address, uint32_t* cuIndex, uint64_t* offset)
{
	uint32_t retval = XLNX_OK;
	std::map<uint32_t, uint64_t>::iterator it;
	uint64_t kernelBaseAddress;
	bool bFound = false;

	for (it = m_cuIndexToAddressMap.begin(); it != m_cuIndexToAddressMap.end(); it++)
	{
		kernelBaseAddress = it->second;

		if ((address >= kernelBaseAddress) && (address < (kernelBaseAddress + MAX_SUPPORTED_CU_SIZE)))
		{
			*cuIndex = it->first;
			*offset = address - kernelBaseAddress;

			bFound = true;
			break; //out of loop
		}
	}


	if (bFound == false)
	{
		retval = XLNX_DEV_INTERFACE_ERROR_CU_INDEX_NOT_FOUND;
	}


	return retval;
}




BufferDescriptor* VirtualDeviceInterface::AllocateBufferHostOnly(uint32_t sizeInBytes, uint32_t bankInfo)
{
	VirtualDeviceBufferDescriptor* pVirtualDescriptor = nullptr;

	XLNX_UNUSED_ARG(bankInfo);

	pVirtualDescriptor = new VirtualDeviceBufferDescriptor();

	if (pVirtualDescriptor != nullptr)
	{
		pVirtualDescriptor->m_deviceAddress = new uint8_t[sizeInBytes];
		pVirtualDescriptor->m_sizeInBytes = sizeInBytes;
		pVirtualDescriptor->m_magicNumber = VIRTUAL_DEVICE_BUFFER_DESCRIPTOR_MAGIC_NUMBER;
	}

	return (VirtualDeviceBufferDescriptor*)pVirtualDescriptor;
}




void VirtualDeviceInterface::FreeBufferHostOnly(BufferDescriptor* pDescriptor)
{
	VirtualDeviceBufferDescriptor* pVirtualDescriptor = (VirtualDeviceBufferDescriptor*)pDescriptor;

	delete[](pVirtualDescriptor->m_deviceAddress);
	pVirtualDescriptor->m_deviceAddress = nullptr;

	pVirtualDescriptor->m_sizeInBytes = 0;
	pVirtualDescriptor->m_magicNumber = 0;

	delete (pVirtualDescriptor);
}





