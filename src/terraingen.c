#include <bmpfile.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void maptofile(char *map[], int, int, char*);
rgb_pixel_t getcolor(int, int);
void badinput_exit(char*);
void printhelp(char*);
int randint(int, int);

int main(int argc, char **argv) {
    if(argc == 1) {
        printf("Usage: %s [OPTIONS] OUTPUT-FILE\n", argv[0]);
        badinput_exit(argv[0]);
    }

    char *filename;
    int size = 0;
    int steps = 0;
    int waterlevel = 0;

    /* TODO */
    /* Rewrite this code using getopt() and getopt_long() */
    int i;
    for(i = 1; i < argc; i+=2) { //all of the optional arguments
        if(strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--size") == 0) {
            size = atoi(argv[i + 1]);
            if(size < 0) {
                printf("Invalid size value. Size must be positive.\n");
                badinput_exit(argv[0]);
            }
        } else if(strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--steps") == 0) {
            steps = atoi(argv[i + 1]);
            if(steps < 0) {
                printf("Invalid steps value. Steps must be positive.\n");
                badinput_exit(argv[0]);
            }
        } else if(strcmp(argv[i], "-w") == 0 || strcmp(argv[i], "--waterlevel") == 0) {
            waterlevel = atoi(argv[i + 1]);
            if(waterlevel < 0 || waterlevel > 255) {
                printf("Invalid waterlevel value. Waterlevel must be between 0 and 255 inclusive.\n");
                badinput_exit(argv[0]);
            }
        } else if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printhelp(argv[0]);
            return 0;
        } else if(argv[i][0] == '-') {
            printf("Bad argument: %s\n", argv[i]);
            badinput_exit(argv[0]);
        }
    }
    if(i > argc) { //this will be false if there is an argument mismatch
        filename = argv[argc - 1];
    } else {
        printf("Missing output file name.\n");
        badinput_exit(argv[0]);
    }

    //some default values if they weren't manually set
    if(size == 0) size = 64;
    if(steps == 0) steps = 61 * size * size; //this just seems to make the prettiest maps
    if(waterlevel == 0) waterlevel = 50;

    printf("Size: %d\n"
           "Steps: %d\n"
           "Waterlevel: %d\n"
           "Output File: %s\n",
           size, steps, waterlevel, filename); //for debugging

    /*
     * The actual terrain generation code starts here
     */

    char map[size][size]; //char to keep it small since its only storing 0-255
    int j;
    for(i = 0; i < size; i++) //fill the map with zeroes to start
        for(j = 0; j < size; j++)
            map[i][j] = 0;

    /* TODO */
    /* Add random seed option to arguments */
    srand(time(NULL)); //seed the random generator

    int x = randint(0, size - 1); //x and y are current location in the random walk
    int y = randint(0, size - 1);
    for(i = 0; i < steps; i++) {
       x += randint(-1, 1);
       y += randint(-1, 1); //move, or not
       x = x % size;
       y = y % size; //prevent walking off map by wrapping around
       map[x][y]++; //increment
       if(map[x][y] > 255) map[x][y] = 255; //add a ceiling
    }

    /*
     * This section is saving the map to a file
     */

    bmpfile_t *bmp;
    bmp = bmp_create(size, size, 24);

    for(x = 0; x < size; x++) {
        for(y = 0; y < size; y++) {
            bmp_set_pixel(bmp, x, y, getcolor(map[x][y], waterlevel));
        }
    }

    bmp_save(bmp, filename);
    bmp_destroy(bmp);

    return 0;
}

/*
 * Returns what color a certain height should be
 */
rgb_pixel_t getcolor(int h, int waterlevel) {
    if(h <= waterlevel)
        return (rgb_pixel_t){255, (h * 255) / waterlevel, 0, 0}; //this makes a lighter blue as it gets shallower
    if(h > waterlevel)
        return (rgb_pixel_t){0, (h * 255) / (256 - waterlevel), 0, 0}; //darker green the closer it is to water
    if(h == 300)
        return (rgb_pixel_t){0x33, 0xCC, 0xFF, 0}; //beach color for tiles flagged as beaches
    if(h > 301)
        return (rgb_pixel_t){0, ((h - 301) * 255) / (256 - waterlevel), 255, 0}; //orange for tree tiles
    return (rgb_pixel_t){0, 0, 255, 0}; //bright red if other, because that means something bad happened
}

void badinput_exit(char *name) {
    printf("Try \'%s --help\' for more information.\n", name);
    exit(1);
}

void printhelp(char *name) {
    printf("Usage: %s [OPTIONS] OUTPUT-FILE\n", name);
    printf("Randomly generates some terrain and saves a bitmap to OUTPUT-FILE\n"
           "By Brik Royster (minikori)\n"
           "\n"
           "Optional arguments:\n"
           "  -n, --size"
           "\t\tThe width/height of the square map to generate (Default = 64). Must be positive.\n"
           "  -s, --steps"
           "\t\tThe number of steps to take in the random walk (Default = 61 * size^2). Must be positive.\n"
           "  -w, --waterlevel"
           "\tThe waterlevel of the random terrain (Default = 50). Must be between 0 and 255 inclusive.\n");
}

/*
 * Returns a random number between min and max inclusive
 * Logic taken from http://c-faq.com/lib/randrange.html
 */
int randint(int min, int max) {
    return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}
