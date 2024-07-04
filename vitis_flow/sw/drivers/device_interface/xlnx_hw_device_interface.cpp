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

#include <cinttypes>
#include <cstring>

#include "xstream.h"

#include "xlnx_hw_device_interface.h"


using namespace XLNX;

#define ADDRESS_SPACE	XCL_ADDR_KERNEL_CTRL







#ifndef XLNX_UNUSED_ARG
#define XLNX_UNUSED_ARG(x)	((void)x)
#endif







static const char* HOST_BANK_TAG = "HOST";








HWDeviceInterface::HWDeviceInterface(xclDeviceHandle deviceHandle)
{


	m_deviceHandle = deviceHandle;
	m_bSharedAccess = false;

	xclGetDeviceInfo2(m_deviceHandle, &m_deviceInfo);

	ReadMACAddresses();

	xrtXclbinUUID(m_deviceHandle, m_xuid);

	m_xclbinBuffer = nullptr;
	m_xclbinBufferLength = 0;

}




HWDeviceInterface::~HWDeviceInterface()
{
	if (m_xclbinBuffer != nullptr)
	{
		delete[] m_xclbinBuffer;
		m_xclbinBuffer = nullptr;

		m_xclbinBufferLength = 0;
	}

}






char* HWDeviceInterface::GetDeviceName(void)
{
	return m_deviceInfo.mName;
}



void HWDeviceInterface::GetPCIDBDF(uint16_t* pDomain, uint8_t* pBus, uint8_t* pDevice, uint8_t* pFunction)
{
	*pDomain	= (m_deviceInfo.mPciSlot >> 16) & 0xFFFF;
	*pBus		= (m_deviceInfo.mPciSlot >> 8) & 0xFF;
	*pDevice	= (m_deviceInfo.mPciSlot >> 3) & 0x1F;
	*pFunction	= (m_deviceInfo.mPciSlot >> 0) & 0x07;
}



MACAddresses* HWDeviceInterface::GetMACAddresses(void)
{
	return &m_macAddresses;
}




xuid_t* HWDeviceInterface::GetBitstreamUUID(void)
{
	return &m_xuid;
}




void HWDeviceInterface::Close(void)
{
	xclClose(m_deviceHandle);
	m_deviceHandle = nullptr;
}







uint32_t HWDeviceInterface::ReadReg32(uint64_t address, uint32_t* value)
{
	uint32_t retval = XLNX_OK;

	xclRead(m_deviceHandle, ADDRESS_SPACE, address, value, sizeof(uint32_t));

	return retval;
}









uint32_t HWDeviceInterface::WriteReg32(uint64_t address, uint32_t value)
{
	uint32_t retval = XLNX_OK;

	xclWrite(m_deviceHandle, ADDRESS_SPACE, address, &value, sizeof(uint32_t));

	return retval;
}






uint32_t HWDeviceInterface::WriteRegWithMask32(uint64_t address, uint32_t value, uint32_t mask)
{
	uint32_t retval = XLNX_OK;
	uint32_t regValue;

	retval = ReadReg32(address, &regValue);

	if (retval == XLNX_OK)
	{
		regValue = regValue & ~mask;
		value = value & mask;

		regValue = regValue | value;

		retval = WriteReg32(address, regValue);
	}

	return retval;
}





uint32_t HWDeviceInterface::BlockReadReg32(uint64_t address, uint32_t* buffer, uint32_t numWords)
{
	uint32_t retval = XLNX_OK;

	xclRead(m_deviceHandle, ADDRESS_SPACE, address, buffer, numWords * sizeof(uint32_t));

	return retval;
}






uint32_t HWDeviceInterface::BlockWriteReg32(uint64_t address, uint32_t* buffer, uint32_t numWords)
{
	uint32_t retval = XLNX_OK;

	xclWrite(m_deviceHandle, ADDRESS_SPACE, address, buffer, numWords * sizeof(uint32_t));

	return retval;
}








uint32_t  HWDeviceInterface::DownloadBitstream(char* filePath)
{
	uint32_t retval = XLNX_OK;

	int localretval = 0;

	FILE* pFile = nullptr;
	unsigned long fileLength = 0;


	bool bDeviceLocked = false;


	pFile = fopen(filePath, "rb");
	if (pFile == nullptr)
	{
		retval = XLNX_DEV_INTERFACE_ERROR_FAILED_TO_OPEN_FILE;
	}


	if (retval == XLNX_OK)
	{
		fseek(pFile, 0, SEEK_END);
		fileLength = ftell(pFile);
		rewind(pFile);

		m_xclbinBuffer = new uint8_t[fileLength];
		m_xclbinBufferLength = fileLength;

		fread(m_xclbinBuffer, 1, fileLength, pFile);
		fclose(pFile);
	}





	if (retval == XLNX_OK)
	{
		//
		//programming a bitstream requires that the device be locked for exclusive access....
		//
		localretval = xclLockDevice(m_deviceHandle);
		if (localretval == 0)
		{
			bDeviceLocked = true;
		}
		else
		{
			bDeviceLocked = false;
			retval = XLNX_DEV_INTERFACE_ERROR_FAILED_TO_LOCK_DEVICE;
		}
	}






	if (retval == 0)
	{
		if (bDeviceLocked)
		{
			localretval = xclLoadXclBin(m_deviceHandle, (xclBin*)m_xclbinBuffer);
			if (localretval != 0)
			{
				retval = XLNX_DEV_INTERFACE_ERROR_FAILED_TO_DOWNLOAD_BITSTREAM;
			}
		}
	}






	//if device was locked, always unlock regardless of whether download was successful...
	if (bDeviceLocked)
	{
		xclUnlockDevice(m_deviceHandle);
	}



	//that act of downloading a new bitsteram will change the details of the running bitstream....we should reload them...
	xclGetDeviceInfo2(m_deviceHandle, &m_deviceInfo);	
	xrtXclbinUUID(m_deviceHandle, m_xuid);

	return retval;
}










uint32_t HWDeviceInterface::ReadMem32(uint64_t address, uint32_t* value)
{
	uint32_t retval = XLNX_OK;


	xclUnmgdPread(m_deviceHandle, 0, value, sizeof(uint32_t), address);


	return retval;
}




uint32_t HWDeviceInterface::WriteMem32(uint64_t address, uint32_t value)
{
	uint32_t retval = XLNX_OK;

	xclUnmgdPwrite(m_deviceHandle, 0, &value, sizeof(uint32_t), address);

	return retval;
}




uint32_t HWDeviceInterface::WriteMemWithMask32(uint64_t address, uint32_t value, uint32_t mask)
{
	uint32_t retval = XLNX_OK;
	uint32_t memValue;

	retval = ReadMem32(address, &memValue);

	if (retval == XLNX_OK)
	{
		memValue = memValue & ~mask;
		value = value & mask;

		memValue = memValue | value;

		retval = WriteMem32(address, memValue);
	}

	return retval;
}




