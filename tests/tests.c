#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/*
  Test file for generic_array.h

  Compile example:
    gcc -std=c11 -Wall -Wextra -pedantic generic_array_test.c -o
  generic_array_test

  Run:
    ./generic_array_test
*/

typedef struct {
  int x;
  int y;
} Point;

static int tests_run = 0;
static int tests_failed = 0;

static int crashes = 0;

#define assert(expression)                                                     \
  if (!(expression)) {                                                         \
    crashes++;                                                                 \
  }

#include "../generic_array.h"

DefArray(Point);

#define EXPECT_TRUE(expr)                                                      \
  do {                                                                         \
    tests_run++;                                                               \
    if (!(expr)) {                                                             \
      tests_failed++;                                                          \
      printf("FAIL: %s:%d: expected true: %s\n", __FILE__, __LINE__, #expr);   \
    }                                                                          \
  } while (0)

#define EXPECT_FALSE(expr)                                                     \
  do {                                                                         \
    tests_run++;                                                               \
    if ((expr)) {                                                              \
      tests_failed++;                                                          \
      printf("FAIL: %s:%d: expected false: %s\n", __FILE__, __LINE__, #expr);  \
    }                                                                          \
  } while (0)

#define EXPECT_ERR(actual, expected)                                           \
  do {                                                                         \
    tests_run++;                                                               \
    arr_error actual_value = (actual);                                         \
    arr_error expected_value = (expected);                                     \
    if (actual_value != expected_value) {                                      \
      tests_failed++;                                                          \
      printf("FAIL: %s:%d: expected error %d, got %d\n", __FILE__, __LINE__,   \
             expected_value, actual_value);                                    \
    }                                                                          \
  } while (0)

#define EXPECT_CRASH(expr)                                                     \
  do {                                                                         \
    tests_run++;                                                               \
    int o_crashes = crashes;                                                   \
    (expr);                                                                    \
    if (crashes <= o_crashes) {                                                \
      tests_failed++;                                                          \
      printf("FAIL: %s:%d: expected crash: %s", __FILE__, __LINE__, #expr);    \
    }                                                                          \
  } while (0)

#define EXPECT_SIZE(actual, expected)                                          \
  do {                                                                         \
    tests_run++;                                                               \
    size_t actual_value = (actual);                                            \
    size_t expected_value = (expected);                                        \
    if (actual_value != expected_value) {                                      \
      tests_failed++;                                                          \
      printf("FAIL: %s:%d: expected size %zu, got %zu\n", __FILE__, __LINE__,  \
             expected_value, actual_value);                                    \
    }                                                                          \
  } while (0)

#define EXPECT_INT(actual, expected)                                           \
  do {                                                                         \
    tests_run++;                                                               \
    int actual_value = (actual);                                               \
    int expected_value = (expected);                                           \
    if (actual_value != expected_value) {                                      \
      tests_failed++;                                                          \
      printf("FAIL: %s:%d: expected int %d, got %d\n", __FILE__, __LINE__,     \
             expected_value, actual_value);                                    \
    }                                                                          \
  } while (0)

#define EXPECT_DOUBLE_CLOSE(actual, expected, epsilon)                         \
  do {                                                                         \
    tests_run++;                                                               \
    double actual_value = (actual);                                            \
    double expected_value = (expected);                                        \
    double difference = actual_value - expected_value;                         \
    if (difference < 0) {                                                      \
      difference = -difference;                                                \
    }                                                                          \
    if (difference > (epsilon)) {                                              \
      tests_failed++;                                                          \
      printf("FAIL: %s:%d: expected double %.8f, got %.8f\n", __FILE__,        \
             __LINE__, expected_value, actual_value);                          \
    }                                                                          \
  } while (0)

static void increment_int(int *value) { *value += 1; }

static void add_index_to_int(int *value, size_t index) { *value += (int)index; }

static void multiply_double_by_two(double *value) { *value *= 2.0; }

static void move_point(Point *point) {
  point->x += 10;
  point->y += 20;
}

static void test_int_array_lifecycle(void) {
  Array(int) array = {0};

  EXPECT_TRUE(intArray_isEmpty(&array));
  EXPECT_SIZE(intArray_size(&array), 0);
  EXPECT_SIZE(intArray_capacity(&array), 0);

  EXPECT_ERR(intArray_init(&array), ARR_SUCCESS);
  EXPECT_SIZE(intArray_size(&array), 0);
  EXPECT_SIZE(intArray_capacity(&array), INIT_CAPACITY);

  EXPECT_CRASH(intArray_init(&array));

  for (int i = 0; i < 40; i++) {
    EXPECT_ERR(intArray_push(&array, i), ARR_SUCCESS);
  }

  EXPECT_SIZE(intArray_size(&array), 40);
  EXPECT_TRUE(intArray_capacity(&array) >= 40);
  EXPECT_FALSE(intArray_isEmpty(&array));

  int value = -1;

  EXPECT_ERR(intArray_get(&array, 0, &value), ARR_SUCCESS);
  EXPECT_INT(value, 0);

  EXPECT_ERR(intArray_get(&array, 39, &value), ARR_SUCCESS);
  EXPECT_INT(value, 39);

  EXPECT_ERR(intArray_exists(&array, 40), ARR_OUT_OF_BOUNDS);
  EXPECT_ERR(intArray_get(&array, 40, &value), ARR_OUT_OF_BOUNDS);
  EXPECT_ERR(intArray_set(&array, 40, 999), ARR_OUT_OF_BOUNDS);

  EXPECT_ERR(intArray_set(&array, 5, 555), ARR_SUCCESS);
  EXPECT_ERR(intArray_get(&array, 5, &value), ARR_SUCCESS);
  EXPECT_INT(value, 555);

  intArray_set_unsafe(&array, 6, 666);
  EXPECT_INT(intArray_get_unsafe(&array, 6), 666);

  intArray_forEach(&array, increment_int);
  EXPECT_ERR(intArray_get(&array, 0, &value), ARR_SUCCESS);
  EXPECT_INT(value, 1);

  intArray_forEachIndexed(&array, add_index_to_int);
  EXPECT_ERR(intArray_get(&array, 3, &value), ARR_SUCCESS);
  EXPECT_INT(value, 7); /* original 3 -> incremented 4 -> plus index 3 = 7 */

  EXPECT_ERR(intArray_pop(&array), ARR_SUCCESS);
  EXPECT_SIZE(intArray_size(&array), 39);

  intArray_clear(&array);
  EXPECT_SIZE(intArray_size(&array), 0);
  EXPECT_TRUE(intArray_isEmpty(&array));
  EXPECT_TRUE(intArray_capacity(&array) >= 40);

  EXPECT_CRASH(intArray_pop(&array));

  EXPECT_ERR(intArray_free(&array), ARR_SUCCESS);
  EXPECT_SIZE(intArray_size(&array), 0);
  EXPECT_SIZE(intArray_capacity(&array), 0);
  EXPECT_TRUE(array.data == NULL);

  EXPECT_CRASH(intArray_free(&array));
}

static void test_push_after_free(void) {
  Array(int) array = {0};

  EXPECT_ERR(intArray_init(&array), ARR_SUCCESS);
  EXPECT_ERR(intArray_push(&array, 10), ARR_SUCCESS);
  EXPECT_ERR(intArray_free(&array), ARR_SUCCESS);

  EXPECT_ERR(intArray_push(&array, 20), ARR_SUCCESS);
  EXPECT_SIZE(intArray_size(&array), 1);
  EXPECT_SIZE(intArray_capacity(&array), INIT_CAPACITY);
  EXPECT_INT(intArray_get_unsafe(&array, 0), 20);

  EXPECT_ERR(intArray_free(&array), ARR_SUCCESS);
}

static void test_double_array(void) {
  Array(double) array = {0};

  EXPECT_ERR(doubleArray_init(&array), ARR_SUCCESS);
  EXPECT_ERR(doubleArray_push(&array, 1.5), ARR_SUCCESS);
  EXPECT_ERR(doubleArray_push(&array, 2.5), ARR_SUCCESS);
  EXPECT_ERR(doubleArray_push(&array, 3.5), ARR_SUCCESS);

  double value = 0.0;

  EXPECT_ERR(doubleArray_get(&array, 1, &value), ARR_SUCCESS);
  EXPECT_DOUBLE_CLOSE(value, 2.5, 0.000001);

  EXPECT_ERR(doubleArray_set(&array, 1, 10.25), ARR_SUCCESS);
  EXPECT_ERR(doubleArray_get(&array, 1, &value), ARR_SUCCESS);
  EXPECT_DOUBLE_CLOSE(value, 10.25, 0.000001);

  doubleArray_forEach(&array, multiply_double_by_two);

  EXPECT_ERR(doubleArray_get(&array, 1, &value), ARR_SUCCESS);
  EXPECT_DOUBLE_CLOSE(value, 20.5, 0.000001);

  EXPECT_ERR(doubleArray_free(&array), ARR_SUCCESS);
}

static void test_char_array(void) {
  Array(char) array = {0};

  EXPECT_ERR(charArray_init(&array), ARR_SUCCESS);
  EXPECT_ERR(charArray_push(&array, 'a'), ARR_SUCCESS);
  EXPECT_ERR(charArray_push(&array, 'b'), ARR_SUCCESS);
  EXPECT_ERR(charArray_push(&array, 'c'), ARR_SUCCESS);

  char value = '\0';

  EXPECT_ERR(charArray_get(&array, 2, &value), ARR_SUCCESS);
  EXPECT_INT(value, 'c');

  EXPECT_ERR(charArray_set(&array, 1, 'z'), ARR_SUCCESS);
  EXPECT_INT(charArray_get_unsafe(&array, 1), 'z');

  EXPECT_ERR(charArray_free(&array), ARR_SUCCESS);
}

static void test_custom_struct_array(void) {
  Array(Point) points = {0};

  EXPECT_ERR(PointArray_init(&points), ARR_SUCCESS);

  EXPECT_ERR(PointArray_push(&points, (Point){.x = 1, .y = 2}), ARR_SUCCESS);
  EXPECT_ERR(PointArray_push(&points, (Point){.x = 3, .y = 4}), ARR_SUCCESS);

  Point value = {0};

  EXPECT_ERR(PointArray_get(&points, 0, &value), ARR_SUCCESS);
  EXPECT_INT(value.x, 1);
  EXPECT_INT(value.y, 2);

  PointArray_forEach(&points, move_point);

  EXPECT_ERR(PointArray_get(&points, 0, &value), ARR_SUCCESS);
  EXPECT_INT(value.x, 11);
  EXPECT_INT(value.y, 22);

  EXPECT_ERR(PointArray_set(&points, 1, (Point){.x = 100, .y = 200}),
             ARR_SUCCESS);
  EXPECT_ERR(PointArray_get(&points, 1, &value), ARR_SUCCESS);
  EXPECT_INT(value.x, 100);
  EXPECT_INT(value.y, 200);

  EXPECT_ERR(PointArray_free(&points), ARR_SUCCESS);
}

int main(void) {
  test_int_array_lifecycle();
  test_push_after_free();
  test_double_array();
  test_char_array();
  test_custom_struct_array();

  printf("\nTests run: %d\n", tests_run);
  printf("Tests failed: %d\n", tests_failed);

  if (tests_failed == 0) {
    printf("All tests passed.\n");
    return EXIT_SUCCESS;
  }

  return EXIT_FAILURE;
}
