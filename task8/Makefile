CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++11 -pedantic

TARGET = matrix_app
OBJS = Matrix.o main.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

Matrix.o: Matrix.cpp Matrix.h
	$(CXX) $(CXXFLAGS) -c Matrix.cpp

main.o: main.cpp Matrix.h
	$(CXX) $(CXXFLAGS) -c main.cpp

clean:
	rm -f $(OBJS) $(TARGET)