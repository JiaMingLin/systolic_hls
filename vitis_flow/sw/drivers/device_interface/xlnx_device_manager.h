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

#ifndef XLNX_DEVICE_MANAGER_H
#define XLNX_DEVICE_MANAGER_H

#include <cstdint>
#include <iostream>

#include "xrt.h"

#include "xlnx_device_interface.h"


 /**
 * @namespace XLNX
 */
namespace XLNX
{



/**
 * @class DeviceManager
 * @brief Enumerates available cards, and provides methods to instantiate a DeviceInterface to a selected card
 */
class DeviceManager
{
public:

	/**
	* @brief Default constructor
	*/
	DeviceManager();

	/**
	* @brief Destructor
	*/
	virtual ~DeviceManager();




public:
	/**
	* @brief Initiates enumeration of available (and usable) cards on the host
	* @returns The number of cards that are available
	*/
	uint32_t EnumerateDevices(void);

	/**
	* @brief Retrieves the number of available cards in the host.  Can be used post EnumerateDevices()
	* @returns The number of cards that are available
	*/
	uint32_t GetNumEnumeratedDevices(void);


	/**
	* @brief Retrieves the human-readable name of a card
	* @param[in] deviceIndex - The index of the card
	* @returns A pointer to a string containing the name of the card
	*/
	char* GetEnumeratedDeviceName(uint32_t deviceIndex);


	/**
	* @brief Retrieve the PCI BDF numbers for the selected card
	* @param[in] deviceIndex - The index of the card
	* @param[out] pBus - the PCI bus number
	* @param[out] pDevice - the PCI device number
	* @param[out] pFunction - the PCI function number
	*/
	void GetEnumeratedDeviceBDF(uint32_t deviceIndex, uint32_t* pBus, uint32_t* pDevice, uint32_t* pFunction);



	/**
	* @brief Factory method to create a DeviceInterface for a real card
	* @param[in] deviceIndex - The index of the card
	* @returns A pointer to a DeviceInterface object than can be used to communicate with the selected card
	*/
	DeviceInterface* CreateHWDeviceInterface(uint32_t deviceIndex);


	/**
	* @brief Factory method to create a DeviceInterface for a virtual device (card)
	* @returns A pointer to a DeviceInterface object than can be used to communicate the device
	*/
	DeviceInterface* CreateVirtualDeviceInterface(void);



private:
	xclDeviceInfo2* m_pDeviceInfoArray;			/**< Holds info about available cards */
	uint32_t m_numDevicesEnumerated;			/**< The number of useable cards found */


};















}




#endif



