#include "fileinfo.h"

void
add_fileinfo(
	struct client_state *state,
	const char *filename
) {
	// NOTE(lukas): check if file is readable
	if (access(filename, R_OK) < 0) {
		if (!state->quiet)
			warn("%s", filename);

		return;
	}

	state->amount_of_fileinfos++;

	state->fileinfos = realloc(state->fileinfos, state->amount_of_fileinfos * sizeof(struct fileinfo));
	if (state->fileinfos == NULL)
		err(EXIT_FAILURE, "could not add file");

	state->fileinfos[state->amount_of_fileinfos - 1] = (struct fileinfo) {
		.name = strdup(filename),
		.mark = false,
	};

	if (state->fileinfos[state->amount_of_fileinfos - 1].name == NULL)
		err(EXIT_FAILURE, "could not add file");

	return;
}

void
add_fileinfos_from_file(
	struct client_state *state,
	FILE *stream
) {
	char *filename = NULL;
	size_t filename_capacity = 0;
	ssize_t filename_size = 0;

	while ((filename_size = getline(&filename, &filename_capacity, stream)) != -1) {
		filename[filename_size - 1] = '\0';

		// NOTE(lukas): filenames cannot be empty
		if (filename[0] == '\0')
			continue;

		add_fileinfo(state, filename);
	}

	free(filename);

	return;
}

static int32_t
fileinfo_cmp(
	const void *lhs,
	const void *rhs
) {
	const struct fileinfo *_lhs = lhs;
	const struct fileinfo *_rhs = rhs;

	return strcoll(_lhs->name, _rhs->name);
}

void
add_fileinfos_recursively(
	struct client_state *state,
	const char *filename
) {
	// NOTE(lukas): empty filename's are not to be added or searchable
	if (filename[0] == '\0')
		return;

	struct stat status;
	if (stat(filename, &status) < 0) {
		if (!state->quiet)
			warn("%s", filename);
		return;
	}

	if (!S_ISDIR(status.st_mode)) {
		add_fileinfo(state, filename);

	} else {
		size_t start = state->amount_of_fileinfos;

		const char *directory_name = filename;

		DIR *directory = opendir(directory_name);

		struct dirent *directory_entry;
		while ((directory_entry = readdir(directory)) != NULL) {
			char *filename = NULL;

			// NOTE(lukas): hidden files are not to be added
			if (directory_entry->d_name[0] == '.')
				continue;

			int error_value;

			if (directory_name[strlen(directory_name) - 1] == '/') {
				error_value = asprintf(&filename, "%s%s", directory_name, directory_entry->d_name);

			} else {
				error_value = asprintf(&filename, "%s/%s", directory_name, directory_entry->d_name);
			}

			if (error_value < 0 || filename == NULL)
				err(EXIT_FAILURE, "asprintf");

			add_fileinfos_recursively(state, filename);

			free(filename);
		}

		closedir(directory);

		qsort(state->fileinfos + start, state->amount_of_fileinfos - start, sizeof(struct fileinfo), fileinfo_cmp);
	}

	return;
}

void
remove_fileinfo(
	struct client_state *state,
	uint32_t index
) {
	free(state->fileinfos[index].name);
	state->fileinfos[index].name = NULL;
	state->fileinfos[index].mark = false;

	state->amount_of_fileinfos--;

	size_t elements_to_move = state->amount_of_fileinfos - index;

	memmove(&state->fileinfos[index], &state->fileinfos[index + 1], elements_to_move * sizeof(struct fileinfo));

	if (state->amount_of_fileinfos == 0) {
		free(state->fileinfos);
		state->fileinfos = NULL;

	} else {
		state->fileinfos = realloc(state->fileinfos, state->amount_of_fileinfos * sizeof(struct fileinfo));
		if (state->fileinfos == NULL)
			err(EXIT_FAILURE, "could not remove file info");
	}

	return;
}

void
toggle_fileinfo_mark(
	struct client_state *state,
	uint32_t index
) {
	state->fileinfos[index].mark = !state->fileinfos[index].mark;

	return;
}

void
write_fileinfo_to_file_depending_on_mark
(
	struct client_state *state,
	FILE *stream
) {
	for (int32_t i = 0; i < state->amount_of_fileinfos; i++) {
		if (state->fileinfos[i].mark)
			fprintf(stream, "%s", state->fileinfos[i].name);
	}

	return;
}
