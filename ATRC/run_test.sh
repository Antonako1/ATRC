#!/bin/bash
gcc -I include test.c -o test.elf
if [ $? -eq 0 ]; then
    echo "Compilation successful."
else
    echo "Compilation failed."
    exit 1
fi
rm -f *.o
./test.elf