#!/bin/bash

echo -e "Select version:\n\t 1.Basic \n\t 2.Pro(without gui) \n\t 3.Pro(with gui)\n(1/2/3): \b"
read version

version_name=""

if [[ version -eq 1 ]]; then
    rm Basic-simulator
    echo "Compiling..."
    g++ source-code/basic/main.cpp -std=c++11 -o Basic-simulator
    version_name="Basic-simulator"
else if [[ version -eq 2 ]];then
    rm Pro-without-GUI
    echo "Compiling..."
    g++ source-code/pro\(without-gui\)/main.cpp -std=c++11 -o Pro-without-GUI
    version_name="Pro-without-GUI"
    else if [[ version -eq 3 ]]; then
        rm Pro
        echo "Compiling..."
        g++ source-code/pro/main.cpp -std=c++11 -o Pro
        version_name="Pro"
        fi
    fi
fi


if [ $? -eq 0 ]; then
    echo "Compiled succesfully."
else
    echo "Compilation Error."
    exit
fi

echo "Coming Online..."

echo "=======================Program Started=============================="

./$version_name

echo "=======================Program Finished=============================="

echo "Going Offline."
echo "Bye!!!"