uint32_t HWDeviceInterface::BlockReadMem32(uint64_t address, uint32_t* buffer, uint32_t numWords)
{
	uint32_t retval = XLNX_OK;


	xclUnmgdPread(m_deviceHandle, 0, buffer, numWords * sizeof(uint32_t), address);


	return retval;
}




uint32_t HWDeviceInterface::BlockWriteMem32(uint64_t address, uint32_t* buffer, uint32_t numWords)
{
	uint32_t retval = XLNX_OK;

	xclUnmgdPwrite(m_deviceHandle, 0, buffer, numWords * sizeof(uint32_t), address);

	return retval;
}




uint32_t HWDeviceInterface::GetArgumentMemTopologyIndex(const char* cuName, uint32_t cuArgIndex, uint32_t* pTopologyIndex)
{
	uint32_t retval = XLNX_OK;

	uint32_t ipLayoutIndex;
	uint32_t memTopologyIndex;

	//This function is used to retrieve the memory bank that the specified kernel argument of the specified kernel is wired to.
	//The correct memory bank needs to be specified when allocating buffers from DDR/HBM.
	//To do this we need to visit several tables in the bitstream info...



	//STEP 1 - convert the CU Name into an IP LAYOUT index...
	retval = GetCUIPLayoutIndex(cuName, &ipLayoutIndex);
	// printf("GetCUIPLayoutIndex: %d\n", retval);



	if (retval == XLNX_OK)
	{
		//STEP 2 - get the connectivity (mem topology) index of the specified arg of the specified kernel...
		retval = GetMemTopologyIndex(ipLayoutIndex, cuArgIndex, &memTopologyIndex);
		// printf("GetMemTopologyIndex: %d\n", retval);
	}


	if (retval == XLNX_OK)
	{
		*pTopologyIndex = memTopologyIndex;
	}


	return retval;
}












uint32_t HWDeviceInterface::TopologyIndexIsHostBank(uint32_t memTopologyIndex, bool* pbIsHostBank)
{
	uint32_t retval = XLNX_OK;
	struct mem_data memDataElement;
	size_t size;
	


	retval = this->GetBitstreamSectionInfo(&memDataElement, &size, axlf_section_kind::MEM_TOPOLOGY, (uint32_t)memTopologyIndex);

	if (retval == XLNX_OK)
	{
		*pbIsHostBank = false; //default

		if (memDataElement.m_used)
		{
			//NOTE - the actual tag we are looking for is "HOST[0]", but here we are only checking that it starts with "HOST"
			if (strncmp((const char*)memDataElement.m_tag, HOST_BANK_TAG, strlen(HOST_BANK_TAG)) == 0)
			{
				*pbIsHostBank = true;
			}
		}
	}

	return retval;

}




BufferDescriptor* HWDeviceInterface::AllocateBufferPair(uint32_t sizeInBytes, uint32_t bankInfo)
{
	HWDeviceBufferDescriptor* pHWBufferDescriptor = nullptr;
	xclBufferHandle bufferHandle;

	bufferHandle = (unsigned int)xclAllocBO(m_deviceHandle, (size_t)sizeInBytes, (xclBOKind)0, 0);
	wait_for_enter("\nPress ENTER to continue...");

	if (bufferHandle != XRT_NULL_BO)
	{	
		printf("before new HWDeviceBufferDescriptor()");
		pHWBufferDescriptor = new HWDeviceBufferDescriptor();
		printf("new HWDeviceBufferDescriptor()");
		if (pHWBufferDescriptor != nullptr)
		{
			pHWBufferDescriptor->m_bufferHandle			= bufferHandle;
			printf("pHWBufferDescriptor->m_bufferHandle");
			pHWBufferDescriptor->m_deviceBufferAddress	= xclGetDeviceAddr(m_deviceHandle, pHWBufferDescriptor->m_bufferHandle);
			printf("pHWBufferDescriptor->m_deviceBufferAddress");
			pHWBufferDescriptor->m_sizeInBytes			= sizeInBytes;
			printf("pHWBufferDescriptor->m_sizeInBytes");
			pHWBufferDescriptor->m_bIsHostOnlyBuffer	= false;
			printf("pHWBufferDescriptor->m_bIsHostOnlyBuffer");
		}
	}

	return (BufferDescriptor*)pHWBufferDescriptor;
}









void HWDeviceInterface::FreeBufferPair(BufferDescriptor* pDescriptor)
{
	HWDeviceBufferDescriptor* pHWBufferDescriptor = (HWDeviceBufferDescriptor*)pDescriptor;

	xclFreeBO(m_deviceHandle, pHWBufferDescriptor->m_bufferHandle);
	pHWBufferDescriptor->m_bufferHandle = (xrtBufferHandle)NULLBO;

	pHWBufferDescriptor->m_deviceBufferAddress = 0;
	pHWBufferDescriptor->m_sizeInBytes = 0;

	
	delete (pHWBufferDescriptor);
}







uint64_t HWDeviceInterface::GetDeviceBufferAddress(BufferDescriptor* pDescriptor)
{
	HWDeviceBufferDescriptor* pHWBufferDescriptor = (HWDeviceBufferDescriptor*)pDescriptor;
	uint64_t address;

	address = pHWBufferDescriptor->m_deviceBufferAddress;

	return address;
}








uint32_t HWDeviceInterface::MapBufferToUserspace(BufferDescriptor* pDescriptor, BufferMapType mapType, void** ppMappedBuffer)
{
	HWDeviceBufferDescriptor* pHWBufferDescriptor = (HWDeviceBufferDescriptor*)pDescriptor;
	uint32_t retval = XLNX_OK;
	void* pMapped = nullptr;
	bool bWrite;

	if (mapType == BufferMapType::READ_ONLY)
	{
		bWrite = false;
	}
	else
	{
		bWrite = true;
	}

	pMapped = xclMapBO(m_deviceHandle, pHWBufferDescriptor->m_bufferHandle, bWrite);

	*ppMappedBuffer = pMapped;

	if (pMapped == nullptr)
	{
		retval = XLNX_DEV_INTERFACE_ERROR_FAILED_TO_MAP_BUFFER;
	}

	return retval;

}







uint32_t HWDeviceInterface::UnmapBufferFromUserspace(BufferDescriptor* pDescriptor, void* pMapped)
{
	HWDeviceBufferDescriptor* pHWBufferDescriptor = (HWDeviceBufferDescriptor*)pDescriptor;
	uint32_t retval = XLNX_OK;
	int rc;

	rc = xclUnmapBO(m_deviceHandle, pHWBufferDescriptor->m_bufferHandle, pMapped);

	if (rc != 0)
	{
		retval = XLNX_DEV_INTERFACE_ERROR_FAILED_TO_UNMAP_BUFFER;
	}

	return retval;
}








