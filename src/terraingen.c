#include <bmpfile.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct tile {
    unsigned char elevation; //the elevation at this tile
    unsigned char id; //an identifier to the tile type
    //possible tile types: 0 = normal ground, 1 = beach, 2 = tree
};
typedef struct tile tile;

void smooth(int w, int h, tile map[w][h], int, int);
void genbeaches(int w, int h, tile map[w][h], int);
void maptofile(int w, int h, tile map[w][h], int, char*);
rgb_pixel_t getcolor(tile, int);
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
    unsigned int seed = 0;

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
           "Seed: %d\n"
           "Output File: %s\n",
           width, height, steps, waterlevel, seed, filename); //for debugging

    /*
     * The actual terrain generation code starts here
     */

    tile map[width][height];
    int j;
    for(i = 0; i < width; i++) { //fill the map with zeroes to start
        for(j = 0; j < height; j++) {
            map[i][j].elevation = 0;
            map[i][j].id = 0;
        }
    }

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
       map[x][y].elevation++; //increment
       if(map[x][y].elevation > 255) map[x][y].elevation = 255; //add a ceiling
    }

    smooth(width, height, map, waterlevel, 2); //smooth out the terrain
    genbeaches(width, height, map, waterlevel);

    maptofile(width, height, map, waterlevel, filename);

    return 0;
}

/*
 * Smooths the terrain
 * This code was modified from the Java code in a few ways:
 *  It no longer takes in a threshold, but rather a number of passes to execute the smoothing algorithm
 *   The map looks silly with anything other than 4 threshold so thats hardcoded now
 *   A higher numpasses results in smoother terrain.
 *  It is now called smooth, rather than cleanUp
 */
void smooth(int width, int height, tile map[width][height], int waterlevel, int numpasses) {
    if(numpasses == 0) return;
    int threshold = 4; //originally an argument, but anything other than 4 looks funny.
    int i, j, u, count;
    unsigned char newmap[width][height]; //this map is here so that we aren't smoothing the map in-place
    for(i = 0; i < width; i++) {
        for(j = 0; j < height; j++) {
            newmap[i][j] = map[i][j].elevation;
        }
    }
    int borders[8][2]; //borders[neighbor][x/y]
    /* TODO */
    /* Search for a better way to get all of the borders around a tile */
    for(i = 0; i < width; i++) {
        for(j = 0; j < height; j++) {
            borders[0][0] = i - 1;  //upper left x
            borders[0][1] = j - 1;  //upper left y
            borders[1][0] = i;      //upper x
            borders[1][1] = j;      //upper y
            borders[2][0] = i + 1;  //upper right x
            borders[2][1] = j - 1;  //upper right y
            borders[3][0] = i - 1;  //left x
            borders[3][1] = j;      //left y
            borders[4][0] = i + 1;  //right x
            borders[4][1] = j;      //right y
            borders[5][0] = i - 1;  //lower left x
            borders[5][1] = j + 1;  //lower left y
            borders[6][0] = i;      //lower x
            borders[6][1] = j + 1;  //lower y
            borders[7][0] = i + 1;  //lower right x
            borders[7][1] = j + 1;  //lower right y

            for(u = 0; u < 8; u++) { //wrap around any out of bounds issues
                if(borders[u][0] < 0) borders[u][0] = width - 1;
                if(borders[u][0] == width) borders[u][0] = 0;
                if(borders[u][1] < 0) borders[u][1] = height - 1;
                if(borders[u][1] == height) borders[u][1] = 0;
            }

            count = 0;
            if(map[i][j].elevation <= waterlevel) {
                //get a count of nonwater tiles around this tile
                for(u = 0; u < 8; u++) {
                    if(map[borders[u][0]][borders[u][1]].elevation > waterlevel) count++;
                }
                //if its more than the threshold it makes this tile nonwater
                if(count > threshold) newmap[i][j] = waterlevel + 2;
            } else { //pretty much the same algorithm here but reversed
                for(u = 0; u < 8; u++) {
                    if(map[borders[u][0]][borders[u][1]].elevation <= waterlevel) count++;
                }
                if(count > threshold) newmap[i][j] = waterlevel - 1;
            }
        }
    }
    for(i = 0; i < width; i++) { //replace all elevations at once
        for(j = 0; j < height; j++) {
            map[i][j].elevation = newmap[i][j];
        }
    }
    smooth(width, height, map, waterlevel, numpasses - 1);
}

