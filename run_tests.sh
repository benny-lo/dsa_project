#!/bin/bash

EXECUTABLE="$1"

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

# Test directory
TEST_DIR="tests"

# Function to run a test

run_test() {
    local input_file="$1"
    local expected_output="$2"
    local test_name="$3"

    echo "Running test: $test_name"

    # Run the program and measure time + memory
    /usr/bin/time -f "%e %M" ./build/bin/dsa_project_$EXECUTABLE < "$input_file" > output.txt 2> stats.txt
    read elapsed_s mem_kb < stats.txt

    # Compare output
    if diff -q output.txt "$expected_output" > /dev/null; then
        echo -e "${GREEN}✓ Test passed: $test_name${NC} (Time: ${elapsed_s}s, Memory: ${mem_kb}KB)"
    else
        echo -e "${RED}✗ Test failed: $test_name${NC} (Time: ${elapsed_s}s, Memory: ${mem_kb}KB)"
        echo "Differences:"q
        diff output.txt "$expected_output" >> diff.txt
    fi

    rm output.txt stats.txt
}

# Run slide test
run_test "$TEST_DIR/slide.txt" "$TEST_DIR/slide.output.txt" "Slide Test"
run_test "$TEST_DIR/insert.txt" "$TEST_DIR/insert.output.txt" "Insert Test"
run_test "$TEST_DIR/test1.txt" "$TEST_DIR/test1.output.txt" "Test 1"
run_test "$TEST_DIR/test2.txt" "$TEST_DIR/test2.output.txt" "Test 2"
run_test "$TEST_DIR/test3.txt" "$TEST_DIR/test3.output.txt" "Test 3"
run_test "$TEST_DIR/input_IN001.txt" "$TEST_DIR/input_IN001.output.txt" "Input IN001"
run_test "$TEST_DIR/input_HS001.txt" "$TEST_DIR/input_HS001.output.txt" "Input HS001"
run_test "$TEST_DIR/input_HS002.txt" "$TEST_DIR/input_HS002.output.txt" "Input HS002"
run_test "$TEST_DIR/input_HS003.txt" "$TEST_DIR/input_HS003.output.txt" "Input HS003"
run_test "$TEST_DIR/input_HS004.txt" "$TEST_DIR/input_HS004.output.txt" "Input HS004"
run_test "$TEST_DIR/input_HS005.txt" "$TEST_DIR/input_HS005.output.txt" "Input HS005"
run_test "$TEST_DIR/input_HS006.txt" "$TEST_DIR/input_HS006.output.txt" "Input HS006"
run_test "$TEST_DIR/input_HS007.txt" "$TEST_DIR/input_HS007.output.txt" "Input HS007"
run_test "$TEST_DIR/input_HS008.txt" "$TEST_DIR/input_HS008.output.txt" "Input HS008"
run_test "$TEST_DIR/input_HS009.txt" "$TEST_DIR/input_HS009.output.txt" "Input HS009"
run_test "$TEST_DIR/input_HS010.txt" "$TEST_DIR/input_HS010.output.txt" "Input HS010"
run_test "$TEST_DIR/input_HS011.txt" "$TEST_DIR/input_HS011.output.txt" "Input HS011" 
run_test "$TEST_DIR/input_HS012.txt" "$TEST_DIR/input_HS012.output.txt" "Input HS012" 
run_test "$TEST_DIR/input_HS013.txt" "$TEST_DIR/input_HS013.output.txt" "Input HS013" 






# Add more tests here as needed
# run_test "$TEST_DIR/another_test.txt" "$TEST_DIR/another_test.output.txt" "Another Test" 