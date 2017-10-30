#include "ch.h"
#include "hal.h"
#include "ms5611.h"

/* MS5611 I²C address */
#define MS5611_ADDR             (0x77)


/* MS5611  I²C registers */
#define MS5611_RESET            (0x1E)
#define MS5611_D1_CONVERSION    (0x48)
#define MS5611_D2_CONVERSION    (0x58)
#define MS5611_READ_ADC         (0x00)
#define MS5611_PROM_C1          (0xA2)
#define MS5611_PROM_C2          (0xA4)
#define MS5611_PROM_C3          (0xA6)
#define MS5611_PROM_C4          (0xA8)
#define MS5611_PROM_C5          (0xAA)
#define MS5611_PROM_C6          (0xAC)


/* Global Variables */
static I2CDriver* ms5611_i2cd;
ms5611_config cfg;


/* Prototypes */
void ms5611_reset(void);
void ms5611_d1_conv(void);
void ms5611_d2_conv(void);
void ms5611_read_prom(void);
uint32_t ms5611_read_adc(void);
static bool ms5611_write(uint8_t cmd);
static bool ms5611_read(uint8_t* rxbuf, uint8_t n);


/* I2C Config */
static const I2CConfig i2c_cfg = {
    OPMODE_I2C,
    10000,
    STD_DUTY_CYCLE,
};


/* MS5611 Config */
void ms5611_configure(I2CDriver* i2cd) {

    ms5611_i2cd = i2cd;
    i2cStart(ms5611_i2cd, &i2c_cfg);
    
    /* Reset Barometer */
    ms5611_reset();
    
    /* Read PROM */
    ms5611_read_prom();
}


/* Calculate Pressure */
int32_t get_pressure(void) {

    uint32_t D1, D2;
    
    /* Request d1 & d2 */
    ms5611_d1_conv();
    D1 = ms5611_read_adc();
    ms5611_d2_conv();
    D2 = ms5611_read_adc();

    /* Compute Absolute Pressure */
    int64_t DT = 0;
    int64_t TEMP = 0;
    int64_t OFF = 0;
    int64_t TMP_OFF = 0;
    int64_t SENS = 0;
    int64_t TMP_PRESSURE = 0;
    int32_t PRESSURE = 0;
    
    /* Eqn 1 */
    DT = (D2 - (cfg.c5 * 256));
    
    /* Eqn 2 */
    TEMP = (2000 + ((DT * cfg.c6)/8388608)); 

    /* Eqn 3 */
    TMP_OFF = ((cfg.c2 * 256) + ((cfg.c4 * DT)/32768));
    OFF = TMP_OFF * 256;
    
    /* Eqn 4 */
    SENS = ((cfg.c1 * 32768) + ((cfg.c3 * DT)/256));
    
    /* Eqn 5 */
    TMP_PRESSURE = ((D1 * SENS)/2097152);
    PRESSURE = ((TMP_PRESSURE - OFF)/32768);

    (void)TEMP;
    return PRESSURE;
}


/* Reset MS5611 */
void ms5611_reset(void) {

    ms5611_write(MS5611_RESET);
    chThdSleepMilliseconds(200);   
}


/* Read PROM */
void ms5611_read_prom(void) {

    uint8_t prom_tmp[2] = {0};

    /* Get C1 */
    ms5611_write(MS5611_PROM_C1);
    ms5611_read(prom_tmp, 2);

    cfg.c1 = (prom_tmp[0] << 8 | prom_tmp[1]);

    /* Get C2 */
    ms5611_write(MS5611_PROM_C2);
    ms5611_read(prom_tmp, 2);

    cfg.c2 = (prom_tmp[0] << 8 | prom_tmp[1]);

    /* Get C3 */
    ms5611_write(MS5611_PROM_C3);
    ms5611_read(prom_tmp, 2);

    cfg.c3 = (prom_tmp[0] << 8 | prom_tmp[1]);
    
    /* Get C4 */
    ms5611_write(MS5611_PROM_C4);
    ms5611_read(prom_tmp, 2);

    cfg.c4 = (prom_tmp[0] << 8 | prom_tmp[1]);


    /* Get C5 */
    ms5611_write(MS5611_PROM_C5);
    ms5611_read(prom_tmp, 2);

    cfg.c5 = (prom_tmp[0] << 8 | prom_tmp[1]);


    /* Get C6 */
    ms5611_write(MS5611_PROM_C6);
    ms5611_read(prom_tmp, 2);

    cfg.c6 = (prom_tmp[0] << 8 | prom_tmp[1]);
}


/* Read ADC */
uint32_t ms5611_read_adc(void) {

    uint8_t retval[3] = {0};

    ms5611_write(MS5611_READ_ADC);
    ms5611_read(retval, 3);
    
    return ((retval[0] << 16) | ((retval[1] << 8)) | (retval[2]));
}


/* Initiate D1 Conversion */
void ms5611_d1_conv(void) {

    ms5611_write(MS5611_D1_CONVERSION);
    chThdSleepMilliseconds(9); 
}

/* Initiate D2 Conversion */
void ms5611_d2_conv(void) {

    ms5611_write(MS5611_D2_CONVERSION);
    chThdSleepMilliseconds(9);         
}


/* Transmit one byte over I2C */
static bool ms5611_write(uint8_t cmd)
{
    static uint8_t buf[1];
    buf[0] = cmd;
    
    msg_t result = i2cMasterTransmitTimeout(
        ms5611_i2cd, MS5611_ADDR, buf, 1, NULL, 0, MS2ST(20));

    if(result == MSG_OK) {
        return true;
    } else if(result == MSG_RESET) {
        i2cflags_t i2c_errs = i2cGetErrors(ms5611_i2cd);
        (void)i2c_errs;
        return false;
    } else if(result == MSG_TIMEOUT) {
        return false;
    } else {
        return false;
    }
}

/* Read n bytes over I2C */
static bool ms5611_read(uint8_t* rxbuf, uint8_t n)
{
    msg_t result = i2cMasterReceiveTimeout(
        ms5611_i2cd, MS5611_ADDR, rxbuf, n, MS2ST(20));
        
    if(result == MSG_OK) {
        return true;
    } else if(result == MSG_RESET) {
        i2cflags_t i2c_errs = i2cGetErrors(ms5611_i2cd);
        (void)i2c_errs;
        return false;
    } else if(result == MSG_TIMEOUT) {
        return false;
    } else {
        return false;
    }
}
