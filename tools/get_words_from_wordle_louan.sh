#!/bin/bash

SCRIPT_NAME=./$(basename $0)
SCRIPT_DIR=$(dirname $(readlink -f $0))

source ${SCRIPT_DIR}/common.sh

LEN=5
NAME=wordle_louan_fr
URL_WORDS="https://raw.githubusercontent.com/LouanBen/wordle-fr/main/mots.txt"

OUT=$(mktemp out.XXXXXX)
wget $URL_WORDS -O $OUT

WORDS=$(mktemp words.XXXXXX)
awk "length() == 5" $OUT | tr '[:upper:]' '[:lower:]' > $WORDS

convert_list_to_c_file $NAME $WORDS $LEN

rm -rf $WORDS
rm -rf $OUT

exit 0
