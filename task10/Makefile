CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O0
TARGET   = analyzer
SRCS     = main.cpp Scanner.cpp Parser.cpp Executor.cpp
OBJS     = $(SRCS:.cpp=.o)
HEADERS  = Lex.h Scanner.h Parser.h Executor.h

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)

test: $(TARGET)
	@echo "Running A-tests..."
	@for f in tests/A*.m; do \
		echo "--- $$f ---"; \
		./$(TARGET) $$f exec 2>&1 || true; \
	done
	@echo "Running B-tests..."
	@for f in tests/B*.m; do \
		echo "--- $$f ---"; \
		./$(TARGET) $$f 2>&1 || true; \
	done