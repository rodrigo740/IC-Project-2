#!/bin/bash

printf 'Testing exercise 2-Part C, with all images from image folder\n'
echo "Compiling file"
g++ ex2c.cpp -o ex2c `pkg-config --cflags --libs opencv`
echo ""
folder="images"
for entry in "$folder"/*
do 
    echo "Testing image: $entry"
    echo ""
    ./ex2c "$entry" 8 8 8
    echo ""
done
