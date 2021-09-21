
TARGET = logger
PUBLISH_DIR=./

#HEARDER=$(wildcard *.hxx)
HEARDER+=$(wildcard ../*.h)
SOURCE+=$(wildcard ../*.cpp)
OBJ=$(patsubst ../%.cpp,%.o,$(SOURCE))
SOURCES+=$(SOURCE)
OBJS+=$(OBJ)

INCL_PATH+=/usr/include/
INCL_PATH_FLAG=$(patsubst %,-I%,$(INCL_PATH))

LIB_PATH+=/lib64
LIB_PATH+=/usr/lib64

LIB_PATH_FLAG=$(patsubst %,-L%,$(LIB_PATH))

LIB_DYNAMIC+=dl
LIB_DYNAMIC+=pthread
LIB_DYNAMIC_FLAG=$(patsubst %,-l%,$(LIB_DYNAMIC))


LIB_STATIC_FLAG=$(patsubst %,-l%,$(LIB_STATIC))


CFLAGS+= -g -Wall -O3 $(INCL_PATH_FLAG) $(LIB_PATH_FLAG)
CPPFLAGS+= -g -std=c++0x -O3 -Wall -Wno-deprecated $(INCL_PATH_FLAG) $(LIB_PATH_FLAG)

CC=g++

all:$(TARGET)

$(TARGET):$(OBJS)
	$(CC) $(CPPFLAGS) $(OBJ) -o $(TARGET) -Wl,-Bstatic $(LIB_STATIC_FLAG) -Wl,-Bdynamic $(LIB_DYNAMIC_FLAG)

$(OBJS):$(SOURCES) $(HEARDER)
	$(CC) $(CPPFLAGS) -c  $(SOURCES)

tolib:
	rm main.o
	ar -rcs libagile_logger.a *.o

rebuild:clean all

clean:
	rm -f *.o
	rm -f *.a
	rm -f $(TARGET)
