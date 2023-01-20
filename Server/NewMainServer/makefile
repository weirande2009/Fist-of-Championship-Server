CC = g++

DIR_INC = ./include
DIR_SRC = ./source
DIR_OBJ = ./obj
DIR_BIN = ./bin

LIBS = -lprotobuf -lpthread -lmysqlclient

GAME = game
PROTOBUF = protobuf
SERVER = server
UTILITIES = utilities

GAME_DIR_SRC = $(DIR_SRC)/$(GAME)
PROTOBUF_DIR_SRC = $(DIR_SRC)/$(PROTOBUF)
SERVER_DIR_SRC = $(DIR_SRC)/$(SERVER)
UTILITIES_DIR_SRC = $(DIR_SRC)/$(UTILITIES)

GAME_SRC = $(wildcard $(GAME_DIR_SRC)/*.cc)
PROTOBUF_SRC = $(wildcard $(PROTOBUF_DIR_SRC)/*.cc)
SERVER_SRC = $(wildcard $(SERVER_DIR_SRC)/*.cc)
UTILITIES_SRC = $(wildcard $(UTILITIES_DIR_SRC)/*.cc)
MAIN_SRC = main.cc

GAME_OBJ = $(patsubst %.cc,%.o,$(notdir $(GAME_SRC))) 
PROTOBUF_OBJ = $(patsubst %.cc,%.o,$(notdir $(PROTOBUF_SRC))) 
SERVER_OBJ = $(patsubst %.cc,%.o,$(notdir $(SERVER_SRC)))
UTILITIES_OBJ = $(patsubst %.cc,%.o,$(notdir $(UTILITIES_SRC)))
MAIN_OBJ = main.o

OBJ = $(GAME_OBJ) $(PROTOBUF_OBJ) $(SERVER_OBJ) $(UTILITIES_OBJ) $(MAIN_OBJ)

TARGET = main

CFLAGS = -Wall -fdiagnostics-color=always -I${DIR_INC} -L/usr/local/lib/ $(LIBS) -std=c++17

all:$(TARGET)

$(TARGET):$(OBJ)
	$(CC) $^ $(CFLAGS) -o $(TARGET) 

$(GAME_OBJ):%.o:$(GAME_DIR_SRC)/%.cc
	$(CC) $(CFLAGS) -c $< -o $@

$(PROTOBUF_OBJ):%.o:$(PROTOBUF_DIR_SRC)/%.cc
	$(CC) $(CFLAGS) -c $< -o $@

$(SERVER_OBJ):%.o:$(SERVER_DIR_SRC)/%.cc
	$(CC) $(CFLAGS) -c $< -o $@

$(UTILITIES_OBJ):%.o:$(UTILITIES_DIR_SRC)/%.cc
	$(CC) $(CFLAGS) -c $< -o $@

$(MAIN_OBJ):%.o:%.cc
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY:clean

clean:
	rm -rf *.o \;


