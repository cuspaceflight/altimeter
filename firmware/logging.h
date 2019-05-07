#ifndef LOGGING_H
#define LOGGING_H

#define TYPE_RAW_PRESSURE     0x01

/* Log Message */
typedef struct __attribute__((packed)) {

    uint8_t type;
    systime_t timestamp;
    uint8_t payload[11]; 
    
} altimeter_log;


/* Logging Functions */
void log_raw_pressure(uint32_t data);

/* Start Logging Thread */
void logging_init(void);

#endif
