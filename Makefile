# Directories
INC_DIR = inc
SRC_DIR = src
TEST_DIR = tests
EXAMPLE_DIR = examples

# Files
INCLUDES = 		$(wildcard $(INC_DIR)/*.inc)
MAIN_SRC = 		main.s
MAIN_OBJ =		main.o
MAIN_BIN =		main
TEST_SRCS = 	$(wildcard $(TEST_DIR)/*.s)
TEST_BINS = 	$(patsubst $(TEST_DIR)/%.s, $(TEST_DIR)/%, $(TEST_SRCS))
EXAMPLE_SRC = $(EXAMPLE_DIR)/example.s
EXAMPLE_OBJ =	$(EXAMPLE_DIR)/example.o
EXAMPLE_BIN =	$(EXAMPLE_DIR)/example

# Compiler and Linker Flags
NASM_FLAGS = -f elf64 -g -w+all -I$(INC_DIR)/

# Default target
all: $(MAIN_BIN) $(TEST_BINS) $(EXAMPLE_BIN)

# Main build
$(MAIN_BIN): $(INCLUDES) $(MAIN_SRC)
	nasm $(NASM_FLAGS) -o $(MAIN_OBJ) $(MAIN_SRC)
	ld -o $(MAIN_BIN) $(MAIN_OBJ)

# Compile tests
$(TEST_BINS): $(TEST_SRCS) $(INCLUDES)
	@for test in $(TEST_SRCS); do \
		obj=$$(echo $$test | sed 's/\.s$$/.o/'); \
		bin=$$(echo $$test | sed 's/\.s$$//'); \
		nasm $(NASM_FLAGS) -o $$obj $$test; \
		ld -o $$bin $$obj; \
	done

# Compile example
$(EXAMPLE_BIN): $(EXAMPLE_SRC) $(INCLUDES)
	nasm $(NASM_FLAGS) -o $(EXAMPLE_OBJ) $(EXAMPLE_SRC)
	ld -o $(EXAMPLE_BIN) $(EXAMPLE_OBJ)

# Run tests with strace
test: $(TEST_BINS)
	@for bin in $(TEST_BINS); do \
		echo "Running $$bin with strace..."; \
		strace ./$$bin; \
		if [ $$? -ne 0 ]; then \
			echo "$$bin failed"; \
			exit 1; \
		fi; \
	done
	@echo "All tests passed."

# Clean up
clean:
	rm -f $(MAIN_OBJ) $(MAIN_BIN) $(EXAMPLE_OBJ) $(EXAMPLE_BIN)
	rm -f $(TEST_DIR)/*.o $(TEST_BINS)

.PHONY: all test clean

