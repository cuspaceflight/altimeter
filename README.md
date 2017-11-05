# CUSF Altimeter
### This project is a barebones altimeter for use in the freshers rocket challenge. The below instructions are specific to Linux systems only.

## Setting up the Repositiory

1. Setup the repository using the following commands:
```
$ git clone https://github.com/cuspaceflight/altimeter.git
$ cd altimeter
$ git submodule update --init
$ cd ChibiOS
$ git checkout stable_16.1.x
```
2. To succesfully compile firmware you will need to install gcc:
```
$ sudo apt-get install gcc-arm-none-eabi
```
3. Test the installation by compiling the firmware template:
```
$ cd ../firmware
$ make
```

## Analysing Log Files

1. Copy the file "log_0000x.bin" you wish to decode into the altimeter/logs directory

2. From the terminal run:
```
$ python3 log_file_parser.py log_0000x.bin
```

The output will print directly to the terminal window.

## Making Firmware Changes

This will be explained in depth in the sessions, however the files which you will want to modify (status.c, measurements.c and logging.c) can be found in the altimeter/firmware directory.

Compile using the command:
``` 
$ make
```
If successful something similar to the below should appear in the terminal window:

```
...
Compiling testbmk.c
Compiling main.c
Compiling ms5611.c
Compiling measure.c
Compiling status.c
Compiling microsd.c
Compiling logging.c
Linking build/altimeter.elf
Creating build/altimeter.bin
Creating build/altimeter.hex
Creating build/altimeter.dmp
Creating build/altimeter.list

   text	   data	    bss	    dec	    hex	filename
  43900	   8896	 122628	 175424	  2ad40	build/altimeter.elf

Done
```

### References

ChibiOs - http://www.chibios.org/dokuwiki/doku.php?id=chibios:documentation:start

GDB - https://github.com/blacksphere/blackmagic/wiki/Useful-GDB-commands
