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

#ifndef XLNX_VIRTUAL_DEVICE_INTERFACE_H
#define XLNX_VIRTIAL_DEVICE_INTERFACE_H

#include <map>

#include "xlnx_device_interface.h"

#ifdef _WIN32
#include "windows/uuid.h"
#else
#include <uuid/uuid.h>
#endif


#ifdef _WIN32
extern "C"
{
	/**
	* @brief Parses a string representing a UUID into a data structure.
	*/
	extern int uuid_parse(char* in, unsigned char* uu);
}
#endif



/**
 * @namespace XLNX
 */
namespace XLNX
{



/**
 * @class VirtualDeviceInterface
 * @brief An implementation of a DeviceInterface that is used to communicate with virtual device.  This allows software development to proceed without needing real HW (or HW emulation)
 */
class VirtualDeviceInterface : public DeviceInterface
{


public:
	/**
	* @brief Default constructor
	*/
	VirtualDeviceInterface();


	/**
	* @brief Destructor
	*/
	virtual ~VirtualDeviceInterface();

public:

	/**
	* @brief Retrieves the "friendly" name of the associated device/card
	* @returns A NULL-terminated string of the device name.
	*/
	char* GetDeviceName(void);


	/**
	* @brief Retrieves the PCI DBDF numbers of the associated device/card
	* @param[out] pDomain - The PCI domain index of the associated device/card
	* @param[out] pBus - The PCI bus index of the associated device/card
	* @param[out] pDevice - The PCI device index of the associated device/card
	* @param[out] pFunction - The PCI function index of the associated device/card
	*/
	void GetPCIDBDF(uint16_t* pDomain, uint8_t* pBus, uint8_t* pDevice, uint8_t* pFunction);


	/**
	* @brief Retrieves the MAC Addresses from the cards EEPROM
	* @returns A pointer to a MACAddresses object.
	*/
	MACAddresses* GetMACAddresses(void);


	/**
	* @brief Retrieves the UUID of the bitstream (XCLBIN) that is currently running on the associated device/card
	* @returns A pointer to a UUID struct
	*/
	xuid_t* GetBitstreamUUID(void);



public:
	/**
	* @brief Closes the handle to the associated device/card and frees any resources.
	*/
	void Close(void);





/** @name Register Access (via absolute address) */
/** @{ */
public:


	/**
	* @brief Reads a single 32-bit register
	* @param[in] address - The absolute AXI address of the register to be read
	* @param[out] value - The value that is read from the register
	* @returns - XLNX_OK on success, error code on failure
	*/
	uint32_t ReadReg32(uint64_t address, uint32_t* value);



	/**
	* @brief Write to a single 32-bit register
	* @param[in] address - The absolute AXI address of the register to be read
	* @param[in] value - The value to be written to the register
	* @returns - XLNX_OK on success, error code on failure
	*/
	uint32_t WriteReg32(uint64_t address, uint32_t value);




	/**
	* @brief Write to a single 32-bit register, using a mask to specify which bits will be modified
	* @param[in] address - The absolute AXI address of the register to be read
	* @param[in] value - The value to be written to the register
	* @param[in] mask - The bitmask that defines which bits in the target register will be modified.
	* @returns - XLNX_OK on success, error code on failure
	*/
	uint32_t WriteRegWithMask32(uint64_t address, uint32_t value, uint32_t mask);


	/**
	* @brief Read from a contiguous block of 32-bit registers
	* @param[in] address - The absolute AXI address of the first register
	* @param[out] buffer - The buffer that will receive the values that are read.
	* @param[in] numWords - The number of 32-bit values to be read
	* @returns - XLNX_OK on success, error code on failure
	*/
	uint32_t BlockReadReg32(uint64_t address, uint32_t* buffer, uint32_t numWords);


	/**
	* @brief Write to a contiguous block of 32-bit registers
	* @param[in] address - The absolute AXI address of the first register
	* @param[in] buffer - The buffer that will containing the values to be written
	* @param[in] numWords - The number of 32-bit values to be written
	* @returns - XLNX_OK on success, error code on failure
	*/
	uint32_t BlockWriteReg32(uint64_t address, uint32_t* buffer, uint32_t numWords);

/** @} */





/** @name Register Access (via CU index) */
/** @{ */
public:


	/**
	* @brief Retrive the base address of a HW kernel (CU)
	* @param[in] cuName - The name of the CU
	* @param[out] address - The address of the CU
	* @returns - XLNX_OK on success, error code on failure
	*
	* The cuName parmeter should be of the form "CLASSNAME:INSTANCENAME"
	* i.e. it is the name listed in the IP_LAYOUT table of the XCLBIN
	* NOTE - instance name may be the same as class name
	* e.g.
	* data_fifo_krnl:df1
	* data_fifo_krnl:df2
	* pingTop:pingTop
	* pongTop:pongTop
	*/
	uint32_t GetCUAddress(const char* cuName, uint64_t* address);


	/**
	* @brief Retrive the internal XRT index of a HW kernel (CU)
	* @param[in] cuName - The name of the CU
	* @param[out] cuIndex - The index of the CU
	* @returns - XLNX_OK on success, error code on failure
	*/
	uint32_t GetCUIndex(const char* cuName, uint32_t* cuIndex);



	/**
	* @brief Read the contents of a single 32-bit register
	* @param[in] cuIndex - The index of the CU (retrieved via GetCUIndex)
	* @param[in] offset - The address offset (in bytes) of the register from the base address of the CU
	* @param[out] value - The value read from the register
	* @returns - XLNX_OK on success, error code on failure
	*/
	uint32_t ReadCUReg32(uint32_t cuIndex, uint64_t offset, uint32_t* value);



	/**
	* @brief Write to a single 32-bit register
	* @param[in] cuIndex - The index of the CU (retrieved via GetCUIndex)
	* @param[in] offset - The address offset (in bytes) of the register from the base address of the CU
	* @param[in] value - The value to be written to the register
	* @returns - XLNX_OK on success, error code on failure
	*/
	uint32_t WriteCUReg32(uint32_t cuIndex, uint64_t offset, uint32_t value);



	/**
	* @brief Write to a single 32-bit register using a mask to specify which bits will be modified
	* @param[in] cuIndex - The index of the CU (retrieved via GetCUIndex)
	* @param[in] offset - The address offset (in bytes) of the register from the base address of the CU
	* @param[in] value - The value to be written to the register
	* @param[in] mask - The bitmask that defines which bits in the target register will be modified.
	* @returns - XLNX_OK on success, error code on failure
	*/
	uint32_t WriteCURegWithMask32(uint32_t cuIndex, uint64_t offset, uint32_t value, uint32_t mask);



	/**
	* @brief Read from a contiguous block of registers
	* @param[in] cuIndex - The index of the CU (retrieved via GetCUIndex)
	* @param[in] offset - The address offset (in bytes) from the base address of the CU, of the first register to be read
	* @param[out] buffer - The buffer to receive the values that are read
	* @param[in] numWords - The number of regsisters to be read
	* @returns - XLNX_OK on success, error code on failure
	*/
	uint32_t BlockReadCUReg32(uint32_t cuIndex, uint64_t offset, uint32_t* buffer, uint32_t numWords);


	/**
	* @brief Write to a contiguous block of registers
	* @param[in] cuIndex - The index of the CU (retrieved via GetCUIndex)
	* @param[in] offset - The address offset (in bytes) from the base address of the CU, of the first register to be read
	* @param[out] buffer - The buffer that contains the values to be written
	* @param[in] numWords - The number of regsisters to be written
	* @returns - XLNX_OK on success, error code on failure
	*/
	uint32_t BlockWriteCUReg32(uint32_t cuIndex, uint64_t offset, uint32_t* buffer, uint32_t numWords);



	/**
	* @brief Attempts to convert an absolute AXI address to a CU index and an offset from the base of that CU
	* @param[in] address - The absolute AXI address
	* @param[out] cuIndex - The internal XRT index of the CU at the specified address
	* @param[out] offset - The byte offset from the base of the CU that is the equivalent of the supplied AXI address
	* @returns - XLNX_OK on success, error code on failure
	*/
	uint32_t ConvertAddressToCUIndexOffset(uint64_t address, uint32_t* cuIndex, uint64_t* offset);

/** @} */




/** @name Memory Topology Functionality */
/** @{ */
public:

