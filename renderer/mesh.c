#include "mesh.h"
#include "array.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NUM_MESHES 10

static mesh_t meshes[MAX_NUM_MESHES];
static mesh_count = 0;

void load_mesh(char* obj_file_name, char* png_file_name, vect3_t scale, vect3_t translation, vect3_t rotation)
{

	load_mesh_obj_file_data(&meshes[mesh_count], obj_file_name);
	load_mesh_png_data(&meshes[mesh_count], png_file_name);
	meshes[mesh_count].scale = scale;
	meshes[mesh_count].translation = translation;
	meshes[mesh_count].rotation = rotation;

	mesh_count++;

}

void load_mesh_obj_file_data(mesh_t* mesh, char* obj_file_name)
{
	FILE* file;

	fopen_s(&file,obj_file_name, "r");

	char line[1024];

	tex2_t* texcoords = NULL;


	if (file == NULL)
	{
		fprintf(stderr, "Unable to open file!\n");
		return;

	}


	while (fgets(line, sizeof(line), file))
	{
		//vertex info
		if (strncmp(line, "v ", 2)==0)
		{
			vect3_t new_vertex;

			sscanf_s(line, "v %f %f %f", &new_vertex.x, &new_vertex.y, &new_vertex.z);
			array_push(mesh->vertices, new_vertex);

		}

		//tex coord info
		if (strncmp(line, "vt ", 3) == 0)
		{
			tex2_t texcoord;
			sscanf_s(line, "vt %f %f", &texcoord.u, &texcoord.v);
			array_push(texcoords, texcoord);
		}

		//face info
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
				.a = vertex_indices[0]-1,
				.b = vertex_indices[1]-1,
				.c = vertex_indices[2]-1,
				.a_uv = texcoords[texture_indices[0] - 1],
				.b_uv = texcoords[texture_indices[1] - 1],
				.c_uv = texcoords[texture_indices[2] - 1],
				.color = 0xFFF78228
				
			};


			array_push(mesh->faces, new_face);
		}
	}

	array_free(texcoords);

}

void load_mesh_png_data(mesh_t* mesh, char* png_file_name)
{
	png_info_t* png_info = malloc(sizeof(png_info_t));

	if (png_info == NULL)
	{
		free(png_info);
		return;
	}

	int error = lodepng_decode32_file(&png_info->png_image, &png_info->texture_width, &png_info->texture_height, png_file_name);
	if (error)
	{
		printf("error %u: %s\n", error, lodepng_error_text(error));
	}
	mesh->texture = png_info;

}

int get_num_meshes(void)
{
	return mesh_count;
}

mesh_t* get_mesh(int index)
{
	return &meshes[index];
}

void free_meshes(void)
{
	for (int i = 0; i < mesh_count; i++)
	{
		free_png_resources(meshes[i].texture);
		array_free(meshes[i].faces);
		array_free(meshes[i].vertices);

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