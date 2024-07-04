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

#ifndef XLNX_DEVICE_INTERFACE_H
#define XLNX_DEVICE_INTERFACE_H

#include <iostream>

#include "xrt.h"
#include "experimental/xrt-next.h"
#include "experimental/xrt_xclbin.h"

#include "xlnx_device_interface_error_codes.h"





 /**
 * @namespace XLNX
 */
namespace XLNX
{




/**
 * @class BufferDescriptor
 * @brief Empty virtual class - has implementation-specific derived classes
 */
class BufferDescriptor
{
	
};







/**
 * @class MACAddress
 * @brief Class to hold a details of MAC addresses for a card
 */
class MACAddresses
{
public:

	static const uint32_t NUM_SUPPORTED_MAC_ADDRESSES = 4;				/**< The number of MAC addresses (legacy style) stored in a cards EEPROM */
	static const uint32_t NUM_MAC_ADDRESS_BYTES = 6;					/**< The number of bytes in a MAC address */
public:
	uint8_t addr[NUM_SUPPORTED_MAC_ADDRESSES][NUM_MAC_ADDRESS_BYTES];	/**< MAC address storage */
	bool isValid[NUM_SUPPORTED_MAC_ADDRESSES];							/**< Valid indicators for each MAC address (remember some older cards only have 2 addresses) */
};











/**
 * @class DeviceInterface
 * @brief An interface class that provides an abstraction layer between the users application and the underlying XRT functions.
 */
class DeviceInterface
{

public:
	/**
	* @brief Retrieves the "friendly" name of the associated device/card
	* @returns A NULL-terminated string of the device name.
	*/
	virtual char* GetDeviceName(void) = 0;
	virtual xclDeviceHandle GetDeviceHandle(void) = 0;
	// virtual xclDeviceInfo2 GetDeviceInfo2(void) = 0;

	/**
	* @brief Retrieves the PCI DBDF numbers of the associated device/card
	* @param[out] pDomain - The PCI domain index of the associated device/card
	* @param[out] pBus - The PCI bus index of the associated device/card
	* @param[out] pDevice - The PCI device index of the associated device/card
	* @param[out] pFunction - The PCI function index of the associated device/card
	*/
	virtual void GetPCIDBDF(uint16_t* pDomain, uint8_t* pBus, uint8_t* pDevice, uint8_t* pFunction) = 0;


	/**
	* @brief Retrieves the MAC Addresses from the cards EEPROM
	* @returns A pointer to a MACAddresses object.
	*/
	virtual MACAddresses* GetMACAddresses(void) = 0;

	/**
	* @brief Retrieves the UUID of the bitstream (XCLBIN) that is currently running on the associated device/card
	* @returns A pointer to a UUID struct
	*/
	virtual xuid_t* GetBitstreamUUID(void) = 0;

public:

	/**
	* @brief Closes the handle to the associated device/card and frees any resources.
	*/
	virtual void Close(void) = 0;







/** @name Register Access (via absolute address) */
/** @{ */
public:


	/**
	* @brief Reads a single 32-bit register
	* @param[in] address - The absolute AXI address of the register to be read
	* @param[out] value - The value that is read from the register
	* @returns - XLNX_OK on success, error code on failure
	*/
	virtual uint32_t ReadReg32(uint64_t address, uint32_t* value) = 0;


	/**
	* @brief Write to a single 32-bit register
	* @param[in] address - The absolute AXI address of the register to be read
	* @param[in] value - The value to be written to the register
	* @returns - XLNX_OK on success, error code on failure
	*/
	virtual uint32_t WriteReg32(uint64_t address, uint32_t value) = 0;


	/**
	* @brief Write to a single 32-bit register, using a mask to specify which bits will be modified
	* @param[in] address - The absolute AXI address of the register to be read
	* @param[in] value - The value to be written to the register
	* @param[in] mask - The bitmask that defines which bits in the target register will be modified.
	* @returns - XLNX_OK on success, error code on failure
	*/
	virtual uint32_t WriteRegWithMask32(uint64_t address, uint32_t value, uint32_t mask) = 0;


