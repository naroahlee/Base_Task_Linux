INCLUDE_DIR := ./inc
SRC_PATH := ./src
CFLAG = -I${INCLUDE_DIR}
CC = /usr/bin/gcc
LDLIBS = -lm

vpath %.c ${SRC_PATH}


.PHONY: all clean install
ALL = hello 

all: ${ALL}

clean:
	rm -rf ${ALL} *.o

install:
	cp ${ALL} ./bin

%.o: %.c
	${CC} ${CFLAG} -c $^ -o $@

hello: hello.o priority_queue.o
	${CC} ${CFLAG} $^ -o $@

