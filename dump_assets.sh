#!/bin/bash

path=$1
target_dir=$2
source_file=${path##*/}
object=${source_file%.*}
target_file="${target_dir}/${object}.cpp"

subdir=$(echo ${path} | sed -r 's/^.*\/(.+)\/.*$/\1/')
case $subdir in
    "gfx")
        temp_file=$(mktemp --suffix='.bmp')
        xxd_var=$(echo ${temp_file} | sed 's/\W/_/g')

        montage ${path} -tile x1 -alpha On -geometry '+0+0' -background 'rgba(0, 0, 0, 0)' -quality 100 ${temp_file}
        xxd -i ${temp_file} ${target_file}
        sed -i "s/${xxd_var}/${object}/g" ${target_file}
        ;;
    "fonts")
        xxd_var=$(echo ${path} | sed 's/\W/_/g')

        xxd -i ${path} ${target_file}
        sed -i "s/${xxd_var}/${object}/g" ${target_file}
        ;;
    *)
        echo "Unknown subdirectory ${subdir}!"
        exit 1
        ;;
esac