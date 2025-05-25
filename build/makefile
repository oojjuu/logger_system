TARGET = agile
SRC_PATH = ../src/logger

DIRS = $(shell find $(SRC_PATH) -maxdepth 10 -type d)

HEADER_FILES = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.h))
CPP_FILES = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cpp))

SOURCE_FILES = $(notdir $(CPP_FILES))
OBJ = $(patsubst %.cpp, %.o, $(SOURCE_FILES))
# OBJ_FILES = $(patsubst %, tmp/%,$(OBJ))

HEARDERS += $(HEADER_FILES)
SOURCES += $(CPP_FILES)
OBJS += $(OBJ)

INCL_PATH += /usr/include/
INCL_PATH = ../src/logger/include/
INCL_PATH_FLAG = $(patsubst %,-I%,$(INCL_PATH))

#LIB_PATH
#LIB_PATH_FLAG = $(patsubst %,-L%,$(LIB_PATH))

LIB_DYNAMIC += dl
LIB_DYNAMIC += pthread
LIB_DYNAMIC_FLAG = $(patsubst %,-l%,$(LIB_DYNAMIC))

#LIB_STATIC
LIB_STATIC_FLAG = $(patsubst %,-l%,$(LIB_STATIC))

CPPFLAGS += -g -std=c++17 -O3 -Wall -Wno-deprecated $(INCL_PATH_FLAG)

CC = g++
AR = ar
LIBFLAGS := -rcs
LIBNAME = libagile_logger.a
define Tolib
	cp -rf libagile_logger.a ../lib/
	cp -rf ../src/logger/include/Logger.h ../include/
	cp -rf ../src/logger/include/LoggerAgile.h ../include/
	cp -rf ../src/logger/include/LoggerBuffer.h ../include/
	cp -rf ../src/logger/include/LoggerConfig.h ../include/
	cp -rf ../src/logger/include/LoggerLevel.h ../include/
	cp -rf ../src/logger/include/LoggerOutput.h ../include/
	rm -f libagile_logger.a
endef

all:$(TARGET) to_lib clean_test test_case

#$(TARGET):$(OBJS)
#	$(CC) $(CPPFLAGS) $(OBJ) -o $(TARGET) -W $(LIB_DYNAMIC_FLAG) $(LIB_STATIC_FLAG)
$(TARGET):$(OBJS) 
	$(AR) $(LIBFLAGS) $(LIBNAME) $(OBJS)

$(OBJS):$(SOURCES) $(HEARDER)
	$(CC) $(CPPFLAGS) -c $(SOURCES)

to_lib:
	$(call Tolib)

test_case:
	cd ../src/test_case/build/ && make

clean:
	rm -f *.o
	rm -f *.a
	rm -rf ../bin/*
	rm -f ../src/test_case/build/*.o
	rm -f ../src/test_case/build/agile_logger_test

clean_test:
	rm -rf ../bin/*
	rm -f ../src/test_case/build/*.o
	rm -f ../src/test_case/build/agile_logger_test

rebuild:clean all