	/**
	* @brief Read from a contiguous block of 32-bit registers
	* @param[in] address - The absolute AXI address of the first register
	* @param[out] buffer - The buffer that will receive the values that are read.
	* @param[in] numWords - The number of 32-bit values to be read
	* @returns - XLNX_OK on success, error code on failure
	*/
	virtual uint32_t BlockReadReg32(uint64_t address, uint32_t* buffer, uint32_t numWords) = 0;


	/**
	* @brief Write to a contiguous block of 32-bit registers
	* @param[in] address - The absolute AXI address of the first register
	* @param[in] buffer - The buffer that will containing the values to be written
	* @param[in] numWords - The number of 32-bit values to be written
	* @returns - XLNX_OK on success, error code on failure
	*/
	virtual uint32_t BlockWriteReg32(uint64_t address, uint32_t* buffer, uint32_t numWords) = 0;


/** @} */










/** @name Register Access (via CU index) - only introduced in 2019.2 */
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
	virtual uint32_t GetCUAddress(const char* cuName, uint64_t* address) = 0;




	/**
	* @brief Retrive the internal XRT index of a HW kernel (CU)
	* @param[in] cuName - The name of the CU
	* @param[out] cuIndex - The index of the CU
	* @returns - XLNX_OK on success, error code on failure
	*/
	virtual uint32_t GetCUIndex(const char* cuName, uint32_t* cuIndex) = 0;


	virtual uint32_t Wait(void) = 0;

	/**
	* @brief Read the contents of a single 32-bit register
	* @param[in] cuIndex - The index of the CU (retrieved via GetCUIndex)
	* @param[in] offset - The address offset (in bytes) of the register from the base address of the CU
	* @param[out] value - The value read from the register
	* @returns - XLNX_OK on success, error code on failure
	*/
	virtual uint32_t ReadCUReg32(uint32_t cuIndex, uint64_t offset, uint32_t* value) = 0;


	/**
	* @brief Write to a single 32-bit register
	* @param[in] cuIndex - The index of the CU (retrieved via GetCUIndex)
	* @param[in] offset - The address offset (in bytes) of the register from the base address of the CU
	* @param[in] value - The value to be written to the register
	* @returns - XLNX_OK on success, error code on failure
	*/
	virtual uint32_t WriteCUReg32(uint32_t cuIndex, uint64_t offset, uint32_t value) = 0;

	

	/**
	* @brief Write to a single 32-bit register using a mask to specify which bits will be modified
	* @param[in] cuIndex - The index of the CU (retrieved via GetCUIndex)
	* @param[in] offset - The address offset (in bytes) of the register from the base address of the CU
	* @param[in] value - The value to be written to the register
	* @param[in] mask - The bitmask that defines which bits in the target register will be modified. 
	* @returns - XLNX_OK on success, error code on failure
	*/
	virtual uint32_t WriteCURegWithMask32(uint32_t cuIndex, uint64_t offset, uint32_t value, uint32_t mask) = 0;


	/**
	* @brief Read from a contiguous block of registers
	* @param[in] cuIndex - The index of the CU (retrieved via GetCUIndex)
	* @param[in] offset - The address offset (in bytes) from the base address of the CU, of the first register to be read
	* @param[out] buffer - The buffer to receive the values that are read
	* @param[in] numWords - The number of regsisters to be read
	* @returns - XLNX_OK on success, error code on failure
	*/
	virtual uint32_t BlockReadCUReg32(uint32_t cuIndex, uint64_t offset, uint32_t* buffer, uint32_t numWords) = 0;



	/**
	* @brief Write to a contiguous block of registers
	* @param[in] cuIndex - The index of the CU (retrieved via GetCUIndex)
	* @param[in] offset - The address offset (in bytes) from the base address of the CU, of the first register to be read
	* @param[out] buffer - The buffer that contains the values to be written
	* @param[in] numWords - The number of regsisters to be written
	* @returns - XLNX_OK on success, error code on failure
	*/
	virtual uint32_t BlockWriteCUReg32(uint32_t cuIndex, uint64_t offset, uint32_t* buffer, uint32_t numWords) = 0;


	static const uint64_t MAX_SUPPORTED_CU_SIZE = 64 * 1024;	/**< This is taken from internal XRT values....*/

