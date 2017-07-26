#!/bin/bash

DIR_PATH=${DIR_ENV}

array=$(ls ${DIR_PATH})
DIR_PATH=${DIR_PATH:7}
for i in ${array[*]}
do
    echo "<a href="http://${DIR_PATH}${i}/">${i}</a><br/>"
done
