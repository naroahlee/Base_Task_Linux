INCLUDE_DIR := ./inc
SRC_PATH := ./src
CFLAG = -I${INCLUDE_DIR} -Wall -O2
CC = /usr/bin/gcc
LDLIBS = -lm -lrt

vpath %.c ${SRC_PATH}

.PHONY: all clean install
ALL = base_task_linux calibration stats

all: ${ALL}

clean:
	rm -rf ${ALL} *.o

install:
	cp ${ALL} ./bin

%.o: %.c
	${CC} ${CFLAG} -c $^ -o $@

base_task_linux: base_task_linux.o tracing.o
	${CC} ${CFLAG} $^ ${LDLIBS} -o $@

calibration: calibration.o
	${CC} ${CFLAG} $^ ${LDLIBS} -o $@

stats: stats.o
	${CC} ${CFLAG} $^ ${LDLIBS} -o $@


Exp001: base_task_linux
	cscope -Rb
	cp base_task_linux /home/NFS_Share/ModeManagerBins/Exp001/Bin

Exp002: base_task_linux
	cscope -Rb
	cp base_task_linux /home/NFS_Share/ModeManagerBins/Exp002/Bin

Calib: calibration stats
	cscope -Rb
	cp calibration /home/NFS_Share/Calib
	cp stats       /home/NFS_Share/Calib
