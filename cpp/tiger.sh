#! /bin/sh

fullpath=$1
filename=$(basename $fullpath)
extension=${filename##*.}
export filename=${filename%.*}

./tiger ${fullpath} 1> ${filename}.S

make -f tiger.mk

