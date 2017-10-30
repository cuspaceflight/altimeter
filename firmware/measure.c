#include "ch.h"
#include "hal.h"
#include "ms5611.h"
#include "measure.h"


/* Measurements Thread */ 
static THD_WORKING_AREA(meas_thd_wa, 1024);
static THD_FUNCTION(meas_thd, arg) {
    
    (void)arg;
    uint32_t pressure;  
  
    /* For now just measure pressure */
    ms5611_configure(&I2CD1);
    
    while(true) {
    
        chThdSleepMilliseconds(80); 
        pressure = get_pressure();
    }
    
    (void) pressure;
}
    
    
/* Start Thread */
void begin_measurements(void) {

    chThdCreateStatic(meas_thd_wa, sizeof(meas_thd_wa), HIGHPRIO, meas_thd, NULL);
}
