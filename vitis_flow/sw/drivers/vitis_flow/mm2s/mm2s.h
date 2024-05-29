#ifndef MM2S_H
#define MM2S_H


#include <cstdint>
#include <thread>
#include <chrono>
#include <fstream>

#include "xlnx_device_interface.h"

#include "mm2s_error_codes.h"

namespace XLNX
{

class MM2S
{


public:
    MM2S();
    virtual ~MM2S();


public:
    uint32_t Initialise(DeviceInterface* pDeviceInterface, const char* cuName);
    uint32_t Uninitialise(void);

public:
    uint32_t StartHWKernel(void);
    // uint32_t IsHWKernelRunning(bool* pbIsRunning);
    std::ofstream fout;

public:
    // static const uint32_t RING_SIZE             = 65536; //elements;
    static const uint32_t DATA_SIZE             = 65536;
    static const uint32_t READ_ELEMENT_SIZE     = 64; //bytes
    // static const uint32_t WRITE_ELEMENT_SIZE    = 32; //bytes

    uint32_t SetupBuffersIfNecessary(void);

    void* GetReadBufferHostVirtualAddress(void);
    void* GetWriteBufferHostVirtualAddress(void);

    static const uint64_t INVALID_HW_BUFFER_ADDRESS = 0xFFFFFFFF;
    uint64_t GetReadBufferHWAddress(void);
    uint64_t GetWriteBufferHWAddress(void);

    uint32_t SyncReadBuffer(void);
    uint32_t SyncWriteBuffer(void);

    // uint32_t GetHWRingReadBufferIndexes(uint32_t* pHeadIndex, uint32_t* pTailIndex);
    // uint32_t GetSWRingReadBufferIndexes(uint32_t* pHeadIndex, uint32_t* pTailIndex);

    // uint32_t GetHWRingWriteBufferIndexes(uint32_t* pHeadIndex, uint32_t* pTailIndex);
    // uint32_t GetSWRingWriteBufferIndexes(uint32_t* pHeadIndex, uint32_t* pTailIndex);

    // void SetSWWriteTailIndex(uint32_t tailIndex);

    // uint32_t StartLatencyCounters(void);
    // uint32_t StopLatencyCounters(void);
    // uint32_t GetLatencyStats(uint32_t durationSeconds, double* max, double* min, double* sum, uint32_t* cnt, uint32_t* cyclesPre, uint32_t* cyclesPost);

// protected:
    // uint32_t InitialiseRingIndexes(void);
    // uint32_t SyncWriteBufferInternal(void);

    // uint32_t SetHWWriteTailIndex(uint32_t tailIndex);

// public:
//     typedef struct
//     {
//         uint32_t txResponseCount;
//         uint32_t rxOp;

//     }Stats;

//     uint32_t GetStats(Stats* pStats);

public:
    void IsInitialised(bool* pbIsInitialised);
    uint32_t GetCUIndex(uint32_t* pCUIndex);
    uint32_t GetCUAddress(uint64_t* pCUAddress);

    uint32_t GetReadBufferCUMemTopologyIndex(uint32_t* pMemTopologyIndex);
    uint32_t GetWriteBufferCUMemTopologyIndex(uint32_t* pMemTopologyIndex);

public:
    uint32_t CheckIsInitialised(void);

    // void SetVerboseTracing(bool bEnabled);


// public: //Throttling

//     //The following functions can be used to control the rate that the HW kernel reads from the H2C ring buffer...
//     uint32_t SetThrottleRate(uint32_t throttleRateClockCycles);
//     uint32_t GetThrottleRate(uint32_t* pThrottleRateClockCycles);


//     typedef struct
//     {
//         uint32_t throttleCounter;
//         uint32_t throttleEvents;
//     }ThrottleStats;

//     uint32_t GetThrottleStats(ThrottleStats* pStats);

protected:
    static const uint32_t KERNEL_MEMORY_ARGUMENT_READ_BUFFER_INDEX = 3;
    static const uint32_t KERNEL_MEMORY_ARGUMENT_WRITE_BUFFER_INDEX = 3;

    uint32_t SetupBufferObjects(void);
    uint32_t CleanupBufferObjects(void);

    // uint32_t SetupBufferObjectsFromHostBank(void);
    uint32_t SetupBufferObjectsFromCardRAM(void);

    // uint32_t CleanupBufferObjectsFromHostBank(void);
    uint32_t CleanupBufferObjectsFromCardRAM(void);

    // uint32_t SetHWReadBufferAddress(uint64_t address);
    // uint32_t SetHWWriteBufferAddress(uint64_t address);

