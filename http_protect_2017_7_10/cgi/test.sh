#!/bin/bash

array=$(ls /home)

for i in ${array[*]}
do
    echo $i
done

