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
    uint8_t buffer[4];
    
    /* Configure Pressure Sensor */
    ms5611_configure(&I2CD1);
    
    /* Test Code */
    while(true) {
     
        pressure = get_pressure();
        
        int i = 0;
        while (i < 4) {
            buffer[i] = (pressure >> i*8);
        }
        
        log_buffer(buffer, 4);    
        
        chThdSleepMilliseconds(50);
    }
}
    

void begin_measurements(void) {

    chThdCreateStatic(meas_thd_wa, sizeof(meas_thd_wa), HIGHPRIO, meas_thd, NULL);
}
