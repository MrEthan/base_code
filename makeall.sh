#!/bin/bash

if [ "$1" = "clean" ];then
{
	make clean
}
else
{
	make -j 5 #multi thread make
}

#�����ļ�ת�Ƶ�output
mv *.o ./output
#mv *.out ./output

fi
