# Netpbm C Library

A lightweight C library for loading, manipulating, and saving **Netpbm** image formats: **PBM**, **PGM**, and **PPM** in both **plain (ASCII)** and **raw (binary)** variants.

<img src="assets/sample.png" alt="sample grayscale image" width="64">

## Features

- Supports PBM (bitmap), PGM (grayscale), and PPM (color) formats
- Supports both ASCII (P1–P3) and binary (P4–P6) encodings
- Load and save Netpbm files
- Programmatic access to pixel data
- Create new images from scratch
- Rotate, transform, or modify image data

## File Format Support

| Type | ASCII Format | Binary Format |
|------|--------------|---------------|
| PBM  | P1           | P4            |
| PGM  | P2           | P5            |
| PPM  | P3           | P6            |

## Getting Started

### Building

```bash
gcc -o netpbm_test main.c netpbm.c
```

Or compile as a library:

```bash
gcc -c netpbm.c -o netpbm.o
ar rcs libnetpbm.a netpbm.o
```

Include the header in your C source:

```C
#include "netpbm.h"
```

Example: Rotate an image 90° clockwise

```C
NetpbmImage *img = NULL;
    NetpbmError err = netpbm_load("../assets/sample.pgm", &img);
    
    if (err != NETPBM_SUCCESS) {
        fprintf(stderr, "Failed to load image: %d\n", err);
        return EXIT_FAILURE;
    }

    printf("Image width: %d, height: %d\n", img->width, img->height);

    // Example of modifying the image data: Rotate the image 90 degrees
    NetpbmImage *copy = netpbm_create_ex(NETPBM_TYPE_PGM, NETPBM_FORMAT_PLAIN, img->height, img->width, img->max_value, 0);

    if (copy == NULL) {
        fprintf(stderr, "Failed to create copy of image\n");
        netpbm_free(&img);
        return EXIT_FAILURE;
    }
    // Rotate the image 90 degrees clockwise
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            int new_x = img->height - 1 - y;
            int new_y = x;
            int pixel_value = netpbm_get_gray(img, x, y);
            if (pixel_value < 0) {
                fprintf(stderr, "Failed to get pixel value at (%d, %d)\n", x, y);
                netpbm_free(&copy);
                netpbm_free(&img);
                return EXIT_FAILURE;
            }
            // Set the pixel value in the rotated image
            netpbm_set_gray(copy, new_x, new_y, pixel_value);
        }
    }
    // Save the rotated image
    err = netpbm_save(copy, "rotated.pgm");
    if (err != NETPBM_SUCCESS) {
        fprintf(stderr, "Failed to save rotated image: %d\n", err);
        netpbm_free(&copy);
        netpbm_free(&img);
        return EXIT_FAILURE;
    }
    netpbm_free(&copy);
    // Save the original image
    err = netpbm_save(img, "test.pgm");
    if (err != NETPBM_SUCCESS) {
        fprintf(stderr, "Failed to save original image: %d\n", err);
        netpbm_free(&img);
        return EXIT_FAILURE;
    }
    netpbm_free(&img);
    printf("Image saved successfully.\n");
```

## API Overview

### Types

```C
typedef enum { NETPBM_TYPE_PBM, NETPBM_TYPE_PGM, NETPBM_TYPE_PPM } NetpbmType;
typedef enum { NETPBM_FORMAT_PLAIN, NETPBM_FORMAT_RAW } NetpbmFormat;

typedef struct {
    unsigned char r, g, b;
} NetpbmColor;

typedef struct {
    NetpbmType type;
    NetpbmFormat format;
    int width, height;
    int max_value;
    union {
        unsigned char *bitmap_data;
        unsigned char *gray_data;
        NetpbmColor *color_data;
    } data;
} NetpbmImage;
````
### Functions

- `NetpbmImage* netpbm_create(...)`
- `NetpbmImage* netpbm_create_ex(...)`
- `void netpbm_free(NetpbmImage** img)`
- `NetpbmError netpbm_load(const char* path, NetpbmImage** img)`
- `NetpbmError netpbm_save(const NetpbmImage* img, const char* path)`

## Error Codes

|Code|Description|
|----|-----------|
|`NETPBM_SUCCESS`|Operation completed successfully|
|`NETPBM_ERROR_IO`|File I/O error|
|`NETPBM_ERROR_UNSUPPORTED_FORMAT`|Unsupported file format|
|`NETPBM_ERROR_UNSUPPORTED_TYPE`|Unsupported image type|
|`NETPBM_ERROR_INVALID_DATA`|Malformed file or header|
|`NETPBM_ERROR_MEMORY_ALLOCATION`|Out of memory|

## License

This project is licensed under the MIT License.