uint32_t HWDeviceInterface::SyncBuffer(BufferDescriptor* pDescriptor, SyncDirection direction)
{
	HWDeviceBufferDescriptor* pHWBufferDescriptor = (HWDeviceBufferDescriptor*)pDescriptor;

	return SyncBuffer(pDescriptor, direction, (size_t)pHWBufferDescriptor->m_sizeInBytes, 0);
}









uint32_t HWDeviceInterface::SyncBuffer(BufferDescriptor* pDescriptor, SyncDirection direction, uint32_t size, uint32_t offset)
{
	HWDeviceBufferDescriptor* pHWBufferDescriptor = (HWDeviceBufferDescriptor*)pDescriptor;
	uint32_t retval = XLNX_OK;
	xclBOSyncDirection dir;
	int rc;


	if (size > 0)
	{
		if (direction == SyncDirection::TO_DEVICE)
		{
			dir = XCL_BO_SYNC_BO_TO_DEVICE;
		}
		else
		{
			dir = XCL_BO_SYNC_BO_FROM_DEVICE;
		}


		rc = xclSyncBO(m_deviceHandle, pHWBufferDescriptor->m_bufferHandle, dir, (size_t)size, (size_t)offset);

		if (rc != 0)
		{
			retval = XLNX_DEV_INTERFACE_ERROR_FAILED_TO_SYNC_BUFFER;
		}
	}

	return retval;
}











uint32_t HWDeviceInterface::GetBitstreamSectionInfo(void* info, size_t* size, enum axlf_section_kind kind, int index)
{
	uint32_t retval = XLNX_OK;
	xuid_t* pBitstreamUUID;


	//[JIRA: DCA-3296] - xclGetSectionInfo will crash if we call it when no XCLBIN file has ever been downloaded.
	// 					 i.e. the card is unprogrammed.
	//                   To work around this, we can check the UUID of what is running on the card.  
	//				     If it is ALL-ZEROES, it means the card is unprogrammed, and we can skip the call to xclGetSectionInfo.



	pBitstreamUUID = GetBitstreamUUID();

	//In emulation mode, the UUID will be empty, so we want to skip the null check
	//i.e. only do the check if we are NOT in emulation mode
#ifndef XCL_EMULATION_MODE
	if (uuid_is_null(*pBitstreamUUID))
	{
		retval = XLNX_DEV_INTERFACE_ERROR_BITSTREAM_NOT_DOWNLOADED;
	}
#endif
	

	if (retval == XLNX_OK)
	{
#ifndef XCL_EMULATION_MODE
	
		int rc = xclGetSectionInfo(m_deviceHandle, info, size, kind, index);

		if (rc != 0)
		{
			retval = XLNX_DEV_INTERFACE_ERROR_INVALID_PARAMETER;
		}
	
#else
		retval = EmulationGetSectionInfo(info, size, kind, index);
#endif
	}

	return retval;
}


uint32_t HWDeviceInterface::GetCUAddress(const char* cuName, uint64_t* address)
{
	uint32_t retval = XLNX_OK;
	int elementIndex;
	size_t size;
	struct ip_data ipDataElement;
	bool bFound = false;


	elementIndex = 0;

	while (retval == XLNX_OK)
	{
		retval = GetBitstreamSectionInfo(&ipDataElement, &size, axlf_section_kind::IP_LAYOUT, elementIndex);

		if (retval == XLNX_OK)
		{
			printf("ipDataElement.m_name: %s \n", (const char*)ipDataElement.m_name);
			if (strncmp(cuName, (const char*)ipDataElement.m_name, MAX_CU_NAME_LENGTH) == 0)
			{
				*address = ipDataElement.m_base_address;
				bFound = true;
				break; //out of loop
			}

			elementIndex++;
		}
	}


	if (bFound == false)
	{
		retval = XLNX_DEV_INTERFACE_ERROR_CU_NAME_NOT_FOUND;
	}

	return retval;
}




uint32_t HWDeviceInterface::GetCUIndex(const char* cuName, uint32_t* cuIndex)
{
	uint32_t retval = XLNX_OK;

	//NOTE - xclIPName2Index does NOT given back the index of the CU in the IP_LAYOUT table in the XCLBIN
	//       Internally the XRT seems to maintain a seperate list of CUs that are ordered in ASCENDING ADDRESS ORDER
	//       It is an index into this INTERNAL LIST that is returned.

	int rc = xclIPName2Index(m_deviceHandle, cuName);
	printf("xclIPName2Index return: %d\n", rc);
	if (rc >= 0)
	{
		*cuIndex = rc;
	}
	else
    {
		retval = XLNX_DEV_INTERFACE_ERROR_CU_NAME_NOT_FOUND;
	}

	return retval;
}


uint32_t HWDeviceInterface::Wait(){

	while (xclExecWait(m_deviceHandle, 1000)==0);
	
}


uint32_t HWDeviceInterface::ReadCUReg32(uint32_t cuIndex, uint64_t offset, uint32_t* value)
{
	uint32_t retval = XLNX_OK;
	int rc;

	if (retval == XLNX_OK)
	{
		rc = xclOpenContext(m_deviceHandle, m_xuid, cuIndex, m_bSharedAccess);
		if (rc != 0)
		{
			retval = XLNX_DEV_INTERFACE_ERROR_IO_FAILED;
		}
	}



	if (retval == XLNX_OK)
	{

#ifndef XCL_EMULATION_MODE
		int rc = xclRegRead(m_deviceHandle, cuIndex, (uint32_t)offset, value);
		if (rc != 0)
		{
			retval = XLNX_DEV_INTERFACE_ERROR_IO_FAILED;
		}
#else
		XLNX_UNUSED_ARG(offset);
		XLNX_UNUSED_ARG(value);

		retval = XLNX_DEV_INTERFACE_ERROR_NOT_SUPPORTED;
#endif

	}


	if (retval == XLNX_OK)
	{
		rc = xclCloseContext(m_deviceHandle, m_xuid, cuIndex);
		if (rc != 0)
		{
			retval = XLNX_DEV_INTERFACE_ERROR_IO_FAILED;
		}
	}


	return retval;
}







