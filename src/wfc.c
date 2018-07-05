
#include "wfc.h"

// ***** private *****

struct List {
    struct Node *head;
    struct Node *tail;
    int size;
};

struct Node {
    struct Node *next;
    void *data;
};

void pushData(struct List *list, void *data) {
    struct Node *newnode = malloc(sizeof(*newnode));
    if(newnode == 0) {
        printf("Error allocating new node\n");
        exit(1);
    }
    newnode->data = data;
    newnode->next = 0;
    if(list->head == 0) {
        list->head = newnode;
        list->tail = newnode;
    }
    else {
        list->tail->next = newnode;
        list->tail = newnode;
    }

    list->size ++;
}

void *getData(struct List *list, int index) {
    if(index < 0 || index >= list->size) {
        printf("Error getting list data\n");
        printf("\tindex: %d\n", index);
        printf("\tsize: %d\n", list->size);
        exit(1);
    }
    struct Node *current = list->head;
    for(int i = 0; i < index; i++) {
        current = current->next;
    }

    return current->data;
}

struct List *initList() {
    struct List *list = malloc(sizeof(*list));
    if(list == 0) {
        printf("Error allocating mem for list\n");
        exit(1);
    }
    list->head = 0;
    list->tail = 0;
    list->size = 0;
    return list;
}

void destroyList(struct List *list) {
    struct Node *current = list->head;
    while(current != 0) {
        struct Node *temp = current;
        current = current->next;
        free(temp->data);
        free(temp);
    }
    free(list);
}

// ***** public *****

void convertWavefront(const char *filename) {
    printf("Converting %s to .vrt\n", filename);
    // ***** Read data from .obj file *****
    FILE *objfile;
    objfile = fopen(filename, "r");

    if(!objfile) {
        printf("ERROR opening %s:", filename);
        perror("");
        exit(1);
    }

    // might need to increase size of a single line someday
    char line[100];

    struct List *positions = initList();
    struct List *normals = initList();
    struct List *texcoords = initList();
    struct List *indices = initList();

    while(fscanf(objfile, "%s", line) != -1) {

        // vertex data
        if(strcmp(line, "v") == 0) {
            // allocate memory for 3 float array
            float *data = malloc(3 * sizeof(float));
            fscanf(objfile, "%f %f %f", 
                    data, 
                    data + 1, 
                    data + 2
                  );
            pushData(positions, (void *)data);
            continue;
        }

        // texture data
        if(strcmp(line, "vt") == 0) {
            float *data = malloc(2 * sizeof(float));
            fscanf(objfile, "%f %f", 
                    data, 
                    data + 1
                  );
            data[1] = 1 - data[1];
            pushData(texcoords, (void *)data);
            continue;
        }

        // normal data
        if(strcmp(line, "vn") == 0) {
            float *data = malloc(3 * sizeof(float));
            fscanf(objfile, "%f %f %f", 
                    data, 
                    data + 1, 
                    data + 2
                  );
            pushData(normals, (void *)data);
            continue;
        }

        // index data
        if(strcmp(line, "f") == 0) {
            int *data = malloc(9 * sizeof(*data));
            fscanf(objfile, "%d/%d/%d %d/%d/%d %d/%d/%d", 
                    data + 0,
                    data + 1,
                    data + 2,
                    data + 3,
                    data + 4,
                    data + 5,
                    data + 6,
                    data + 7,
                    data + 8 
                  );
            pushData(indices, (void *)data);
            continue;
        }

        // ignore (maybe add more line headers in future)
        else {
            fgets(line, 100, objfile);
        }
    }

    fclose(objfile);

    // write data to custom model file

    // creates output file name by removing .obj
    // from input and replacing it with .vrt
    char fn[100];
    char *loc;
    strcpy(fn, filename);
    loc = strrchr(fn, '.');
    int index = loc - fn;
    fn[index] = '\0';
    strcat(fn, ".vrt\0");

    FILE *output_file;
    output_file = fopen(fn, "wb");
    
    // output number of vertices
    // each index contains 3 vertices, describing one triangle
    int num_verts = indices->size * 3;
    fwrite(&num_verts, sizeof(num_verts), 1, output_file);

    struct Node *current = indices->head;
    while(current != 0) {
        int *index_data = (int *)current->data;
        for(int i = 0; i < 8; i += 3) {

            float *pos = (float *)getData(positions, index_data[i + 0] - 1);
            float *tex = (float *)getData(texcoords, index_data[i + 1] - 1);
            float *norm = (float *)getData(normals, index_data[i + 2] - 1);

            fwrite(pos, sizeof(*pos), 3, output_file);
            fwrite(norm, sizeof(*norm), 3, output_file);
            fwrite(tex, sizeof(*tex), 2, output_file);
        }
        current = current->next;
    }

    fclose(output_file);

    // destroy all malloced data

    destroyList(positions);
    destroyList(normals);
    destroyList(texcoords);
    destroyList(indices);

    return;
}
