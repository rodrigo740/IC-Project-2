#!/bin/bash

printf 'Testing exercise 2-Part C, with all images from image folder and (7 4 4) levels of quantization (expecting minor differences between original and compressed image)\n'
echo "Compiling file"
g++ ex2c.cpp -o ex2c -lstdc++fs `pkg-config --cflags --libs opencv`
echo ""
folder="images"
i=1
for entry in "$folder"/*
do 
    echo "Testing image: $entry"
    echo "Output image: output$i.ppm"
    echo "Quantization levels 7 4 4"
    echo ""
    ./ex2c "$entry" "images/output$i.ppm" 7 4 4
    echo ""
    i=$((i+1))
done
