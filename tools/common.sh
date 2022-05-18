convert_list_to_c_file()
{
    local name=$1
    local words=$2
    local len=$3
    local lang=$4

    local prefix="${name}_${lang}_${len}"
    local h_file="${prefix}_words.h"

    nr_word=$(wc -l $words | cut -d' ' -f1)

    echo "#include <words.h>" > $h_file
    echo "" >> $h_file

    echo  "static const char *${prefix}_words[] = {" >> $h_file
    n=0
    for w in $(cat $words)
    do
        [ $n -eq 0 ] && echo -n "  " >> $h_file
        echo -n "\"$w\", " >> $h_file
        n=$((n+1))
        [ $n -eq 8 ] && echo "" >> $h_file && n=0
    done
    echo "" >> $h_file
    echo "};" >> $h_file
    echo >> $h_file

    echo "static const struct word ${prefix} = {" >> $h_file
    echo "  \"${name}\"," >> $h_file
    echo "  ${prefix}_words," >> $h_file
    echo "  ${nr_word}," >> $h_file
    echo "  ${len}," >> $h_file
    echo "  ${lang^^}," >> $h_file
    echo "};" >> $h_file
}
