#!/bin/bash

if [ "$1" = "clean" ];then
{
	make clean
}
else
{
	make -j 50 #multi thread make
}

#�ƶ�.o�ļ�
mv *.o ./output
mv src/*.o ./output
mv src/comm/*.o ./output
mv src/encrypt/*.o ./output
#mv *.out ./output

fi
