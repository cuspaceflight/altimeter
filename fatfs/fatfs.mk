# FATFS files.
FATFSSRC = ${CHIBIOS}/os/various/fatfs_bindings/fatfs_diskio.c \
           ${CHIBIOS}/os/various/fatfs_bindings/fatfs_syscall.c \
           ../fatfs/src/ff.c \
           ../fatfs/src/option/unicode.c

FATFSINC = ../fatfs/src
