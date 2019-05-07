#!/usr/bin/env python3

import sys
import pvlib
import struct
import numpy as np
import matplotlib.pyplot as plt

# Useage
if len(sys.argv) != 2:
    print("Usage: {} <logfile.bin>".format(sys.argv[0]))
    sys.exit(1)

# Message Type Definitions
MESSAGE_PRESSURE = 1
PACKET_SIZE = 16    
        
# Open log file
with open(sys.argv[1], 'rb') as log:

    log.read()
    num_samples = int(log.tell()/PACKET_SIZE)
    log.seek(0)
    print("File contains", num_samples, "samples:")

    time = np.zeros(num_samples, dtype=float)
    pressure = np.zeros(num_samples, dtype=float)
    height = np.zeros(num_samples, dtype=float)

    for n in range(0, num_samples):
        log.seek(n*PACKET_SIZE)
        data = struct.unpack('<BI', log.read(5))
        if (data[0] == MESSAGE_PRESSURE):
            time[n] = data[1]/10000.0
            pressure[n] = struct.unpack('<I', log.read(4))[0]
            height[n] = pvlib.atmosphere.pres2alt(pressure[n])
            print("Time = %.3f" % time[n], "s", "    Pressure = %.2f" % (pressure[n]/100.0), "mBar", "    Height = %.2f" % height[n], "m")


fig, ax1 = plt.subplots()
color = 'tab:red'
ax1.set_xlabel('Time (s)')
ax1.set_ylabel('Pressure (mBar)', color=color)
ax1.plot(time, pressure/100.0, color=color)
ax1.tick_params(axis='y', labelcolor=color)

ax2 = ax1.twinx()
color = 'tab:blue'
ax2.set_ylabel('Height', color=color)
ax2.plot(time, height, color=color)
ax2.tick_params(axis='y', labelcolor=color)
fig.tight_layout()
plt.show()