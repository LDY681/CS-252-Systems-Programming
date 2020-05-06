#!/bin/bash
COUNT=10
if egrep -q [0-9] testdoc.txt ; then
    let COUNT=COUNT+5
fi
echo $COUNT
