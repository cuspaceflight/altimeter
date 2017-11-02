#ifndef LOGGING_H
#define LOGGING_H

#define TYPE_BUFFER     0x01

/* Log Message */
typedef struct __attribute__((packed)) {

    uint8_t type;
    systime_t timestamp;
    uint8_t payload[123]; 
    
} altimeter_log;


/* Logging Functions */
void log_buffer(uint8_t *data, size_t len);

/* Start Logging Thread */
void logging_init(void);

#endif
