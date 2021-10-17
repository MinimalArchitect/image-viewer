#include "options.h"

#include "config.h"

static void
version(
	void
) {
	extern char *__progname;

	fprintf(stdout, "%s %s\n", __progname, VERSION);

	return;
}

void
usage(
	void
) {
	extern char *__progname;

	fprintf(stderr, "usage: %s [bfioqrv] [-g geometry] [-G gamma] [-n num] [-s mode] [-z zoom] file...\n", __progname);

	exit(EXIT_FAILURE);
}

struct options
get_options(
	int32_t argc,
	char **argv
) {
	struct options options = {
		.read_stdin = false,
		.write_stdout = false,
		.recursive_filename_search = false,
		.starting_file_number = 0,

		.scale_mode = SCALE_FIT,
		.zoom = 100.0f,
		.gamma = 0,

		.fullscreen = false,
		.bar_hidden = false,
		.geometry = "",

		.quiet = false,
	};

	int option;
	while ((option = getopt(argc, argv, "iorn: s:z:G: fbg: qv")) != -1) {
		switch (option) {
		case 'i':
			options.read_stdin = true;
			break;

		case 'o':
			options.write_stdout = true;
			break;

		case 'r':
			options.recursive_filename_search = true;
			break;

		case 'n': {
			const char *error_string = NULL;
			options.recursive_filename_search = strtonum(optarg, 0, UINT_MAX, &error_string);
			if (error_string != NULL)
				errx(EXIT_FAILURE, "start image number is %s: %s", error_string, optarg);
			break;
		}

		case 's': {
			if (strlen(optarg) != 1)
				errx(EXIT_FAILURE, "invalid scale mode");

			const char *scale_modes = "dfwh";
			char *tmp = strchr(scale_modes, optarg[0]);

			if (tmp == NULL || *tmp == '\0')
				errx(EXIT_FAILURE, "invalid scale mode");

			options.scale_mode = tmp - scale_modes;
			break;
		}
		case 'z': {
			const char *error_string = NULL;
			options.zoom = strtodecnum(optarg, 0.0f, FLT_MAX, &error_string);
			if (error_string != NULL)
				errx(EXIT_FAILURE, "zoom value is %s: %s", error_string, optarg);
			break;
		}
		case 'G': {
			const char *error_string = NULL;
			options.gamma = strtodecnum(optarg, -gamma_range, gamma_range, &error_string);
			if (error_string != NULL)
				errx(EXIT_FAILURE, "zoom value is %s: %s", error_string, optarg);
			break;
		}
		case 'f':
			options.fullscreen = true;
			break;

		case 'b':
			options.bar_hidden = true;
			break;

		case 'g':
			options.geometry = optarg;
			break;

		case 'q':
			options.quiet = true;
			break;

		case 'v':
			version();
			break;

		default:
			usage();
		}
	}

	return options;
}
