
// controls all aspects of a model,
// from loading the vertices, normals
// texture coordinates, textures, 
// to drawing the model, loading shaders,
// buffering the data, etc... this will
// do it all

#ifndef MODEL_H
#define MODEL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cglm/cglm.h>

// TODO: cache textures incase other models use the same texture.
// This will increase loading times. Maybe create a texture manager
// file

struct Model {
    
};

//will load all textures, vertex data, buffer the data
void initializeModel(struct Model *model, char *filename) {

    // load vertices, vertex texture coords,
    // vertex normals, etc...
    FILE *file;
    file = fopen("test.obj", "r");

    char line[100];

    // load vertices into a buffer
    // load normals into a buffer
    // load tex coords into a buffer
    // load indices into a buffer

    // after everything is loaded, close the file
    // construct vertex data butter, with size being 
    // sizeof(float) * num indices * 3 vertices * 3 norms * 2 tex coords

    struct List vertices;
    initList(vertices);

    while(fscanf(file, "%s", line) != -1){
        if(strcmp(line, "v") == 0) {
            printf("v line\n");
            float val1, val2, val3;
            fscanf(file, "%f %f %f", &val1, &val2, &val3);
            printf("Values: %f, %f, %f\n", val1, val2, val3);
        }
        else if(strcmp(line, "vt") == 0) {
            printf("texture line\n");
        else if(strcmp(line, "vn") == 0) {
            printf("normal line\n");
            float norm1, norm2, norm3;
            fscanf(file, "%f %f %f", &norm1, &norm2, &norm3);
            printf("values: %f, %f, %f\n", norm1, norm2, norm3);
        }
            float tex1, tex2;
            fscanf(file, "%f %f", &tex1, &tex2);
            printf("values: %f, %f\n", tex1, tex2);
        }
        else if(strcmp(line, "vn") == 0) {
            printf("normal line\n");
            float norm1, norm2, norm3;
            fscanf(file, "%f %f %f", &norm1, &norm2, &norm3);
            printf("values: %f, %f, %f\n", norm1, norm2, norm3);
        }
        else {
            printf("ignoring line with header: %s\n", line);
            fgets(line, 100, file);
        }
    }

    fclose(file);
}

struct List {
    struct Node*head;
    struct Node*tail;
};

struct Node {
    struct Node *prev;
    struct Node *next;
    void *data;
};

void pushData(struct List *list, void *data) {
    struct Node *new_node = malloc(sizeof( *new_node));
    if(list->head == 0) {
        list->head = new_node;
        list->tail = new_node;
        new_node->prev = 0;
    }
    else {
        list->tail->next = new_node;
        list->tail = new_node;
        new_node->prev = list->tail;
    }
}

void deleteList(struct List *list) {
    struct Node *current_node = list->head;
    while(current_node != 0) {
        free(current_node->data);
        Node *temp_node = current_node;
        current_node = current_node->next;
        free(temp_node);
    }
}

void initList(struct List *list) {
    list->head = 0;
    list->tail = 0;
}

#endif