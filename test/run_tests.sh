#!/bin/bash
for f in *.out;
do
  echo "Running $f ..."
  ./$f
done
