#! /bin/sh

echo "runtime test ============================="
FILES=`ls -1 ../testcases/runtime`

if [ $1 = "arm" ]; then
TARGET_CPU="arm"
EXEC="qemu-arm"
else
TARGET_CPU="x86"
fi

for f in $FILES ;do
	echo [[${f}]] 
	./tiger.sh ../testcases/runtime/$f "SILENT=@ TARGET=${TARGET_CPU}"
	$EXEC bin/${f%.*}
done
