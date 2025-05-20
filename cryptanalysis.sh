#! /usr/bin/bash

LABWORK_NUMBER="$1"

# gcc ./labwork${LABWORK_NUMBER}/main.c -o ./bin/labwork${LABWORK_NUMBER}.out
# ./bin/labwork${LABWORK_NUMBER}.out

# gcc -Wall -Werror ./labwork${LABWORK_NUMBER}/main.c -o ./bin/labwork${LABWORK_NUMBER}.out && ./bin/labwork${LABWORK_NUMBER}.out
make && ./bin/labwork${LABWORK_NUMBER}


# compile to shared for cpython: 
# gcc -shared -o ./labwork3/labwork3.so -fPIC ./labwork3/main.c
