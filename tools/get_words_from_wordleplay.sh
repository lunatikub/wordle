#!/bin/bash

# Download the list of words from `https://wordleplay.com/dic`.
# $1: Specify the language wanted in parameter.
# $2: Specify the length wanted in parameter.

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

NAME=wordleplay

echo "$NAME: $URL_WORDS"

for lang in $LANG
do
    URL_WORDS=https://wordleplay.com/dic/$lang.json
    echo " + language: $lang ($URL_WORDS)"

    OUT=$(mktemp out.XXXXXX)
    wget $URL_WORDS -O $OUT 2>&1 2>/dev/null

    for len in $LEN
    do
        echo "  + len: $len"

        WORDS=$(mktemp words.XXXXXX)
        sed 's/,/\n/g' $OUT |
            tr -d '"' |
            tr -d '[' |
            tr -d ']' |
            sed 's/^ //' |
            awk "length() == $len" > $WORDS

        convert_list_to_c_file $NAME $WORDS $len $lang
        rm -rf $WORDS
    done

    rm -rf $OUT
done

exit 0
