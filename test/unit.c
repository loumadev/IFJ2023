#include "unit.h"

#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#define get_duration_ms(start) round((double)(clock() - start) * 1000 / CLOCKS_PER_SEC)
#define print(format, ...) if(!unit_g.is_silent) printf(format, ## __VA_ARGS__)

static struct unit_global unit_g;

void unit_begin() {
	unit_g.files_count = 0;
	unit_g.suites_count = 0;

	unit_g.tests_run_total_count = 0;
	unit_g.current_test_description = NULL;
	unit_g.last_assertion = NULL;
	unit_g.last_error_message = NULL;

	unit_g.files_failed_count = 0;
	unit_g.suites_failed_count = 0;
	unit_g.tests_failed_count = 0;

	unit_g.has_current_file_failed = false;
	unit_g.has_current_suit_failed = false;
	unit_g.has_current_test_failed = false;

	unit_g.start_time = clock();
	unit_g.current_test_start_time = 0;
	unit_g.duration_ms = 0;
}

void unit_end() {
	unit_g.duration_ms = get_duration_ms(unit_g.start_time);
	unit_print_results();
}

void unit_set_silent(bool silent) {
	unit_g.is_silent = silent;
}

int unit_get_result() {
	return unit_g.tests_failed_count;
}


void unit_register_file(char *name) {
	print("\n" WHITE BOLD "%s\n" RST, name);

	unit_g.files_count++;
	unit_g.has_current_file_failed = false;
}

void unit_register_suit(char *desc) {
	print(" • %s\n", desc);

	unit_g.suites_count++;
	unit_g.has_current_suit_failed = false;
	unit_g.current_test_start_time = 1;
}


void unit_run_test_start(char *description, char *location) {
	(void)(location);
	unit_g.has_current_test_failed = false;
	unit_g.current_test_description = description;

	print("    " RUNNING "▶  " RST "%s (running) " DARK_GREY "%s" RST, unit_g.current_test_description, location);
	fflush(stdout);

	unit_g.current_test_start_time = clock();
}

void unit_run_test_finish() {
	long duration_ms = get_duration_ms(unit_g.current_test_start_time);

	if(unit_g.has_current_test_failed) {
		unit_g.tests_failed_count++;

		if(!unit_g.has_current_suit_failed) {
			unit_g.suites_failed_count++;
			unit_g.has_current_suit_failed = true;
		}

		// print("\r    " FAILED "✕  " RST "%s   " RED "%s: %s\n" RST, unit_g.current_test_description, unit_g.last_assertion, unit_g.last_error_message);
		print(ERASE_LINE "\r    " FAILED "✕  " RST "%s   \n" RST, unit_g.current_test_description);
		print("         " RED "%s   %s\n" RST, unit_g.last_assertion, unit_g.last_error_message);
	} else {
		print(ERASE_LINE "\r    " PASSED "✓  " RST "%s (%ldms)\n" RST, unit_g.current_test_description, duration_ms);
	}

	// TODO: Maybe use String as error message and free it here?

	unit_g.tests_run_total_count++;
	unit_g.has_current_test_failed = false;
	unit_g.current_test_start_time = 0;
}


void unit_run_test_suite(char *desc, void (*test)(void)) {
	unit_register_suit(desc);

	test();

	if(unit_g.current_test_start_time == 1) {
		print(DARK_GREY "\r    !  Test suite did not run any tests\n" RST);
	}

	if(unit_g.has_current_suit_failed && !unit_g.has_current_file_failed) {
		unit_g.files_failed_count++;
		unit_g.has_current_file_failed = true;
	}

	unit_g.has_current_suit_failed = false;
}

bool unit_assert(bool condition, char *name, char *message) {
	unit_g.last_assertion = name;
	unit_g.last_error_message = message;

	if(condition) {
		return false;
	} else {
		unit_g.has_current_test_failed = true;
		return true;
	}
}

void unit_print_results() {
	print("\n");

	if(unit_g.tests_failed_count == 0) {
		print(WHITE BOLD "Files:       " PASSED "%d passed" RST ", " GREY "%d total\n" RST, unit_g.files_count - unit_g.files_failed_count, unit_g.files_count);
		print(WHITE BOLD "Test Suites: " PASSED "%d passed" RST ", " GREY "%d total\n" RST, unit_g.suites_count - unit_g.suites_failed_count, unit_g.suites_count);
		print(WHITE BOLD "Tests:       " PASSED "%d passed" RST ", " GREY "%d total\n" RST, unit_g.tests_run_total_count - unit_g.tests_failed_count, unit_g.tests_run_total_count);
		print(WHITE BOLD "Time:        " YELLOW "%.3fs\n" RST, (float)unit_g.duration_ms / 1000);
	} else {
		print(WHITE BOLD "Files:       " FAILED "%d failed" RST ", " GREY "%d total\n" RST, unit_g.files_failed_count, unit_g.files_count);
		print(WHITE BOLD "Test Suites: " FAILED "%d failed" RST ", " GREY "%d total\n" RST, unit_g.suites_failed_count, unit_g.suites_count);
		print(WHITE BOLD "Tests:       " FAILED "%d failed" RST ", " GREY "%d total\n" RST, unit_g.tests_failed_count, unit_g.tests_run_total_count);
		print(WHITE BOLD "Time:        " YELLOW "%.3fs\n" RST, (float)unit_g.duration_ms / 1000);
	}
}

#undef print
#undef get_duration_ms
