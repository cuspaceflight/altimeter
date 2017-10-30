#ifndef MS5611_H
#define MS5611_H

#include "hal.h"

/* MS5611 Config */
typedef struct __attribute__((packed)) {

    uint16_t c1;        /* SENS */
    uint16_t c2;        /* OFF */
    uint16_t c3;        /* TCS */
    uint16_t c4;        /* TCO */
    uint16_t c5;        /* TREF */
    uint16_t c6;        /* TEMPSENS */
    
} ms5611_config;

/* Configure the MS5611 Barometer */
void ms5611_configure(I2CDriver* i2cd);

/* Returns the current pressure */
int32_t get_pressure(void);

#endif
