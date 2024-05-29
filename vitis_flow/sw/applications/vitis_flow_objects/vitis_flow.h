#ifndef VISTIS_FLOW_H
#define VISTIS_FLOW_H

#include "xlnx_device_interface.h"

#include "mm2s.h"
#include "module_add_const.h"
#include "module_mul_const.h"
#include "s2mm.h"

namespace XLNX{

class VitisFlow{

public:
    VitisFlow();

public:
	uint32_t Initialise(DeviceInterface* pDeviceInterface);
    uint32_t Uninitialise(void);

public:
    uint32_t StartMM2SDataMover(void);
    uint32_t StartS2MMDataMover(void);

public:
    MM2S mm2s;
    AddConstant add_const;
    MulConstant mul_const;
    S2MM s2mm;

};

}
#endif // VISTIS_FLOW_H