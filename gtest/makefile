CC = gcc
CPP = g++
LINK = g++

LIBS = -lpthread
#编译.so must add -fPIC option  https://www.cnblogs.com/cswuyg/p/3830703.html
CCFLAGS = -g #-fPIC
CPPFLAGS = -g #-fPIC
LINKFLAGS = -Wall #-fPIC -shared

TARGET = base_code_tester

INCLUDES  = -I.
INCLUDES += -I./include
INCLUDES += -I./cases
INCLUDES += -I../inc

CPPFILES = $(wildcard	*.cc \
						src/gtest-all.cc \
						cases/*.cc \
						cases/data_struct/*.cc)
CFILES = $(wildcard	*.c \
					cases/*.c)

#OBJFILE = $(CFILES:.c=.o) $(CPPFILES:.cpp=.o) $(CPPFILES:.cc=.o)
OBJFILE =$(patsubst %.cc,%.o,$(CPPFILES))
OBJFILE +=$(patsubst %.c,%.o,$(CFILES))

$(warning OBJFILE:$(OBJFILE))

all:$(TARGET)

$(TARGET): $(OBJFILE)
	$(LINK) $^ $(INCLUDES) $(LIBS) $(LINKFLAGS) -L../lib -lbase_code -o $@
	
%.o:%.c
	$(CC) -o $@ -c $(CCFLAGS) $(INCLUDES) $< 

%.o:%.cc
	$(CPP) -o $@ -c $(CPPFLAGS)  $(INCLUDES) $< 

%.o:%.cpp
	$(CPP) -o $@ -c $(CPPFLAGS) $(INCLUDES) $<

install:
	tsxs -i -o $(TARGET)

clean:
	rm -rf $(TARGET)
	rm -rf $(OBJFILE)
	make -C .. clean