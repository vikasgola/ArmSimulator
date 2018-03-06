#! /bin/bash

rm simulator

echo "Compiling..."
g++ main.cpp -std=c++11 -o simulator

if [ $? -eq 0 ]; then
    echo "Compiled succesfully."
else
    echo "Compilation Error."
    exit
fi

echo "Coming Online..."

echo "=======================Program Started=============================="

./simulator

echo "=======================Program Finished=============================="

echo "Going Offline."
echo "Bye!!!"
