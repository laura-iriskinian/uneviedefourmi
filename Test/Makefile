# Makefile for ant colony simulation algorithms

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2

# Target executables
BFS_TARGET = bfs
DIJKSTRA_TARGET = dijkstra

# Source files
BFS_SRC = bfs.cpp
DIJKSTRA_SRC = dijkstra.cpp

# Default target - build both executables
all: $(BFS_TARGET) $(DIJKSTRA_TARGET)

# Build BFS executable
$(BFS_TARGET): $(BFS_SRC)
	$(CXX) $(CXXFLAGS) -o $(BFS_TARGET) $(BFS_SRC)

# Build Dijkstra executable
$(DIJKSTRA_TARGET): $(DIJKSTRA_SRC)
	$(CXX) $(CXXFLAGS) -o $(DIJKSTRA_TARGET) $(DIJKSTRA_SRC)

# Clean built files
clean:
	rm -f $(BFS_TARGET) $(DIJKSTRA_TARGET)

# Rebuild everything
rebuild: clean all

# Install (optional - copy to system path)
install: all
	sudo cp $(BFS_TARGET) /usr/local/bin/
	sudo cp $(DIJKSTRA_TARGET) /usr/local/bin/

# Uninstall
uninstall:
	sudo rm -f /usr/local/bin/$(BFS_TARGET)
	sudo rm -f /usr/local/bin/$(DIJKSTRA_TARGET)

# Run tests with example file (adjust filename as needed)
test: all
	@echo "Testing BFS algorithm:"
	./$(BFS_TARGET) colony.txt
	@echo ""
	@echo "Testing Dijkstra algorithm:"
	./$(DIJKSTRA_TARGET) colony.txt

# Show help
help:
	@echo "Available targets:"
	@echo "  all        - Build both BFS and Dijkstra executables (default)"
	@echo "  bfs        - Build only BFS executable"
	@echo "  dijkstra   - Build only Dijkstra executable"
	@echo "  clean      - Remove built executables"
	@echo "  rebuild    - Clean and rebuild everything"
	@echo "  install    - Install executables to /usr/local/bin"
	@echo "  uninstall  - Remove executables from /usr/local/bin"
	@echo "  test       - Run both algorithms with colony.txt"
	@echo "  help       - Show this help message"

# Declare phony targets
.PHONY: all clean rebuild install uninstall test help