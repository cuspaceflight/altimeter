#!/usr/bin/env python3

import sys
import pvlib
import struct
import numpy as np
from scipy import signal
import matplotlib as mpl
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

    # Init Arrays
    time = np.zeros(num_samples, dtype=float)
    pressure = np.zeros(num_samples, dtype=float)
    height = np.zeros(num_samples, dtype=float)

    # Extract Pressure Readings in mBar
    for n in range(0, num_samples):
        log.seek(n*PACKET_SIZE)
        data = struct.unpack('<BI', log.read(5))
        if (data[0] == MESSAGE_PRESSURE):
            time[n] = data[1]/10000.0
            pressure[n] = struct.unpack('<I', log.read(4))[0]          
            print("Time = %.3f" % time[n], "s", "    Pressure = %.2f" % (pressure[n]/100.0), "mBar")


# Low Pass Filter Sampled Data
b, a = signal.butter(7, 2.5, fs=20.0)
press_filt = signal.filtfilt(b, a, pressure)

# Convert to Altitude using ISA Lookup Table
height = pvlib.atmosphere.pres2alt(press_filt)

# Plot Output
fig, axs = plt.subplots(2, 1)
color = 'tab:red'
axs[0].plot(time, press_filt/100.0, color=color)
axs[0].set_xlabel('Time')
axs[0].set_ylabel('Pressure (mBar)', color=color)
axs[0].yaxis.set_major_formatter(mpl.ticker.StrMethodFormatter('{x:.1f}'))
axs[0].grid(True)
color = 'tab:blue'
axs[1].plot(time, height, color=color)
axs[1].set_xlabel('Time')
axs[1].set_ylabel('Height (m)', color=color)
axs[1].grid(True)

fig.tight_layout()
plt.show()