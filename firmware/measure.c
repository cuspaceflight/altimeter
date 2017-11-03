#include "ch.h"
#include "hal.h"
#include "ms5611.h"
#include "measure.h"
#include "logging.h"

/* Measurement Thread */ 
static THD_WORKING_AREA(meas_thd_wa, 1024);
static THD_FUNCTION(meas_thd, arg) {
    
    (void)arg;
    uint32_t pressure;
    
    /* Configure Pressure Sensor */
    ms5611_configure(&I2CD1);
    
    /* Log Raw Data to SD Card */
    while(true) {
     
        /* Read Pressure */
        pressure = get_pressure();     
        
        /* Log Pressure */
        log_raw_pressure(pressure);    
        
        /* Sleep */
        chThdSleepMilliseconds(5);
    }
}
    
/* Start Measuring */
void begin_measurements(void) {

    chThdCreateStatic(meas_thd_wa, sizeof(meas_thd_wa), HIGHPRIO, meas_thd, NULL);
}
