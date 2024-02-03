CXX = g++
CXXFLAGS = -std=c++11 -Wall -O3

TARGET = main
SRCS = basic.cpp
OBJS = $(SRCS:.cpp=.o)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean

run: main
	./main

clean:
	rm -f $(OBJS) $(TARGET) main
