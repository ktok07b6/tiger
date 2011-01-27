#! /bin/sh

echo "runtime test ============================="
FILES=`ls -1 ../testcases/runtime`
for f in $FILES ;do
	echo runtime test ${f}
	./tiger.sh ../testcases/runtime/$f SILENT=@
	qemu-arm ${f%.*}
done