uint32_t HWDeviceInterface::WriteCUReg32(uint32_t cuIndex, uint64_t offset, uint32_t value)
{
	uint32_t retval = XLNX_OK;
	int rc;

	if (retval == XLNX_OK)
	{
		rc = xclOpenContext(m_deviceHandle, m_xuid, cuIndex, m_bSharedAccess);
		if (rc != 0)
		{
			retval = XLNX_DEV_INTERFACE_ERROR_IO_FAILED;
		}
	}


	if (retval == XLNX_OK)
	{
#ifndef XCL_EMULATION_MODE
		int rc = xclRegWrite(m_deviceHandle, cuIndex, (uint32_t)offset, value);
		if (rc != 0)
		{
			retval = XLNX_DEV_INTERFACE_ERROR_IO_FAILED;
		}
#else
		XLNX_UNUSED_ARG(offset);
		XLNX_UNUSED_ARG(value);

		retval = XLNX_DEV_INTERFACE_ERROR_NOT_SUPPORTED;
#endif
	}

	if (retval == XLNX_OK)
	{
		rc = xclCloseContext(m_deviceHandle, m_xuid, cuIndex);
		if (rc != 0)
		{
			retval = XLNX_DEV_INTERFACE_ERROR_IO_FAILED;
		}
	}

	return retval;
}








uint32_t HWDeviceInterface::WriteCURegWithMask32(uint32_t cuIndex, uint64_t offset, uint32_t value, uint32_t mask)
{
	uint32_t retval = XLNX_OK;
	int rc;
	uint32_t regValue;


	if (retval == XLNX_OK)
	{
		rc = xclOpenContext(m_deviceHandle, m_xuid, cuIndex, m_bSharedAccess);
		if (rc != 0)
		{
			retval = XLNX_DEV_INTERFACE_ERROR_IO_FAILED;
		}
	}


	if (retval == XLNX_OK)
	{
		retval = ReadCUReg32(cuIndex, (uint32_t)offset, &regValue);
	}


	if (retval == XLNX_OK)
	{
		regValue = regValue & ~mask;
		value = value & mask;

		regValue = regValue | value;

		retval = WriteCUReg32(cuIndex, (uint32_t)offset, regValue);
	}



	if (retval == XLNX_OK)
	{
		rc = xclCloseContext(m_deviceHandle, m_xuid, cuIndex);
		if (rc != 0)
		{
			retval = XLNX_DEV_INTERFACE_ERROR_IO_FAILED;
		}
	}


	return retval;
}








uint32_t HWDeviceInterface::BlockReadCUReg32(uint32_t cuIndex, uint64_t offset, uint32_t* buffer, uint32_t numWords)
{
	uint32_t retval = XLNX_OK;
	uint32_t i;
	int rc;


	if (retval == XLNX_OK)
	{
		rc = xclOpenContext(m_deviceHandle, m_xuid, cuIndex, m_bSharedAccess);
		if (rc != 0)
		{
			retval = XLNX_DEV_INTERFACE_ERROR_IO_FAILED;
		}
	}


	if (retval == XLNX_OK)
	{

		for (i = 0; i < numWords; i++)
		{
			retval = ReadCUReg32(cuIndex, offset + (i * sizeof(uint32_t)), &buffer[i]);

			if (retval != XLNX_OK)
			{
				break; //out of loop 
			}
		}
	}


	if (retval == XLNX_OK)
	{
		rc = xclCloseContext(m_deviceHandle, m_xuid, cuIndex);
		if (rc != 0)
		{
			retval = XLNX_DEV_INTERFACE_ERROR_IO_FAILED;
		}
	}


	return retval;
}







uint32_t HWDeviceInterface::BlockWriteCUReg32(uint32_t cuIndex, uint64_t offset, uint32_t* buffer, uint32_t numWords)
{
	uint32_t retval = XLNX_OK;
	uint32_t i;
	int rc;


	if (retval == XLNX_OK)
	{
		rc = xclOpenContext(m_deviceHandle, m_xuid, cuIndex, m_bSharedAccess);
		if (rc != 0)
		{
			retval = XLNX_DEV_INTERFACE_ERROR_IO_FAILED;
		}
	}




	if (retval == XLNX_OK)
	{
		for (i = 0; i < numWords; i++)
		{
			retval = WriteCUReg32(cuIndex, offset + (i * sizeof(uint32_t)), buffer[i]);

			if (retval != XLNX_OK)
			{
				break; //out of loop 
			}
		}
	}



	if (retval == XLNX_OK)
	{
		rc = xclCloseContext(m_deviceHandle, m_xuid, cuIndex);
		if (rc != 0)
		{
			retval = XLNX_DEV_INTERFACE_ERROR_IO_FAILED;
		}
	}


	return retval;
}











uint32_t HWDeviceInterface::GetCUIPLayoutIndex(const char* cuName, uint32_t* ipLayoutIndex)
{
	uint32_t retval = XLNX_OK;
	struct ip_data ipDataElement;
	uint32_t elementIndex;
	size_t size;
	bool bFound = false;


	elementIndex = 0;
	while (retval == XLNX_OK)
	{
		retval = this->GetBitstreamSectionInfo(&ipDataElement, &size, axlf_section_kind::IP_LAYOUT, elementIndex);

		if (retval == XLNX_OK)
		{
			if (ipDataElement.m_type == IP_KERNEL) //only interested in KERNEL blocks...
			{
				if (strncmp(cuName, (const char*)ipDataElement.m_name, MAX_CU_NAME_LENGTH) == 0)
				{
					*ipLayoutIndex = elementIndex;
					bFound = true;
					break; //out of loop
				}
			}

			elementIndex++;
		}
	}


	if (bFound == false)
	{
		retval = XLNX_DEV_INTERFACE_ERROR_CU_NAME_NOT_FOUND;
	}


	return retval;
}







uint32_t HWDeviceInterface::GetMemTopologyIndex(uint32_t ipLayoutIndex, uint32_t kernelArgIndex, uint32_t* memTopologyIndex)
{
	uint32_t retval = XLNX_OK;
	struct connection connectionElement;
	size_t size;
	int elementIndex;
	bool bFound = false;


	//This function searches through the CONNECTIVITY table to find the mem topology index for the specified kernel argument index
	//of the specified kernel index (i.e. IP layout index)...

	elementIndex = 0;

	while (retval == XLNX_OK)
	{
		retval = this->GetBitstreamSectionInfo(&connectionElement, &size, axlf_section_kind::CONNECTIVITY, elementIndex);

		if (retval == XLNX_OK)
		{
			if ((connectionElement.m_ip_layout_index == (int32_t)ipLayoutIndex) && (connectionElement.arg_index == (int32_t)kernelArgIndex))
			{
				*memTopologyIndex = (uint32_t) connectionElement.mem_data_index;
				bFound = true;
				break; //out of loop 
			}

			elementIndex++;
		}
	}




	if (bFound == false)
	{
		retval = XLNX_DEV_INTERFACE_ERROR_MEM_TOPOLOGY_INDEX_NOT_FOUND;
	}

	return retval;
}





