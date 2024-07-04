#ifndef S2MM_ADDRESS_MAP_H
#define S2MM_ADDRESS_MAP_H


 /* The following are the standard kernel control registers */
#define S2MM_KERNEL_CONTROL_OFFSET						(0x00000000)
#define S2MM_KERNEL_GLOBAL_INTERRUPT_ENABLE_OFFSET		(0x00000004)
#define S2MM_KERNEL_IP_INTERRUPT_ENABLE_OFFSET			(0x00000008)
#define S2MM_KERNEL_IP_INTERRUPT_STATUS_OFFSET			(0x0000000C)

// #define S2MM_CTRL_OFFSET                                  (0x00000010)
// #define S2MM_RING_READ_BUFFER_HEAD_INDEX_OFFSET           (0x00000018)
// #define S2MM_RING_WRITE_BUFFER_TAIL_INDEX_OFFSET          (0x00000020)

// #define S2MM_THROTTLE_RATE_OFFSET                         (0x00000028)


// #define S2MM_STATUS_OFFSET                                (0x00000050)
// #define S2MM_RING_READ_BUFFER_TAIL_INDEX_OFFSET           (0x00000058)
// #define S2MM_TX_RESPONSE_INDEX_OFFSET                     (0x00000068)
// #define S2MM_RING_WRITE_BUFFER_HEAD_INDEX_OFFSET          (0x00000078)
// #define S2MM_NUM_RX_OP_OFFSET                             (0x00000080)

/* latency */
// #define S2MM_LATENCY_MIN_OFFSET                           (0x00000088)
// #define S2MM_LATENCY_MAX_OFFSET                           (0x00000090)
// #define S2MM_LATENCY_SUM_OFFSET                           (0x00000098)
// #define S2MM_LATENCY_CNT_OFFSET                           (0x000000A0)
// #define S2MM_CYCLES_PRE_OFFSET                            (0x000000A8)
// #define S2MM_CYCLES_POST_OFFSET                           (0x000000B0)

// #define S2MM_THROTTLE_COUNT_OFFSET                        (0x000000B8)
// #define S2MM_TROTTLE_EVENT_OFFSET                         (0x000000C0)

// #define S2MM_READ_BUFFER_ADDRESS_LOWER_WORD_OFFSET        (0x000000E0)
// #define S2MM_READ_BUFFER_ADDRESS_UPPER_WORD_OFFSET        (0x000000E4)
// #define S2MM_WRITE_BUFFER_ADDRESS_LOWER_WORD_OFFSET       (0x0000013C)
// #define S2MM_WRITE_BUFFER_ADDRESS_UPPER_WORD_OFFSET       (0x00000140)

#endif

