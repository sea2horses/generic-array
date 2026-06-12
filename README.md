# generic_array.h

A small header-only generic dynamic array implementation for C.

This library uses macros to generate type-specific array structs and functions, such as `intArray`, `doubleArray`, `charArray`, and any custom type you declare with `DeclArray(Type)`.

## Features

- Header-only implementation
- Type-specific dynamic arrays
- Automatic capacity growth
- Safe `get` and `set` with bounds checking
- Unsafe `get_unsafe` and `set_unsafe` for direct access
- `push`, `pop`, `clear`, `free`
- `forEach` and `forEachIndexed`
- Custom print support
- Built-in default arrays for `int`, `double`, and `char`
- Support for custom structs through `DeclArray(Type)`

## Basic Setup

Include the header:

```c
#include "generic_array.h"
```

Compile with a modern C compiler:

```bash
gcc -std=c11 -Wall -Wextra -pedantic main.c -o main
```

## Creating an Array

Use `Array(type)` to declare the array.

Important: always initialize the array to `{0}` before calling `init`.

```c
Array(int) numbers = {0};

arr_error error = intArray_init(&numbers);

if (error != ARR_SUCCESS) {
  printf("Could not initialize array\n");
}
```

The `{0}` initialization matters because `init` checks whether `data` is already non-`NULL` to avoid double initialization.

## Pushing Values

```c
intArray_push(&numbers, 10);
intArray_push(&numbers, 20);
intArray_push(&numbers, 30);
```

The array automatically grows when it reaches its current capacity.

## Getting Values Safely

The safe `get` function uses an output pointer and returns an `arr_error`.

```c
int value = 0;

if (intArray_get(&numbers, 1, &value) == ARR_SUCCESS) {
  printf("Value: %d\n", value);
}
```

If the index does not exist, it returns:

```c
ARR_OUT_OF_BOUNDS
```

## Setting Values Safely

```c
arr_error error = intArray_set(&numbers, 1, 99);

if (error != ARR_SUCCESS) {
  printf("Could not set value\n");
}
```

## Unsafe Access

Unsafe access skips bounds checking.

```c
int first = intArray_get_unsafe(&numbers, 0);

intArray_set_unsafe(&numbers, 0, 123);
```

Use these only when you already know the index is valid.

## Checking an Index

```c
if (intArray_exists(&numbers, 2) == ARR_SUCCESS) {
  printf("Index exists\n");
}
```

## Size, Capacity, and Empty Check

```c
printf("Size: %zu\n", intArray_size(&numbers));
printf("Capacity: %zu\n", intArray_capacity(&numbers));

if (intArray_isEmpty(&numbers)) {
  printf("Array is empty\n");
}
```

## Popping Values

`pop` removes the last element logically by reducing `count`.

```c
// If the array isn't empty the program will panic!
intArray_pop(&numbers);
```

## Clearing an Array

`clear` removes all elements logically but keeps the allocated capacity.

```c
intArray_clear(&numbers);
```

This is useful if you want to reuse the array without reallocating memory.

## Freeing an Array

`free` releases the allocated memory and resets the array.

```c
// Double freeing will cause a panic!
intArray_free(&numbers);
```

After freeing, the array can be reused with `push`, since `push` can grow from capacity `0`.

```c
intArray_push(&numbers, 50);
```

You should still call `free` again when finished.

```c
intArray_free(&numbers);
```

## Printing an Array

The array print function receives a custom print callback.

```c
void print_int(int value, FILE* stream) {
  fprintf(stream, "%d", value);
}

intArray_print(&numbers, print_int, stdout);
```

Output example:

```text
[10, 20, 30]
```

## forEach

`forEach` receives a pointer to each element, so it can modify the array.

```c
void double_value(int *value) {
  *value *= 2;
}

intArray_forEach(&numbers, double_value);
```

## forEachIndexed

`forEachIndexed` receives a pointer to each element and its index.

```c
void add_index(int *value, size_t index) {
  *value += (int)index;
}

intArray_forEachIndexed(&numbers, add_index);
```

## Using Other Built-In Types

By default, the header declares arrays for:

```c
DefArray(int);
DefArray(double);
DefArray(char);
```

So you can use:

```c
Array(double) decimals = {0};

doubleArray_init(&decimals);
doubleArray_push(&decimals, 3.14);
doubleArray_push(&decimals, 2.71);
doubleArray_free(&decimals);
```

And:

```c
Array(char) letters = {0};

charArray_init(&letters);
charArray_push(&letters, 'a');
charArray_push(&letters, 'b');
charArray_free(&letters);
```

## Custom Types

For custom structs, define the type first, then call `DeclArray(Type)`.

```c
typedef struct {
  const char *name;
  int age;
} Person;

DeclArray(Person);
```

Then use it like this:

```c
Array(Person) people = {0};

PersonArray_init(&people);

PersonArray_push(&people, (Person){.name = "Ada", .age = 36});
PersonArray_push(&people, (Person){.name = "Dennis", .age = 70});

Person person;

if (PersonArray_get(&people, 0, &person) == ARR_SUCCESS) {
  printf("%s is %d years old\n", person.name, person.age);
}

PersonArray_free(&people);
```

## Important Limitation

`DefArray(type)` works best with single-token type names.

This works:

```c
DeclArray(int);
DeclArray(Person);
```

This is problematic:

```c
DeclArray(unsigned int);
DeclArray(struct Person);
```

Use a `typedef` instead:

```c
typedef unsigned int uint;

DeclArray(uint);
```

For structs, prefer:

```c
typedef struct {
  int x;
  int y;
} Point;

DeclArray(Point);
```

## Disabling Default Types

If you do not want the default `int`, `double`, and `char` arrays, define:

```c
#define GENERIC_ARRAY_DISABLE_DEFAULTS
#include "generic_array.h"
```

Then declare only the arrays you need:

```c
DefArray(float);
```

## Error Codes

The array functions return `arr_error` values when something can fail.

```c
typedef enum {
  ARR_SUCCESS = 0,
  ARR_OUT_OF_MEMORY,
  ARR_OUT_OF_BOUNDS
} arr_error;
```

Common meanings:

| Error | Meaning |
|---|---|
| `ARR_SUCCESS` | Operation completed successfully |
| `ARR_OUT_OF_MEMORY` | Allocation or reallocation failed |
| `ARR_OUT_OF_BOUNDS` | Invalid index access |

## Complete Minimal Example

```c
#include <stdio.h>
#include "generic_array.h"

static void print_int(int value) {
  printf("%d", value);
}

int main(void) {
  Array(int) numbers = {0};

  if (intArray_init(&numbers) != ARR_SUCCESS) {
    return 1;
  }

  intArray_push(&numbers, 10);
  intArray_push(&numbers, 20);
  intArray_push(&numbers, 30);

  int value = 0;

  if (intArray_get(&numbers, 1, &value) == ARR_SUCCESS) {
    printf("Index 1: %d\n", value);
  }

  printf("Array: ");
  intArray_print(&numbers, print_int);
  printf("\n");

  intArray_free(&numbers);

  return 0;
}
```

## Testing

A simple test file can be compiled like this:

```bash
gcc -std=c11 -Wall -Wextra -pedantic generic_array_test.c -o generic_array_test
./generic_array_test
```

(You can also use the justfile for it)

Expected output:

```text
Tests run: 119 (as of 011/06/2026)
Tests failed: 0
All tests passed.
```

## Notes

This project intentionally uses C macros to simulate generic arrays. It is not the same as true generics in languages like C++, Rust, or Java, but it provides a practical and lightweight way to create reusable typed arrays in C.

