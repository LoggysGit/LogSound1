#ifndef STM_GATEWAY
#define STM_GATEWAY

#include <stdint.h>

#include "DataProtocol.hpp"

/* --- User Constants ---*/
#define STM_MOSI 23
#define STM_MISO 19
#define STM_SCLK 18
#define STM_CS   5
/* ---                ---*/

/* --- User Variables ---*/
extern LogSoundPacket tx_packet __attribute__((aligned(32)));
/* ---                ---*/

/* --- User Functions ---*/
// Calculate CRC16
uint16_t calculate_crc(LogSoundPacket* packet, int32_t size);
// Parse audio stream on 2 float* arrays - L and R
void parse_music(const int16_t* stream, int32_t* out_l, int32_t* out_r);
// Pack data in LogSoundPacket
void compile_data(int32_t* data_r, int32_t* data_l, float pressure, float light, float* gyro, CMD* commands);
// Send packet to STM
void send_data();
// Init
void stm_gateway_init();
/* ---                ---*/

#endif