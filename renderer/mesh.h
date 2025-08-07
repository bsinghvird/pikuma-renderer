#ifndef  MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"

#define N_CUBE_VERTICES 8
#define N_CUBE_FACES (6 * 2)

extern vect3_t cube_vertices[N_CUBE_VERTICES];
extern face_t cube_faces[N_CUBE_FACES];

typedef struct {
	vect3_t* vertices; //dynamic array of vertices
	face_t* faces;// dyanmic array of faces (the magic of pointers)
	vect3_t rotation;
}mesh_t;

extern mesh_t mesh;

void load_cube_mesh_data(void);

#endif // ! MESH_H
