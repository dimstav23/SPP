
#!/bin/bash
set -e

RESULTS="./results"
declare -a indices=( ctree rtree rbtree hashmap_tx )
declare -a ops=( insert get )

printf "|                                 Space Overhead                                  |\n"
printf "|---------------------------------------------------------------------------------|\n"
printf "|%17s|%31s|%31s|\n" "Data" "insert             " "get              "
printf "|%17s|%15s|%15s|%15s|%15s|\n" "Structure" "Megabytes" "(%)" "Megabytes" "(%)"
printf "|---------------------------------------------------------------------------------|\n"

for index in "${indices[@]}"
do
    printf "|%17s" $index
    for op in "${ops[@]}"
    do
        pmdk_file="$RESULTS/pmdk_${index}_${op}.txt"        
        while IFS=":" read descr values
        do
            if [[ "$descr" == *"Total used bytes"* ]]; then
                pmdk_bytes=`echo "$values" | cut -d "[" -f 1`
                # echo $pmdk_bytes
            fi
        done <  "$pmdk_file"

        spp_file="$RESULTS/spp_${index}_${op}.txt"        
        while IFS=":" read descr values
        do
            if [[ "$descr" == *"Total used bytes"* ]]; then
                spp_bytes=`echo "$values" | cut -d "[" -f 1`
                # echo $spp_bytes
            fi
        done <  "$spp_file"

        diff="$((spp_bytes-pmdk_bytes))"
        printf "|%15s" $(($diff/1024/1024))
        percentage=$( echo "scale=6; $diff/$pmdk_bytes*100" | bc -l )
        printf "|%15s" $percentage
    done
    printf "|\n"

done

printf "|---------------------------------------------------------------------------------|\n"
