#!/usr/bin/env python3

import sys
import struct

# Useage
if len(sys.argv) != 2:
    print("Usage: {} <logfile.bin>".format(sys.argv[0]))
    sys.exit(1)

# Message Type Definitions
MESSAGE_PRESSURE = 1        
        
# Open log file
with open(sys.argv[1], 'rb') as log:

    # Read File
    log.read();

    # File pointer
    i = 0
    num_bytes = log.tell()
    
    # Loop until EOF
    while i in range(num_bytes):
        
        # Seek to next log
        log.seek(i)
        
        # Read Metadata
        header = log.read(5)    
        
        # Get Message Metadata
        meta_data = struct.unpack('<BI', header)
        log_type = meta_data[0]
        systick = meta_data[1]
        systick /= 10000.0
        
        # Read Raw Pressure 
        if (log_type == MESSAGE_PRESSURE):
                       
            payload = log.read(4)
            pressure = struct.unpack('<I', payload)
            print("Time = ", systick, "s", "    Pressure = ", pressure[0]/100.0, "mBar")
   
        # Increment file pointer
        i += 128
