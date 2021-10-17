#include "image.h"

void
destroy_image(
	struct image *image
) {
	free(image->format_name);

	for (u_int64_t i = 0; i < image->amount_of_channels; i++) {
		free(image->channel_info[i].name);

		image->channel_info[i].name = NULL;
		image->channel_info[i].size = 0;
	}

	free(image->channel_info);

	image->channel_info = NULL;
	image->amount_of_channels = 0;

	free(image->meta_data);

	image->meta_data = NULL;
	image->meta_data_size = 0;

	image->width = 0;
	image->height = 0;

	free(image->compression_algorithm);

	image->compression_algorithm = NULL;

	free(image->data);

	image->data = NULL;
	image->data_size = 0;

	return;
}

int
read_image(
	struct image *image,
	FILE * restrict stream
) {
	memset(image, 0, sizeof(struct image));

	// read format_name
	{
		size_t capacity = 0;
		ssize_t size = getdelim(&image->format_name, &capacity, '\0', stream);

		if (size < 0 || strcmp(image->format_name, "image") != 0) {
			destroy_image(image);

			return -1;
		}
	}

	// read amount_of_channels
	{
		size_t size = fread(&image->amount_of_channels, sizeof(u_int64_t), 1, stream);

		if (size < 1 || feof(stream) || ferror(stream)) {
			destroy_image(image);

			return -1;
		}
	}

	// read channel_info
	if (image->amount_of_channels > 0) {
		image->channel_info = calloc(image->amount_of_channels, sizeof(image->channel_info[0]));
		if (image->channel_info == NULL) {
			image->amount_of_channels = 0;
			destroy_image(image);

			return -1;
		}

		for (u_int64_t i = 0; i < image->amount_of_channels; i++) {
			// read channel_info[i].name
			{
				size_t capacity = 0;
				ssize_t size = getdelim(&image->channel_info[i].name, &capacity, '\0', stream);

				if (size < 0) {
					destroy_image(image);

					return -1;
				}
			}
			// read channel_info[i].size
			{
				size_t size = fread(&image->channel_info[i].size, sizeof(u_int8_t), 1, stream);

				if (size < 1 || feof(stream) || ferror(stream)) {
					destroy_image(image);

					return -1;
				}
			}
		}
	}
	// read meta_data_size
	{
		size_t size = fread(&image->meta_data_size, sizeof(u_int64_t), 1, stream);

		if (size < 1 || feof(stream) || ferror(stream)) {
			destroy_image(image);

			return -1;
		}
	}

	// read meta_data
	if (0 < image->meta_data_size) {
		image->meta_data = malloc(image->meta_data_size * sizeof(u_int8_t));
		if (image->meta_data == NULL) {
			destroy_image(image);

			return -1;
		}

		size_t size = fread(image->meta_data, sizeof(u_int8_t), image->meta_data_size, stream);

		if (size < image->meta_data_size || feof(stream) || ferror(stream)) {
			destroy_image(image);

			return -1;
		}
	}

	// read width
	{
		size_t size = fread(&image->width, sizeof(u_int64_t), 1, stream);

		if (size < 1 || feof(stream) || ferror(stream)) {
			destroy_image(image);

			return -1;
		}
	}

	// read height
	{
		size_t size = fread(&image->height, sizeof(u_int64_t), 1, stream);

		if (size < 1 || feof(stream) || ferror(stream)) {
			destroy_image(image);

			return -1;
		}
	}
	// read compression_algorithm
	{
		size_t capacity = 0;
		ssize_t size = getdelim(&image->compression_algorithm, &capacity, '\0', stream);

		if (size < 0) {
			destroy_image(image);

			return -1;
		}
	}
	// read data_size
	{
		size_t size = fread(&image->data_size, sizeof(u_int64_t), 1, stream);

		if (size < 1 || feof(stream) || ferror(stream)) {
			destroy_image(image);

			return -1;
		}
	}

	// read data
	if (0 < image->data_size) {
		image->data = malloc(image->data_size * sizeof(u_int8_t));
		if (image->data == NULL) {
			destroy_image(image);

			return -1;
		}

		size_t size = fread(image->data, sizeof(u_int8_t), image->data_size, stream);

		if (size < image->data_size || feof(stream) || ferror(stream)) {
			destroy_image(image);

			return -1;
		}
	}

	return 0;
}

int
write_image(
	struct image *image,
	FILE * restrict stream
) {
	if (strcmp(image->format_name, "image") != 0)
		return -1;

	fwrite(image->format_name, sizeof(char), strlen(image->format_name) + 1, stream);

	fwrite(&image->amount_of_channels, sizeof(u_int64_t), 1, stream);
	if (image->channel_info != NULL) {
		for (u_int64_t i = 0; i < image->amount_of_channels; i++) {
			fwrite(image->channel_info[i].name, sizeof(char), strlen(image->channel_info[i].name) + 1, stream);

			fwrite(&image->channel_info[i].size, sizeof(u_int8_t), 1, stream);
		}
	}

	fwrite(&image->meta_data_size, sizeof(u_int64_t), 1, stream);
	if (image->meta_data != NULL)
		fwrite(image->meta_data, sizeof(u_int8_t), image->meta_data_size, stream);

	fwrite(&image->width, sizeof(u_int64_t), 1, stream);
	fwrite(&image->height, sizeof(u_int64_t), 1, stream);

	fwrite(image->compression_algorithm, sizeof(char), strlen(image->compression_algorithm) + 1, stream);

	fwrite(&image->data_size, sizeof(u_int64_t), 1, stream);
	if (image->data != NULL)
		fwrite(image->data, sizeof(u_int8_t), image->data_size, stream);

	if (ferror(stream))
		return -1;

	return 0;
}
