#ifndef IMAGE_H
#define IMAGE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct image {
	char *format_name;

	u_int64_t amount_of_channels;
	struct {
		char *name;
		u_int8_t size; // in bytes
	} *channel_info;

	u_int64_t meta_data_size;
	u_int8_t *meta_data;

	u_int64_t width; // in pixel
	u_int64_t height; // in pixel

	char *compression_algorithm;

	u_int64_t data_size;
	u_int8_t *data;
};

void destroy_image(struct image *image);

int read_image(struct image *image, FILE * restrict stream);
int write_image(struct image *image, FILE * restrict stream);

#endif /* IMAGE_H */
