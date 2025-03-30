#!/bin/bash

# Directory where output folders are located (change if needed)
# base_dir="/home/cs21b077/suma_code/dataset/udwt_graphs/Germany"
# file="/home/cs21b077/suma_code/dataset/udwt_graphs/GermanyRoadudwt.txt"
base_dir="/home/cs21b077/suma_code/dataset/udwt_graphs/wiki"
file="/home/cs21b077/suma_code/dataset/udwt_graphs/wikiudwt.txt"

# Loop over each folder matching the pattern "output_*"
for dir in "$base_dir"/output_*; do
    if [[ -d "$dir" ]]; then
        echo "Processing folder: $dir"
        
        ./most_optimal "$file" "$dir/final_static.txt" "$dir/final_updates.txt"
        
        # Optional: Add a delay if you want to space out commands (e.g., sleep 1)
    else
        echo "$dir is not a directory, skipping..."
    fi
done

echo "Batch processing complete."
