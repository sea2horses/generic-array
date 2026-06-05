COMPILER := 'gcc'

list:
  @just --list

# Builds the main application
build-main:
  {{ COMPILER}} -o build/main main.c

# Builds (and run) the main application
main: build-main
  ./build/main

# Builds the tests
build-tests:
  {{ COMPILER }} -o build/tests tests/tests.c

# Builds (and run) the tests
test: build-tests
  ./build/tests
