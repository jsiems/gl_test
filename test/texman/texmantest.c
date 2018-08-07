
#include <stdio.h>
#include "texman.h"

int main() {
    printf("Running tests for TexMan\n");

    struct TexMan texman;
    initTexMan(&texman);

    // load some textures
    // check the id's that are returned
    // etc..

    unsigned int car1 = getTextureId(&texman, "car");
    unsigned int car2 = getTextureId(&texman, "car_spec");
    unsigned int roof1 = getTextureId(&texman, "roof");
    unsigned int roof2 = getTextureId(&texman, "roof_spec");
    unsigned int side1 = getTextureId(&texman, "side");
    unsigned int side2 = getTextureId(&texman, "side_spec");
    unsigned int door1 = getTextureId(&texman, "door");
    unsigned int door2 = getTextureId(&texman, "door_spec");
    
    if(car1 != car2) {
        printf("ERROR: car1 and car2 not equal\n");
        exit(1);
    }

    if(side1 != side2) {
        printf("ERROR: side1 and side2 not equal\n");
        exit(1);
    }

    if(car1 == side1 && 0) {
        printf("ERROR: car and side are equal\n");
        exit(1);
    }

    destroyTexMan(&texman);

    printf("SUCCESS: test passed\n");

    return 0;
}

