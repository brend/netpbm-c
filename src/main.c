#include <stdio.h>
#include "netpbm.h"

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

/**
 * @brief Runs a simple test of the Netpbm library.
 */
int main() {
    NetpbmImage *img = NULL;
    
    if (netpbm_load("/Users/waldrumpus/Downloads/sample-pgm-files-sample_5184x3456.pgm", &img) >= 0) {
        printf("Image width: %d, height: %d\n", img->width, img->height);

        // Example of modifying the image data: Rotate the image 90 degrees
        NetpbmImage *copy = netpbm_create(PGM, RAW, img->height, img->width, 255);
        for (int y = 0; y < img->height; y++) {
            for (int x = 0; x < img->width; x++) {
                copy->data.gray_data[x * img->height + (img->height - 1 - y)] = img->data.gray_data[y * img->width + x];
            }
        }
        // Save the rotated image
        netpbm_save(copy, "rotated.pgm");
        netpbm_free(&copy);
        // Save the original image
        netpbm_save(img, "test.pgm");
        netpbm_free(&img);
    } else {
        fprintf(stderr, "Failed to load image.\n");
        return EXIT_FAILURE;
    }
    return 0;
}