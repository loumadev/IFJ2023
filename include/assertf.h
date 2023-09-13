#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#include "colors.h"

#ifndef ASSERTF_H
#define ASSERTF_H

#define log_error(M, ...) errno \
	? fprintf(stderr, RED "[ASSERT] " M ": %s\n  at%s:%d\n" RESET, strerror(errno), ## __VA_ARGS__, __FILE__, __LINE__) \
	: fprintf(stderr, RED "[ASSERT] " M "\n    at %s:%d\n" RESET, ## __VA_ARGS__, __FILE__, __LINE__)
#define assertf(A, M, ...) (!(A) && (log_error(M, ## __VA_ARGS__), abort(), 0))

#endif
