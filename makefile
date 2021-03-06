#设置编译选项
CC = gcc
CFLAGS = -g -D_REENTRANT -Wall -std=gnu99 -rdynamic 
INC = -I./inc -I./src -I./src/comm -I./lib/ecc -I./lib/openssl/include/
LIB = -lrt -pthread -lcrypto -lssl #-L./lib/

#源文件
SOURCES = $(wildcard *.c src/*.c src/comm/*.c src/encrypt/*.c lib/ecc/*.c)  #列出当前目录下所有.c文件 lib/libcstl-2.3/src/*.c

#目标依赖文件.o
OBJS = $(patsubst %.c,%.o,$(SOURCES))

#目标文件
all: a.out #debug

#编译目标依赖文件
%.o:%.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@ $(LIB)

#makefile调试
debug:
		@echo $(SOURCES)
		@echo $(OBJS)

a.out:$(OBJS)
	$(CC) $(CFLAGS) $(INC)   $(OBJS) -o a.out $(LIB)

.PHONY: clean
clean:
	rm -rf *.o $(OBJS) all
