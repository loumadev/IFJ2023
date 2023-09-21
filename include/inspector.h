#include <stdio.h>

#include "colors.h"
#include "overload.h"

#ifndef INSPECTOR_H
#define INSPECTOR_H

#include "internal/String.h"
#include "internal/Array.h"
#include "internal/HashMap.h"
#include "internal/TextRange.h"

#define TYPE YELLOW
#define FIELD BLUE
#define NUMBER GREEN
#define POINTER PURPLE
#define STRING GOLD
#define BOOLEAN AQUA
#define NONE RED
#define PUNCT GREY
#define DUMP DARK_AQUA

#define ENDL RST "\n"
#define PTR_FORMAT POINTER "<Object at %p>" RST
#define NULL_FORMAT NONE "NULL" RST

#define indent(n) for(unsigned int i = 0; i < n; i++) printf("  ");
#define _indent(is_prop) if(!(is_prop)) {indent(depth);}

void _print_char(char var);
void _print_string(char *var);
void _print_long(long var);
void _print_ulong(unsigned long var);
void _print_double(double var);
void _print_pointer(void *var);
void _print_boolean(int var);
void _print_String(String *var);
void _print_Array(Array *var);
void _print_HashMap(HashMap *var);
void _print_TextRange(TextRange *var);

#define dumpvar(...) do { \
		printf("\n" DARK_GREY "from %s:%d:" ENDL, __FILE__, __LINE__); \
		overload(__dumpvar, __VA_ARGS__); \
} while(0)
#define __dumpvar1(var) do { \
		printf(DUMP #var PUNCT " = "); \
		_Generic((var), \
		         /* Native types */ \
		         char : _print_char, \
		         char* : _print_string, \
		         unsigned long : _print_ulong, \
		         long : _print_long, \
		         unsigned int : _print_ulong, \
		         int : _print_long, \
		         float : _print_double, \
		         double : _print_double, \
		         bool : _print_boolean, \
		         void* : _print_pointer, \
		         /* Internal type */ \
		         String * : _print_String, \
		         Array * : _print_Array, \
		         HashMap * : _print_HashMap, \
		         TextRange * : _print_TextRange, \
			 default: _print_pointer \
		)(var); \
} while(0);
#define __dumpvar2(var1, var2) do {__dumpvar1(var1); __dumpvar1(var2);} while(0)
#define __dumpvar3(var1, var2, var3) do {__dumpvar2(var1, var2); __dumpvar1(var3);} while(0)
#define __dumpvar4(var1, var2, var3, var4) do {__dumpvar3(var1, var2, var3); __dumpvar1(var4);} while(0)
#define __dumpvar5(var1, var2, var3, var4, var5) do {__dumpvar4(var1, var2, var3, var4); __dumpvar1(var5);} while(0)
#define __dumpvar6(var1, var2, var3, var4, var5, var6) do {__dumpvar5(var1, var2, var3, var4, var5); __dumpvar1(var6);} while(0)
#define __dumpvar7(var1, var2, var3, var4, var5, var6, var7) do {__dumpvar6(var1, var2, var3, var4, var5, var6); __dumpvar1(var7);} while(0)
#define __dumpvar8(var1, var2, var3, var4, var5, var6, var7, var8) do {__dumpvar7(var1, var2, var3, var4, var5, var6, var7); __dumpvar1(var8);} while(0)
// #define println(format, ...) // format can be format or any variable

#define print_separator() printf(PUNCT ", " RST)
#define print_type(name) printf(TYPE name RST)
#define print_null_type(name) do {_indent(isProperty); printf(TYPE name PUNCT " { " NULL_FORMAT PUNCT " }" ENDL);} while(0)

#define print_type_head(name, type) printf(TYPE name " " PUNCT type " " RST)
#define print_type_tail(type) printf(" " PUNCT type ENDL)

#define print_type_begin(name) do {_indent(isProperty); printf(TYPE name PUNCT " {" ENDL); depth++;} while(0)
#define print_type_end() do {depth--; indent(depth); printf(PUNCT "}" ENDL);} while(0)
#define print_obj_begin(is_prop) do {_indent((is_prop)); printf(PUNCT "{" ENDL); depth++;} while(0)
#define print_obj_end() do {depth--; indent(depth); printf(PUNCT "}" ENDL);} while(0)
#define print_arr_begin(is_prop) do {_indent((is_prop)); printf(PUNCT "[" ENDL); depth++;} while(0)
#define print_arr_end() do {depth--; indent(depth); printf(PUNCT "]" ENDL);} while(0)

#define print_null() printf(NULL_FORMAT RST)
#define print_null_nl() printf(NULL_FORMAT ENDL)
#define print_pointer(ptr) printf(PTR_FORMAT RST, (ptr))
#define print_pointer_nl(ptr) printf(PTR_FORMAT ENDL, (ptr))

#define print_null_field() printf(NULL_FORMAT ENDL)
#define print_pointer_field(ptr) printf(PTR_FORMAT ENDL, (ptr))
#define print_boolean_field(b) printf(BOOLEAN "%s" ENDL, (b) ? "true" : "false")

#define print_formatted_field(format, ...) do {indent(depth); printf(FIELD format PUNCT ": " RST, ## __VA_ARGS__);} while(0)

#define print_field(...) do {indent(depth); overload(__print_field, __VA_ARGS__);} while(0)
#define __print_field1(name) printf(FIELD name PUNCT ": " RST)
// #define __print_field2(name, type) printf(FIELD name PUNCT ": " type)
// #define __print_field2(format, variable) printf(FIELD format PUNCT ": " RST, variable)
#define __print_field3(name, format, ...) printf(FIELD name PUNCT ": " RST format ENDL, ## __VA_ARGS__)

#define __print_field4(name, format, ...) __print_field3(name, format, ## __VA_ARGS__)
#define __print_field5(name, format, ...) __print_field3(name, format, ## __VA_ARGS__)
#define __print_field6(name, format, ...) __print_field3(name, format, ## __VA_ARGS__)
#define __print_field7(name, format, ...) __print_field3(name, format, ## __VA_ARGS__)
#define __print_field8(name, format, ...) __print_field3(name, format, ## __VA_ARGS__)

void print_string(char *str, char *end);

#endif
