#!/bin/bash

DIR_PATH=${DIR_ENV}

array=$(ls ${DIR_PATH})
DIR_PATH=${DIR_PATH:8}
for i in ${array[*]}
do
    echo "<a href="${DIR_PATH}/${i}">${i}</a><br/>"
done
