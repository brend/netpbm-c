#include "netpbm.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

size_t netpbm_data_size(const NetpbmImage *img) {
    switch (img->type) {
        case NETPBM_TYPE_PBM:
            return img->width * img->height / 8;
        case NETPBM_TYPE_PGM:
            return img->width * img->height * (img->max_value > 255 ? 2 : 1);
        case NETPBM_TYPE_PPM:
            return img->width * img->height * sizeof(NetpbmColor);
        default:
            return 0; // Unsupported type
    }
}

NetpbmImage *netpbm_create_ex(
    NetpbmType type, 
    NetpbmFormat format, 
    int width, 
    int height, 
    int max_value,
    int zero_fill
) {
    NetpbmImage *img = (NetpbmImage *)malloc(sizeof(NetpbmImage));

    // Check if memory allocation was successful
    if (img == NULL) {
        return NULL;
    }

    // Initialize the image properties
    img->type = type;
    img->format = format;
    img->width = width;
    img->height = height;
    img->max_value = max_value;

    // Allocate memory for the image data
    size_t data_size = netpbm_data_size(img);
    void *data = malloc(data_size);
    switch (type) {
        case NETPBM_TYPE_PBM:
            img->data.bitmap_data = (unsigned char *)data;
            break;
        case NETPBM_TYPE_PGM:
            img->data.gray_data = (unsigned char *)data;
            break;
        case NETPBM_TYPE_PPM:
            img->data.color_data = (NetpbmColor *)data;
            break;
        default:
            free(img);
            return NULL;
    }

    // Check if memory allocation was successful
    if (img->data.bitmap_data == NULL && img->data.gray_data == NULL && img->data.color_data == NULL) {
        free(img);
        return NULL;
    }

    // Initialize the image data
    if (zero_fill != 0) {
        size_t size = netpbm_data_size(img);
        switch (type) {
            case NETPBM_TYPE_PBM:
                memset(img->data.bitmap_data, 0, size);
                break;
            case NETPBM_TYPE_PGM:
                memset(img->data.gray_data, 0, size);
                break;
            case NETPBM_TYPE_PPM:
                memset(img->data.color_data, 0, size);
                break;
        }
    }

    return img;
}

NetpbmImage *netpbm_create(NetpbmType type, NetpbmFormat format, int width, int height, int max_value) {
    return netpbm_create_ex(type, format, width, height, max_value, 1);
}

void netpbm_free(NetpbmImage **img) {
    if (img == NULL || *img == NULL) {
        return;
    }

    if (*img != NULL) {
        switch ((*img)->type) {
            case NETPBM_TYPE_PBM:
                free((*img)->data.bitmap_data);
                break;
            case NETPBM_TYPE_PGM:
                free((*img)->data.gray_data);
                break;
            case NETPBM_TYPE_PPM:
                free((*img)->data.color_data);
                break;
        }
        free(*img);
    }
    *img = NULL;
}

NetpbmError netpbm_write_data(FILE *file, const NetpbmImage *img) {
    switch (img->type) {
        case NETPBM_TYPE_PBM:
            switch (img->format) {
                case NETPBM_FORMAT_PLAIN:
                    for (int i = 0; i < img->width * img->height; i++) {
                        int bit = (img->data.bitmap_data[i / 8] >> (7 - (i % 8))) & 1;
                        if (fprintf(file, "%d%c", bit, (i + 1) % img->width == 0 ? '\n' : ' ') < 0)
                            return NETPBM_ERROR_IO;
                    }
                    return NETPBM_SUCCESS;
                case NETPBM_FORMAT_RAW:
                    return fwrite(img->data.bitmap_data, 1, img->width * img->height / 8, file) > 0 ? NETPBM_SUCCESS : NETPBM_ERROR_IO;
                default:
                    return NETPBM_ERROR_UNSUPPORTED_FORMAT;
            }
        case NETPBM_TYPE_PGM:
            switch (img->format) {
                case NETPBM_FORMAT_PLAIN:
                    for (int i = 0; i < img->width * img->height; i++) {
                        if (fprintf(file, "%d%c", img->data.gray_data[i], (i + 1) % img->width == 0 ? '\n' : ' ') < 0)
                            return NETPBM_ERROR_IO;
                    }
                    return NETPBM_SUCCESS;
                case NETPBM_FORMAT_RAW:
                    return fwrite(img->data.gray_data, img->max_value > 255 ? 2 : 1, img->width * img->height, file) > 0 ? NETPBM_SUCCESS : NETPBM_ERROR_IO;
                default:
                    return NETPBM_ERROR_UNSUPPORTED_FORMAT;
            }
        case NETPBM_TYPE_PPM:
            switch (img->format) {
                case NETPBM_FORMAT_PLAIN:
                for (int i = 0; i < img->width * img->height; i++) {
                    NetpbmColor c = img->data.color_data[i];
                    if (fprintf(file, "%d %d %d%c", c.r, c.g, c.b, (i + 1) % img->width == 0 ? '\n' : ' ') < 0)
                        return NETPBM_ERROR_IO;
                }
                return NETPBM_SUCCESS;
                case NETPBM_FORMAT_RAW:
                    return fwrite(img->data.color_data, sizeof(NetpbmColor), img->width * img->height, file) > 0 ? NETPBM_SUCCESS : NETPBM_ERROR_IO;
                default:
                    return NETPBM_ERROR_UNSUPPORTED_FORMAT;
            }
        default:
            return NETPBM_ERROR_UNSUPPORTED_TYPE;
    }
}

