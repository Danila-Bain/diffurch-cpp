# run like make run FILE=my_cpp.cpp
# Default target
run: bin
	# @echo "Compiling and running $(FILE)..."
	@make ./bin/$(basename $(notdir $(FILE))).o
	@./bin/$(basename $(notdir $(FILE))).o

# Create ./bin directory if it doesn't exist
bin:
	@mkdir -p bin

# Rule to compile the C++ file into an executable
./bin/%.o: %.cpp
	# @echo "Compiling $<..."
	@g++ -g -std=c++23 -o $@ $<

# Clean generated files
clean:
	@echo "Cleaning up..."
	@rm -rf bin

.PHONY: run clean
