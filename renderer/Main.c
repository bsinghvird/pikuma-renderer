#include<stdio.h>
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "array.h"
#include "matrix.h"

triangle_t* triangles_to_render = NULL;


vect3_t camera_position = { .x = 0, .y = 0,.z = 0 };




enum Render_mode selected_render_mode = RENDER_FILLED_TRIANGLES_SOLID_COLOR;
enum cull_mode selected_cull_mode = CULL_BACKFACE;

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

	load_cube_mesh_data();
	//my_load_obj_file_data("./assets/appa_triangulated.obj");
	//load_obj_file_data("./assets/cube.obj");
	//load_obj_file_data("./assets/appa_triangulated.obj");
	//load_obj_file_data("./assets/f22.obj");

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
			else if (event.key.keysym.sym == SDLK_1)
				selected_render_mode = RENDER_WIREFRAME_AND_RED_DOT_VERTEX;
		
			else if (event.key.keysym.sym == SDLK_2)
				selected_render_mode = RENDER_WIREFRAME;
			else if (event.key.keysym.sym == SDLK_3)
				selected_render_mode = RENDER_FILLED_TRIANGLES_SOLID_COLOR;
			else if (event.key.keysym.sym == SDLK_4)
				selected_render_mode = RENDER_FILLED_AND_WIREFRAME;
			else if (event.key.keysym.sym == SDLK_c)
				selected_cull_mode = CULL_BACKFACE;
			else if (event.key.keysym.sym == SDLK_d)
				selected_cull_mode = CULL_NONE;
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


bool my_cull_face(vect3_t a, vect3_t b, vect3_t c)
{


	//my attempt at back face culling
	vect3_t vector_ab = vect3_sub(b, a);
	vect3_t vector_ac = vect3_sub(c, a);

	vect3_t normal = vect3_cross(vector_ab, vector_ac);


	//I pretty much had it right, but I had these swapped around 
	vect3_t camera_ray = vect3_sub(camera_position, a);

	float dot = vect3_dot(camera_ray, normal);


	return dot < 0;

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
	mesh.rotation.y += 0.01;
	mesh.rotation.z += 0.01;

	mesh.scale.x += 0.002;
	mesh.scale.y += 0.001;

	mesh.translation.x += 0.01;
	mesh.translation.z = 5.0;

	mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
	mat4_t translation_matrix = mat4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);
	mat4_t rotation_x = mat4_make_rotation_x(mesh.rotation.x);
	mat4_t rotation_y = mat4_make_rotation_y(mesh.rotation.y);
	mat4_t rotation_z = mat4_make_rotation_z(mesh.rotation.z);

	int num_faces = array_length(mesh.faces);

	for (int i = 0; i < num_faces; i++)
	{
		face_t mesh_face = mesh.faces[i];

		vect3_t face_vertices[3];
		face_vertices[0] = mesh.vertices[mesh_face.a - 1];
		face_vertices[1] = mesh.vertices[mesh_face.b - 1];
		face_vertices[2] = mesh.vertices[mesh_face.c - 1];

		

		vect4_t transformed_vertices[3];


		for (int j = 0; j < 3; j++)
		{
			vect4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);


			transformed_vertex = mat4_mul_vec4(scale_matrix, transformed_vertex);

			transformed_vertex = mat4_mul_vec4(rotation_x, transformed_vertex);
			transformed_vertex = mat4_mul_vec4(rotation_y, transformed_vertex);
			transformed_vertex = mat4_mul_vec4(rotation_z, transformed_vertex);

			transformed_vertex = mat4_mul_vec4(translation_matrix, transformed_vertex);

			
			transformed_vertices[j] = transformed_vertex;

		}

		if (selected_cull_mode == CULL_BACKFACE)
		{
			vect3_t vector_a = vec3_from_vec4(transformed_vertices[0]);
			vect3_t vector_b = vec3_from_vec4(transformed_vertices[1]);
			vect3_t vector_c = vec3_from_vec4(transformed_vertices[2]);


			vect3_t vector_ab = vect3_sub(vector_b, vector_a);
			vect3_t vector_ac = vect3_sub(vector_c, vector_a);
			vect3_normalize(&vector_ab);
			vect3_normalize(&vector_ac);

			vect3_t normal = vect3_cross(vector_ab, vector_ac);
			vect3_normalize(&normal);

			vect3_t camera_ray = vect3_sub(camera_position, vector_a);

			float dot_normal_camera = vect3_dot(normal, camera_ray);


			if (dot_normal_camera < 0)
			{
				continue;
			}
		}


		//if (my_cull_face(vector_a, vector_b, vector_c))
		//{
		//	continue;
		//}
		


		vect2_t projected_points[3];

		for (int j = 0; j < 3; j++)
		{

			projected_points[j] =  project(vec3_from_vec4(transformed_vertices[j]));

			projected_points[j].x += (window_width / 2);
			projected_points[j].y += (window_height / 2);

			
		}


		float avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3.0;


		triangle_t projected_triangle =
		{
			.points = {
				{projected_points[0].x, projected_points[0].y},
				{projected_points[1].x, projected_points[1].y},
				{projected_points[2].x, projected_points[2].y}
			},
			.color = mesh_face.color,
			.avg_depth = avg_depth
		};



		array_push(triangles_to_render, projected_triangle);


	}

	int num_triangles_to_render = array_length(triangles_to_render);


	for (int i = 0; i < num_triangles_to_render; i++)
	{
		for (int j = i; j < num_triangles_to_render; j++)
		{
			if (triangles_to_render[i].avg_depth < triangles_to_render[j].avg_depth) {

				triangle_t temp = triangles_to_render[i];
				triangles_to_render[i] = triangles_to_render[j];
				triangles_to_render[j] = temp;
			}
		}
	}


}