	/**
	* @brief Retrieves the memory bank that a HW kernel argument is wired to.
	* @param[in] cuName - The name of the CU to query
	* @param[in] cuArgIndex - The argument index
	* @param[out] pTopologyIndex - The index of the memory bank that the specified argument is wired to in HW
	* @returns - XLNX_OK on success, error code on failure
	*
	* A HW kernel that takes a pointer argument will have that argument wired to a specific memory bank within the FPGA.
	* This means than the pointer that is supplied must point to a location within that memory bank.
	* The following function can be used to query the memory bank that a particular argument is wired to.
	*/
	uint32_t GetArgumentMemTopologyIndex(const char* cuName, uint32_t argIndex, uint32_t* pTopologyIndex);


	/**
	* @brief Queries if a memory bank represents the special "Host" memory bank used by Slave Bridge.
	* @param[in] topologyIndex - The topology index of a memory bank (retrieved by GetArgumentMemTopologyIndex)
	* @param[out] pbIsHostBank - true if topology index represents Host bank, false if it does not.
	* @returns - XLNX_OK on success, error code on failure
	*
	* The Host Bank is a new memory bank that is used with the Slave Bridge IP block.  When a kernel argument is wired
	* to the host bank, it can write directly in host RAM, without needing to go via card DDR.
	* This function can be used to determine if this Host bank is used.
	*/
	uint32_t TopologyIndexIsHostBank(uint32_t topologyIndex, bool* pbIsHostBank);

/** @} */




/** @name Device+Host Buffer Allocation */
/** @{ */
public:



	/**
	* @brief Allocates a pair of buffers (one on host, one on the card)
	* @param[in] sizeInBytes - The size of the buffers
	* @param[in] bankInfo - the index of the card memory bank to allocate the card buffer from.
	* @returns - A pointer to buffer descriptor that can be used to reference the buffer pair.
	*
	* This function can be used allocate a pair of buffers - one on the host and one on the card.
	* To access the host buffer from a userspace application, it must first be mapped using a call to MapBufferToUserspace
	* Data can be moved between the pair of buffers using the SyncBuffer function.
	*
	* Use GetArgumentMemTopologyIndex to get the topologyIndex/bankInfo
	*/
	BufferDescriptor* AllocateBufferPair(uint32_t sizeInBytes, uint32_t bankInfo);


	/**
	* @brief Frees a pair of buffers
	* @param[in] pDescriptor - The buffer descriptor that is used to reference the buffer pair
	*/
	void FreeBufferPair(BufferDescriptor* descriptor);


	/**
	* @brief Retrieves the address of the buffer on the card.
	* @param[in] pDescriptor - The buffer descriptor that is used to reference the buffer pair
	* @returns - the HW address of the card buffer.
	*/
	uint64_t GetDeviceBufferAddress(BufferDescriptor* descriptor);



	/**
	* @brief Map a host buffer into userspace so it may be accessed from the users application.
	* @param[in] pDescriptor - The buffer descriptor that is used to reference the buffer pair
	* @param[in] mapType - value representing how the buffer will be accessed on the host (i.e. read-only -vs- read-write)
	* @param[out] ppMapped - the virtual address of the host buffer.  This is the address that can be used to read/write to the host buffer from the users application.
	* @returns - XLNX_OK on success, error code on failure
	*/
	uint32_t MapBufferToUserspace(BufferDescriptor* pDescriptor, BufferMapType mapType, void** ppMapped);


	/**
	* @brief Unmap a host buffer from a users application address space
	* @param[in] pDescriptor - The buffer descriptor that is used to reference the buffer pair
	* @param[in] pMapped - the virtual address of the host buffer.
	* @returns - XLNX_OK on success, error code on failure
	*/
	uint32_t UnmapBufferFromUserspace(BufferDescriptor* pDescriptor, void* pMapped);



	/**
	* @brief DMA data between the host and the card
	* @param[in] pDescriptor - The buffer descriptor that is used to reference the buffer pair
	* @param[in] direction - The direction that data will be moved.
	* @returns - XLNX_OK on success, error code on failure
	*/
	uint32_t SyncBuffer(BufferDescriptor* pDescriptor, SyncDirection direction);


