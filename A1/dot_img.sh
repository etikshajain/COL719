for file in ./io/ast_*.dot; do
    dot -Tpng "$file" -o "${file%.dot}.png"
done

for file in ./io/dfg*.dot; do
    dot -Tpng "$file" -o "${file%.dot}.png"
done

