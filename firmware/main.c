#include "ch.h"
#include "hal.h"
#include "measure.h"
#include "status.h"
#include "logging.h"

/* Application Entry Point */
int main(void){

    /* Allow debug access during WFI sleep */
    DBGMCU->CR |= DBGMCU_CR_DBG_SLEEP;

    /* Initialise ChibiOS */
    halInit();
    chSysInit();
    
    /* Start Status Monitor */
    begin_status();
    
    /* Start Datalogger */
    logging_init();

    /* Start Measurement Handler*/
    begin_measurements();

    /* Do Nothing */
    while (true) {
    }
}
