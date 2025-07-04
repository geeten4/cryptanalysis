#! /usr/bin/bash

LABWORK_NUMBER="$1"

make labwork${LABWORK_NUMBER} && ./bin/labwork${LABWORK_NUMBER}/labwork${LABWORK_NUMBER}


# compile to shared for cpython: 
# gcc -shared -o ./labwork3/labwork3.so -fPIC ./labwork3/main.c
