# Netpbm C Library

A lightweight C library for loading, manipulating, and saving **Netpbm** image formats: **PBM**, **PGM**, and **PPM** in both **plain (ASCII)** and **raw (binary)** variants.

<img src="assets/sample.pgm" alt="sample grayscale image">

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
NetpbmImage *img;
if (netpbm_load("input.pgm", &img) != NETPBM_SUCCESS) {
    fprintf(stderr, "Failed to load image.\n");
    return 1;
}

// Create rotated image (width and height are swapped)
NetpbmImage *rotated = netpbm_create(
    img->type,
    img->format,
    img->height,
    img->width,
    img->max_value
);

// Rotate pixels
for (int y = 0; y < img->height; y++) {
    for (int x = 0; x < img->width; x++) {
        rotated->data.gray_data[x * rotated->width + (rotated->width - 1 - y)] =
            img->data.gray_data[y * img->width + x];
    }
}

netpbm_save(rotated, "rotated.pgm");

netpbm_free(&img);
netpbm_free(&rotated);
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
