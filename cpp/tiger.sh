#! /bin/sh

fullpath=$1
filename=$(basename $fullpath)
extension=${filename##*.}
export filename=${filename%.*}

./tiger ${fullpath} 1> obj/${filename}.S 2> /dev/null

make -f tiger.mk $2

