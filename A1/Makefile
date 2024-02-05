CXX = g++
CXXFLAGS = -std=c++11 -Wall -O3

all: ast dfg

ast: ast.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

dfg: dfg.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

run: all
	./ast
	./dfg

clean:
	rm -f ast dfg
	rm -f ./io/*.dot ./io/*.png
