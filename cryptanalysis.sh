#! /usr/bin/bash

LABWORK_NUMBER="$1"

gcc ./labwork${LABWORK_NUMBER}/main.c -o ./bin/labwork${LABWORK_NUMBER}.out
./bin/labwork${LABWORK_NUMBER}.out
