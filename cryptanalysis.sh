#! /usr/bin/bash

LABWORK_NUMBER="$1"

# Check if LABWORK_NUMBER is a number
if [[ "$LABWORK_NUMBER" =~ ^[0-9]+$ ]]; then
    make labwork${LABWORK_NUMBER} && ./bin/labwork${LABWORK_NUMBER}/labwork${LABWORK_NUMBER}
elif [[ "$LABWORK_NUMBER" == "final" ]]; then
    make labwork_${LABWORK_NUMBER} && ./bin/labwork_${LABWORK_NUMBER}/labwork_${LABWORK_NUMBER}
else
    echo "Error: LABWORK_NUMBER must be a number or 'final'"
    exit 1
fi
