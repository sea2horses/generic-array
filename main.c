#include <stdio.h>

/* Redefinition of assert for demonstration purposes */
#define assert(expr) \
  do { \
    if(!(expr)) { \
      printf("(assert) CRASH EXPECTED! Expression %s not satisfied.\n", #expr); \
    } \
  } while(0)

#include "generic_array.h"

/*
  Demo file for generic_array.h

  Compile example:
    gcc -std=c11 -Wall -Wextra -pedantic main.c -o demo

  Run:
    ./demo
*/

typedef struct {
  const char *name;
  int age;
} Person;

/* Generate a PersonArray type and PersonArray_* functions. */
DefArray(Person);

static void print_int(int value, FILE* stream) {
  fprintf(stream, "%d", value);
}

static void print_double(double value, FILE* stream) {
  fprintf(stream, "%.2f", value);
}

static void print_char(char value, FILE* stream) {
  fprintf(stream, "'%c'", value);
}

static void print_person(Person person, FILE* stream) {
  fprintf(stream, "{ name: \"%s\", age: %d }", person.name, person.age);
}

static void double_int(int *value) {
  *value *= 2;
}

static void add_index_to_int(int *value, size_t index) {
  *value += (int)index;
}

static void halve_double(double *value) {
  *value /= 2.0;
}

static void uppercase_char(char *value) {
  if (*value >= 'a' && *value <= 'z') {
    *value = (char)(*value - ('a' - 'A'));
  }
}

static void birthday(Person *person) {
  person->age += 1;
}

static void print_section(const char *title) {
  printf("\n=== %s ===\n", title);
}

int main(void) {
  print_section("intArray demo");

  Array(int) numbers = {0};

  arr_error error = intArray_init(&numbers);
  printf("init: %s\n", arr_error_to_string(error));

  for (int i = 1; i <= 20; i++) {
    error = intArray_push(&numbers, i);
    if (error != ARR_SUCCESS) {
      printf("push failed at %d: %s\n", i, arr_error_to_string(error));
      intArray_free(&numbers);
      return 1;
    }
  }

  printf("numbers = ");
  intArray_print(&numbers, print_int, stdout);
  printf("\n");

  printf("size: %zu\n", intArray_size(&numbers));
  printf("capacity: %zu\n", intArray_capacity(&numbers));
  printf("is empty: %s\n", intArray_isEmpty(&numbers) ? "true" : "false");

  int value = 0;
  error = intArray_get(&numbers, 5, &value);
  printf("safe get index 5: %s, value = %d\n", arr_error_to_string(error), value);

  error = intArray_set(&numbers, 5, 999);
  printf("safe set index 5 to 999: %s\n", arr_error_to_string(error));

  error = intArray_exists(&numbers, 1000);
  printf("exists index 1000: %s\n", arr_error_to_string(error));

  intArray_set_unsafe(&numbers, 0, -10);
  printf("unsafe get index 0 after unsafe set: %d\n", intArray_get_unsafe(&numbers, 0));

  printf("after set operations = ");
  intArray_print(&numbers, print_int, stdout);
  printf("\n");

  intArray_forEach(&numbers, double_int);
  printf("after forEach double_int = ");
  intArray_print(&numbers, print_int, stdout);
  printf("\n");

  intArray_forEachIndexed(&numbers, add_index_to_int);
  printf("after forEachIndexed add_index_to_int = ");
  intArray_print(&numbers, print_int, stdout);
  printf("\n");

  error = intArray_pop(&numbers);
  printf("pop: %s\n", arr_error_to_string(error));

  printf("after pop = ");
  intArray_print(&numbers, print_int, stdout);
  printf("\n");

  intArray_clear(&numbers);
  printf("after clear size: %zu, capacity still: %zu\n",
         intArray_size(&numbers),
         intArray_capacity(&numbers));


  printf("pop on empty array: ");
  intArray_pop(&numbers);

  error = intArray_free(&numbers);
  printf("free: %s\n", arr_error_to_string(error));

  printf("free again: ");
  error = intArray_free(&numbers);

  print_section("push after free demo");

  error = intArray_push(&numbers, 123);
  printf("push after free: %s\n", arr_error_to_string(error));

  if (error == ARR_SUCCESS) {
    printf("numbers = ");
    intArray_print(&numbers, print_int, stdout);
    printf("\n");
  }

  intArray_free(&numbers);

  print_section("doubleArray demo");

  Array(double) decimals = {0};
  doubleArray_init(&decimals);

  doubleArray_push(&decimals, 3.14159);
  doubleArray_push(&decimals, 2.71828);
  doubleArray_push(&decimals, 1.41421);

  printf("decimals = ");
  doubleArray_print(&decimals, print_double, stdout);
  printf("\n");

  doubleArray_forEach(&decimals, halve_double);

  printf("after halve_double = ");
  doubleArray_print(&decimals, print_double, stdout);
  printf("\n");

  doubleArray_free(&decimals);

  print_section("charArray demo");

  Array(char) letters = {0};
  charArray_init(&letters);

  charArray_push(&letters, 'a');
  charArray_push(&letters, 'b');
  charArray_push(&letters, 'c');

  printf("letters = ");
  charArray_print(&letters, print_char, stdout);
  printf("\n");

  charArray_forEach(&letters, uppercase_char);

  printf("after uppercase_char = ");
  charArray_print(&letters, print_char, stdout);
  printf("\n");

  charArray_free(&letters);

  print_section("custom struct PersonArray demo");

  Array(Person) people = {0};
  PersonArray_init(&people);

  PersonArray_push(&people, (Person){.name = "Ada", .age = 36});
  PersonArray_push(&people, (Person){.name = "Dennis", .age = 70});
  PersonArray_push(&people, (Person){.name = "Grace", .age = 85});

  printf("people = ");
  PersonArray_print(&people, print_person, stdout);
  printf("\n");

  Person second_person;
  error = PersonArray_get(&people, 1, &second_person);
  printf("safe get person index 1: %s, ", arr_error_to_string(error));

  if (error == ARR_SUCCESS) {
    print_person(second_person, stdout);
  }

  printf("\n");

  PersonArray_set(&people, 1, (Person){.name = "Ken", .age = 80});
  PersonArray_forEach(&people, birthday);

  printf("after set and birthday = ");
  PersonArray_print(&people, print_person, stdout);
  printf("\n");

  PersonArray_free(&people);

  return 0;
}
