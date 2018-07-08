#!/bin/bash

if [ "$1" = "clean" ];then
{
	make clean
}
else
{
	make -j 5 #multi thread make
}

#生成文件转移到output
mv *.o ./output
mv src/*.o ./output
mv src/comm/*.o ./output
#mv *.out ./output

fi
