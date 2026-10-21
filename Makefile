CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic

TARGET = orbit
SOURCES = main.cpp storage.cpp scoring.cpp matching.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