/*
 * Adds beaches to the terrain
 * A tile turns to beach if it is nonwater and has water in one of its four cardinal directions
 */
void genbeaches(int width, int height, tile map[width][height], int waterlevel) {
    int borders[8][2]; //borders[neighbor][x/y]
    int i, j, u;
    /* TODO */
    /* Search for a better way to get all of the borders around a tile */
    /* This function can definitely be done in less code */
    for(i = 0; i < width; i++) {
        for(j = 0; j < height; j++) {
            borders[0][0] = i - 1;  //upper left x
            borders[0][1] = j - 1;  //upper left y
            borders[1][0] = i;      //upper x
            borders[1][1] = j;      //upper y
            borders[2][0] = i + 1;  //upper right x
            borders[2][1] = j - 1;  //upper right y
            borders[3][0] = i - 1;  //left x
            borders[3][1] = j;      //left y
            borders[4][0] = i + 1;  //right x
            borders[4][1] = j;      //right y
            borders[5][0] = i - 1;  //lower left x
            borders[5][1] = j + 1;  //lower left y
            borders[6][0] = i;      //lower x
            borders[6][1] = j + 1;  //lower y
            borders[7][0] = i + 1;  //lower right x
            borders[7][1] = j + 1;  //lower right y

            for(u = 0; u < 8; u++) { //wrap around any out of bounds issues
                if(borders[u][0] < 0) borders[u][0] = width - 1;
                if(borders[u][0] == width) borders[u][0] = 0;
                if(borders[u][1] < 0) borders[u][1] = height - 1;
                if(borders[u][1] == height) borders[u][1] = 0;
            }

            if(map[borders[1][0]][borders[1][1]].elevation <= waterlevel && map[i][j].elevation > waterlevel)
                map[i][j].id = 1;
            else if(map[borders[3][0]][borders[3][1]].elevation <= waterlevel && map[i][j].elevation > waterlevel)
                map[i][j].id = 1;
            else if(map[borders[4][0]][borders[4][1]].elevation <= waterlevel && map[i][j].elevation > waterlevel)
                map[i][j].id = 1;
            else if(map[borders[6][0]][borders[6][1]].elevation <= waterlevel && map[i][j].elevation > waterlevel)
                map[i][j].id = 1;
        }
    }
}

/*
 * Save a map to a bitmap file
 */
void maptofile(int width, int height, tile map[width][height], int waterlevel, char *filename) {
    bmpfile_t *bmp;
    bmp = bmp_create(width, height, 24);

    int i, j;
    for(i = 0; i < width; i++) {
        for(j = 0; j < height; j++) {
            bmp_set_pixel(bmp, i, j, getcolor(map[i][j], waterlevel));
        }
    }

    bmp_save(bmp, filename);
    bmp_destroy(bmp);
}

/*
 * Returns what color a certain height should be
 */
rgb_pixel_t getcolor(tile t, int waterlevel) {
    rgb_pixel_t color = {0, 0, 255, 0}; //bright red be default to show errors easily
    if(t.elevation <= waterlevel)
        color = (rgb_pixel_t){255, (t.elevation * 255) / waterlevel, 0, 0}; //this makes a lighter blue as it gets shallower
    if(t.elevation > waterlevel)
        color = (rgb_pixel_t){0, (t.elevation * 255) / (256 - waterlevel), 0, 0}; //darker green the closer it is to water
    if(t.id == 1)
        color = (rgb_pixel_t){0x33, 0xCC, 0xFF, 0}; //beach color for tiles flagged as beaches
    if(t.id == 2)
        /* TODO */
        /* implement a tree density variable to change tree color for */
        color = (rgb_pixel_t){0, 0xA5, 0xFF, 0}; //orange for tree tiles
    return color;
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