// uint32_t HWDeviceInterface::GetStreamIDs(uint32_t memTopologyIndex, uint64_t* routeID, uint64_t* flowID)
// {
// 	uint32_t retval = XLNX_OK;
// 	struct mem_data memDataElement;
// 	size_t size;

// 	retval = this->GetBitstreamSectionInfo(&memDataElement, &size, axlf_section_kind::MEM_TOPOLOGY, (uint32_t)memTopologyIndex);

// 	if (retval == XLNX_OK)
// 	{
// 		if (memDataElement.m_type == MEM_STREAMING)
// 		{
// 			*routeID = memDataElement.route_id;
// 			*flowID = memDataElement.flow_id;
// 		}
// 		else
// 		{
// 			retval = XLNX_DEV_INTERFACE_ERROR_STREAM_IDS_NOT_FOUND;
// 		}
// 	}
	
// 	return retval;
// }









// uint32_t HWDeviceInterface::GetStreamIDs(const char* cuName, uint32_t cuArgIndex, uint64_t* routeID, uint64_t* flowID)
// {
// 	uint32_t retval = XLNX_OK;
// 	uint32_t ipLayoutIndex;
// 	uint32_t memTopologyIndex;

// 	//This function is used to retrieve the route/flow IDs of the specified kernel argument of the specified kernel.
// 	//NOTE - This kernel argument must be a STREAMING arg or this function will fail, returning an appropriate error code.
// 	//To do this we need to visit several tables in the bitstream info...



// 	//STEP 1 - convert the CU Name into an IP LAYOUT index...
// 	retval = GetCUIPLayoutIndex(cuName, &ipLayoutIndex);
	
	

	
// 	if (retval == XLNX_OK)
// 	{
// 		//STEP 2 - get the connectivity (mem topology) index of the specified arg of the specified kernel...
// 		retval = GetMemTopologyIndex(ipLayoutIndex, cuArgIndex, &memTopologyIndex);
// 	}


// 	if (retval == XLNX_OK)
// 	{
// 		//STEP 3 - get the route/flow IDs of the specified mem topology index...
// 		retval = GetStreamIDs(memTopologyIndex, routeID, flowID);
// 	}


// 	return retval;
// }








//  uint32_t HWDeviceInterface::OpenStream(const char* cuName, uint32_t cuStreamArgIndex, StreamDirection direction, StreamHandleType* streamHandle)
// {
// 	 uint32_t retval = XLNX_OK;
// 	 uint64_t routeID;
// 	 uint64_t flowID;
// 	 struct xclQueueContext queueContext;
// 	 int rc;


// 	 retval = GetStreamIDs(cuName, cuStreamArgIndex, &routeID, &flowID);

// 	 if (retval == XLNX_OK)
// 	 {
// 		 queueContext.route = routeID;
// 		 queueContext.flow = flowID;
//          queueContext.flags = 0;

// 		 if (direction == StreamDirection::READ_STREAM)
// 		 {
// 			 rc = xclCreateReadQueue(m_deviceHandle, &queueContext, streamHandle);
//              if (!rc)
//              {
// #ifndef XCL_EMULATION_MODE
//                 rc = xclSetQueueOpt(m_deviceHandle, *streamHandle, STREAM_OPT_AIO_MAX_EVENT, 1);
// #else
// 				//TODO - xclSetQueueOpt doesn't exist in emulation libraries yet...
// #endif
//              }
// 		 }
// 		 else
// 		 {
// 			 rc = xclCreateWriteQueue(m_deviceHandle, &queueContext, streamHandle);
// 		 }

	
// 		 if (rc != 0)
// 		 {
// 			 retval = XLNX_DEV_INTERFACE_ERROR_FAILED_TO_CREATE_STREAM;
// 		 }
// 	 }


// 	 return retval;
// }






// uint32_t HWDeviceInterface::CloseStream(StreamHandleType streamHandle)
// {
// 	uint32_t retval = XLNX_OK;
// 	int rc;

// 	rc = xclDestroyQueue(m_deviceHandle, streamHandle);

// 	if (rc != 0)
// 	{
// 		retval = XLNX_DEV_INTERFACE_ERROR_FAILED_TO_DESTROY_STREAM;
// 	}

// 	return retval;
// }








// uint32_t HWDeviceInterface::WriteToStream(StreamHandleType streamHandle, void* dataBuffer, uint32_t numBytesToWrite, uint32_t* numBytesWritten)
// {
// 	uint32_t retval = XLNX_OK;
// 	struct xclReqBuffer bufferDescriptor;
// 	struct xclQueueRequest request;

// 	bufferDescriptor.buf		= (char*) dataBuffer;
// 	bufferDescriptor.len		= numBytesToWrite;
// 	bufferDescriptor.buf_hdl	= 0;


// 	request.op_code		= XCL_QUEUE_WRITE;
// 	request.bufs		= &bufferDescriptor;
// 	request.buf_num		= 1;
// 	request.flag		= XCL_QUEUE_REQ_EOT;


// 	*numBytesWritten = xclWriteQueue(m_deviceHandle, streamHandle, &request);

// 	if (*numBytesWritten != numBytesToWrite)
// 	{
// 		retval = XLNX_DEV_INTERFACE_ERROR_WRITE_TO_STREAM_FAILED;
// 	}

// 	return retval;
// }



// uint32_t HWDeviceInterface::ReadFromStreamBlocking(StreamHandleType streamHandle, void* dataBuffer, uint32_t numBytesToRead, uint32_t* numBytesRead)
// {
// 	uint32_t retval = XLNX_OK;
// 	struct xclReqBuffer bufferDescriptor;
// 	struct xclQueueRequest request;
	
	

// 	bufferDescriptor.buf		= (char*)dataBuffer;
// 	bufferDescriptor.len		= numBytesToRead;
// 	bufferDescriptor.buf_hdl	= 0;


// 	request.op_code		= XCL_QUEUE_READ;
// 	request.bufs		= &bufferDescriptor;
// 	request.buf_num		= 1;
// 	request.flag		= XCL_QUEUE_REQ_EOT;


// 	*numBytesRead = xclReadQueue(m_deviceHandle, streamHandle, &request);

//     if (*numBytesRead != numBytesToRead)
//     {
//         retval = XLNX_DEV_INTERFACE_ERROR_READ_FROM_STREAM_FAILED;
//     }

// 	return retval;
// }



// uint32_t HWDeviceInterface::ReadFromStreamNonBlocking(StreamHandleType streamHandle, void* dataBuffer, uint32_t numBytesToRead, uint32_t* numBytesRead)
// {
// 	uint32_t retval = XLNX_OK;
// 	struct xclReqBuffer bufferDescriptor;
// 	struct xclQueueRequest request;
	
	

