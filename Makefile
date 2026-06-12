CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Isrc

SRC_DIR = src
OBJ_DIR = obj

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))
TARGET = mini_compiler

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(TARGET) output/*

test: $(TARGET)
	mkdir -p output
	@echo "Running test suite..."
	./$(TARGET) test/valid_program1.pas
	./$(TARGET) test/valid_program2.pas
	./$(TARGET) test/valid_program3.pas
	./$(TARGET) test/err_lexical.pas
	./$(TARGET) test/err_syntax.pas
	./$(TARGET) test/err_semantic.pas
	@echo "All tests finished. Output files written to output/ directory."
