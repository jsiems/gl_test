
#include <stdio.h>
#include "texman.h"
#include "model.h"

int main() {
    printf("hello there\n");

    struct TexMan texman;
    initTexMan(&texman);

    struct Model model;
    initializeModel(&model, &texman, "house");

    printf("after initing model\n");


    destroyTexMan(&texman);
    destroyModel(&model);

    return 0;
}
