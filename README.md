# CUSF Altimeter
### This project is a barebones altimeter for use in the freshers rocket challenge.

1. Setup the repository using the following commands:
```
$ git clone https://github.com/cuspaceflight/altimeter.git
$ cd altimeter
$ git submodule update --init
$ cd ChibiOs
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
