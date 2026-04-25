CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -Wpedantic -g
TARGET = program

SOURCES = main.cpp String.cpp StringList.cpp
OBJECTS = $(SOURCES:.cpp=.o)
HEADERS = IterableObject.h String.h List.h StringList.h

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

task3: $(TARGET)
	./$(TARGET)

.PHONY: all clean task3
