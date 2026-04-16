#ifndef DATA_PROTOCOL_HPP
#define DATA_PROTOCOL_HPP

#include <stdint.h>

#define AUDIO_BLOCK_SIZE 32
#define MAX_COMMANDS 8

struct CMD {
    char     cmd_id[3];    // CMD Type
    char     cmd_val[32];      // Argument
};

#pragma pack(push, 1) // Alignment
struct LogSoundPacket {
    uint16_t sync_word;  // Control start bytes
    
    // Audio Stream (PCM 16 / 24 bit in 32)
    int32_t  audio_l[AUDIO_BLOCK_SIZE];
    int32_t  audio_r[AUDIO_BLOCK_SIZE];
    
    // Sensors in R (Fixed point: float * 1000)
    int32_t  pressure;
    int32_t  light;
    int32_t  imu_accel[3]; // X, Y, Z
    int32_t  imu_gyro[3];  // Pitch, Roll, Yaw
    
    // CMDs
    CMD      cmds[MAX_COMMANDS];
    
    uint16_t crc16;        // Control final summ
};
#pragma pack(pop)

struct SubmissionPacket{
    uint16_t sync_word;

    int32_t  result;
    CMD      cmds[MAX_COMMANDS];

    uint16_t end_word;
};

#endif