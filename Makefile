INCLUDE_DIR := ./inc
SRC_PATH := ./src
CFLAG = -I${INCLUDE_DIR} -Wall -O2
CC = /usr/bin/gcc
LDLIBS = -lm -lrt

vpath %.c ${SRC_PATH}

.PHONY: all clean install
ALL = base_task_linux calib_stage1 calib_stage2 stats_stage1 stats_stage2

all: ${ALL}

clean:
	rm -rf ${ALL} *.o

install:
	cp ${ALL} ./bin

%.o: %.c
	${CC} ${CFLAG} -c $^ -o $@

base_task_linux: base_task_linux.o tracing.o
	${CC} ${CFLAG} $^ ${LDLIBS} -o $@

calib_stage1: calib_stage1.o tracing.o
	${CC} ${CFLAG} $^ ${LDLIBS} -o $@

calib_stage2: calib_stage2.o
	${CC} ${CFLAG} $^ ${LDLIBS} -o $@

stats_stage1: stats_stage1.o tracing.o
	${CC} ${CFLAG} $^ ${LDLIBS} -o $@

stats_stage2: stats_stage2.o
	${CC} ${CFLAG} $^ ${LDLIBS} -o $@

Exp001: base_task_linux
	cscope -Rb
	cp base_task_linux /home/NFS_Share/ModeManagerBins/Exp001/Bin

Exp002: base_task_linux
	cscope -Rb
	cp base_task_linux /home/NFS_Share/ModeManagerBins/Exp002/Bin

Calib: calib_stage1 stats_stage1 calib_stage2 stats_stage2
	cscope -Rb
	cp calib_stage1 /home/lihaoran/Calib_Stage1
	cp stats_stage1 /home/lihaoran/Calib_Stage1
	cp calib_stage2 /home/lihaoran/Calib_Stage2
	cp stats_stage2 /home/lihaoran/Calib_Stage2
