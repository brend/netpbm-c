#include <stdio.h>
#include "netpbm.h"

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

/**
 * @brief Runs a simple test of the Netpbm library.
 */
int main() {
    NetpbmImage *img = NULL;
    NetpbmError err = netpbm_load("/Users/waldrumpus/code/pbm/ascii.pgm", &img);
    
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
            copy->data.gray_data[new_y * copy->width + new_x] = img->data.gray_data[y * img->width + x];
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

    return 0;
}