	/**
	* @brief DMA a subset of data between the host and the card
	* @param[in] pDescriptor - The buffer descriptor that is used to reference the buffer pair
	* @param[in] direction - The direction that data will be moved.
	* @param[in] size - The amount of data to move (in bytes)
	* @param[in] offset - the offset (in bytes) from the start of the buffer to the start of the data to be moved
	* @returns - XLNX_OK on success, error code on failure
	*/
	uint32_t SyncBuffer(BufferDescriptor* pDescriptor, SyncDirection direction, uint32_t size, uint32_t offset);

/** @} */





/** @name Device Memory (DDR, HBM, etc.) Access */
/** @{ */
public:


	/**
	* @brief Read a single 32-bit location in card RAM
	* @param[in] address - the address within card RAM to read.
	* @param[out] value - the value read from the memory location.
	* @returns - XLNX_OK on success, error code on failure
	*/
	uint32_t ReadMem32(uint64_t address, uint32_t* value);


	/**
	* @brief Write to a single 32-bit location in card RAM
	* @param[in] address - the address within card RAM to read.
	* @param[out] value - the value to be written to memory.
	* @returns - XLNX_OK on success, error code on failure
	*/
	uint32_t WriteMem32(uint64_t address, uint32_t value);


	/**
	* @brief Write to a single 32-bit location in card RAM, using a mask to specify which bits will be modified.
	* @param[in] address - the address within card RAM to read.
	* @param[out] value - the value to be written to memory.
	* @returns - XLNX_OK on success, error code on failure
	*/
	uint32_t WriteMemWithMask32(uint64_t address, uint32_t value, uint32_t mask);


	/**
	* @brief Read from a contiguous block of locations in card RAM
	* @param[in] address - the address of the first location to read from.
	* @param[out] buffer - the buffer to receive the values that are read.
	* @param[in] numWords - the number of 32-bit words to read.
	* @returns - XLNX_OK on success, error code on failure
	*/
	uint32_t BlockReadMem32(uint64_t address, uint32_t* buffer, uint32_t numWords);


	/**
	* @brief Write to a contiguous block of locations in card RAM.
	* @param[in] address - the address of the first location to write to.
	* @param[in] buffer - the buffer containing the values to be written.
	* @param[in] numWords - the number of 32-bit words to write.
	* @returns - XLNX_OK on success, error code on failure
	*/
	uint32_t BlockWriteMem32(uint64_t address, uint32_t* buffer, uint32_t numWords);

/** @} */






/** @name Host-Only Buffer Allocation */
/** @{ */
public:


	/**
	* @brief Allocate a host-only buffer that is accessible to/from the card via Slave Bridge
	* @param[in] sizeInBytes - The size of the buffer to allocate
	* @param[in] bankInfo - The index of the "HOST" memory bank in the HW design
	* @returns - a pointer to a buffer descriptor that can be used to reference the buffer.
	*
	* NOTE - in the following function, use GetArgumentMemTopologyIndex to get the topologyIndex/bankInfo
	*/
	virtual BufferDescriptor* AllocateBufferHostOnly(uint32_t sizeInBytes, uint32_t bankInfo);


	/**
	* @brief Free a host-only buffer
	* @param[in] pDescriptor - pointer to the descriptor used to reference the buffer
	*/
	virtual void FreeBufferHostOnly(BufferDescriptor* pDescriptor);
	
/** @} */





/** @name Bitstream Functions */
/** @{ */
public:

	/**
	* @brief Download a bitstream (XCLBIN) file to the card
	* @param[in] filePath - Path to the file
	* @returns - XLNX_OK on success, error code on failure
	*/
	uint32_t DownloadBitstream(char* filePath);



