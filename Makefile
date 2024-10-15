CXX := $(shell if command -v clang++ > /dev/null 2>&1; then echo clang++; else echo g++; fi)
CXXFLAGS = -std=c++17 -Wall -Wextra -I.

SRC_DIR = src

OBJ_DIR = bin/object

SRCS = $(SRC_DIR)/main.cpp

OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

TARGET = bin/main

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean
