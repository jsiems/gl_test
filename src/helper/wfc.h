#ifndef WAVEFRONT_CONVERTER_H
#define WAVEFRONT_CONVERTER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dirfuncs.h"

/*  Format of .VRT:
*   [num_meshes](int)
*       each mesh will have:
*       [tex_name_length](int)
*       [tex_name](tex_name_length bytes)
*       [spec_name_length](int, can be 0)
*       [spec_name](spec_name_length bytes)
*       [num_vertices](int)
*           each vertex:
*           [position](3 floats)
*           [norm_vec](3 floats)
*           [tex_coord](2 floats)          
*/

// converts wavefront object in filename
// to custom model data storage file
void convertWavefront(const char *objfilename);

#endif
