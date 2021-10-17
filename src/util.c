#include "util.h"

long long
strtonum(
	const char *string,
	long long minimum_value,
	long long maximum_value,
	const char **error_string
) {
	if (minimum_value > maximum_value) {
		if (error_string)
			*error_string = "invalid";

		errno = EINVAL;

		return 0;
	}

	char *end_string = NULL;
	long long value = strtoll(string, &end_string, 10);

	if (end_string == string || end_string[0] != '\0') {
		if (error_string != NULL)
			*error_string = "invalid";

		errno = EINVAL;

		return 0;
	}

	if (value < minimum_value || (errno == ERANGE && value == LLONG_MIN)) {
		if (error_string != NULL)
			*error_string = "too small";

		errno = ERANGE;

		return 0;
	}

	if (value > maximum_value || (errno == ERANGE && value == LLONG_MAX)) {
		if (error_string != NULL)
			*error_string = "too large";

		errno = ERANGE;

		return 0;
	}

	if (error_string != NULL)
		*error_string = NULL;

	errno = 0;

	return value;
}

long double
strtodecnum(
	const char *string,
	long double minimum_value,
	long double maximum_value,
	const char **error_string
) {
	if (minimum_value > maximum_value) {
		if (error_string)
			*error_string = "invalid";

		errno = EINVAL;

		return 0.0;
	}

	char *end_string = NULL;
	long double value = strtold(string, &end_string);

	if (end_string == string || end_string[0] != '\0') {
		if (error_string != NULL)
			*error_string = "invalid";

		errno = EINVAL;

		return 0.0;
	}

	if (value < minimum_value || (errno == ERANGE && value == -HUGE_VAL)) {
		if (error_string != NULL)
			*error_string = "too small";

		errno = ERANGE;

		return 0.0;
	}

	if (value > maximum_value || (errno == ERANGE && value == HUGE_VAL)) {
		if (error_string != NULL)
			*error_string = "too large";

		errno = ERANGE;

		return 0.0;
	}

	if (error_string != NULL)
		*error_string = NULL;

	errno = 0;

	return value;
}
