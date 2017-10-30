#include "ch.h"
#include "hal.h"
#include "measure.h"


/* Heartbeat Thread */ 
static THD_WORKING_AREA(waHBT, 128);
static THD_FUNCTION(HBT, arg) {
  
    (void)arg;
    chRegSetThreadName("heartbeat");    
    while (true) {
        
        palSetPad(GPIOC, GPIOC_STAT_HBT);
        //palSetPad(GPIOC, GPIOC_BEEP);
        chThdSleepMilliseconds(100);
        
        palClearPad(GPIOC, GPIOC_STAT_HBT);
        //palClearPad(GPIOC, GPIOC_BEEP);    
        chThdSleepMilliseconds(500);
    }
}


/* Application Entry Point */
int main(void) {

    /* Allow debug access during WFI sleep */
    DBGMCU->CR |= DBGMCU_CR_DBG_SLEEP;

    /* Initialise ChibiOS */
    halInit();
    chSysInit();
    
    /* Start Measurements */
    begin_measurements();

    /* Heartbeat Init */
    chThdCreateStatic(waHBT, sizeof(waHBT), NORMALPRIO, HBT, NULL);

    /* Do Nothing */
    while (true) {
    }
}
