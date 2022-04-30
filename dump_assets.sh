#!/bin/bash

for path in assets/*; do
    source_file=${path##*/}
    object=${source_file%.*}
    target_file="$1/${object}.cpp"
    temp_file=$(mktemp --suffix='.bmp')
    xxd_var=$(echo ${temp_file} | sed 's/\W/_/g')

    montage ${path} -tile x1 -alpha On -geometry '+0+0' -background 'rgba(0, 0, 0, 0)' -quality 100 ${temp_file}
    xxd -i ${temp_file} ${target_file}
    sed -i "s/${xxd_var}/${object}/g" ${target_file}
done