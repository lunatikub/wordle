convert_list_to_c_file()
{
    local name=$1
    local words=$2
    local len=$3

    local H_FILE="${name}_${len}_words.h"

    nr_word=$(wc -l $words | cut -d' ' -f1)
    echo "static const unsigned ${name}_${len}_nr_word = $nr_word;"  > $H_FILE
    echo "" >> $H_FILE

    echo  "static const char *${name}_${len}_words[] = {" >> $H_FILE
    n=0
    for w in $(cat $words)
    do
        [ $n -eq 0 ] && echo -n "  " >> $H_FILE
        echo -n "\"$w\", " >> $H_FILE
        n=$((n+1))
        [ $n -eq 8 ] && echo "" >> $H_FILE && n=0
    done
    echo "" >> $H_FILE
    echo "};" >> $H_FILE
}
