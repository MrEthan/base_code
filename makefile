#设置编译选项
CC = gcc
CFLAGS = -g -D_REENTRANT -Wall
INC = -I./inc -I./src -I./lib/
LIB = -pthread -L./lib/

#源文件
SOURCES = $(wildcard *.c src/*.c )  #列出当前目录下所有.c文件 lib/libcstl-2.3/src/*.c

#目标依赖文件.o
OBJS = $(patsubst %.c,%.o,$(SOURCES))

#目标文件
all: debug a.out

#编译目标依赖文件
%.o:%.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@ $(LIB)

#makefile调试
debug:
		#@echo $(SOURCES)
		#@echo $(OBJS)

a.out:$(OBJS)
	$(CC) $(CFLAGS) $(INC)   $(OBJS) -o a.out $(LIB)

.PHONY: clean
clean:
	rm -rf *.o $(OBJS) all