// 	bufferDescriptor.buf		= (char*)dataBuffer;
// 	bufferDescriptor.len		= numBytesToRead;
// 	bufferDescriptor.buf_hdl	= 0;


// 	request.op_code		= XCL_QUEUE_READ;
// 	request.bufs		= &bufferDescriptor;
// 	request.buf_num		= 1;
// 	request.flag		= XCL_QUEUE_REQ_EOT;
//     request.flag        |= XCL_QUEUE_REQ_NONBLOCKING;


// 	int rc = xclReadQueue(m_deviceHandle, streamHandle, &request);

//     *numBytesRead = (uint32_t)rc;
//     if (rc < 0)
//     {
//         retval = XLNX_DEV_INTERFACE_ERROR_READ_FROM_STREAM_FAILED;
//     }

// 	return retval;
// }




// uint32_t HWDeviceInterface::PollReadStream(StreamHandleType streamHandle, uint32_t* numBytesRead, int timeout)
// {
//     uint32_t retval = XLNX_OK;
//     int minCompletion = 1;
//     int maxCompletion = 1;
//     int numEvents = 0;
//     int numCompletions = 0;
//     struct xclReqCompletion completions;

//     *numBytesRead = 0;

// #ifndef XCL_EMULATION_MODE

//     numEvents = xclPollQueue(m_deviceHandle, streamHandle, minCompletion, maxCompletion, &completions, &numCompletions, timeout);
//     if (numCompletions)
//     {
//         *numBytesRead = (uint32_t)completions.nbytes;
//     }

//     if (numEvents < 0)
//     {
//         retval = XLNX_DEV_INTERFACE_ERROR_POLL_STREAM_FAILED;
//     }

// #else
// 	XLNX_UNUSED_ARG(streamHandle);
// 	XLNX_UNUSED_ARG(timeout);

// 	retval = XLNX_DEV_INTERFACE_ERROR_NOT_SUPPORTED;
// #endif

//     return retval;
// }



// uint32_t HWDeviceInterface::AllocateStreamBuffer(uint64_t bufferSize, StreamBufferHandleType* pBufferHandle, void** ppBuffer)
// {
// 	uint32_t retval = XLNX_OK;
// 	void* pBuffer = nullptr;

// 	pBuffer = xclAllocQDMABuf(m_deviceHandle, (size_t)bufferSize, pBufferHandle);

// 	if (pBuffer == nullptr)
// 	{
// 		retval = XLNX_DEV_INTERFACE_ERROR_STREAM_BUFFER_ALLOCATION_FAILED;
// 	}

// 	*ppBuffer = pBuffer;

// 	return retval;
// }





// uint32_t HWDeviceInterface::DeallocateStreamBuffer(StreamBufferHandleType bufferHandle)
// {
// 	uint32_t retval = XLNX_OK;
// 	int rc;
	
// 	rc = xclFreeQDMABuf(m_deviceHandle, bufferHandle);

// 	if (rc != 0)
// 	{
// 		retval = XLNX_DEV_INTERFACE_ERROR_STREAM_BUFFER_DEALLOCATION_FAILED;
// 	}

// 	return retval;
// }


void HWDeviceInterface::ReadMACAddresses(void)
{
	uint32_t retval = 0;
	bool bSuccess;


	//The old method for storing MAC addresses used 4 independent slots.  Each slot stored an individual MAC address.
	//Each of these slots is presented as an independent sysfs node.

	//Because different cards have different numbers of ports, some cards need more than 4 mac addresses.
	//To support this, a new method of providing MAC addresses was created.  Instead of storing individual
	//addresses, only the value of the FIRST address is stored, along with a count of the number of 
	//CONTIGUOUS ADDRESSES that card has been assigned by manufacturing.  This means the other addresses
	//are dynamically calculated by ADDING AN OFFSET to the FIRST address.

	for (uint32_t i = 0; i < MACAddresses::NUM_SUPPORTED_MAC_ADDRESSES; i++)
	{
		bSuccess = ReadMACAddressOldMethod(i);

		if (bSuccess == false)
		{
			bSuccess = ReadMACAddressNewMethod(i);
		}
	}
	

}




bool HWDeviceInterface::ReadMACAddressOldMethod(uint32_t index)
{
	uint32_t retval = 0;
	static const uint32_t BUFFER_SIZE = 256;
	char sysfsPathBuffer[BUFFER_SIZE + 1];

	//Unfortunately there is no direct XRT function to retrieve the board info.  The best we can do is to go through sysfs...
	const char* sysfsSubDev = "xmc";
	const char* sysfsNodePrefix = "mac_addr";
	char sysfsNodeBuffer[BUFFER_SIZE + 1];
	FILE* pFile = NULL;
	char strBuffer[BUFFER_SIZE + 1];
	uint32_t numTokens;
	bool bOKToContinue = false;
	uint8_t a, b, c, d, e, f;
	const uint8_t INVALID_ADDRESS_BYTE = 0xFF;
	bool bValid;




#ifdef __linux__

		bOKToContinue = true;

		//need to create proper node name...it is simply our node prefix followed by an incrememting number...
		snprintf(sysfsNodeBuffer, BUFFER_SIZE, "%s%u", sysfsNodePrefix, index);
#ifndef XCL_EMULATION_MODE
		retval = xclGetSysfsPath(m_deviceHandle, sysfsSubDev, sysfsNodeBuffer, sysfsPathBuffer, (size_t)BUFFER_SIZE);
		if (retval != 0)
		{
			bOKToContinue = false;
		}
#else
		//TODO xclGetSysfsPath doesn't exist yet in emulation libraries...
		bOKToContinue = false;
#endif

		if (bOKToContinue)
		{
			pFile = fopen(sysfsPathBuffer, "r");
			if (pFile == NULL)
			{
				bOKToContinue = false;
			}
		}

		if (bOKToContinue)
		{
			if (fgets(strBuffer, BUFFER_SIZE, pFile) == NULL)
			{
				bOKToContinue = false;
			}
		}


		if (bOKToContinue)
		{
			//convert to lower case...will make next parsing step easier...
			for (uint32_t j = 0; j < strlen(strBuffer); j++)
			{
				strBuffer[j] = tolower(strBuffer[j]);
			}
		}

		if (bOKToContinue)
		{

			numTokens = sscanf(strBuffer, "%02" SCNx8 ":%02" SCNx8 ":%02" SCNx8 ":%02" SCNx8 ":%02" SCNx8 ":%02" SCNx8, &a, &b, &c, &d, &e, &f);
			if (numTokens != MACAddresses::NUM_MAC_ADDRESS_BYTES)
			{
				bOKToContinue = false;
			}
		}


		//NOTE - always closing file if it is non-null, regardless of whether the previous calls succeeded/failed...
		if (pFile != NULL)
		{
			fclose(pFile);
			pFile = NULL;
		}



		if (bOKToContinue)
		{
			m_macAddresses.addr[index][0] = a;
			m_macAddresses.addr[index][1] = b;
			m_macAddresses.addr[index][2] = c;
			m_macAddresses.addr[index][3] = d;
			m_macAddresses.addr[index][4] = e;
			m_macAddresses.addr[index][5] = f;
		}
		else
		{
			m_macAddresses.addr[index][0] = INVALID_ADDRESS_BYTE;
			m_macAddresses.addr[index][1] = INVALID_ADDRESS_BYTE;
			m_macAddresses.addr[index][2] = INVALID_ADDRESS_BYTE;
			m_macAddresses.addr[index][3] = INVALID_ADDRESS_BYTE;
			m_macAddresses.addr[index][4] = INVALID_ADDRESS_BYTE;
			m_macAddresses.addr[index][5] = INVALID_ADDRESS_BYTE;
		}



		//An invalid address consists of ALL 0xFF values.  
		//If ANY byte is anything other than 0xFF, then we will consider the address as valid...
		bValid = false;
		for (uint32_t j = 0; j < MACAddresses::NUM_MAC_ADDRESS_BYTES; j++)
		{
			if (m_macAddresses.addr[index][j] != INVALID_ADDRESS_BYTE)
			{
				bValid = true;
}
		}

		m_macAddresses.isValid[index] = bValid;


#else 

	//Since Windows doesn't have the sysfs concept...we will need a different approach...

	//TODO
	//TODO
	//TODO

#endif

	return bOKToContinue;
}


