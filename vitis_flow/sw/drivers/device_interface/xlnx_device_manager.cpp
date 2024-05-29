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

#include "xlnx_device_manager.h"
#include "xlnx_hw_device_interface.h"
#include "xlnx_virtual_device_interface.h"



using namespace XLNX;




DeviceManager::DeviceManager()
{
	m_pDeviceInfoArray = nullptr;
	m_numDevicesEnumerated = 0;
}



DeviceManager::~DeviceManager()
{

}






uint32_t DeviceManager::EnumerateDevices(void)
{
	int retval = 0;
	xclDeviceHandle tempDeviceHandle = nullptr;

	if (m_pDeviceInfoArray != nullptr)
	{
		delete[] m_pDeviceInfoArray;

		m_pDeviceInfoArray = nullptr;
		m_numDevicesEnumerated = 0;
	}


	m_numDevicesEnumerated = xclProbe();

	if (m_numDevicesEnumerated > 0)
	{
		//allocate our device info array based on the number of devices we have detected...
		m_pDeviceInfoArray = new xclDeviceInfo2[m_numDevicesEnumerated];

		for (uint32_t i = 0; i < m_numDevicesEnumerated; i++)
		{
			tempDeviceHandle = xclOpen(i, NULL, XCL_QUIET);

			if (tempDeviceHandle != nullptr)
			{
				retval = xclGetDeviceInfo2(tempDeviceHandle, &m_pDeviceInfoArray[i]);

				if (retval == 0)
				{
					//we have successfully retrieved the device info...don't need the handle any more...
					xclClose(tempDeviceHandle);
					tempDeviceHandle = nullptr;
				}
				else
				{
					//TODO handle error	
				}
			}

		}
	}

	return m_numDevicesEnumerated;
}







uint32_t DeviceManager::GetNumEnumeratedDevices(void)
{
	return m_numDevicesEnumerated;
}





char* DeviceManager::GetEnumeratedDeviceName(uint32_t deviceIndex)
{
	char* pName = nullptr;

	if (deviceIndex < m_numDevicesEnumerated)
	{
		pName = m_pDeviceInfoArray[deviceIndex].mName;
	}


	return pName;
}




void DeviceManager::GetEnumeratedDeviceBDF(uint32_t deviceIndex, uint32_t* pBus, uint32_t* pDevice, uint32_t* pFunction)
{
	if (deviceIndex < m_numDevicesEnumerated)
	{
		*pBus		= (m_pDeviceInfoArray[deviceIndex].mPciSlot >> 8) & 0xFF;
		*pDevice	= (m_pDeviceInfoArray[deviceIndex].mPciSlot >> 3) & 0x1F;
		*pFunction	= (m_pDeviceInfoArray[deviceIndex].mPciSlot >> 0) & 0x07;
	}
	else
	{
		*pBus		= 0;
		*pDevice	= 0;
		*pFunction	= 0;
	}
}







DeviceInterface* DeviceManager::CreateHWDeviceInterface(uint32_t deviceIndex)
{
	DeviceInterface* deviceInterface = nullptr;
	xclDeviceHandle deviceHandle;

	if (deviceIndex < m_numDevicesEnumerated)
	{
		deviceHandle = xclOpen(deviceIndex, NULL, XCL_QUIET);

		if (deviceHandle != nullptr)
		{
			deviceInterface = new HWDeviceInterface(deviceHandle);
		}
	}


	return deviceInterface;
}












DeviceInterface* DeviceManager::CreateVirtualDeviceInterface(void)
{
	DeviceInterface* deviceInterface = nullptr;

	deviceInterface = new VirtualDeviceInterface();

	return deviceInterface;

}







