#include "stm_gateway.hpp"
#include <string.h>
#include "driver/spi_slave.h"

#include "esp_log.h"

static const char *TAG = "STM_GATEWAY";

LogSoundPacket tx_packet __attribute__((aligned(32)));

// CRC16-CCITT
uint16_t calculate_crc(LogSoundPacket* packet, int32_t size) {
    uint16_t crc = 0xFFFF;
    uint8_t* data = (uint8_t*)packet;
    for (int i = 0; i < size; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000) crc = (crc << 1) ^ 0x1021;
            else crc <<= 1;
        }
    }
    return crc;
}

void parse_music(const int16_t* stream, int32_t* out_l, int32_t* out_r) {
    for (int i = 0; i < AUDIO_BLOCK_SIZE; i++) {
        out_l[i] = (int32_t)stream[i * 2] << 16;
        out_r[i] = (int32_t)stream[i * 2 + 1] << 16;
    }
}

void compile_data(int32_t* data_r, int32_t* data_l, float press, float light_f, float* imu_gyro_accel, CMD* commands) {
    // Audio
    memcpy(tx_packet.audio_r, data_r, sizeof(int32_t) * AUDIO_BLOCK_SIZE);
    memcpy(tx_packet.audio_l, data_l, sizeof(int32_t) * AUDIO_BLOCK_SIZE);

    // Sensors
    tx_packet.pressure = (int32_t)(press * 1000.0f);
    tx_packet.light    = (int32_t)(light_f * 1000.0f);

    // IMU (3 Accel + 3 Gyro)
    for(int i = 0; i < 3; i++) {
        tx_packet.imu_accel[i] = (int32_t)(imu_gyro_accel[i] * 1000.0f);
        tx_packet.imu_gyro[i]  = (int32_t)(imu_gyro_accel[i + 3] * 1000.0f);
    }

    // Commands
    for(int i = 0; i < MAX_COMMANDS; i++) {
        tx_packet.cmds[i] = commands[i];
    }

    // Start bytes
    tx_packet.sync_word = 0xAA55;
    // CRC16
    tx_packet.crc16 = calculate_crc(&tx_packet, sizeof(LogSoundPacket) - 2);
}

SubmissionPacket rx_packet __attribute__((aligned(32)));

void send_data() {
    spi_slave_transaction_t t = {};
    
    t.length = sizeof(LogSoundPacket) * 8; 
    t.tx_buffer = &tx_packet;
    t.rx_buffer = &rx_packet; 

    esp_err_t ret = spi_slave_transmit(SPI3_HOST, &t, portMAX_DELAY);

    if (ret == ESP_OK) {
        if (rx_packet.sync_word == 0xAA55) {
            // Send commands into main process
        }
    }
}

void stm_gateway_init(){
    spi_bus_config_t buscfg = {
        .mosi_io_num = STM_MOSI,
        .miso_io_num = STM_MISO,
        .sclk_io_num = STM_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };

    spi_slave_interface_config_t slvcfg = {
        .spics_io_num = STM_CS,
        .flags = 0,
        .queue_size = 3,
        .mode = 0, // SPI Mode 0 (CPOL=0, CPHA=0)
    };

    // SPI3 (VSPI) Slave & DMA
    esp_err_t ret = spi_slave_initialize(SPI3_HOST, &buscfg, &slvcfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        //ESP_LOGE(TAG, "Failed to init SPI Slave: %s", esp_err_to_name(ret));
    }
}