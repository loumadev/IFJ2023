/**
 * @file include/assertf.h
 * @author Jaroslav Louma <xlouma00@stud.fit.vutbr.cz>
 * @brief This file is part of the IFJ23 project.
 * @copyright Copyright (c) 2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "overload.h"
#include "colors.h"

#ifndef ASSERTF_H
#define ASSERTF_H

#define log_error(M, ...) errno \
	? fprintf(stderr, RED "[ASSERT] " M ": %s\n    at%s:%d\n" RESET, ## __VA_ARGS__, strerror(errno), __FILE__, __LINE__) \
	: fprintf(stderr, RED "[ASSERT] " M "\n    at %s:%d\n" RESET, ## __VA_ARGS__, __FILE__, __LINE__)
// #define assertf(A, M, ...) (!(A) && (log_error(M, ## __VA_ARGS__), abort(), 0))

#define assertf(...) overload(__assertf, __VA_ARGS__)
#define __assertf1(A) __assertf3(A, "Assertion '" #A "' failed")
#define __assertf2(A, M) __assertf3(A, M)
#define __assertf3(A, M, ...) (!(A) && (log_error(M, ## __VA_ARGS__), abort(), 0))
#define __assertf4(A, M, ...) __assertf3(A, M, ## __VA_ARGS__)
#define __assertf5(A, M, ...) __assertf3(A, M, ## __VA_ARGS__)
#define __assertf6(A, M, ...) __assertf3(A, M, ## __VA_ARGS__)
#define __assertf7(A, M, ...) __assertf3(A, M, ## __VA_ARGS__)
#define __assertf8(A, M, ...) __assertf3(A, M, ## __VA_ARGS__)

#define fassertf(...) overload(__fassertf, __VA_ARGS__)
#define __fassertf0() __fassertf2("Forced assertion triggered")
#define __fassertf1(M) __fassertf2(M)
#define __fassertf2(M, ...) (log_error(M, ## __VA_ARGS__), abort())
#define __fassertf3(M, ...) __fassertf2(M, ## __VA_ARGS__)
#define __fassertf4(M, ...) __fassertf2(M, ## __VA_ARGS__)
#define __fassertf5(M, ...) __fassertf2(M, ## __VA_ARGS__)
#define __fassertf6(M, ...) __fassertf2(M, ## __VA_ARGS__)
#define __fassertf7(M, ...) __fassertf2(M, ## __VA_ARGS__)
#define __fassertf8(M, ...) __fassertf2(M, ## __VA_ARGS__)

#define warnf(M, ...) errno \
	? fprintf(stderr, YELLOW "[WARN] " M ": %s\n    at%s:%d\n" RESET, ## __VA_ARGS__, strerror(errno), __FILE__, __LINE__) \
	: fprintf(stderr, YELLOW "[WARN] " M "\n    at %s:%d\n" RESET, ## __VA_ARGS__, __FILE__, __LINE__)

#endif

/** End of file include/assertf.h **/
