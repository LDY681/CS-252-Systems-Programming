#!/bin/bash
foo="Hello"
foo="$foo World"
echo $foo
a="hello"
b="world"
c=$a$b
echo $c
echo ${#foo}
