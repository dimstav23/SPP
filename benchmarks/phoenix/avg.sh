#!/bin/bash

# Get the average per variant and per experiment among the executed runs
cd results/phoenix

declare -A sum
declare -A count

# Iterate through files
for file in *_*.txt; do
  # Skip processing if the file ends with "_avg.txt"
  [[ "$file" == *_avg.txt ]] && continue

  # Extract variant, benchmark, and time from the filename
  IFS="_" read -r variant benchmark repeat <<< "$(basename "$file" .txt)"
  
  # Extract the time value from the file
  time=$(grep "Total time:" "$file" | awk '{print $3}')
  echo $variant $benchmark $repeat ":" $time
  # Add the time to a sum for calculating the average
  ((sum["$variant"_"$benchmark"] += time))
  ((count["$variant"_"$benchmark"]++))
done

# Calculate the average and write to a new file
for key in "${!sum[@]}"; do
  avg=$(echo "scale=6; ${sum[$key]} / ${count[$key]}" | bc)
  echo "Average time: $avg us" > "${key}_avg.txt"
done