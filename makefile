#设置编译选项
CC = @echo [CC] $@;gcc
CPP = @echo [CPP] $@;g++
AR = @echo [AR] $@;ar
CFLAGS = -g -O0 -D_REENTRANT -Wall -std=gnu99 -rdynamic
SHARED_FLAGS = -shared -fPIC

OUTPUT_DIR = output
LIB_DIR = lib

INC = -I./inc -I./src -I./src/comm -I./lib/ecc -I./lib/openssl/include/
LIB = -lrt -pthread -lcrypto -lssl -L./lib/
#源文件
LIB_SRC = $(wildcard src/*.c \
					 src/comm/*.c \
					 src/ipc/*.c \
					 src/select/*.c \
					 src/encrypt/*.c \
					 src/co_routines/*.c \
					 src/data_struct/*.c \
					 lib/ecc/*.c)  #列出当前目录下所有.c文件 lib/libcstl-2.3/src/*.c
BIN_SRC = $(wildcard *.c)

#目标依赖文件.o
LIB_OBJS = $(patsubst %.c, %.o, $(LIB_SRC))
BIN_OBJS = $(patsubst %.c, %.o, $(BIN_SRC))

LIB_TARGET = $(LIB_DIR)/libbase_code.so
LIB_TARGET_STATIC = $(LIB_DIR)/libbase_code.a
BIN_TARGET = a.out
GTEST_TARGET = gtester

#$(warning LIB_OBJS:$(LIB_OBJS))

#目标文件
all: $(LIB_TARGET_STATIC) $(BIN_TARGET) $(GTEST_TARGET) #debug $(LIB_TARGET)
	# make -C gtest

# 编译base_code库
lib:$(LIB_TARGET_STATIC) $(LIB_TARGET)

#makefile调试
debug:
	@echo LIB_SRC: $(LIB_SRC)
	@echo LIB_OBJS: $(LIB_OBJS)
	@echo LIB_TARGET: $(LIB_TARGET)
	@echo LIB_TARGET_STATIC: $(LIB_TARGET_STATIC)
	@echo BIN_SRC: $(BIN_SRC)
	@echo BIN_OBJS: $(BIN_OBJS)
	@echo BIN_TARGET: $(BIN_TARGET)
	@echo GTEST_SRC: $(GTEST_SRC)
	@echo GTEST_INC: $(GTEST_INC)
	@echo GTEST_OBJS: $(GTEST_OBJS)
	@echo GTEST_TARGET: $(GTEST_TARGET)

# 编译动态库
$(LIB_TARGET):$(LIB_OBJS)
	$(CC) $^ $(CFLAGS) $(SHARED_FLAGS) $(INC) $(LIB) -o $@

# 编译静态库
$(LIB_TARGET_STATIC):$(LIB_OBJS)
	$(AR) -rcs -o $@ $^

# 编译可执行文件
$(BIN_TARGET):$(BIN_OBJS)
	$(CC) $(CFLAGS) $^ $(INC) $(LIB) -L./lib -lbase_code -o $@

#编译目标依赖文件
%.o:%.c
	$(CC) $(CFLAGS) $(SHARED_FLAGS) $(INC) -c $< -o $@ $(LIB)

###################################################################
# gtest编译
GTEST_DIR = gtest
GTEST_INC = -I./$(GTEST_DIR)/include
GTEST_INC += -I./$(GTEST_DIR)/cases
GTEST_INC += -I./$(GTEST_DIR)/../inc
GTEST_LIB = -lpthread -L./lib -lbase_code

GTEST_SRC = $(wildcard	$(GTEST_DIR)/*.cc \
            			$(GTEST_DIR)/src/gtest-all.cc \
            			$(GTEST_DIR)/cases/*.cc \
            			$(GTEST_DIR)/cases/data_struct/*.cpp)
GTEST_OBJS = $(patsubst %.cc, %.o, $(patsubst %.cpp, %.o, $(GTEST_SRC)))

$(GTEST_TARGET):$(GTEST_OBJS)
	$(CPP) -g -Wall $^ $(GTEST_INC) $(GTEST_LIB) -L./lib -lbase_code -o $@

%.o:%.cc
	$(CPP) -g -Wall $(GTEST_INC) -c $< -o $@
%.o:%.cpp
	$(CPP) -g -Wall $(GTEST_INC) -c $< -o $@
###################################################################

.PHONY: clean
clean:
	rm -rf *.o $(LIB_OBJS) $(BIN_OBJS) $(LIB_TARGET) $(BIN_TARGET) $(GTEST_TARGET)
