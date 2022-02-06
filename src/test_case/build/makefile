
TARGET = agile_logger_test
SRC_PATH = ../

DIRS = $(shell find $(SRC_PATH) -maxdepth 10 -type d)

HEADER_FILES = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.h))
CPP_FILES = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cpp))

SOURCE_FILES = $(notdir $(CPP_FILES))
OBJ = $(patsubst %.cpp, %.o, $(SOURCE_FILES))

HEARDERS += $(HEADER_FILES)
SOURCES += $(CPP_FILES)
OBJS += $(OBJ)

INCL_PATH += /usr/include
INCL_PATH += ../../../include
INCL_PATH_FLAG = $(patsubst %,-I%,$(INCL_PATH))

LIB_PATH += ../../../lib
LIB_PATH_FLAG = $(patsubst %,-L%,$(LIB_PATH))

LIB_DYNAMIC += dl
LIB_DYNAMIC += pthread
LIB_DYNAMIC_FLAG = $(patsubst %,-l%,$(LIB_DYNAMIC))

LIB_STATIC += agile_logger
LIB_STATIC_FLAG = $(patsubst %,-l%,$(LIB_STATIC))


CPPFLAGS += -g -std=c++0x -O3 -Wall -Wno-deprecated $(INCL_PATH_FLAG) $(LIB_PATH_FLAG)

CC = g++

all:$(TARGET) 
	cp -rf $(TARGET) ../../../bin

$(TARGET):$(OBJS)
	$(CC) $(CPPFLAGS) $(OBJ) -o $(TARGET) -W $(LIB_DYNAMIC_FLAG) $(LIB_STATIC_FLAG)

$(OBJS):$(SOURCES) $(HEARDER)
	$(CC) $(CPPFLAGS) -c  $(SOURCES)

clean:
	rm -f *.o
	rm -f *.a
	rm -f $(TARGET)

rebuild:clean all