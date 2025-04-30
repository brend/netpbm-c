#include "netpbm.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
    switch (type) {
        case PBM:
            img->data.bitmap_data = (byte *)malloc(width * height / 8);
            break;
        case PGM:
            img->data.gray_data = (byte *)malloc(width * height * (max_value > 255 ? 2 : 1));
            break;
        case PPM:
            img->data.color_data = (Color *)malloc(width * height * sizeof(Color));
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
        switch (type) {
            case PBM:
                memset(img->data.bitmap_data, 0, width * height / 8);
                break;
            case PGM:
                memset(img->data.gray_data, 0, width * height * (max_value > 255 ? 2 : 1));
                break;
            case PPM:
                for (int i = 0; i < width * height; i++) {
                    img->data.color_data[i].r = 0;
                    img->data.color_data[i].g = 0;
                    img->data.color_data[i].b = 0;
                }
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
            case PBM:
                free((*img)->data.bitmap_data);
                break;
            case PGM:
                free((*img)->data.gray_data);
                break;
            case PPM:
                free((*img)->data.color_data);
                break;
        }
        free(*img);
    }
    *img = NULL;
}

int netpbm_write_data(FILE *file, const NetpbmImage *img) {
    switch (img->type) {
        case PBM:
            switch (img->format) {
                case PLAIN:
                    for (int i = 0; i < img->width * img->height / 8; i++) {
                        if (fputc(img->data.bitmap_data[i], file) == EOF) {
                            return -1;
                        }
                    }
                    return 0;
                case RAW:
                    return fwrite(img->data.bitmap_data, 1, img->width * img->height / 8, file);
            }
            break;
        case PGM:
            switch (img->format) {
                case PLAIN:
                    for (int i = 0; i < img->width * img->height; i++) {
                        if (fputc(img->data.gray_data[i], file) == EOF) {
                            return -1;
                        }
                    }
                    return 0;
                case RAW:
                    return fwrite(img->data.gray_data, img->max_value > 255 ? 2 : 1, img->width * img->height, file);
            }
            break;
        case PPM:
            switch (img->format) {
                case PLAIN:
                    for (int i = 0; i < img->width * img->height; i++) {
                        if (fputc(img->data.color_data[i].r, file) == EOF) return -1;
                        if (fputc(img->data.color_data[i].g, file) == EOF) return -1;
                        if (fputc(img->data.color_data[i].b, file) == EOF) return -1;
                    }
                    return 0;
                case RAW:
                    return fwrite(img->data.color_data, sizeof(Color), img->width * img->height, file);
            }
            break;
    }
    // Error: unsupported type or format
    return -1;
}

int netpbm_write_header(FILE *file, const NetpbmImage *img) {
    switch (img->type) {
        case PBM:
            switch (img->format) {
                case PLAIN:
                    return fprintf(file, "P1\n%d %d\n", img->width, img->height);
                case RAW:
                    return fprintf(file, "P4\n%d %d\n", img->width, img->height);
            }
            break;
        case PGM:
            switch (img->format) {
                case PLAIN:
                    return fprintf(file, "P2\n%d %d\n%d\n", img->width, img->height, img->max_value);
                case RAW:
                    return fprintf(file, "P5\n%d %d\n%d\n", img->width, img->height, img->max_value);
            }
            break;
        case PPM:
            switch (img->format) {
                case PLAIN:
                    return fprintf(file, "P3\n%d %d\n%d\n", img->width, img->height, img->max_value);
                case RAW:
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
        case PBM:
            switch (img->format) {
                case PLAIN:
                    for (int i = 0; i < img->width * img->height / 8; i++) {
                        img->data.bitmap_data[i] = fgetc(file);
                    }
                    return 0;
                case RAW:
                    return fread(img->data.bitmap_data, 1, img->width * img->height / 8, file);
            }
            break;
        case PGM:
            switch (img->format) {
                case PLAIN:
                    for (int i = 0; i < img->width * img->height; i++) {
                        img->data.gray_data[i] = fgetc(file);
                    }
                    return 0;
                case RAW:
                    return fread(img->data.gray_data, img->max_value > 255 ? 2 : 1, img->width * img->height, file);
            }
            break;
        case PPM:
            switch (img->format) {
                case PLAIN:
                    for (int i = 0; i < img->width * img->height; i++) {
                        img->data.color_data[i].r = fgetc(file);
                        img->data.color_data[i].g = fgetc(file);
                        img->data.color_data[i].b = fgetc(file);
                    }
                    return 0;
                case RAW:
                    return fread(img->data.color_data, sizeof(Color), img->width * img->height, file);
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
        type = PBM;
        format = PLAIN;
    } else if (strcmp(header, "P4") == 0) {
        type = PBM;
        format = RAW;
    } else if (strcmp(header, "P2") == 0) {
        type = PGM;
        format = PLAIN;
    } else if (strcmp(header, "P5") == 0) {
        type = PGM;
        format = RAW;
    } else if (strcmp(header, "P3") == 0) {
        type = PPM;
        format = PLAIN;
    } else if (strcmp(header, "P6") == 0) {
        type = PPM;
        format = RAW;
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
    if (type != PBM && fscanf(file, "%d", &max_value) != 1) {
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
