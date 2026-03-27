CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11 -O2
OBJS = matrix.o main.o
TARGET = test_matrix

.PHONY: all test clean

all: test

test: $(TARGET)
	./$(TARGET) test

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

matrix.o: matrix.cpp matrix.h
	$(CXX) $(CXXFLAGS) -c matrix.cpp

main.o: main.cpp matrix.h
	$(CXX) $(CXXFLAGS) -c main.cpp

clean:
	rm -f $(OBJS) $(TARGET)