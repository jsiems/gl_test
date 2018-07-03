
#include <stdio.h>

#include "wfc.h"

// floats per vertex data
#define FPV 8
#define INPUT_FILE "test.obj"
#define OUTPUT_FILE "test.vrt"

int main() {
    printf("\nRunning wfc test\n\n");

    printf("Converting %s...\n", INPUT_FILE);
    convertWavefront(INPUT_FILE);

    printf("Reading %s...\n", OUTPUT_FILE);
    
    FILE *file;
    file = fopen(OUTPUT_FILE, "rb");
    if(!file) {
        printf("ERROR opening %s\n", OUTPUT_FILE);
        perror("");
        exit(1);
    }

    // read total number of vertices from input file
    int num_verts;
    fread(&num_verts, sizeof(num_verts), 1, file);
    float *verts = malloc(num_verts * FPV * sizeof(*verts));
    if(!verts) {
        printf("ERROR: could not allocate memory for verts\n");
        exit(1);
    }

    // load all data into verts array
    for(int i = 0; i < num_verts; i ++) {
        fread(verts + i * FPV, sizeof(*verts), FPV, file);
    }

    fclose(file);

    printf("Comparing converted data to expected data...\n");

    // this is gonna fail from now on
    float expected_results[96] = {
         1.0, 0.0,  1.0, 0.0, -1.0, 0.0, 1.0, 0.0,
        -1.0, 0.0, -1.0, 0.0, -1.0, 0.0, 0.0, 1.0,
         1.0, 0.0, -1.0, 0.0, -1.0, 0.0, 0.0, 0.0,
        -1.0, 2.0, -1.0, 0.0,  1.0, 0.0, 1.0, 0.0,
         1.0, 2.0,  1.0, 0.0,  1.0, 0.0, 0.0, 1.0,
         1.0, 2.0, -1.0, 0.0,  1.0, 0.0, 0.0, 0.0,
         1.0, 2.0, -1.0, 1.0, -0.0, 0.0, 0.0, 1.0,
         1.0, 0.0,  1.0, 1.0, -0.0, 0.0, 1.0, 0.0,
         1.0, 0.0, -1.0, 1.0, -0.0, 0.0, 1.0, 1.0,
         1.0, 2.0,  1.0, 0.0, -0.0, 1.0, 0.0, 1.0,
        -1.0, 0.0,  1.0, 0.0, -0.0, 1.0, 1.0, 0.0,
         1.0, 0.0,  1.0, 0.0, -0.0, 1.0, 1.0, 1.0,
    };

    for(int i = 0; i < num_verts; i ++) {
        for(int j = 0; j < FPV; j ++) {
            int index = i * FPV + j;
            if(verts[index] != expected_results[index]) {
                printf("\nFAIL: verts does not match expected results\n");
                printf("vertex %d, val %d, index %d\n", i, j, index);
                printf("verts: %f\n", verts[index]);
                printf("expected: %f\n", expected_results[index]);
                exit(1);
            }
        }
    }

    free(verts);

    remove(OUTPUT_FILE);

    printf("Test passed\n");

    return 0;
}
