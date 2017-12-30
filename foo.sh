#!/bin/bash

for a in `find . -name *.in`;do
  b=`echo $a | sed 's/\.\/riscv/\/Users\/jleidel\/dev\/working\/xbgas-tools\/riscv/g'`
  echo copying $a to $b
  cp $a $b
done