bool HWDeviceInterface::ReadMACAddressNewMethod(uint32_t index)
{
	uint32_t retval = 0;
	static const uint32_t BUFFER_SIZE = 256;
	char sysfsPathBuffer[BUFFER_SIZE + 1];

	//Unfortunately there is no direct XRT function to retrieve the board info.  The best we can do is to go through sysfs...
	const char* sysfsSubDev = "xmc";
	const char* sysfsFistMACNodePrefix = "mac_addr_first";
	const char* sysfsNumMACNodePrefix = "mac_contiguous_num";
	char sysfsNodeBuffer[BUFFER_SIZE + 1];
	FILE* pFile = NULL;
	char strBuffer[BUFFER_SIZE + 1];
	uint32_t numTokens;
	bool bOKToContinue = false;
	uint8_t a, b, c, d, e, f;
	const uint8_t INVALID_ADDRESS_BYTE = 0xFF;
	bool bValid;
	uint32_t numContiguousMACAddresses = 0;




#ifdef __linux__

	bOKToContinue = true;

	//first need to check how many contiguous addresses this card supports...
	snprintf(sysfsNodeBuffer, BUFFER_SIZE, "%s", sysfsNumMACNodePrefix);
#ifndef XCL_EMULATION_MODE
	retval = xclGetSysfsPath(m_deviceHandle, sysfsSubDev, sysfsNodeBuffer, sysfsPathBuffer, (size_t)BUFFER_SIZE);
	if (retval != 0)
	{
		bOKToContinue = false;
	}
#else
	//TODO xclGetSysfsPath doesn't exist yet in emulation libraries...
	bOKToContinue = false;
#endif


	if (bOKToContinue)
	{
		pFile = fopen(sysfsPathBuffer, "r");
		if (pFile == NULL)
		{
			bOKToContinue = false;
		}
	}

	if (bOKToContinue)
	{
		if (fgets(strBuffer, BUFFER_SIZE, pFile) == NULL)
		{
			bOKToContinue = false;
		}
	}


	if (bOKToContinue)
	{

		numTokens = sscanf(strBuffer, "%u", &numContiguousMACAddresses);
		if (numTokens != 1)
		{
			bOKToContinue = false;
		}
	}


	//NOTE - always closing file if it is non-null, regardless of whether the previous calls succeeded/failed...
	if (pFile != NULL)
	{
		fclose(pFile);
		pFile = NULL;
	}



	if (bOKToContinue)
	{
		if (index >= numContiguousMACAddresses)
		{
			//Then this card doesn't support all the MAC addresses we need...
			bOKToContinue = false;
		}
	}





	if (bOKToContinue)
	{
		//Now read the FIRST MAC....
		snprintf(sysfsNodeBuffer, BUFFER_SIZE, "%s", sysfsFistMACNodePrefix);
#ifndef XCL_EMULATION_MODE
		retval = xclGetSysfsPath(m_deviceHandle, sysfsSubDev, sysfsNodeBuffer, sysfsPathBuffer, (size_t)BUFFER_SIZE);
		if (retval != 0)
		{
			bOKToContinue = false;
		}
#else
		//TODO xclGetSysfsPath doesn't exist yet in emulation libraries...
		bOKToContinue = false;
#endif
	}


	if (bOKToContinue)
	{
		pFile = fopen(sysfsPathBuffer, "r");
		if (pFile == NULL)
		{
			bOKToContinue = false;
		}
	}

	if (bOKToContinue)
	{
		if (fgets(strBuffer, BUFFER_SIZE, pFile) == NULL)
		{
			bOKToContinue = false;
		}
	}


	if (bOKToContinue)
	{
		//convert to lower case...will make next parsing step easier...
		for (uint32_t j = 0; j < strlen(strBuffer); j++)
		{
			strBuffer[j] = tolower(strBuffer[j]);
		}
	}

	if (bOKToContinue)
	{

		numTokens = sscanf(strBuffer, "%02" SCNx8 ":%02" SCNx8 ":%02" SCNx8 ":%02" SCNx8 ":%02" SCNx8 ":%02" SCNx8, &a, &b, &c, &d, &e, &f);
		if (numTokens != MACAddresses::NUM_MAC_ADDRESS_BYTES)
		{
			bOKToContinue = false;
		}
	}


	//NOTE - always closing file if it is non-null, regardless of whether the previous calls succeeded/failed...
	if (pFile != NULL)
	{
		fclose(pFile);
		pFile = NULL;
	}




	if (bOKToContinue)
	{
		//Now we need to increment the FIRST address by the index to get the address we want...
		IncrementMACAddress(&a, &b, &c, &d, &e, &f, index);
	}



	if (bOKToContinue)
	{
		m_macAddresses.addr[index][0] = a;
		m_macAddresses.addr[index][1] = b;
		m_macAddresses.addr[index][2] = c;
		m_macAddresses.addr[index][3] = d;
		m_macAddresses.addr[index][4] = e;
		m_macAddresses.addr[index][5] = f;
	}
	else
	{
		m_macAddresses.addr[index][0] = INVALID_ADDRESS_BYTE;
		m_macAddresses.addr[index][1] = INVALID_ADDRESS_BYTE;
		m_macAddresses.addr[index][2] = INVALID_ADDRESS_BYTE;
		m_macAddresses.addr[index][3] = INVALID_ADDRESS_BYTE;
		m_macAddresses.addr[index][4] = INVALID_ADDRESS_BYTE;
		m_macAddresses.addr[index][5] = INVALID_ADDRESS_BYTE;
	}



	//An invalid address consists of ALL 0xFF values.  
	//If ANY byte is anything other than 0xFF, then we will consider the address as valid...
	bValid = false;
	for (uint32_t j = 0; j < MACAddresses::NUM_MAC_ADDRESS_BYTES; j++)
	{
		if (m_macAddresses.addr[index][j] != INVALID_ADDRESS_BYTE)
		{
			bValid = true;
		}
	}

	m_macAddresses.isValid[index] = bValid;


#else 

	//Since Windows doesn't have the sysfs concept...we will need a different approach...

	//TODO
	//TODO
	//TODO

#endif

	return bOKToContinue;
}


