#include <bmpfile.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
    int width = 0;
    int height = 0;
    int steps = 0;
    int waterlevel = 0;
    unsigned int seed;

    /* TODO */
    /* Rewrite this code using getopt() and getopt_long() */

    int i;
    for(i = 1; i < argc; i+=2) { //all of the optional arguments
        if(strcmp(argv[i], "-w") == 0 || strcmp(argv[i], "--width") == 0) {
            width = atoi(argv[i + 1]);
            if(width < 0) {
                printf("Invalid width value. Width must be positive.\n");
                badinput_exit(argv[0]);
            }
        } else if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--height") == 0) {
            height = atoi(argv[i + 1]);
            if(height < 0) {
                printf("Invalid height value. Height must be positive.\n");
                badinput_exit(argv[0]);
            }
        } else if(strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--steps") == 0) {
            steps = atoi(argv[i + 1]);
            if(steps < 0) {
                printf("Invalid steps value. Steps must be positive.\n");
                badinput_exit(argv[0]);
            }
        } else if(strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--waterlevel") == 0) {
            waterlevel = atoi(argv[i + 1]);
            if(waterlevel < 0 || waterlevel > 255) {
                printf("Invalid waterlevel value. Waterlevel must be between 0 and 255 inclusive.\n");
                badinput_exit(argv[0]);
            }
        } else if(strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--seed") == 0) {
            seed = atoi(argv[i + 1]);
            if(seed < 0) {
                printf("Invalid seed value. Seed must be positive.\n");
                badinput_exit(argv[0]);
            }
        } else if(strcmp(argv[i], "--help") == 0) {
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
    if(width == 0) width = 256;
    if(height == 0) height = 256;
    if(steps == 0) steps = 61 * width * height; //this just seems to make the prettiest maps
    if(waterlevel == 0) waterlevel = 50;
    if(seed == 0) seed = time(NULL);

    printf("Width: %d\n"
           "Height: %d\n"
           "Steps: %d\n"
           "Waterlevel: %d\n"
           "Output File: %s\n",
           width, height, steps, waterlevel, filename); //for debugging

    /*
     * The actual terrain generation code starts here
     */

    struct tile {
        unsigned char elevation; //the elevation at this tile
        unsigned char id; //an identifier to the tile type
        //possible tile types: 0 = normal ground, 1 = beach, 2 = tree
    };
    typedef struct tile tile;
    
    char map[width][height];
    int j;
    for(i = 0; i < width; i++) //fill the map with zeroes to start
        for(j = 0; j < height; j++)
            map[i][j] = 0;

    /* TODO */
    /* Add random seed option to arguments */
    srand(seed); //seed the random generator

    int x = randint(0, width - 1); //x and y are current location in the random walk
    int y = randint(0, height - 1);
    for(i = 0; i < steps; i++) {
       x += randint(-1, 1);
       y += randint(-1, 1); //move, or not
       if(x == width) x = 0;
       if(y == height) y = 0;
       if(x == -1) x = width - 1;
       if(y == -1) y = height - 1;//prevent walking off map by adding wrap-around
       //printf("Current location: %d,%d\n", x, y);
       map[x][y]++; //increment
       if(map[x][y] > 255) map[x][y] = 255; //add a ceiling
    }

    /*
     * This section is saving the map to a file
     */
    bmpfile_t *bmp;
    bmp = bmp_create(width, height, 24);

    for(i = 0; i < width; i++) {
        for(j = 0; j < height; j++) {
            bmp_set_pixel(bmp, i, j, getcolor(map[i][j], waterlevel));
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
           "  -w, --width"
           "\t\tThe width of the square map to generate (Default = 256). Must be positive.\n"
           "  -h, --height"
           "\t\tThe height of the square map to generate (Default = 256). Must be positive.\n"
           "  -s, --steps"
           "\t\tThe number of steps to take in the random walk (Default = 61 * size^2). Must be positive.\n"
           "  -l, --waterlevel"
           "\t\tThe number of steps to take in the random walk (Default = 61 * size^2). Must be positive.\n"
           "  -r, --seed"
           "\tThe random seed to use in terrain generation (Default = current system time). Must be positive.\n");
}

/*
 * Returns a random number between min and max inclusive
 * Logic taken from http://c-faq.com/lib/randrange.html
 */
int randint(int min, int max) {
    return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}
