#!/bin/bash

mkdir -p bin
gcc -std=c11 -Wno-unused-result `llvm-config --cflags` compiler/*.c `llvm-config --ldflags --libs core analysis native bitwriter --system-libs` -lstdc++ -lm -o bin/summus
