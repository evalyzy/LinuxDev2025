CC = gcc
CFLAGS = -Wall -Wextra -std=c11

TARGET = esub

all: $(TARGET)

$(TARGET): esub.c
	$(CC) $(CFLAGS) -o $(TARGET) esub.c

test: $(TARGET)
	@echo "Running tests..."
	@# Test 1: Simple substitution
	@./esub "world" "universe" "Hello world" > out.txt
	@echo "Hello world" | sed -E 's/world/universe/' > exp.txt
	@diff out.txt exp.txt && echo "Test 1 Passed" || echo "Test 1 Failed"

	@# Test 2: Backreference
	@./esub "(a)(b)" "BA \2 \1" "zz ab zz" > out.txt
	@echo "zz ab zz" | sed -E 's/(a)(b)/BA \2 \1/' > exp.txt
	@diff out.txt exp.txt && echo "Test 2 Passed" || echo "Test 2 Failed"

	@# Test 3: No match
	@./esub "abc" "def" "xyz" > out.txt
	@echo "xyz" | sed -E 's/abc/def/' > exp.txt
	@diff out.txt exp.txt && echo "Test 3 Passed" || echo "Test 3 Failed"
	
	@# Test 4: Escape backslash
	@./esub "a" "\\\\" "bab" > out.txt
	@echo "bab" | sed -E 's/a/\\/' > exp.txt
	@diff out.txt exp.txt && echo "Test 4 Passed" || echo "Test 4 Failed"

	@rm -f out.txt exp.txt

clean:
	rm -f $(TARGET) out.txt exp.txt

