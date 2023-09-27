#include <time.h>
#include <string.h>
#include "internal/String.h"
#include "inspector.h"

#ifndef UNIT_H
#define UNIT_H

#define RUNNING YELLOW
#define PASSED GREEN
#define FAILED RED
#define MESSAGE GREY
#define ERASE_LINE "\033[2K"

struct unit_global {
	int files_count;
	int suites_count;

	int tests_run_total_count;
	char *current_test_description;
	char *last_assertion;
	char *last_error_message;

	int files_failed_count;
	int suites_failed_count;
	int tests_failed_count;

	bool has_current_file_failed;
	bool has_current_suit_failed;
	bool has_current_test_failed;

	bool is_silent;

	clock_t start_time;
	clock_t current_test_start_time;
	long duration_ms;
};

void unit_begin();
void unit_end();
void unit_set_silent(bool silent);
int unit_get_result();
void unit_register_file(char *name);
void unit_register_suit(char *desc);
void unit_run_test_start(char *description, char *location);
void unit_run_test_finish();
void unit_run_test_suite(char *desc, void (*test)(void));
bool unit_assert(bool condition, char *name, char *message);
void unit_print_results();

// Internals

#define __locationFormat(file, line) DARK_GREY "(at " file ":" line ")" RST
#define __location() __locationFormat(__FILE__, "%d")
#define __string(format, ...) String_fromFormat(format, ## __VA_ARGS__)->value
#define __error(format, ...) __string(RED format " " __location() RST, ## __VA_ARGS__, __LINE__)

#define UNIT_EXPECT(condition, message) if(unit_assert(condition, #condition, message)) break
#define UNIT_REGISTER_FILE(name) unit_register_file(name)
#define UNIT_REGISTER_SUIT(func, desc, file, line) unit_run_test_suite(WHITE desc " " __locationFormat(file, line) RST, func)
#define UNIT_BEGIN() unit_begin()
#define UNIT_END() unit_end()
#define UNIT_RESULT() unit_get_result()
#define UNIT_SILENT(state) unit_set_silent(state)

// For user to use

#define DESCRIBE(id, name) void unit__ ## id()

#define TEST_BEGIN(name) unit_run_test_start(name, __string(__location(), __LINE__)); do
#define TEST_END() while(0); unit_run_test_finish();
#define TEST(name, block) TEST_BEGIN(name) block TEST_END();

#define EXPECT_TRUE(condition) UNIT_EXPECT((condition) == true, __error("Expected true, got false "))
#define EXPECT_FALSE(condition) UNIT_EXPECT((condition) == false, __error("Expected false, got true "))

#define EXPECT_EQUAL_STRING(a, b) UNIT_EXPECT(strcmp(a, b) == 0, __error("Expected " #a " == " #b ", got \"%s\" != \"%s\"", a, b))
#define EXPECT_EQUAL_CHAR(a, b) UNIT_EXPECT(a == b, __error("Expected " #a " == " #b ", got '%c' != '%c'", a, b))
#define EXPECT_EQUAL_INT(a, b) UNIT_EXPECT(a == b, __error("Expected " #a " == " #b ", got %ld != %ld", a, b))
#define EXPECT_EQUAL_FLOAT(a, b) UNIT_EXPECT(a == b, __error("Expected " #a " == " #b ", got %f != %f", (float)a, (float)b))

#define EXPECT_NOT_EQUAL_STRING(a, b) UNIT_EXPECT(strcmp(a, b) != 0, __error("Expected " #a " != " #b ", got \"%s\" == \"%s\"", a, b))
#define EXPECT_NOT_EQUAL_CHAR(a, b) UNIT_EXPECT(a != b, __error("Expected " #a " != " #b ", got '%c' == '%c'", a, b))
#define EXPECT_NOT_EQUAL_INT(a, b) UNIT_EXPECT(a != b, __error("Expected " #a " != " #b ", got %ld == %ld", a, b))
#define EXPECT_NOT_EQUAL_FLOAT(a, b) UNIT_EXPECT(a != b, __error("Expected " #a " != " #b ", got %f == %f", (float)a, (float)b))

#define EXPECT_GREATER_CHAR(a, b) UNIT_EXPECT(a > b, __error("Expected " #a " > " #b ", got '%c' <= '%c'", a, b))
#define EXPECT_GREATER_INT(a, b) UNIT_EXPECT(a > b, __error("Expected " #a " > " #b ", got %ld <= %ld", a, b))
#define EXPECT_GREATER_FLOAT(a, b) UNIT_EXPECT(a > b, __error("Expected " #a " > " #b ", got %lf <= %lf", a, b))

#define EXPECT_GREATER_OR_EQUAL_CHAR(a, b) UNIT_EXPECT(a >= b, __error("Expected " #a " >= " #b ", got '%c' < '%c'", a, b))
#define EXPECT_GREATER_OR_EQUAL_INT(a, b) UNIT_EXPECT(a >= b, __error("Expected " #a " >= " #b ", got %ld < %ld", a, b))
#define EXPECT_GREATER_OR_EQUAL_FLOAT(a, b) UNIT_EXPECT(a >= b, __error("Expected " #a " >= " #b ", got %lf < %lf", a, b))

#define EXPECT_LESS_CHAR(a, b) UNIT_EXPECT(a < b, __error("Expected " #a " < " #b ", got '%c' >= '%c'", a, b))
#define EXPECT_LESS_INT(a, b) UNIT_EXPECT(a < b, __error("Expected " #a " < " #b ", got %ld >= %ld", a, b))
#define EXPECT_LESS_FLOAT(a, b) UNIT_EXPECT(a < b, __error("Expected " #a " < " #b ", got %lf >= %lf", a, b))

#define EXPECT_LESS_OR_EQUAL_CHAR(a, b) UNIT_EXPECT(a <= b, __error("Expected " #a " <= " #b ", got '%c' > '%c'", a, b))
#define EXPECT_LESS_OR_EQUAL_INT(a, b) UNIT_EXPECT(a <= b, __error("Expected " #a " <= " #b ", got %ld > %ld", a, b))
#define EXPECT_LESS_OR_EQUAL_FLOAT(a, b) UNIT_EXPECT(a <= b, __error("Expected " #a " <= " #b ", got %lf > %lf", a, b))

#endif
