#!/bin/bash

echo $(pwd)
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:lib
./a.out