NetpbmError netpbm_write_header(FILE *file, const NetpbmImage *img) {
    switch (img->type) {
        case NETPBM_TYPE_PBM:
            switch (img->format) {
                case NETPBM_FORMAT_PLAIN:
                    return fprintf(file, "P1\n%d %d\n", img->width, img->height) > 0 ? NETPBM_SUCCESS : NETPBM_ERROR_IO;
                case NETPBM_FORMAT_RAW:
                    return fprintf(file, "P4\n%d %d\n", img->width, img->height) > 0 ? NETPBM_SUCCESS : NETPBM_ERROR_IO;
                default:
                    return NETPBM_ERROR_UNSUPPORTED_FORMAT;
            }
        case NETPBM_TYPE_PGM:
            switch (img->format) {
                case NETPBM_FORMAT_PLAIN:
                    return fprintf(file, "P2\n%d %d\n%d\n", img->width, img->height, img->max_value) > 0 ? NETPBM_SUCCESS : NETPBM_ERROR_IO;
                case NETPBM_FORMAT_RAW:
                    return fprintf(file, "P5\n%d %d\n%d\n", img->width, img->height, img->max_value) > 0 ? NETPBM_SUCCESS : NETPBM_ERROR_IO;
                default:
                    return NETPBM_ERROR_UNSUPPORTED_FORMAT;
            }
        case NETPBM_TYPE_PPM:
            switch (img->format) {
                case NETPBM_FORMAT_PLAIN:
                    return fprintf(file, "P3\n%d %d\n%d\n", img->width, img->height, img->max_value) > 0 ? NETPBM_SUCCESS : NETPBM_ERROR_IO;
                case NETPBM_FORMAT_RAW:
                    return fprintf(file, "P6\n%d %d\n%d\n", img->width, img->height, img->max_value) > 0 ? NETPBM_SUCCESS : NETPBM_ERROR_IO;
                default:
                    return NETPBM_ERROR_UNSUPPORTED_FORMAT;
            }
        default:
            return NETPBM_ERROR_UNSUPPORTED_TYPE;
    }
}

NetpbmError netpbm_save(const NetpbmImage *img, const char *filename) {
    FILE *file = fopen(filename, "wb");

    // Check if the file was opened successfully
    if (file == NULL) {
        fprintf(stderr, "Error opening file for writing: %s\n", filename);
        return NETPBM_ERROR_IO;
    }

    NetpbmError err = netpbm_write_header(file, img);
    if (err != NETPBM_SUCCESS) {
        fprintf(stderr, "Error writing header: %d\n", err);
        fclose(file);
        return err;
    }

    err = netpbm_write_data(file, img);
    if (err != NETPBM_SUCCESS) {
        fprintf(stderr, "Error writing data: %d\n", err);
        fclose(file);
        return err;
    }

    fclose(file);
    return NETPBM_SUCCESS;
}

NetpbmError netpbm_read_data(FILE *file, NetpbmImage *img) {
    switch (img->type) {
        case NETPBM_TYPE_PBM:
            switch (img->format) {
                case NETPBM_FORMAT_PLAIN:
                    for (int i = 0; i < img->width * img->height; i++) {
                        int val;
                        if (fscanf(file, "%d", &val) != 1) return -1;
                        if (val != 0 && val != 1) return -1;
                        if (val) img->data.bitmap_data[i / 8] |= (1 << (7 - (i % 8)));
                    }
                    return NETPBM_SUCCESS;
                case NETPBM_FORMAT_RAW:
                {
                    size_t size = img->width * img->height / 8;
                    if (fread(img->data.bitmap_data, 1, img->width * img->height / 8, file) != size) {
                        return NETPBM_ERROR_IO;
                    }
                    return NETPBM_SUCCESS;
                }
                default:
                    return NETPBM_ERROR_UNSUPPORTED_FORMAT;
            }
        case NETPBM_TYPE_PGM:
            switch (img->format) {
                case NETPBM_FORMAT_PLAIN:
                    for (int i = 0; i < img->width * img->height; i++) {
                        int val;
                        if (fscanf(file, "%d", &val) != 1) return NETPBM_ERROR_IO;
                        img->data.gray_data[i] = (unsigned char)val;
                    }
                    return NETPBM_SUCCESS;
                case NETPBM_FORMAT_RAW: 
                {
                    size_t size = img->width * img->height * (img->max_value > 255 ? 2 : 1);
                    if (fread(img->data.gray_data, img->max_value > 255 ? 2 : 1, img->width * img->height, file) != size) {
                        return NETPBM_ERROR_IO;
                    }
                    return NETPBM_SUCCESS;
                }
                default:
                    return NETPBM_ERROR_UNSUPPORTED_FORMAT;
            }
        case NETPBM_TYPE_PPM:
            switch (img->format) {
                case NETPBM_FORMAT_PLAIN:
                    for (int i = 0; i < img->width * img->height; i++) {
                        int r, g, b;
                        if (fscanf(file, "%d %d %d", &r, &g, &b) != 3) return NETPBM_ERROR_IO;
                        img->data.color_data[i].r = (unsigned char)r;
                        img->data.color_data[i].g = (unsigned char)g;
                        img->data.color_data[i].b = (unsigned char)b;
                    }
                    return NETPBM_SUCCESS;
                case NETPBM_FORMAT_RAW:
                {
                    size_t size = img->width * img->height * sizeof(NetpbmColor);
                    if (fread(img->data.color_data, sizeof(NetpbmColor), img->width * img->height, file) != size) {
                        return NETPBM_ERROR_IO;
                    }
                    return NETPBM_SUCCESS;
                }
                default: 
                    return NETPBM_ERROR_UNSUPPORTED_FORMAT;
            }
        default:
            return NETPBM_ERROR_UNSUPPORTED_TYPE;
    }
}