    uint32_t SyncReadBufferInternal(void);
    uint32_t SetHWRingReadHeadIndex(uint32_t headIndex);

protected: //IO
    uint32_t ReadReg32(uint64_t offset, uint32_t* value);
    uint32_t WriteReg32(uint64_t offset, uint32_t value);
    uint32_t WriteRegWithMask32(uint64_t offset, uint32_t value, uint32_t mask);
    uint32_t BlockReadReg32(uint64_t offset, uint32_t* buffer, uint32_t numWords);

    uint32_t BlockReadMem32(uint64_t address, uint32_t* buffer, uint32_t numWords);


// public: //Processing Thread
//     typedef struct
//     {
//         uint32_t numRxPackets;
//         uint32_t numTxPackets;
//     }ThreadStats;

//     uint32_t StartProcessingThread(void);
//     uint32_t StopProcessingThread(void);

//     uint32_t IsProcessingThreadRunning(bool* pbIsRunning);

//     //The following function can be used to control if the processing thread
//     //yields execution to other thread/processes on the system
//     uint32_t SetThreadYield(bool bEnable);
//     uint32_t GetThreadYield(bool* pbEnabled);

//     uint32_t GetThreadStats(ThreadStats* pStats);



//     //The following function can be used to control the a delay that gets applied to the polling thread
//     //when running in HW emulation mode.  Because HW emulation runs much slower than real HW, by introducing
//     //a delay, we can reduce the load on the simulation.
//     static const uint32_t HW_EMU_POLL_DELAY_DEFAULT_SECONDS = 30;
//     void SetHWEmulationPollDelay(uint32_t delaySeconds);
//     void GetHWEmulationPollDelay(uint32_t* pDelaySeconds);



// protected: //Processing Thread
//     void ThreadFunc(void);

//     uint32_t WriteData(orderEntryOperation_t* src);

//     // bool ProcessPricingData(orderBookResponse_t* response, orderEntryOperation_t* operation);

// public:
//     typedef struct _DMAStats
//     {
//         uint32_t totalSyncOperations;
//         uint32_t bufferWrapArounds;
//         uint32_t transferHighTide;
//         uint64_t totalBytesTransferred;

//     }DMAStats;

//     void GetDMAStats(DMAStats* pH2CStats, DMAStats* pC2HStats);
//     void ResetDMAStats(void);


// protected:
//     void DMAStatsLogWrapAround(DMAStats* pStats);
//     void DMAStatsLogTransfer(DMAStats* pStats, uint32_t numElements);






// public:
//     //The following function can be used to control the maxium number of elements that will be DMA'd in one go
//     //This allows us to spread the load on the DMA system.
//     // NOTE - Defaults to value of 0 which means "unlimited" i.e. ALL new elements will be DMA'd in one go.
//     void SetDMAChunkSize(uint32_t numElements);
//     void GetDMAChunkSize(uint32_t* pNumElements);







protected:
    uint32_t m_initialisedMagicNumber;

    uint32_t m_cuIndex;
    uint64_t m_cuAddress;

    DeviceInterface* m_pDeviceInterface;

    uint32_t m_readBufferMemTopologyIndex;
    uint32_t m_writeBufferMemTopologyIndex;

    BufferDescriptor* m_pReadBufferDescriptor;
    BufferDescriptor* m_pWriteBufferDescriptor;

    void* m_readBufferHostVirtualAddress;
    void* m_writeBufferHostVirtualAddress;

    uint64_t m_readBufferHWAddress;
    uint64_t m_writeBufferHWAddress;

    // uint32_t m_lastReadHeadIndex;
    // uint32_t m_lastReadTailIndex;

    // uint32_t m_lastWriteHeadIndex;
    // uint32_t m_lastWriteTailIndex;

    bool m_bNeedToSetupBuffers;

    // bool m_bUsingHostBank;
    // uint32_t m_hostBankMemoryTopologyIndex;
    // BufferDescriptor* m_pHostBankBufferDescriptor;
    // void* m_hostBankBufferVirtualAddress;

    // // processing thread
    // std::thread m_processingThread;
    // ThreadStats m_threadStats;
    // bool m_bKeepRunning;
    // bool m_bVerboseTracing;
    // bool m_bYield;
    // uint32_t m_previousTailIndex;
    // HostPricingInterface* m_pPricingInterface = nullptr;


    // HostPricingEngine m_hostPricingEngine;

    // DMA status
    // DMAStats m_dmaH2CStats;
    // DMAStats m_dmaC2HStats;

    // uint32_t m_maxDMAChunkSize; //in elements


    // uint32_t m_hwEmulationPollDelaySeconds;

}; //end class OrderBookDataMover



} //end namespace XLNX



#endif
