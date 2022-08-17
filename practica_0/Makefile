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

all: clean compile send

clean:
	rm -f bin/*
	ssh robot@${IP} "rm /home/robot/${PRACT_FOLDER}/a.out"

compile:
	${CC} ${SRC_DIR}/helloWorld.c -o ${BIN_DIR}/a.out -L../lib/ -lev3c-native -I../include/

send:
	scp ${BIN_DIR}/a.out robot@${IP}:/home/robot/${PRACT_FOLDER}
# @@echo ${current_dir}

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