	/**
	* @brief Attempts to convert an absolute AXI address to a CU index and an offset from the base of that CU
	* @param[in] address - The absolute AXI address
	* @param[out] cuIndex - The internal XRT index of the CU at the specified address
	* @param[out] offset - The byte offset from the base of the CU that is the equivalent of the supplied AXI address
	* @returns - XLNX_OK on success, error code on failure
	*/
	virtual uint32_t ConvertAddressToCUIndexOffset(uint64_t address, uint32_t* cuIndex, uint64_t* offset) = 0;


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
	virtual uint32_t GetArgumentMemTopologyIndex(const char* cuName, uint32_t cuArgIndex, uint32_t* pTopologyIndex) = 0;


	
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
	virtual uint32_t TopologyIndexIsHostBank(uint32_t topologyIndex, bool* pbIsHostBank) = 0;

/** @} */







/** @name Host+Device Buffer Allocation */
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
	virtual BufferDescriptor* AllocateBufferPair(uint32_t sizeInBytes, uint32_t bankInfo) = 0;



	/**
	* @brief Frees a pair of buffers
	* @param[in] pDescriptor - The buffer descriptor that is used to reference the buffer pair
	*/
	virtual void FreeBufferPair(BufferDescriptor* pDescriptor) = 0;


	/**
	* @brief Retrieves the address of the buffer on the card.
	* @param[in] pDescriptor - The buffer descriptor that is used to reference the buffer pair
	* @returns - the HW address of the card buffer.
	*/
	virtual uint64_t GetDeviceBufferAddress(BufferDescriptor* pDescriptor) = 0;



	/** BufferMapType describes the options for how a buffer can be mapped into the users application space */
	enum class BufferMapType 
	{
		READ_ONLY,		/**< Mapping option to indicate buffer will be used only for reading by the host */
		READ_WRITE		/**< Mapping option to indicate buffer will be used for both reading and writing by the host */
	};

	/**
	* @brief Map a host buffer into userspace so it may be accessed from the users application.
	* @param[in] pDescriptor - The buffer descriptor that is used to reference the buffer pair
	* @param[in] mapType - value representing how the buffer will be accessed on the host (i.e. read-only -vs- read-write) 
	* @param[out] ppMapped - the virtual address of the host buffer.  This is the address that can be used to read/write to the host buffer from the users application.
	* @returns - XLNX_OK on success, error code on failure
	*/
	virtual uint32_t MapBufferToUserspace(BufferDescriptor* pDescriptor, BufferMapType mapType, void** ppMapped) = 0;



	/**
	* @brief Unmap a host buffer from a users application address space
	* @param[in] pDescriptor - The buffer descriptor that is used to reference the buffer pair
	* @param[in] pMapped - the virtual address of the host buffer.
	* @returns - XLNX_OK on success, error code on failure
	*/
	virtual uint32_t UnmapBufferFromUserspace(BufferDescriptor* pDescriptor, void* pMapped) = 0;



	/** SyncDirection describes the direction that data will be DMA'd  */
	enum class SyncDirection
	{
		TO_DEVICE,				/**< Move data from the host to the card */
		FROM_DEVICE				/**< Move data from the card to the host */
	};

	/**
	* @brief DMA data between the host and the card
	* @param[in] pDescriptor - The buffer descriptor that is used to reference the buffer pair
	* @param[in] direction - The direction that data will be moved.
	* @returns - XLNX_OK on success, error code on failure
	*/
	virtual uint32_t SyncBuffer(BufferDescriptor* pDescriptor, SyncDirection direction) = 0;


	/**
	* @brief DMA a subset of data between the host and the card
	* @param[in] pDescriptor - The buffer descriptor that is used to reference the buffer pair
	* @param[in] direction - The direction that data will be moved.
	* @param[in] size - The amount of data to move (in bytes)
	* @param[in] offset - the offset (in bytes) from the start of the buffer to the start of the data to be moved
	* @returns - XLNX_OK on success, error code on failure
	*/
	virtual uint32_t SyncBuffer(BufferDescriptor* pDescriptor, SyncDirection direction, uint32_t size, uint32_t offset) = 0;

/** @} */





/** @name Host+Device Buffer Allocation */
/** @{ */
public: 


