/**
 * @file src/inspector.c
 * @author Jaroslav Louma <xlouma00@stud.fit.vutbr.cz>
 * @brief This file is part of the IFJ23 project.
 * @copyright Copyright (c) 2023
 */

#include "inspector.h"

char* format_char(char ch) {
	switch(ch) {
		case 0: return "NUL";
		case 1: return "SOH";
		case 2: return "STX";
		case 3: return "ETX";
		case 4: return "EOT";
		case 5: return "ENQ";
		case 6: return "ACK";
		case 7: return "BEL";
		case 8: return "BS";
		case 9: return "TAB";
		case 10: return "LF";
		case 11: return "VT";
		case 12: return "FF";
		case 13: return "CR";
		case 14: return "SO";
		case 15: return "SI";
		case 16: return "DLE";
		case 17: return "DC1";
		case 18: return "DC2";
		case 19: return "DC3";
		case 20: return "DC4";
		case 21: return "NAK";
		case 22: return "SYN";
		case 23: return "ETB";
		case 24: return "CAN";
		case 25: return "EM";
		case 26: return "SUB";
		case 27: return "ESC";
		case 28: return "FS";
		case 29: return "GS";
		case 30: return "RS";
		case 31: return "US";
		// default: return ch;
		default: return (char[2]){ch, 0};
	}
}

void print_string(char *start, char *end) {
	if(!start) return (void)print_null_nl();

	printf(STRING "\"");
	char *ptr = start;
	while((end && (ptr < end)) || (!end && *ptr)) {
		if(*ptr == '"') {
			printf("\\\"");
		} else if(*ptr == '\\') {
			printf("\\\\");
		} else if(*ptr == '\n') {
			printf("\\n");
		} else if(*ptr == '\r') {
			printf("\\r");
		} else if(*ptr == '\t') {
			printf("\\t");
		} else {
			putchar(*ptr);
		}

		ptr++;
	}
	printf("\"" RST);
}

void _print_char(char var) {
	printf(STRING "'%c'" ENDL, var);
}

void _print_string(char *var) {
	if(!var) return (void)printf(NULL_FORMAT);
	printf(STRING "\"%s\"" ENDL, var);
}

void _print_long(long var) {
	printf(NUMBER "%ld" ENDL, var);
}

void _print_ulong(unsigned long var) {
	printf(NUMBER "%lu" ENDL, var);
}

void _print_double(double var) {
	printf(NUMBER "%f" ENDL, var);
}

void _print_pointer(void *var) {
	if(!var) return (void)printf(NULL_FORMAT);
	printf(PTR_FORMAT ENDL, var);
}

void _print_boolean(int var) {
	printf(BOOLEAN "%s" ENDL, var ? "true" : "false");
}

#define PRINT_COMPACT

void _print_String(String *var) {
	#ifdef PRINT_COMPACT
	String_print_compact(var);
	#else
	String_print(var, 0, 0);
	#endif
}

void _print_Array(Array *var) {
	#ifdef PRINT_COMPACT
	Array_print_compact(var);
	#else
	Array_print(var, 0, 0);
	#endif
}

void _print_HashMap(HashMap *var) {
	#ifdef PRINT_COMPACT
	HashMap_print_compact(var);
	#else
	HashMap_print(var, 0, 0);
	#endif
}

void _print_TextRange(TextRange *var) {
	#ifdef PRINT_COMPACT
	TextRange_print_compact(var);
	#else
	TextRange_print(var, 0, 0);
	#endif
}

void _print_Token(Token *var) {
	Token_print(var, 0, 0);
}

/** End of file src/inspector.c **/
