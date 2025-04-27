#! /usr/bin/bash

if [ $1 = "labwork1" ]; then
    gcc ./labwork1/main.c -o ./bin/labwork1.out
    ./bin/labwork1.out
elif [ $1 = "labwork2" ]; then
    gcc ./labwork2/main.c -o ./bin/labwork2.out
    ./bin/labwork2.out
fi
