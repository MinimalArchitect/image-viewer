#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <math.h>

long long strtonum(const char *string, long long minimum_value, long long maximum_value, const char **error_string);

long double strtodecnum(const char *string, long double minimum_value, long double maximum_value, const char **error_string);

#endif /* UTIL_H*/