NetpbmError netpbm_load(const char *filename, NetpbmImage **img) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        return NETPBM_ERROR_IO;
    }

    char header[3];
    if (fgets(header, sizeof(header), file) == NULL) {
        fprintf(stderr, "Error reading header from file: %s\n", filename);
        fclose(file);
        return NETPBM_ERROR_INVALID_DATA;
    }

    NetpbmType type;
    NetpbmFormat format;

    if (strcmp(header, "P1") == 0) {
        type = NETPBM_TYPE_PBM;
        format = NETPBM_FORMAT_PLAIN;
    } else if (strcmp(header, "P4") == 0) {
        type = NETPBM_TYPE_PBM;
        format = NETPBM_FORMAT_RAW;
    } else if (strcmp(header, "P2") == 0) {
        type = NETPBM_TYPE_PGM;
        format = NETPBM_FORMAT_PLAIN;
    } else if (strcmp(header, "P5") == 0) {
        type = NETPBM_TYPE_PGM;
        format = NETPBM_FORMAT_RAW;
    } else if (strcmp(header, "P3") == 0) {
        type = NETPBM_TYPE_PPM;
        format = NETPBM_FORMAT_PLAIN;
    } else if (strcmp(header, "P6") == 0) {
        type = NETPBM_TYPE_PPM;
        format = NETPBM_FORMAT_RAW;
    } else {
        fclose(file);
        return NETPBM_ERROR_UNSUPPORTED_FORMAT;
    }

    int width, height, max_value;

    // Read width and height
    if (fscanf(file, "%d %d", &width, &height) != 2) {
        fprintf(stderr, "Error reading width and height from file: %s\n", filename);
        fclose(file);
        return NETPBM_ERROR_INVALID_DATA;
    }

    // Read max value for PGM and PPM
    if (type != NETPBM_TYPE_PBM && fscanf(file, "%d", &max_value) != 1) {
        fprintf(stderr, "Error reading max value from file: %s\n", filename);
        fclose(file);
        return NETPBM_ERROR_INVALID_DATA;
    }

    // Create the image without zero fill
    *img = netpbm_create_ex(type, format, width, height, max_value, 0);
    if (*img == NULL) {
        fclose(file);
        return NETPBM_ERROR_MEMORY_ALLOCATION;
    }

    // Read the image data
    NetpbmError err = netpbm_read_data(file, *img);
    if (err != NETPBM_SUCCESS) {
        fprintf(stderr, "Error reading image data from file: %s\n", filename);
        netpbm_free(img);
        fclose(file);
        return err;
    }

    fclose(file);
    return NETPBM_SUCCESS;
}

int netpbm_get_gray(const NetpbmImage* img, int x, int y) {
    if (!img || img->type != NETPBM_TYPE_PGM || x < 0 || y < 0 || x >= img->width || y >= img->height)
        return -1;
    return img->data.gray_data[y * img->width + x];
}

int netpbm_set_gray(NetpbmImage* img, int x, int y, unsigned char value) {
    if (!img || img->type != NETPBM_TYPE_PGM || x < 0 || y < 0 || x >= img->width || y >= img->height)
        return -1;
    img->data.gray_data[y * img->width + x] = value;
    return 0;
}

int netpbm_get_color(const NetpbmImage* img, int x, int y, NetpbmColor* out) {
    if (!img || img->type != NETPBM_TYPE_PPM || x < 0 || y < 0 || x >= img->width || y >= img->height)
        return -1;
    *out = img->data.color_data[y * img->width + x];
    return 0;
}

int netpbm_set_color(NetpbmImage* img, int x, int y, NetpbmColor color) {
    if (!img || img->type != NETPBM_TYPE_PPM || x < 0 || y < 0 || x >= img->width || y >= img->height)
        return -1;
    img->data.color_data[y * img->width + x] = color;
    return 0;
}