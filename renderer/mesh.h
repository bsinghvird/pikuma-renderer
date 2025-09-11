#ifndef  MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"
#include "png_info.h"

typedef struct {
	vect3_t* vertices; //dynamic array of vertices
	face_t* faces;// dyanmic array of faces (the magic of pointers)
	
	png_info_t* texture;
	
	vect3_t rotation;
	vect3_t scale;
	vect3_t translation;
}mesh_t;

void load_mesh(char* ob_file_name, char* png_file_name, vect3_t scale, vect3_t translation, vect3_t rotation);
void load_mesh_obj_file_data(mesh_t* mesh, char* obj_file_name);
void load_mesh_png_data(mesh_t* mesh, char* png_file_name);
int get_num_meshes(void);

mesh_t* get_mesh(int index);

void free_meshes(void);
//void my_load_obj_file_data(char* filename);

#endif // ! MESH_H
