#include "ch.h"
#include "hal.h"

/* Status Thread */ 
static THD_WORKING_AREA(waSTAT, 128);
static THD_FUNCTION(STAT, arg) {
  
    (void)arg;
    chRegSetThreadName("status");    
    while (true) {
        
        palSetPad(GPIOB, GPIOB_STAT);
        //palSetPad(GPIOB, GPIOB_BEEP);
        chThdSleepMilliseconds(100);
        
        palClearPad(GPIOB, GPIOB_STAT);
        //palClearPad(GPIOB, GPIOB_BEEP);    
        chThdSleepMilliseconds(500);
    }
}

void begin_status(void){

    chThdCreateStatic(waSTAT, sizeof(waSTAT), NORMALPRIO, STAT, NULL);
}
