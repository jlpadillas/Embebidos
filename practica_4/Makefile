# --------------------------------------------------------------------------- #
# Directory where binaries are saved
BIN_DIR = bin
# Directory where libraries are saved
LIB_DIR = ../lib
# Directory where headers are saved
INC_DIR = ../include
# Directory where scripts are saved
SRC_DIR = src
# Flags used to compile c files
CFLAGS = -Wall -Werror
# Compiler to use
CC = arm-none-linux-gnueabi-gcc
# --------------------------------------------------------------------------- #
# Robot's IP
IP = 10.42.0.117
# Current Working Directory
MAKEFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
PWD := $(dir $(MAKEFILE_PATH))
PRACT_FOLDER := $(notdir $(patsubst %/,%,$(PWD)))

# --------------------------------------------------------------------------- #

all: clean load_cpu

clean:
	rm -f bin/*
	ssh -i ~/.ssh/ robot@${IP} "rm -f /home/robot/${PRACT_FOLDER}/*"

create_folder:
	ssh -i ~/.ssh/ robot@${IP} "mkdir -p /home/robot/${PRACT_FOLDER}"

load_cpu: create_folder
	${CC} ${SRC_DIR}/load_cpu.c -o ${BIN_DIR}/load_cpu -L${LIB_DIR} \
	-lev3c-cross -lev3c-addons -I${INC_DIR} -lm -lpthread
	scp ${BIN_DIR}/load_cpu robot@${IP}:/home/robot/${PRACT_FOLDER}

tick: create_folder
	${CC} ${SRC_DIR}/tick.c -o ${BIN_DIR}/tick -L${LIB_DIR} \
	-lev3c-cross -lev3c-addons -I${INC_DIR} -lm -lpthread
	scp ${BIN_DIR}/tick robot@${IP}:/home/robot/${PRACT_FOLDER}

send:
#	@@echo ${PWD}
#	@@echo ${PRACT_FOLDER}
# Creating the folder
	ssh -i ~/.ssh/ robot@${IP} "mkdir -p /home/robot/${PRACT_FOLDER}"
	scp ${BIN_DIR}/a.out robot@${IP}:/home/robot/${PRACT_FOLDER}

send_reset:
	ssh -i ~/.ssh/ robot@${IP} "mkdir -p /home/robot/${PRACT_FOLDER}"
	scp ${SRC_DIR}/reset_motors.sh robot@${IP}:/home/robot/${PRACT_FOLDER}

run:
	ssh -i ~/.ssh/ robot@${IP} "/home/robot/${PRACT_FOLDER}/a.out"
#	./bin/a.out



# CFLAGS=-Werror -Wall libev3c-native.a
# FILENAME=helloWorld
# DIR=/home/robot/practica_0

# all : clean $(FILENAME) run

# $(FILENAME) : $(FILENAME).c
# 	$(CC) -o $(DIR)/bin/$@ $^ $(CFLAGS)

# run:
# 	$(DIR)/bin/$(FILENAME)

# clean:
# 	rm -f $(DIR)/bin/$(FILENAME)