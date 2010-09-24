#! /bin/sh

TIGER=../cpp/tiger
rm -f result.txt

echo "basic test =============================" >> result.txt
FILES=`ls -1 basic`
for f in $FILES ;do
	echo $TIGER basic${f}
	$TIGER basic/$f
	if [ $? = 0 ] ; then
		echo basic/${f} succeeded >> result.txt
	else
		echo basic/${f} failed >> result.txt
	fi
done


echo "integration test =============================" >> result.txt
FILES=`ls -1 integration`
for f in $FILES; do
	$TIGER integration/$f
	RET=$?

	# is this error test ?
	grep "error :" integration/$f > /dev/null
	if [ $? = 0 ] ; then
		ERROR=\(error\)
	else
		ERROR=
	fi

	if [ $RET = 0 ] ; then
		echo integration/${f}${ERROR} succeeded >> result.txt
	else
		echo integration/${f}${ERROR} failed >> result.txt
	fi

done

