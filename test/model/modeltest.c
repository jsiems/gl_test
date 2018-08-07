
#include <stdio.h>
#include "texman.h"
#include "model.h"

int main() {
    printf("test running\n");

    struct TexMan texman;
    initTexMan(&texman);

    struct Model model;
    initializeModel(&model, &texman, "house");

    destroyTexMan(&texman);
    destroyModel(&model);

    printf("done, test passed?\n");

    return 0;
}