	/**
	* @brief Read a single 32-bit location in card RAM
	* @param[in] address - the address within card RAM to read.
	* @param[out] value - the value read from the memory location.
	* @returns - XLNX_OK on success, error code on failure
	*/
	virtual uint32_t ReadMem32(uint64_t address, uint32_t* value) = 0;



	/**
	* @brief Write to a single 32-bit location in card RAM
	* @param[in] address - the address within card RAM to read.
	* @param[out] value - the value to be written to memory.
	* @returns - XLNX_OK on success, error code on failure
	*/
	virtual uint32_t WriteMem32(uint64_t address, uint32_t value) = 0;



	/**
	* @brief Write to a single 32-bit location in card RAM, using a mask to specify which bits will be modified.
	* @param[in] address - the address within card RAM to read.
	* @param[out] value - the value to be written to memory.
	* @returns - XLNX_OK on success, error code on failure
	*/
	virtual uint32_t WriteMemWithMask32(uint64_t address, uint32_t value, uint32_t mask) = 0;



	/**
	* @brief Read from a contiguous block of locations in card RAM
	* @param[in] address - the address of the first location to read from.
	* @param[out] buffer - the buffer to receive the values that are read.
	* @param[in] numWords - the number of 32-bit words to read.
	* @returns - XLNX_OK on success, error code on failure
	*/
	virtual uint32_t BlockReadMem32(uint64_t address, uint32_t* buffer, uint32_t numWords) = 0;



	/**
	* @brief Write to a contiguous block of locations in card RAM.
	* @param[in] address - the address of the first location to write to.
	* @param[in] buffer - the buffer containing the values to be written.
	* @param[in] numWords - the number of 32-bit words to write.
	* @returns - XLNX_OK on success, error code on failure
	*/
	virtual uint32_t BlockWriteMem32(uint64_t address, uint32_t* buffer, uint32_t numWords) = 0;

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
	virtual BufferDescriptor* AllocateBufferHostOnly(uint32_t sizeInBytes, uint32_t bankInfo) = 0;


