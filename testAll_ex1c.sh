#!/bin/bash

printf 'Testing exercise 1-Part C, with all images from image folder\n'
echo "Compiling file"
g++ ex1c.cpp -o ex1c -lstdc++fs `pkg-config --cflags --libs opencv`
echo ""
folder="images"
i=1
for entry in "$folder"/*
do 
    echo "Testing image: $entry"
    echo "Output image: output$i.ppm"
    echo ""
    ./ex1c "$entry" "images/output$i.ppm"
    echo ""
    i=$((i+1))
done
