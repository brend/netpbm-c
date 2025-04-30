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

int netpbm_write_data(FILE *file, const NetpbmImage *img) {
    switch (img->type) {
        case NETPBM_TYPE_PBM:
            switch (img->format) {
                case NETPBM_FORMAT_PLAIN:
                    for (int i = 0; i < img->width * img->height / 8; i++) {
                        if (fputc(img->data.bitmap_data[i], file) == EOF) {
                            return -1;
                        }
                    }
                    return 0;
                case NETPBM_FORMAT_RAW:
                    return fwrite(img->data.bitmap_data, 1, img->width * img->height / 8, file);
            }
            break;
        case NETPBM_TYPE_PGM:
            switch (img->format) {
                case NETPBM_FORMAT_PLAIN:
                    for (int i = 0; i < img->width * img->height; i++) {
                        if (fputc(img->data.gray_data[i], file) == EOF) {
                            return -1;
                        }
                    }
                    return 0;
                case NETPBM_FORMAT_RAW:
                    return fwrite(img->data.gray_data, img->max_value > 255 ? 2 : 1, img->width * img->height, file);
            }
            break;
        case NETPBM_TYPE_PPM:
            switch (img->format) {
                case NETPBM_FORMAT_PLAIN:
                    for (int i = 0; i < img->width * img->height; i++) {
                        if (fputc(img->data.color_data[i].r, file) == EOF) return -1;
                        if (fputc(img->data.color_data[i].g, file) == EOF) return -1;
                        if (fputc(img->data.color_data[i].b, file) == EOF) return -1;
                    }
                    return 0;
                case NETPBM_FORMAT_RAW:
                    return fwrite(img->data.color_data, sizeof(NetpbmColor), img->width * img->height, file);
            }
            break;
    }
    // Error: unsupported type or format
    return -1;
}

int netpbm_write_header(FILE *file, const NetpbmImage *img) {
    switch (img->type) {
        case NETPBM_TYPE_PBM:
            switch (img->format) {
                case NETPBM_FORMAT_PLAIN:
                    return fprintf(file, "P1\n%d %d\n", img->width, img->height);
                case NETPBM_FORMAT_RAW:
                    return fprintf(file, "P4\n%d %d\n", img->width, img->height);
            }
            break;
        case NETPBM_TYPE_PGM:
            switch (img->format) {
                case NETPBM_FORMAT_PLAIN:
                    return fprintf(file, "P2\n%d %d\n%d\n", img->width, img->height, img->max_value);
                case NETPBM_FORMAT_RAW:
                    return fprintf(file, "P5\n%d %d\n%d\n", img->width, img->height, img->max_value);
            }
            break;
        case NETPBM_TYPE_PPM:
            switch (img->format) {
                case NETPBM_FORMAT_PLAIN:
                    return fprintf(file, "P3\n%d %d\n%d\n", img->width, img->height, img->max_value);
                case NETPBM_FORMAT_RAW:
                    return fprintf(file, "P6\n%d %d\n%d\n", img->width, img->height, img->max_value);
            }
            break;
    }
    // Error: unsupported type or format
    return -1; 
}

int netpbm_save(const NetpbmImage *img, const char *filename) {
    FILE *file = fopen(filename, "wb");

    // Check if the file was opened successfully
    if (file == NULL) {
        return -1;
    }

    if (netpbm_write_header(file, img) < 0) {
        fclose(file);
        return -1;
    }

    if (netpbm_write_data(file, img) < 0) {
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}

int netpbm_read_data(FILE *file, NetpbmImage *img) {
    switch (img->type) {
        case NETPBM_TYPE_PBM:
            switch (img->format) {
                case NETPBM_FORMAT_PLAIN:
                    for (int i = 0; i < img->width * img->height / 8; i++) {
                        img->data.bitmap_data[i] = fgetc(file);
                    }
                    return 0;
                case NETPBM_FORMAT_RAW:
                    return fread(img->data.bitmap_data, 1, img->width * img->height / 8, file);
            }
            break;
        case NETPBM_TYPE_PGM:
            switch (img->format) {
                case NETPBM_FORMAT_PLAIN:
                    for (int i = 0; i < img->width * img->height; i++) {
                        img->data.gray_data[i] = fgetc(file);
                    }
                    return 0;
                case NETPBM_FORMAT_RAW:
                    return fread(img->data.gray_data, img->max_value > 255 ? 2 : 1, img->width * img->height, file);
            }
            break;
        case NETPBM_TYPE_PPM:
            switch (img->format) {
                case NETPBM_FORMAT_PLAIN:
                    for (int i = 0; i < img->width * img->height; i++) {
                        img->data.color_data[i].r = fgetc(file);
                        img->data.color_data[i].g = fgetc(file);
                        img->data.color_data[i].b = fgetc(file);
                    }
                    return 0;
                case NETPBM_FORMAT_RAW:
                    return fread(img->data.color_data, sizeof(NetpbmColor), img->width * img->height, file);
            }
            break;
    }
    // Error: unsupported type or format
    return -1;
}

int netpbm_load(const char *filename, NetpbmImage **img) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        return -1;
    }

    char header[3];
    if (fgets(header, sizeof(header), file) == NULL) {
        fclose(file);
        return -1;
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
        return -1; // Unsupported format
    }

    int width, height, max_value;

    // Read width and height
    if (fscanf(file, "%d %d", &width, &height) != 2) {
        fclose(file);
        return -1; // Error reading width and height
    }

    // Read max value for PGM and PPM
    if (type != NETPBM_TYPE_PBM && fscanf(file, "%d", &max_value) != 1) {
        fclose(file);
        return -1; // Error reading max value
    }

    // Create the image without zero fill
    *img = netpbm_create_ex(type, format, width, height, max_value, 0);
    if (*img == NULL) {
        fclose(file);
        return -1; // Memory allocation error
    }

    // Read the image data
    if (netpbm_read_data(file, *img) < 0) {
        netpbm_free(img);
        fclose(file);
        return -1; // Error reading image data
    }

    fclose(file);
    return 0; // Success
}
