#ifndef MM2S_ADDRESS_MAP_H
#define MM2S_ADDRESS_MAP_H


 /* The following are the standard kernel control registers */
#define MM2S_KERNEL_CONTROL_OFFSET						(0x00000000)
#define MM2S_KERNEL_GLOBAL_INTERRUPT_ENABLE_OFFSET		(0x00000004)
#define MM2S_KERNEL_IP_INTERRUPT_ENABLE_OFFSET			(0x00000008)
#define MM2S_KERNEL_IP_INTERRUPT_STATUS_OFFSET			(0x0000000C)

// #define MM2S_CTRL_OFFSET                                  (0x00000010)
// #define MM2S_RING_READ_BUFFER_HEAD_INDEX_OFFSET           (0x00000018)
// #define MM2S_RING_WRITE_BUFFER_TAIL_INDEX_OFFSET          (0x00000020)

// #define MM2S_THROTTLE_RATE_OFFSET                         (0x00000028)


// #define MM2S_STATUS_OFFSET                                (0x00000050)
// #define MM2S_RING_READ_BUFFER_TAIL_INDEX_OFFSET           (0x00000058)
// #define MM2S_TX_RESPONSE_INDEX_OFFSET                     (0x00000068)
// #define MM2S_RING_WRITE_BUFFER_HEAD_INDEX_OFFSET          (0x00000078)
// #define MM2S_NUM_RX_OP_OFFSET                             (0x00000080)

/* latency */
// #define MM2S_LATENCY_MIN_OFFSET                           (0x00000088)
// #define MM2S_LATENCY_MAX_OFFSET                           (0x00000090)
// #define MM2S_LATENCY_SUM_OFFSET                           (0x00000098)
// #define MM2S_LATENCY_CNT_OFFSET                           (0x000000A0)
// #define MM2S_CYCLES_PRE_OFFSET                            (0x000000A8)
// #define MM2S_CYCLES_POST_OFFSET                           (0x000000B0)

// #define MM2S_THROTTLE_COUNT_OFFSET                        (0x000000B8)
// #define MM2S_TROTTLE_EVENT_OFFSET                         (0x000000C0)

// #define MM2S_READ_BUFFER_ADDRESS_LOWER_WORD_OFFSET        (0x000000E0)
// #define MM2S_READ_BUFFER_ADDRESS_UPPER_WORD_OFFSET        (0x000000E4)
// #define MM2S_WRITE_BUFFER_ADDRESS_LOWER_WORD_OFFSET       (0x0000013C)
// #define MM2S_WRITE_BUFFER_ADDRESS_UPPER_WORD_OFFSET       (0x00000140)

#endif

