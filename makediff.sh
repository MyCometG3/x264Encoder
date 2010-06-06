#!/bin/sh

doEval() {
	if [ ! -f "${CHECK}" ]
	then 
		echo "File not found: ${CHECK}"
		exit 1
	fi
}

doDiff() {
	CHECK="${SRC}/${TARGET}"
	doEval
	CHECK="${DST}/${TARGET}"
	doEval
	
	echo diff -u ${SRC}/${TARGET} ${DST}/${TARGET}
	diff -u "${SRC}/$TARGET" "${DST}/$TARGET" \
		> "${PREFIX}.`basename $TARGET`.${REV}.diff"
	ExitCode=$?
}

#

echo "revision of libav? \c"; read REV
SRC="libav${REV}.org"
DST="libav${REV}"
PREFIX="ffmpeg"
#echo SRC=${SRC}, DST=${DST}, PREFIX=${PREFIX}

TARGET="configure"
doDiff

TARGET="libavcodec/libx264.c"
doDiff

#TARGET="libavcodec/utils.c"
#doDiff

#

echo "revision of x264? \c"; read REV
SRC="x264${REV}.org"
DST="x264${REV}"
PREFIX="x264"
#echo SRC=${SRC}, DST=${DST}, PREFIX=${PREFIX}

TARGET="configure"
doDiff

