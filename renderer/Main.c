#include<stdio.h>
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "array.h"

triangle_t* triangles_to_render = NULL;


vect3_t camera_position = { .x = 0, .y = 0,.z = -5 };



float fov_factor = 640;

bool is_running = false;
int previous_frame_time = 0;

void setup(void)
{
	//allocate the required memory in bytes to hold the color buffer
	color_buffer = (uint32_t*)malloc(sizeof(uint32_t) * window_width * window_height);

	//creatning an SDL texture that is used to display the color buffer
	color_buffer_texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		window_width,
		window_height
		);

	//load_cube_mesh_data();
	//my_load_obj_file_data("./assets/appa_triangulated.obj");
	//load_obj_file_data("./assets/cube.obj");
	//load_obj_file_data("./assets/appa_triangulated.obj");
	load_obj_file_data("./assets/f22.obj");


}

void process_input(void)
{
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type)
	{
		case SDL_QUIT:
			is_running = false;
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE)
				is_running = false;
			break;
	}

}


vect2_t project(vect3_t point)
{
	vect2_t projected_point =
	{
		.x = (fov_factor * point.x)/point.z,
		.y = (fov_factor * point.y)/point.z
	};

	return projected_point;
}


vect2_t project_orthographic(vect3_t point)
{
	vect2_t projected_point =
	{
		.x = (fov_factor * point.x),
		.y = (fov_factor * point.y)
	};

	return projected_point;
}


void update(void)
{
	//spin locks wweeeeee
	//while (!SDL_TICKS_PASSED(SDL_GetTicks(), previous_frame_time + FRAME_TARGET_TIME));

	int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);


	if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
	{
		SDL_Delay(time_to_wait);
	}

	previous_frame_time = SDL_GetTicks();


	triangles_to_render = NULL;


	mesh.rotation.x += 0.01;
	mesh.rotation.y += 0.00;
	mesh.rotation.z += 0.00;

	int num_faces = array_length(mesh.faces);

	for (int i = 0; i < num_faces; i++)
	{
		face_t mesh_face = mesh.faces[i];

		vect3_t face_vertices[3];
		face_vertices[0] = mesh.vertices[mesh_face.a - 1];
		face_vertices[1] = mesh.vertices[mesh_face.b - 1];
		face_vertices[2] = mesh.vertices[mesh_face.c - 1];


		triangle_t projected_triangle;

		for (int j = 0; j < 3; j++)
		{
			vect3_t transformed_vertex = face_vertices[j];

			transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x);
			transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
			transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);
			transformed_vertex.z -= camera_position.z;

			transformed_vertex.z -= camera_position.z;

			vect2_t projected_point = project(transformed_vertex);

			projected_point.x += (window_width / 2),
			projected_point.y += (window_height / 2),

			projected_triangle.points[j] = projected_point;

		}

		array_push(triangles_to_render, projected_triangle);


	}

}


void render()
{
	//draw_grid();



	int num_triangles = array_length(triangles_to_render);

	for (int i = 0; i < num_triangles; i++)
	{
		triangle_t triangle = triangles_to_render[i];

		draw_rect(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xffffff00);
		draw_rect(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xffffff00);
		draw_rect(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xffffff00);

		draw_triangle(&triangle, 0xffff3300);

/*		draw_triangle_points(
			triangle.points[0].x,
			triangle.points[0].y,
			triangle.points[1].x,
			triangle.points[1].y,
			triangle.points[2].x,
			triangle.points[2].y,
			0xffff3300);*/		

	}


	array_free(triangles_to_render);

	render_color_buffer();

	clear_color_buffer(0xFF000000);

	SDL_RenderPresent(renderer);
}


void free_resources(void)
{
	free(color_buffer);
	array_free(mesh.faces);
	array_free(mesh.vertices);
}

int main(int argc, char* args[])
{
	is_running = initialize_window();

	setup();


	vect3_t myvector = { 2.0,3.0,1.0 };


	while (is_running)
	{
		process_input();
		update();
		render();
	}

	destroy_window();
	free_resources();

	return 0;
}