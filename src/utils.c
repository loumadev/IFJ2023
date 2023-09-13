#include "utils.h"

void print_string(char *start, char *end) {
	if(!start) {
		printf("NULL");
		return;
	}

	putchar('"');
	char *ptr = start;
	while((end && (ptr < end)) || (!end && *ptr)) {
		if(*ptr == '"') {
			printf("\\\"");
		}
		else if(*ptr == '\\') {
			printf("\\\\");
		}
		else if(*ptr == '\n') {
			printf("\\n");
		}
		else if(*ptr == '\r') {
			printf("\\r");
		}
		else if(*ptr == '\t') {
			printf("\\t");
		}
		else {
			putchar(*ptr);
		}

		ptr++;
	}
	putchar('"');
}