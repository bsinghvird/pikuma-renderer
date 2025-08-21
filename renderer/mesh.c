#include "mesh.h"
#include "array.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

mesh_t mesh = {
	.vertices = NULL,
	.faces = NULL,
	.rotation = {0,0,0},
	.scale = {1.0, 1.0, 1.0},
	.translation = {0,0,0}
};


vect3_t cube_vertices[N_CUBE_VERTICES] =
{
	{.x = -1, .y = -1, .z = -1},
	{.x = -1, .y = 1, .z = -1},
	{.x = 1, .y = 1, .z = -1},
	{.x = 1, .y = -1, .z = -1},
	{.x = 1, .y = 1, .z = 1},
	{.x = 1, .y = -1, .z = 1},
	{.x = -1, .y = 1, .z = 1},
	{.x = -1, .y = -1, .z = 1}
};

face_t cube_faces[N_CUBE_FACES] = {
	// front
	{.a = 1, .b = 2, .c = 3, .a_uv = { 0, 0 }, .b_uv = { 0, 1 }, .c_uv = { 1, 1 }, .color = 0xffff0000 },
	{.a = 1, .b = 3, .c = 4, .a_uv = { 0, 0 }, .b_uv = { 1, 1 }, .c_uv = { 1, 0 }, .color = 0xffff0000 },
	// right
	{.a = 4, .b = 3, .c = 5, .a_uv = { 0, 0 }, .b_uv = { 0, 1 }, .c_uv = { 1, 1 }, .color = 0xff00ff00 },
	{.a = 4, .b = 5, .c = 6, .a_uv = { 0, 0 }, .b_uv = { 1, 1 }, .c_uv = { 1, 0 }, .color = 0xff00ff00 },
	// back
	{.a = 6, .b = 5, .c = 7, .a_uv = { 0, 0 }, .b_uv = { 0, 1 }, .c_uv = { 1, 1 }, .color = 0xff0000ff },
	{.a = 6, .b = 7, .c = 8, .a_uv = { 0, 0 }, .b_uv = { 1, 1 }, .c_uv = { 1, 0 }, .color = 0xff0000ff },
	// left
	{.a = 8, .b = 7, .c = 2, .a_uv = { 0, 0 }, .b_uv = { 0, 1 }, .c_uv = { 1, 1 }, .color = 0xffffff00 },
	{.a = 8, .b = 2, .c = 1, .a_uv = { 0, 0 }, .b_uv = { 1, 1 }, .c_uv = { 1, 0 }, .color = 0xffffff00 },
	// top
	{.a = 2, .b = 7, .c = 5, .a_uv = { 0, 0 }, .b_uv = { 0, 1 }, .c_uv = { 1, 1 }, .color = 0xffff00ff },
	{.a = 2, .b = 5, .c = 3, .a_uv = { 0, 0 }, .b_uv = { 1, 1 }, .c_uv = { 1, 0 }, .color = 0xffff00ff },
	// bottom
	{.a = 6, .b = 8, .c = 1, .a_uv = { 0, 0 }, .b_uv = { 0, 1 }, .c_uv = { 1, 1 }, .color = 0xff00ffff },
	{.a = 6, .b = 1, .c = 4, .a_uv = { 0, 0 }, .b_uv = { 1, 1 }, .c_uv = { 1, 0 }, .color = 0xff00ffff }
};


void load_cube_mesh_data(void)
{
	for (int i = 0; i < N_CUBE_VERTICES; i++)
	{
		vect3_t cube_vertex = cube_vertices[i];
		array_push(mesh.vertices, cube_vertex);
	}

	for (int i = 0; i < N_CUBE_FACES; i++)
	{
		face_t cube_face = cube_faces[i];
		array_push(mesh.faces, cube_face);
	}
}


void load_obj_file_data(char* filename)
{
	FILE* file;

	fopen_s(&file,filename, "r");

	char line[512];

	if (file == NULL)
	{
		fprintf(stderr, "Unable to open file!\n");
		return;

	}


	while (fgets(line, sizeof(line), file))
	{
		if (strncmp(line, "v ", 2)==0)
		{
			vect3_t new_vertex;

			sscanf_s(line, "v %f %f %f", &new_vertex.x, &new_vertex.y, &new_vertex.z);
			array_push(mesh.vertices, new_vertex);

		}

		else if (strncmp(line, "f ", 2) == 0)
		{

			int num_indices = 9;
			int vertex_indices[3];
			int texture_indices[3];
			int normal_indices[3];

			int num_indices_read = sscanf_s(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
				&vertex_indices[0], &texture_indices[0], &normal_indices[0],
				&vertex_indices[1], &texture_indices[1], &normal_indices[1], 
				&vertex_indices[2], &texture_indices[2], &normal_indices[2]);

			//when exporting obj from maya sometimes there's not texture indices. not really sure why but this deals with that
			if (num_indices_read != num_indices)
			{
				sscanf_s(line, "f %d//%d %d//%d %d//%d",
					&vertex_indices[0], &normal_indices[0],
					&vertex_indices[1], &normal_indices[1],
					&vertex_indices[2], &normal_indices[2]);
			}

			face_t new_face =
			{
				.a = vertex_indices[0],
				.b = vertex_indices[1],
				.c = vertex_indices[2],
				.color = 0xFFF78228
				
			};


			array_push(mesh.faces, new_face);
		}
	}

}



//void my_load_obj_file_data(char* filename)
//{
//	FILE* file;
//	fopen_s(&file, filename, "r");
//
//	// Buffer to store each line of the file.
//	char line[256];
//
//	// Check if the file was opened successfully.
//	if (file != NULL) {
//		// Read each line from the file and store it in the
//		// 'line' buffer.
//		while (fgets(line, sizeof(line), file)) {
//			// Print each line to the standard output.
//
//
//			char temp[256];
//
//			strcpy(temp, line);
//
//			char* token = strtok(line, " ");
//
//			if (strcmp(token, "v") == 0)
//			{
//
//				vect3_t new_vertex;
//
//				token = strtok(NULL, " ");
//				new_vertex.x = strtof(token, NULL);
//				token = strtok(NULL, " ");
//				new_vertex.y = strtof(token, NULL);
//				token = strtok(NULL, " ");
//				new_vertex.z = strtof(token, NULL);
//
//				array_push(mesh.vertices, new_vertex);
//
//			}
//
//			else if (strcmp(token, "f") == 0)
//			{
//
//				face_t new_face;
//
//				int count = 0;
//
//				token = strtok(NULL, " ");
//				//token = strtok(NULL, "/");
//				new_face.a = atoi(token);
//				token = strtok(NULL, " ");
//				new_face.b = atoi(token);
//				token = strtok(NULL, " ");
//				new_face.c = atoi(token);
//
//				array_push(mesh.faces, new_face);
//
//			}
//			
//		}
//
//		// Close the file stream once all lines have been
//		// read.
//		fclose(file);
//	}
//	else {
//		// Print an error message to the standard error
//		// stream if the file cannot be opened.
//		fprintf(stderr, "Unable to open file!\n");
//	}
//}