
#include "wfc.h"

// ***** private *****

struct Mesh {
    struct Material *mat;
    int num_indices;
    int start_pos;
};

struct Material {
    char *name;
    char *texture_name;
};

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

void convertWavefront(const char *objfn) {
    printf("Converting %s to .vrt\n", objfn);
    // TODO: read material objfn from .obj instead of
    // assuming it will have the same name. This might 
    // burn me someday

    // ***** Read data from materials file *****

    //convert .obj to .mtl
    char *matfn = malloc(sizeof(*matfn) *(strlen(objfn) + 1));
    strcpy(matfn, objfn);
    char *loc = strrchr(matfn, '.');
    int index = loc - matfn;
    matfn[index] = '\0';
    strcat(matfn, ".mtl");

    FILE *matfile;
    matfile = fopen(matfn, "r");
    if(!matfile) {
        printf("ERROR opening %s:", matfn);
        perror("");
        exit(1);
    }
    
    free(matfn);

    struct List *materials = initList();
    // might need to increase size of a single line someday
    char line[256];

    while(fscanf(matfile, "%s", line) != -1) {
        if(strcmp(line, "newmtl") == 0) {
            fscanf(matfile, "%s", line);
            struct Material *newmat = malloc(sizeof(struct Material));
            newmat->name = malloc(sizeof(*newmat->name) * (strlen(line) + 1));
            strcpy(newmat->name, line);

            pushData(materials, (void *)newmat);

            continue;
        }

        if(strcmp(line, "map_Kd") == 0) {
            fscanf(matfile, "%s", line);
            struct Material *mat = (struct Material *)materials->tail->data;
            mat->texture_name = malloc(sizeof(*mat->texture_name) * (strlen(line) + 1));
            strcpy(mat->texture_name, line);
            continue;
        }

        char c = '0';
        while(c != '\n')
            c = fgetc(matfile);
    }

    fclose(matfile);

    // ***** Read data from .obj file *****
    FILE *objfile;
    objfile = fopen(objfn, "r");

    if(!objfile) {
        printf("ERROR opening %s\n", objfn);
        perror("");
        exit(1);
    }

    struct List *positions = initList();
    struct List *normals = initList();
    struct List *texcoords = initList();
    struct List *indices = initList();
    struct List *meshes = initList();

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
            // why did I do this? I forget.
            // something to do with the images need
            // to be flipped along the y axis
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

        // material information
        if(strcmp(line, "usemtl") == 0) {
            struct Mesh *newmesh = malloc(sizeof(*newmesh));
            fscanf(objfile, "%s", line);
            struct Node *node = materials->head;
            struct Material *current = (struct Material *)node->data;
            while(strcmp(line, current->name) != 0) {
                node = node->next;
                if(node == 0) {
                    printf("unable to find material %s in materials list\n", line);
                    exit(1);
                }
                current = (struct Material *)node->data;
            }
            newmesh->mat = (struct Material *)node->data;
            newmesh->start_pos = indices->size;
            newmesh->num_indices = 0;
            pushData(meshes, (void *)newmesh);
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
            struct Mesh *tail = (struct Mesh *)meshes->tail->data;
            tail->num_indices += 1;
            continue;
        }

        // ignore (maybe add more line headers in future)
        fgets(line, 100, objfile);
    }

    fclose(objfile);

    // write data to custom model file

    // creates output file name by removing .obj
    // from input and replacing it with .vrt
    char *vrtfn = malloc(sizeof(*vrtfn) *(strlen(objfn) + 1));
    strcpy(vrtfn, objfn);
    loc = strrchr(vrtfn, '.');
    index = loc - vrtfn;
    vrtfn[index] = '\0';
    strcat(vrtfn, ".vrt\0");

    FILE *output_file;
    output_file = fopen(vrtfn, "wb");
    if(!output_file) {
        printf("ERROR opening %s\n", vrtfn);
        perror("");
        exit(1);
    }

    free(vrtfn);
    
    //write the total number of meshes in this model
    int num_meshes = meshes->size;
    fwrite(&num_meshes, sizeof(num_meshes), 1, output_file);

    struct Node *current_mesh = meshes->head;
    while(current_mesh != 0) {
        // output size of texture file name (wihtout extension)
        struct Mesh *mesh = (struct Mesh *)current_mesh->data;
        struct Material *mat = mesh->mat;

        char *texname = malloc(sizeof(*texname) * (strlen(mat->texture_name) + 1));
        strcpy(texname, mat->texture_name);
        int index = strrchr(texname, '.') - texname;
        texname[index] = '\0';

        // write sixe of texture name
        int size = strlen(texname);
        fwrite(&size, sizeof(size), 1, output_file);
        //write the texture name (NO EXTENSION)
        fwrite(texname, sizeof(*texname), size, output_file);
        free(texname);

        // write number of vertices in this mesh
        // each index has 3 vertices, hence the * 3
        int num_verts = mesh->num_indices * 3;
        fwrite(&num_verts, sizeof(num_verts), 1, output_file);
        
        // output the vertex data
        for(int i = 0; i < mesh->num_indices; i ++) {
            int *index_data = (int *)getData(indices, mesh->start_pos + i);
            for(int j = 0; j < 8; j += 3) {

                float *pos = (float *)getData(positions, index_data[j + 0] - 1);
                float *tex = (float *)getData(texcoords, index_data[j + 1] - 1);
                float *norm = (float *)getData(normals, index_data[j + 2] - 1);

                fwrite(pos, sizeof(*pos), 3, output_file);
                fwrite(norm, sizeof(*norm), 3, output_file);
                fwrite(tex, sizeof(*tex), 2, output_file);
            }
        }

        current_mesh = current_mesh->next;
    }
    
    fclose(output_file);

    struct Node *current = materials->head;
    while(current != 0) {
        struct Material *mat = (struct Material *)current->data;
        free(mat->name);
        free(mat->texture_name);
        current = current->next;
    }

    destroyList(materials);
    destroyList(positions);
    destroyList(normals);
    destroyList(texcoords);
    destroyList(indices);
    destroyList(meshes);

    return;
}
