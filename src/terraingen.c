#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void badinput_exit(char*);
void print_help(char*);

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
            print_help(argv[0]);
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

    return 0;
}

void badinput_exit(char *name) {
    printf("Try \'%s --help\' for more information.\n", name);
    exit(1);
}

void print_help(char *name) {
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