void render()
{
	//draw_grid();



	int num_triangles = array_length(triangles_to_render);

	for (int i = 0; i < num_triangles; i++) {
		triangle_t triangle = triangles_to_render[i];

		uint32_t wire_color = 0xff0000ff;
		uint32_t fill_color = 0xFFF78228;

		if (selected_render_mode == RENDER_WIREFRAME_AND_RED_DOT_VERTEX)
		{
			draw_triangle(&triangle, wire_color);
			draw_rect(triangle.points[0].x-3, triangle.points[0].y-3, 6, 6, 0xffff0000);
			draw_rect(triangle.points[1].x-3, triangle.points[1].y-3, 6, 6, 0xffff0000);
			draw_rect(triangle.points[2].x-3, triangle.points[2].y, 6, 6, 0xffff0000);
	
		}
		else if (selected_render_mode == RENDER_WIREFRAME)
		{
			draw_triangle(&triangle, wire_color);
		}
		else if (selected_render_mode == RENDER_FILLED_TRIANGLES_SOLID_COLOR)
		{
			draw_filled_triangle(&triangle, triangle.color);
		}
		else if (selected_render_mode == RENDER_FILLED_AND_WIREFRAME)
		{
			draw_filled_triangle(&triangle, triangle.color);

			draw_triangle(&triangle, wire_color);
		}


		/*draw_filled_triangle(&triangle, 0xff113300);

		draw_triangle(&triangle, 0xffff3300);*/

		// Draw filled triangle
		//draw_filled_triangle_points(
		//	triangle.points[0].x, triangle.points[0].y, // vertex A
		//	triangle.points[1].x, triangle.points[1].y, // vertex B
		//	triangle.points[2].x, triangle.points[2].y, // vertex C
		//	
		//);

		//// Draw unfilled triangle
		//draw_triangle_points(
		//	triangle.points[0].x, triangle.points[0].y, // vertex A
		//	triangle.points[1].x, triangle.points[1].y, // vertex B
		//	triangle.points[2].x, triangle.points[2].y, // vertex C
		//	0xFF000000
		//);
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