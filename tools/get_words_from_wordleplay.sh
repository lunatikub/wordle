#!/bin/bash

SCRIPT_NAME=./$(basename $0)
SCRIPT_DIR=$(dirname $(readlink -f $0))

source ${SCRIPT_DIR}/common.sh

NAME=wordleplay_fr
URL_WORDS=https://wordleplay.com/dic/fr.json?v=0.1.1

OUT=$(mktemp words.XXXXXX)
wget $URL_WORDS -O $OUT

for len in $(seq 4 6)
do
    WORDS=$(mktemp words.XXXXXX)
    sed 's/,/\n/g' $OUT | sed 's/[["]//g' | awk "length() == ${len}" > $WORDS

    convert_list_to_c_file $NAME $WORDS $len

    rm -rf $WORDS
done

rm -rf $OUT

exit 0
