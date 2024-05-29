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

#ifndef XLNX_DEVICE_INTERFACE_ERROR_CODES_H
#define XLNX_DEVICE_INTERFACE_ERROR_CODES_H


#ifndef XLNX_OK
#define XLNX_OK													        (0x00000000)
#endif

#define XLNX_DEV_INTERFACE_ERROR_IO_FAILED						        (0x00000001)
#define XLNX_DEV_INTERFACE_ERROR_FAILED_TO_OPEN_FILE			        (0x00000002)
#define XLNX_DEV_INTERFACE_ERROR_FAILED_TO_LOCK_DEVICE			        (0x00000003)
#define XLNX_DEV_INTERFACE_ERROR_FAILED_TO_DOWNLOAD_BITSTREAM	        (0x00000004)
#define XLNX_DEV_INTERFACE_ERROR_INVALID_PARAMETER				        (0x00000005)
#define XLNX_DEV_INTERFACE_ERROR_NOT_IMPLEMENTED				        (0x00000006)
#define XLNX_DEV_INTERFACE_ERROR_NOT_SUPPORTED					        (0x00000007)
#define XLNX_DEV_INTERFACE_ERROR_CU_NAME_NOT_FOUND				        (0x00000008)
#define XLNX_DEV_INTERFACE_ERROR_CU_INDEX_NOT_FOUND				        (0x00000009)
#define XLNX_DEV_INTERFACE_ERROR_OFFSET_OUT_OF_RANGE_FOR_CU		        (0x0000000A)
#define XLNX_DEV_INTERFACE_ERROR_MEM_TOPOLOGY_INDEX_NOT_FOUND           (0x0000000B)
#define XLNX_DEV_INTERFACE_ERROR_STREAM_IDS_NOT_FOUND                   (0x0000000C)
#define XLNX_DEV_INTERFACE_ERROR_FAILED_TO_CREATE_STREAM                (0x0000000D)
#define XLNX_DEV_INTERFACE_ERROR_FAILED_TO_DESTROY_STREAM               (0x0000000E)
#define XLNX_DEV_INTERFACE_ERROR_WRITE_TO_STREAM_FAILED                 (0x0000000F)
#define XLNX_DEV_INTERFACE_ERROR_READ_FROM_STREAM_FAILED                (0x00000010)
#define XLNX_DEV_INTERFACE_ERROR_BITSTREAM_NOT_DOWNLOADED               (0x00000011)
#define XLNX_DEV_INTERFACE_ERROR_XCLBIN_SECTION_NOT_FOUND               (0x00000012)
#define XLNX_DEV_INTERFACE_ERROR_XCLBIN_SECTION_NOT_SUPPORTED           (0x00000013)
#define XLNX_DEV_INTERFACE_ERROR_XCLBIN_ELEMENT_INDEX_OUT_OF_RANGE      (0x00000014)
#define XLNX_DEV_INTERFACE_ERROR_STREAM_BUFFER_ALLOCATION_FAILED        (0x00000015)
#define XLNX_DEV_INTERFACE_ERROR_STREAM_BUFFER_DEALLOCATION_FAILED      (0x00000016)
#define XLNX_DEV_INTERFACE_ERROR_FAILED_TO_MAP_BUFFER                   (0x00000017)
#define XLNX_DEV_INTERFACE_ERROR_FAILED_TO_UNMAP_BUFFER                 (0x00000018)
#define XLNX_DEV_INTERFACE_ERROR_FAILED_TO_SYNC_BUFFER                  (0x00000019)
#define XLNX_DEV_INTERFACE_ERROR_POLL_STREAM_FAILED                     (0x0000001A)
#endif

