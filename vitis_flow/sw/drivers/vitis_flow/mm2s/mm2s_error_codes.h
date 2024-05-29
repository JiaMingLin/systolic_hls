#ifndef MM2S_ERROR_CODES_H
#define MM2S_ERROR_CODES_H


#include <cstdint>


#ifndef XLNX_OK
#define XLNX_OK											(0x00000000)
#endif

#define MM2S_ERROR_NOT_INITIALISED			            (0x00000001)
#define MM2S_ERROR_IO_FAILED					        (0x00000002)
#define MM2S_ERROR_INVALID_PARAMETER         	        (0x00000003)
#define MM2S_ERROR_CU_NAME_NOT_FOUND			        (0x00000004)
#define MM2S_ERROR_CU_INDEX_NOT_FOUND			        (0x00000005)
#define MM2S_ERROR_FAILED_TO_ALLOCATE_BUFFER_OBJECT     (0x00000006)
#define MM2S_ERROR_FAILED_TO_MAP_BUFFER_OBJECT          (0x00000007)
#define MM2S_ERROR_FAILED_TO_SYNC_BUFFER_OBJECT         (0x00000008)
















#endif
