#include <stdio.h>
#include "netpbm.h"

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

/**
 * @brief Runs a simple test of the Netpbm library.
 */
int main() {
    NetpbmImage *img = netpbm_create(PGM, RAW, 800, 600, 255);
    printf("Image width: %d, height: %d\n", img->width, img->height);
    netpbm_free(&img);
    return 0;
}