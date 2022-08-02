#!/bin/bash
set -e

RESULTS="./results"
declare -a log_size=("4" "128" "512" "2048" "4096")
repeats=100

printf "|                               RECOVERY TIME (ms)                               |\n"
printf "|--------------------------------------------------------------------------------|\n"
printf "|%20s|%11s|%11s|%11s|%11s|%11s|\n" "Number of PMEMoids:" "100" "1000" "10K" "100K" "1M"
printf "|--------------------------------------------------------------------------------|\n"

for FILE in $RESULTS/*; do 
    variant="$(basename -- $FILE)"
    printf "|%20s" $variant
    acc=0
    index=0
    while IFS=";", read -r number_of_objects log_size pool_size recovery_time; do
    #acc the repeats values
    acc=`echo $acc + $recovery_time | bc`
    index=$(( $index + 1 ))

    #dump the average and start over
    if [ $index -eq $repeats ]; then
        # acc="scale=6; $acc / $repeats" | bc
        acc=$( echo "scale=6; $acc/$repeats*1000" | bc -l )
        printf "|%11f" $acc
        index=0
        acc=0
    fi

    done < <(tail -n +2 $FILE)
    printf "|\n"
done

printf "|--------------------------------------------------------------------------------|\n"