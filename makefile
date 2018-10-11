#���ñ���ѡ��
CC = gcc
CFLAGS = -g -D_REENTRANT -Wall -std=gnu99 -rdynamic 
INC = -I./inc -I./src -I./src/comm -I./lib/ecc -I./lib/openssl/include/
LIB = -lrt -pthread -lcrypto -lssl #-L./lib/

#Դ�ļ�
SOURCES = $(wildcard *.c src/*.c src/comm/*.c src/encrypt/*.c lib/ecc/*.c)  #�г���ǰĿ¼������.c�ļ� lib/libcstl-2.3/src/*.c

#Ŀ�������ļ�.o
OBJS = $(patsubst %.c,%.o,$(SOURCES))

#Ŀ���ļ�
all: a.out #debug

#����Ŀ�������ļ�
%.o:%.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@ $(LIB)

#makefile����
debug:
		@echo $(SOURCES)
		@echo $(OBJS)

a.out:$(OBJS)
	$(CC) $(CFLAGS) $(INC)   $(OBJS) -o a.out $(LIB)

.PHONY: clean
clean:
	rm -rf *.o $(OBJS) all