void HWDeviceInterface::IncrementMACAddress(uint8_t* pA, uint8_t* pB, uint8_t* pC, uint8_t* pD, uint8_t* pE, uint8_t* pF, uint32_t index)
{
	uint64_t value;

	//Here we pack the MAC address into a uin64_t and then do a simple mathematical increment.
	//This takes care of any overflow/carry situations.

	value = 0;

	value |= ((uint64_t)*pA << 40);
	value |= ((uint64_t)*pB << 32);
	value |= ((uint64_t)*pC << 24);
	value |= ((uint64_t)*pD << 16);
	value |= ((uint64_t)*pE << 8);
	value |= ((uint64_t)*pF << 0);

	value = value + index;

	*pA = (value >> 40) & 0xFF;
	*pB = (value >> 32) & 0xFF;
	*pC = (value >> 24) & 0xFF;
	*pD = (value >> 16) & 0xFF;
	*pE = (value >> 8) & 0xFF;
	*pF = (value >> 0) & 0xFF;	
}


uint32_t HWDeviceInterface::GetClocks(uint32_t frequencyMHz[MAX_SUPPORTED_CLOCKS], uint32_t* pNumClocks)
{
	uint32_t retval = XLNX_OK;

#ifndef XCL_EMULATION_MODE	
	for (uint32_t i = 0; i < MAX_SUPPORTED_CLOCKS; i++)
	{
		if (i < m_deviceInfo.mNumClocks)
		{
			frequencyMHz[i] = m_deviceInfo.mOCLFrequency[i];
		}
		else
		{
			frequencyMHz[i] = 0;
		}
	}

	*pNumClocks = m_deviceInfo.mNumClocks;

#else
	//JIRA: DCA-3367 - m_deviceInfo.mNumClocks is not filled in correctly in HW emu mode...

	for (uint32_t i = 0; i < MAX_SUPPORTED_CLOCKS; i++)
	{
		frequencyMHz[i] = m_deviceInfo.mOCLFrequency[i];
	}

	*pNumClocks = MAX_SUPPORTED_CLOCKS;

#endif

	return retval;
}


xclDeviceHandle HWDeviceInterface::GetDeviceHandle(void){
	return m_deviceHandle;
}
// xclDeviceInfo2 HWDeviceInterface::GetDeviceInfo2(void){
// 	return m_deviceInfo;
// }







uint32_t HWDeviceInterface::ConvertAddressToCUIndexOffset(uint64_t address, uint32_t* cuIndex, uint64_t* offset)
{
	uint32_t retval = XLNX_OK;
	int elementIndex;
	size_t size;
	struct ip_data ipDataElement;
	bool bFound = false;

	// We will loop through all the available kerels to check if the specified address is within the range of 
	// a particular kernel.

	elementIndex = 0;

	while (retval == XLNX_OK)
	{
		retval = GetBitstreamSectionInfo(&ipDataElement, &size, axlf_section_kind::IP_LAYOUT, elementIndex);

		if (retval == XLNX_OK)
		{
			if ((address >= ipDataElement.m_base_address) && (address < (ipDataElement.m_base_address + MAX_SUPPORTED_CU_SIZE)))
			{		

				// Now get the index of this kernel - REMEMBER we cannot use the index of the IP LAYOUT table.
				// We need to ask XRT to supply the index from its own internal list
				retval = GetCUIndex((char*)(ipDataElement.m_name), cuIndex);

				if (retval == XLNX_OK)
				{
					*offset = address - ipDataElement.m_base_address;
					bFound = true;
					break;
				}
			}

			elementIndex++;
		}
	}


	if (bFound == false)
	{
		retval = XLNX_DEV_INTERFACE_ERROR_CU_INDEX_NOT_FOUND;
	}


	return retval;
}





BufferDescriptor* HWDeviceInterface::AllocateBufferHostOnly(uint32_t sizeInBytes, uint32_t bankInfo)
{
	HWDeviceBufferDescriptor* pDescriptor = nullptr;
	xclBufferHandle bufferHandle = NULLBO;


	bufferHandle = xclAllocBO(m_deviceHandle, (size_t)sizeInBytes, (xclBOKind)0, XCL_BO_FLAGS_HOST_ONLY | bankInfo);

	if (bufferHandle != NULLBO)
	{
		pDescriptor = new HWDeviceBufferDescriptor();

		if (pDescriptor != nullptr)
		{
			pDescriptor->m_bufferHandle			= bufferHandle;
			pDescriptor->m_deviceBufferAddress	= xclGetDeviceAddr(m_deviceHandle, pDescriptor->m_bufferHandle); //NOTE - this will give back the device address for the special "HOST[0]" bank
			pDescriptor->m_sizeInBytes			= sizeInBytes;
			pDescriptor->m_bIsHostOnlyBuffer	= true;
		}
	}
	

	return (BufferDescriptor*)pDescriptor;
}






void HWDeviceInterface::FreeBufferHostOnly(BufferDescriptor* pDescriptor)
{
	HWDeviceBufferDescriptor* pHWBufferDescriptor = (HWDeviceBufferDescriptor*)pDescriptor;
	
	xclFreeBO(m_deviceHandle, pHWBufferDescriptor->m_bufferHandle);
	
	pHWBufferDescriptor->m_bufferHandle			= (xrtBufferHandle)NULLBO;
	pHWBufferDescriptor->m_bIsHostOnlyBuffer	= false;
	pHWBufferDescriptor->m_deviceBufferAddress	= 0;
	pHWBufferDescriptor->m_sizeInBytes			= 0;

	delete (pHWBufferDescriptor);
}


