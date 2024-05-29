#ifndef MM2S_H
#define MM2S_H

#include <cstdint>
#include "xlnx_device_interface.h"

namespace XLNX{

class MM2S{
    public:
        uint32_t Initialise(DeviceInterface* pDeviceInterface, const char* cuName);

    public:
        uint32_t Start(void);
        uint32_t Stop(void);
        uint32_t IsRunning(bool* pbIsRunning);
    
    public:
        void IsInitialised(bool* pbIsInitialised);
	    uint32_t GetCUAddress(uint64_t* pCUAddress);
	    uint32_t GetCUIndex(uint32_t* pCUIndex);
    
    protected:
	    uint32_t CheckIsInitialised(void);
    
    protected:
	    uint32_t ReadReg32(uint64_t offset, uint32_t* value);
	    uint32_t WriteReg32(uint64_t offset, uint32_t value);
	    uint32_t WriteRegWithMask32(uint64_t offset, uint32_t value, uint32_t mask);
	    uint32_t BlockReadReg32(uint64_t offset, uint32_t* buffer, uint32_t numWords);
    
    protected:
	    uint32_t m_initialisedMagicNumber;
	    uint64_t m_cuAddress;
	    uint32_t m_cuIndex;
	    DeviceInterface* m_pDeviceInterface;
};

}
#endif //MM2S_H