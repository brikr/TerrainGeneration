#include <bmpfile.h>

int main() {
    bmpfile_t *bmp;
    rgb_pixel_t color = {0xFF, 0, 0, 0}; // blue green red alpha
    
    bmp = bmp_create(200, 200, 24); //width, height, depth
    int x, y;
    for(x = 0; x < 200; x++) { //write color in all pixels
        for(y = 0; y < 200; y++) {
            bmp_set_pixel(bmp, x, y, color);
        }
    }

    bmp_save(bmp, "test.bmp");
    bmp_destroy(bmp);

    return 0;
}
