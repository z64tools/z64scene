#!/bin/bash

LIST=$(find src/ -type f -name '*.c')

echo ${LIST}

for i in ${LIST}
do
  uncrustify -c /mnt/f/uncrustify.cfg --replace --no-backup $i
done