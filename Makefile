CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

TARGET = cpell

SRC = main.cpp \
      Parsing/parser.cpp \
      Execute/execute.cpp \
      Commands/cd.cpp \
      utils/code.cpp \
      Commands/environments.cpp

OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) -lreadline

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run