	/**
	* @brief Extract info on the RUNNING (i.e. currently programmed) bitstream (NOT IMPLEMENT IN VIRTUAL DEVICE)
	* @param[out] info - a pointer to a data structure to receive the info.  The type of data structure depends on the "kind" parameter
	* @param[out] size - the amount of data placed into the "info" data structure
	* @param[in] kind - the type of info to retrieve.  This refers to one of the metadata tables within an XCLBIN
	* @param[in] index - the index of the element within the metadata table to retrieve.
	* @returns - XLNX_OK on success, error code on failure
	*
	* This function can be used to iterate over the various metadata info tables that are present within an XCLBIN.
	*/
	uint32_t GetBitstreamSectionInfo(void* info, size_t* size, enum axlf_section_kind kind, int index);

/** @} */


// /** @name Streaming Interface */
// /** @{ */
// public:

// 	/**
// 	* @brief Open a stream
// 	* @param[in] cuName - The name of the HW kernel (CU) that the stream will attach to
// 	* @param[in] cuStreamArgIndex - The argument index within the kernel that will represents the end of the stream in HW.
// 	* @param[in] direction - The direction data will move in the stream
// 	* @param[out] streamHandle - The handle that will get populated by this call.  This is used to refer to the stream in other calls.
// 	* @returns - XLNX_OK on success, error code on failure
// 	*/
// 	uint32_t OpenStream(const char* cuName, uint32_t cuStreamArgIndex, StreamDirection direction, StreamHandleType* streamHandle);


// 	/**
// 	* @brief Close a stream and free up any associated resources.
// 	* @param[in] streamHandle - The handle representing the stream.
// 	* @returns - XLNX_OK on success, error code on failure
// 	*/
// 	uint32_t CloseStream(StreamHandleType streamHandle);


// 	/**
// 	* @brief Write data to a stream
// 	* @param[in] streamHandle - The handle representing the stream.
// 	* @param[in] dataBuffer - The buffer containg the data to be written to the stream.
// 	* @param[in] numBytesToWrite - The number of bytes to write.
// 	* @param[out] numBytesWritten - The number of bytes that were successfully written to the stream
// 	* @returns - XLNX_OK on success, error code on failure
// 	*/
// 	uint32_t WriteToStream(StreamHandleType streamHandle, void* dataBuffer, uint32_t numBytesToWrite, uint32_t* numBytesWritten);


// 	/**
// 	* @brief Perform a read from the stream.
// 	* @param[in] streamHandle - The handle representing the stream.
// 	* @param[in] dataBuffer - The buffer to receive the read data
// 	* @param[in] numBytesToRead - The number of bytes to read.
// 	* @param[out] numBytesRead - The number of bytes that were successfully read from the stream
// 	* @returns - XLNX_OK on success, error code on failure
// 	*
// 	* This function BLOCKS until the requested number of bytes have been read (or an error occurs)
// 	*/
// 	uint32_t ReadFromStreamBlocking(StreamHandleType streamHandle, void* dataBuffer, uint32_t numBytesToRead, uint32_t* numBytesRead);


// 	/**
// 	* @brief Perform a read from the stream.
// 	* @param[in] streamHandle - The handle representing the stream.
// 	* @param[in] dataBuffer - The buffer to receive the read data
// 	* @param[in] numBytesToRead - The number of bytes to read.
// 	* @param[out] numBytesRead - The number of bytes that were successfully read from the stream
// 	* @returns - XLNX_OK on success, error code on failure
// 	*
// 	* This function begins an asynchronous read from the stream.  This means PollReadStream must be used to determine when data has been successfully read.
// 	*/
// 	uint32_t ReadFromStreamNonBlocking(StreamHandleType streamHandle, void* dataBuffer, uint32_t numBytesToRead, uint32_t* numBytesRead);


// 	/**
// 	* @brief Poll for completion of asynchronous reads from a stream
// 	* @param[in] streamHandle - The handle representing the stream.
// 	* @param[out] numBytesRead - The number of bytes that were successfully read from the stream.
// 	* @param[in] timeout - The maximum amount of time to wait for the read to complete.
// 	* @returns - XLNX_OK on success, error code on failure
// 	*/
//     uint32_t PollReadStream(StreamHandleType streamHandle, uint32_t* numBytesRead, int timeout);





// public:
// 	/**
// 	* @brief Allocate a buffer that can be used for stream read/write operations
// 	* @param[in] bufferSize - The buffer size in bytes
// 	* @param[out] pBufferHandle - A handle use to refer to the buffer
// 	* @param[out] ppBuffer - The virtual address used to access the buffer from the users application.
// 	* @returns - XLNX_OK on success, error code on failure
// 	*/
// 	uint32_t AllocateStreamBuffer(uint64_t bufferSize, StreamBufferHandleType* pBufferHandle, void** ppBuffer);


// 	/**
// 	* @brief Deallocate a buffer that was used for stream read/write operations
// 	* @param[in] bufferHandle - The handle used to refer to the buffer
// 	* @returns - XLNX_OK on success, error code on failure
// 	*/
// 	uint32_t DeallocateStreamBuffer(StreamBufferHandleType bufferHandle);

// /** @} */




/** @name Clocks */
/** @{ */
public:
	/**
	* @brief Extract the clock frequencies of the running bitstream
	* @param[out] frequencyMHz - a pointer to an array that will receive the clock frequency values
	* @param[out] pNumClocks - the number of frequency values that were written into the array.
	* @returns - XLNX_OK on success, error code on failure
	*/
	uint32_t GetClocks(uint32_t frequencyMHz[MAX_SUPPORTED_CLOCKS], uint32_t* pNumClocks);

/** @} */





private:
	//NOTE - maintaining TWO seperate maps - one for registers, and one for memory...
	std::map<uint64_t, uint32_t> m_regMap;		/**< Backing store for registers */
	std::map<uint64_t, uint32_t> m_memMap;		/**< Backing store for card memory */


	/**
	* @brief Reads a value from one of the backing store maps
	* @param[in] map - the map object to read from
	* @param[in] address - The address to read from
	* @param[out] value - The value that was read from the specified address
	* @returns - XLNX_OK on success, error code on failure
	*/
	uint32_t InternalRead32(std::map<uint64_t, uint32_t>& map, uint64_t address, uint32_t* value);


	/**
	* @brief Writes a value to one of the backing store maps
	* @param[in] map - the map object to write to
	* @param[in] address - The address to write to
	* @param[in] value - The value to be written
	* @returns - XLNX_OK on success, error code on failure
	*/
	uint32_t InternalWrite32(std::map<uint64_t, uint32_t>& map, uint64_t address, uint32_t value);




private:		
	uint32_t m_numCUs;										/**< The number of HW kernels (CUs) that have been added */
	std::map<const char*, uint32_t> m_cuNameToIndexMap;		/**< Map to convert from kernel name to its base address */
	std::map<uint32_t, uint64_t> m_cuIndexToAddressMap;		/**< Map to convert CU index to its base address */

	static const uint64_t CU_START_ADDRESS = 0x01C00000;	/**< AXI address to start allocating kernels from in virtual device (taken from U250) */
	static const uint64_t CU_OFFSET		   = 0x00010000;	/**< Address apperture of each kernel in virtual device */



	/**
	* @brief Add a new HW kernel (CU) to the virtual device if a matching one is not found already
	* @param[in] cuName - The name of the kernel (CU)
	* @returns XLNX_OK on success, error code on failure
	*/
	uint32_t InternalAddNewCUIfNecessary(const char* cuName);	

	/**
	* @brief Retrieve the index of the specified kernel (CU) name
	* @param[in] cuName - The name of the kernel (CU)
	* @param[out] index - The index of that kernel.
	* @returns XLNX_OK on success, error code on failure
	*/
	uint32_t InternalGetCUIndex(const char* cuName, uint32_t* index);


	/**
	* @brief Retrieve the AXI base address of the specified kernel (CU) index
	* @param[in] cuIndex - The index of the kernel (CU) (see InternalGetCUIndex)
	* @param[out] baseAddress - The base address of that kernel in the virtual device
	* @returns XLNX_OK on success, error code on failure
	*/
	uint32_t InternalGetCUBaseAddress(uint32_t cuIndex, uint64_t* baseAddress);
	
	MACAddresses m_macAddresses;		/**< MAC addresses (read from cards EEPROM) */

	xuid_t m_xuid;						/**< Bitstream UUID */
};




/**
 * @class VirtualDeviceBufferDescriptor
 * @brief Holds info on a buffer that can be used on HW
 * @note In a virtual device, the buffer is simply allocated from host memory.
 */
class VirtualDeviceBufferDescriptor : public BufferDescriptor
{
public:
	uint8_t* m_deviceAddress;	/**< Address of buffer */
	uint32_t m_sizeInBytes;		/**< Size of buffer */
	uint32_t m_magicNumber;		/**< Magic number indicating if descriptor is valid */
};


}


#endif

