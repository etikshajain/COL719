for file in ast_*.dot; do
    dot -Tpng "$file" -o "${file%.dot}.png"
done

