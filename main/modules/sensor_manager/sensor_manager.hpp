#ifndef SENSOR_MANAGER
#define SENSOR_MANAGER

#include <stdint.h>

/* --- User Constants ---*/
#define BTN_PIN 0

#define ENC_X 0
#define ENC_Y 0
#define ENC_BTN 0

#define I2C_SDA 0
#define I2C_SCK 0
/* ---                ---*/

/* --- User Variables ---*/

/* ---                ---*/

/* --- User Functions ---*/
float get_pressure();
float get_light();
/* ---                ---*/

#endif