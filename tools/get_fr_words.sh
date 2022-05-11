#!/bin/bash

##
## Create the header c files with the list of french words of lenght from 5 to 9
##

FROM_LEN=5
TO_LEN=9

SCRIPT_NAME=./$(basename $0)
SCRIPT_DIR=$(dirname $(readlink -f $0))
SRC_DIR=$SCRIPT_DIR/../src

URL_WORDS="https://raw.githubusercontent.com/LouanBen/wordle-fr/main/mots.txt"

WORDS=$(mktemp words.XXXXXX)

wget $URL_WORDS -O $WORDS

generate_list()
{
    local len=$1
    local list=$2

    awk "length() == $len" $WORDS | tr '[:upper:]' '[:lower:]' > $list
}

convert_list_to_c_file()
{
    local list=$1
    local h_file=$2
    local len=$3

    nr_word=$(wc -l $list | cut -d' ' -f1)
    echo "static const unsigned nr_word_$len = $nr_word;"  > $h_file
    echo "" >> $h_file

    echo  "static const char *words_${len}[] = {" >> $h_file
    n=0
    for w in $(cat $list)
    do
        [ $n -eq 0 ] && echo -n "  " >> $h_file
        echo -n "\"$w\", " >> $h_file
        n=$((n+1))
        [ $n -eq 8 ] && echo "" >> $h_file && n=0
    done
    echo "" >> $h_file
    echo "};" >> $h_file
}

for len in $(seq $FROM_LEN $TO_LEN)
do
    list=fr_words_$len.list
    h_file=$SRC_DIR/fr_words_$len.h
    echo $list -> $h_file
    generate_list $len $list
    convert_list_to_c_file $list $h_file $len
done

rm -rf $WORDS
rm -rf fr_words_*.list

exit 0
