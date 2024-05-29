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

#include <string.h> //for memcpy

#include "xlnx_hw_device_interface.h"

using namespace XLNX;






uint32_t HWDeviceInterface::EmulationGetSectionInfo(void* info, size_t * size, enum axlf_section_kind sectionKind, int index)
{
    uint32_t retval = XLNX_OK;
	struct axlf* pAXLF = nullptr;
	struct axlf_section_header* pSectionHeader = nullptr;
	uint8_t* pSection = nullptr;

	if ((m_xclbinBuffer == nullptr) || (m_xclbinBufferLength == 0))
	{
		retval = XLNX_DEV_INTERFACE_ERROR_BITSTREAM_NOT_DOWNLOADED;
	}




	if (retval == XLNX_OK)
	{
		pAXLF = (struct axlf*)m_xclbinBuffer;
		
		// First retrieve the section header for the section.
		pSectionHeader = (struct axlf_section_header*) xclbin::get_axlf_section(pAXLF, sectionKind);

		if (pSectionHeader == nullptr)
		{
			retval = XLNX_DEV_INTERFACE_ERROR_XCLBIN_SECTION_NOT_FOUND;
		}
	}




	if (retval == XLNX_OK)
	{
		// The section header gives us the OFFSET to the actual section within the overall XCLBIN.
		// So to get a pointer to the actual section we are interested in, simply add the offset to
		// the base address of our XCLBIN buffer...
		pSection = ((uint8_t*)pAXLF + pSectionHeader->m_sectionOffset);
	}




	if (retval == XLNX_OK)
	{
		switch (sectionKind)
		{
			case(axlf_section_kind::MEM_TOPOLOGY):
			{
				retval = EmulationGetMemTopologyElement((struct mem_topology*)pSection, index, info, size);
				break;
			}

			case(axlf_section_kind::CONNECTIVITY):
			{
				retval = EmulationGetConnectivityElement((struct connectivity*)pSection, index, info, size);
				break;
			}

			case(axlf_section_kind::IP_LAYOUT):
			{
				retval = EmulationGetIPLayoutElement((struct ip_layout*)pSection, index, info, size);
				break;
			}

			default:
			{
				retval = XLNX_DEV_INTERFACE_ERROR_XCLBIN_SECTION_NOT_SUPPORTED;
				break;
			}
		}

	}
	



    return retval;
}







uint32_t HWDeviceInterface::EmulationGetIPLayoutElement(struct ip_layout* pIPLayoutSection, int elementIndex, void* pElementBuffer, size_t* pElementSize)
{
	uint32_t retval = XLNX_OK;
	void* pElement;
	
	if (elementIndex >= pIPLayoutSection->m_count)
	{
		retval = XLNX_DEV_INTERFACE_ERROR_XCLBIN_ELEMENT_INDEX_OUT_OF_RANGE;
	}


	if (retval == XLNX_OK)
	{
		pElement = &(pIPLayoutSection->m_ip_data[elementIndex]);

		memcpy(pElementBuffer, pElement, sizeof(struct ip_data));
		*pElementSize = sizeof(struct ip_data);
	}



	return retval;
}







uint32_t HWDeviceInterface::EmulationGetMemTopologyElement(struct mem_topology* pMemTopologySection, int elementIndex, void* pElementBuffer, size_t* pElementSize)
{
	uint32_t retval = XLNX_OK;
	void* pElement;

	if (elementIndex >= pMemTopologySection->m_count)
	{
		retval = XLNX_DEV_INTERFACE_ERROR_XCLBIN_ELEMENT_INDEX_OUT_OF_RANGE;
	}


	if (retval == XLNX_OK)
	{
		pElement = &(pMemTopologySection->m_mem_data[elementIndex]);

		memcpy(pElementBuffer, pElement, sizeof(struct mem_data));
		*pElementSize = sizeof(struct mem_data);
	}



	return retval;
}





uint32_t HWDeviceInterface::EmulationGetConnectivityElement(struct connectivity* pConnectivity, int elementIndex, void* pElementBuffer, size_t* pElementSize)
{
	uint32_t retval = XLNX_OK;
	void* pElement;

	if (elementIndex >= pConnectivity->m_count)
	{
		retval = XLNX_DEV_INTERFACE_ERROR_XCLBIN_ELEMENT_INDEX_OUT_OF_RANGE;
	}


	if (retval == XLNX_OK)
	{
		pElement = &(pConnectivity->m_connection[elementIndex]);

		memcpy(pElementBuffer, pElement, sizeof(struct connection));
		*pElementSize = sizeof(struct connection);
	}

	return retval;
}


