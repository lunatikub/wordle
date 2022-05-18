#!/bin/bash

# Download the list of words from `https://wordleplay.com/dic`.
# Specify the language wanted in parameter.
# Specify the length wanted in parameter.

SCRIPT_NAME=./$(basename $0)
SCRIPT_DIR=$(dirname $(readlink -f $0))

source ${SCRIPT_DIR}/common.sh

usage()
{
    echo "$SCRIPT_NAME <lang> <lenght>"
    exit 1
}

LANG=$1
[ -z "${LANG}" ] && usage

LEN=$2
[ -z "${LEN}" ] && usage

NAME=wordleplay_$LANG
URL_WORDS=https://wordleplay.com/dic/$LANG.json

OUT=$(mktemp out.XXXXXX)
wget $URL_WORDS -O $OUT

WORDS=$(mktemp words.XXXXXX)
sed 's/,/\n/g' $OUT |
    tr -d '"' |
    tr -d '[' |
    tr -d ']' |
    sed 's/^ //' |
    awk "length() == $LEN" > $WORDS

convert_list_to_c_file $NAME $WORDS $LEN

rm -rf $WORDS
rm -rf $OUT

exit 0