	/**
	* @brief Free a host-only buffer
	* @param[in] pDescriptor - pointer to the descriptor used to reference the buffer
	*/
	virtual void FreeBufferHostOnly(BufferDescriptor* pDescriptor) = 0;

/** @} */







// /** @name Stream Interface */
// /** @{ */
// public:

// 	//NOTE - Reading/writing from/to streams uses an underlying HW DMA mechanism.  This has a restriction that
// 	//		 the address of the user buffers being supplied MUST BE ON A 4KB PAGE BOUNDARY.
// 	//



// 	typedef uint64_t StreamHandleType;				/**<  Abstraction of XRT handle type */
// 	typedef uint64_t StreamBufferHandleType;		/**<  Abstraction of XRT handle type */


// 	enum class StreamDirection
// 	{
// 		READ_STREAM,	/**< Card-to-Host */
// 		WRITE_STREAM	/**< Host-to-Card */
// 	};

// 	/**
// 	* @brief Open a stream
// 	* @param[in] cuName - The name of the HW kernel (CU) that the stream will attach to
// 	* @param[in] cuStreamArgIndex - The argument index within the kernel that will represents the end of the stream in HW.
// 	* @param[in] direction - The direction data will move in the stream
// 	* @param[out] streamHandle - The handle that will get populated by this call.  This is used to refer to the stream in other calls.
// 	* @returns - XLNX_OK on success, error code on failure
// 	*/
// 	virtual uint32_t OpenStream(const char* cuName, uint32_t cuStreamArgIndex, StreamDirection direction, StreamHandleType* streamHandle) = 0;

// 	/**
// 	* @brief Close a stream and free up any associated resources.
// 	* @param[in] streamHandle - The handle representing the stream.
// 	* @returns - XLNX_OK on success, error code on failure
// 	*/
// 	virtual uint32_t CloseStream(StreamHandleType streamHandle) = 0;


// 	/**
// 	* @brief Write data to a stream
// 	* @param[in] streamHandle - The handle representing the stream.
// 	* @param[in] dataBuffer - The buffer containg the data to be written to the stream.
// 	* @param[in] numBytesToWrite - The number of bytes to write.
// 	* @param[out] numBytesWritten - The number of bytes that were successfully written to the stream
// 	* @returns - XLNX_OK on success, error code on failure
// 	*/
// 	virtual uint32_t WriteToStream(StreamHandleType streamHandle, void* dataBuffer, uint32_t numBytesToWrite, uint32_t* numBytesWritten) = 0;


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
// 	virtual uint32_t ReadFromStreamBlocking(StreamHandleType streamHandle, void* dataBuffer, uint32_t numBytesToRead, uint32_t* numBytesRead) = 0;




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
// 	virtual uint32_t ReadFromStreamNonBlocking(StreamHandleType streamHandle, void* dataBuffer, uint32_t numBytesToRead, uint32_t* numBytesRead) = 0;




// 	/**
// 	* @brief Poll for completion of asynchronous reads from a stream
// 	* @param[in] streamHandle - The handle representing the stream.
// 	* @param[out] numBytesRead - The number of bytes that were successfully read from the stream.
// 	* @param[in] timeout - The maximum amount of time to wait for the read to complete.
// 	* @returns - XLNX_OK on success, error code on failure
// 	*/
//     virtual uint32_t PollReadStream(StreamHandleType streamHandle, uint32_t* numBytesRead, int timeout) = 0;



// 	/**
// 	* @brief Allocate a buffer that can be used for stream read/write operations
// 	* @param[in] bufferSize - The buffer size in bytes
// 	* @param[out] pBufferHandle - A handle use to refer to the buffer
// 	* @param[out] ppBuffer - The virtual address used to access the buffer from the users application.
// 	* @returns - XLNX_OK on success, error code on failure
// 	*/
// 	virtual uint32_t AllocateStreamBuffer(uint64_t bufferSize, StreamBufferHandleType* pBufferHandle, void** ppBuffer) = 0;


// 	/**
// 	* @brief Deallocate a buffer that was used for stream read/write operations
// 	* @param[in] bufferHandle - The handle used to refer to the buffer
// 	* @returns - XLNX_OK on success, error code on failure
// 	*/
// 	virtual uint32_t DeallocateStreamBuffer(StreamBufferHandleType bufferHandle) = 0;
	

// /** @} */






/** @name Bitstream Functions */
/** @{ */
public:

	/**
	* @brief Download a bitstream (XCLBIN) file to the card
	* @param[in] filePath - Path to the file
	* @returns - XLNX_OK on success, error code on failure
	*/
	virtual uint32_t DownloadBitstream(char* filePath) = 0;



	/**
	* @brief Extract info on the RUNNING (i.e. currently programmed) bitstream
	* @param[out] info - a pointer to a data structure to receive the info.  The type of data structure depends on the "kind" parameter
	* @param[out] size - the amount of data placed into the "info" data structure
	* @param[in] kind - the type of info to retrieve.  This refers to one of the metadata tables within an XCLBIN
	* @param[in] index - the index of the element within the metadata table to retrieve.
	* @returns - XLNX_OK on success, error code on failure
	* 
	* This function can be used to iterate over the various metadata info tables that are present within an XCLBIN.
	*/
	virtual uint32_t GetBitstreamSectionInfo(void* info, size_t* size, enum axlf_section_kind kind, int index) = 0;

/** @} */






/** @name Clocks */
/** @{ */
public: 

	static const uint32_t MAX_SUPPORTED_CLOCKS = 4; /**< Taken from XRT internal code */

	/**
	* @brief Extract the clock frequencies of the running bitstream
	* @param[out] frequencyMHz - a pointer to an array that will receive the clock frequency values
	* @param[out] pNumClocks - the number of frequency values that were written into the array.
	* @returns - XLNX_OK on success, error code on failure
	*/
	virtual uint32_t GetClocks(uint32_t frequencyMHz[MAX_SUPPORTED_CLOCKS], uint32_t* pNumClocks) = 0;

/** @} */






/** @name Misc */
/** @{ */
public:
	static const uint32_t MAX_CU_NAME_LENGTH = 64;	/**< Taken from XRT internal code */
/** @} */



}; //end class DeviceInterface




} //end namespace XLNX





#endif //XLNX_DEVICE_INTERFACE_H

