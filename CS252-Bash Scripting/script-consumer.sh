#!/bin/bash


for index in $(seq 100000); do
	value=$(awk "BEGIN {print $RANDOM * $index}")
	echo $value
    buffer[$index]=$value
done

