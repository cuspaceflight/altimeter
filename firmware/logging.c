#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "osal.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "microsd.h"
#include "logging.h"
#include "status.h"

#define LOG_MEMPOOL_ITEMS 64
#define LOG_CACHE_SIZE    512

/* Function Prototypes */
static void mem_init(void);
static void _log(altimeter_log *packet);

/* Log cache to ensure that the SD card is 
 * written to one page (512 bytes) at a time.
 */
static volatile char log_cache[LOG_CACHE_SIZE];

/* Memory pool to store incoming data 
 * before it is placed in the cache.
 */
static memory_pool_t log_mempool;

/* Mailbox for storing pointers to queued data */
static mailbox_t log_mailbox;

/* Statically allocated memory used for the memory pool */
static volatile char mempool_buffer[LOG_MEMPOOL_ITEMS * sizeof(altimeter_log)]
                     __attribute__((aligned(sizeof(stkalign_t))))
                     __attribute__((section(".ccm")));
                     
/* Statically allocated memory used for the queue in mailbox */
static volatile msg_t mailbox_buffer[LOG_MEMPOOL_ITEMS]
                      __attribute__((section(".ccm")));
                      

/* Datalogging Thread */
static THD_WORKING_AREA(logging_wa, 3072);
THD_FUNCTION(logging_thread, arg) {

    (void)arg;
    chRegSetThreadName("Logging");

    /* Packet Size */
    static const int packet_size = sizeof(altimeter_log);
    
    /* Pointer to Keep Track of Cache */
    volatile char* cache_ptr = log_cache;
    
    /* File System Variables */
    SDFS file_system;
    SDFILE file;
    SDRESULT write_res;
    SDRESULT open_res;
      
    /* Mailbox Variables */             
    msg_t mailbox_res;       
    intptr_t data_msg;     

    /* Initalise Memory */
    mem_init();
      
    /* Attempt to Open log_xxxxx.bin */
    while (microsd_open_file_inc(&file, "log", "bin", &file_system) != FR_OK);
    

    /* Begin Logging */
    while (true) {

        /* Wait for message to be avaliable */
        mailbox_res = chMBFetch(&log_mailbox, (msg_t*)&data_msg, MS2ST(100));

        /* Re-attempt if mailbox was reset or fetch failed */
        if (mailbox_res != MSG_OK || data_msg == 0) continue;

        /* Put packet in static cache and free from memory pool */
        memcpy((void*)cache_ptr, (void*)data_msg, packet_size);
        chPoolFree(&log_mempool, (void*)data_msg);

        /* Detect full cache and write to SD card */
        if(cache_ptr + packet_size >= log_cache + LOG_CACHE_SIZE) {
            
            /* Attempt to Write Cache */
            write_res = microsd_write(&file, (char*)log_cache, LOG_CACHE_SIZE);
                             
            while (write_res != FR_OK) {           
                
                /* Attempt to Re-open File */
                microsd_close_file(&file);
                open_res = microsd_open_file_inc(&file, "log", "bin", &file_system);
                
                if(open_res == FR_OK) {
                    
                    /* Re-attempt to Write Cache */
                    write_res = microsd_write(&file, (char*)log_cache, LOG_CACHE_SIZE);
                }
            }

            /* Reset Cache Pointer */
            cache_ptr = log_cache;        
        
        } else {
        
            /* Increment Cache Pointer */
            cache_ptr += packet_size;
        }
    }
}


/* Initialise Mailbox and Memorypool */
static void mem_init(void) {
    
    chMBObjectInit(&log_mailbox, (msg_t*)mailbox_buffer, LOG_MEMPOOL_ITEMS);
    chPoolObjectInit(&log_mempool, sizeof(altimeter_log), NULL);

    /* Fill Memory Pool with Statically Allocated Bits of Memory */
    chPoolLoadArray(&log_mempool, (void*)mempool_buffer, LOG_MEMPOOL_ITEMS);
}


/* Start Logging */
void logging_init(void) {

    /* Create Datalogging Thread */
    chThdCreateStatic(logging_wa, sizeof(logging_wa), NORMALPRIO, logging_thread, NULL);
}


/* Log a buffer */
void log_raw_pressure(uint32_t data) {
    
    /* Populate and log packet */
    altimeter_log pkt;
    pkt.type = TYPE_RAW_PRESSURE;
    pkt.timestamp = chVTGetSystemTime();
    memset(pkt.payload, 0, 11);
    memcpy(pkt.payload, &data, 4);
    _log(&pkt);
}


/* Pass a formatted packet to the logging thread */
static void _log(altimeter_log *packet) {

    void* msg;
    msg_t retval;

    /* Allocate space for the packet in the mempool */
    msg = chPoolAlloc(&log_mempool);
    if (msg == NULL) return;
    
    /* Copy the packet into the mempool */
    memcpy(msg, (void*)packet, sizeof(altimeter_log));
    
    /* Post the location of the packet into the mailbox */
    retval = chMBPost(&log_mailbox, (intptr_t)msg, TIME_IMMEDIATE);
    if (retval != MSG_OK) {
        chPoolFree(&log_mempool, msg);
        return;
    }
}
