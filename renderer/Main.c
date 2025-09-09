#include<stdio.h>
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "array.h"
#include "matrix.h"
#include "light.h"
#include "texture.h"
#include "triangle.h"
#include "camera.h"
#include "clipping.h"


#define MAX_TRIANGLES_PER_MESH 10000

triangle_t triangles_to_render[MAX_TRIANGLES_PER_MESH];

int num_triangles_to_render = 0;

float fov_factor = 640;

bool is_running = false;


int previous_frame_time = 0;

float delta_time = 0.0;


mat4_t projection_matrix;
mat4_t view_matrix;
mat4_t world_matrix;

//light_t light_source;

void setup(void)
{

	//setters for render method 
	set_render_mode(RENDER_TEXTURED);
	set_cull_mode(CULL_BACKFACE);


	float aspect_ratio_y = (float)get_window_height() / (float)get_window_width();
	float aspect_ratio_x = (float)get_window_width() / (float)get_window_height();

	float fov_y = M_PI / 3.0; //radians wooo 60 degrees
	float fov_x = atan(tan(fov_y / 2) * aspect_ratio_x)*2;
	
	float z_near = 0.1;
	float z_far = 100.0;

	projection_matrix = mat4_make_perspective(fov_y, aspect_ratio_y, z_near, z_far);

	//initialize FRUSTUMS
	init_frustum_planes(fov_x, fov_y, z_near, z_far);




	/*light_source.direction.x = 0;
	light_source.direction.y = 0;
	light_source.direction.z = 1;*/


	init_light(vect3_new(0, 0, 1));


	/*mesh_texture = (uint32_t*) REDBRICK_TEXTURE;
	texture_width = 64;
	texture_height = 64;*/

	//load_cube_mesh_data();
	// 
	//load_obj_file_data("./assets/cube.obj");
	//load_png_texture_data("./assets/cube.png");
	// 
	//my_load_obj_file_data("./assets/appa_triangulated.obj");

	//load_obj_file_data("./assets/appa_triangulated.obj");
	
	load_obj_file_data("./assets/f22.obj");
	load_png_texture_data("./assets/f22.png");

	//load_obj_file_data("./assets/drone.obj");
	//load_png_texture_data("./assets/drone.png");

	//load_obj_file_data("./assets/f117.obj");
	//load_png_texture_data("./assets/f117.png");

	/*load_obj_file_data("./assets/efa.obj");
	load_png_texture_data("./assets/efa.png");*/

	//load_obj_file_data("./assets/crab.obj");
	//load_png_texture_data("./assets/crab.png");


}

void process_input(void)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			is_running = false;
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE)
				is_running = false;
			else if (event.key.keysym.sym == SDLK_1)
				set_render_mode(RENDER_WIREFRAME_AND_RED_DOT_VERTEX);
			else if (event.key.keysym.sym == SDLK_2)
				set_render_mode(RENDER_WIREFRAME);
			else if (event.key.keysym.sym == SDLK_3)
				set_render_mode(RENDER_FILLED_TRIANGLES_SOLID_COLOR);
			else if (event.key.keysym.sym == SDLK_4)
				set_render_mode(RENDER_FILLED_AND_WIREFRAME);
			else if (event.key.keysym.sym == SDLK_5)
				set_render_mode(RENDER_TEXTURED);
			else if (event.key.keysym.sym == SDLK_6)
				set_render_mode(RENDER_TEXTURED_AND_WIREFRAME);
			else if (event.key.keysym.sym == SDLK_c)
				set_cull_mode(CULL_BACKFACE);
			else if (event.key.keysym.sym == SDLK_x)
				set_cull_mode(CULL_NONE);

			if (event.key.keysym.sym == SDLK_SPACE)
				camera.position.y += 3.0 * delta_time;
			if (event.key.keysym.sym == SDLK_LCTRL)
				camera.position.y -= 3.0 * delta_time;

			if (event.key.keysym.sym == SDLK_d)
			{
				vect3_t up_direction = { 0,1,0 };
				vect3_t side = vect3_cross(camera.direction, up_direction);
				camera.side_velocity = vect3_mul(side, 5.0 * delta_time);
				camera.position = vect3_sub(camera.position, camera.side_velocity);
			}
			if (event.key.keysym.sym == SDLK_a)
			{
				vect3_t up_direction = { 0,1,0 };
				vect3_t side = vect3_cross(camera.direction, up_direction);
				camera.side_velocity = vect3_mul(side, 5.0 * delta_time);
				camera.position = vect3_add(camera.position, camera.side_velocity);
			}
			if (event.key.keysym.sym == SDLK_LEFT)
				camera.yaw -= 1.0 * delta_time;
			if (event.key.keysym.sym == SDLK_RIGHT)
				camera.yaw += 1.0 * delta_time;

			if (event.key.keysym.sym == SDLK_UP)
				camera.pitch -= 1.0 * delta_time;
			if (event.key.keysym.sym == SDLK_DOWN)
				camera.pitch += 1.0 * delta_time;

			if (event.key.keysym.sym == SDLK_w)
			{
				camera.forward_velocity = vect3_mul(camera.direction, 5.0 * delta_time);
				camera.position = vect3_add(camera.position, camera.forward_velocity);
			}
			if (event.key.keysym.sym == SDLK_s)
			{
				camera.forward_velocity = vect3_mul(camera.direction, 5.0 * delta_time);
				camera.position = vect3_sub(camera.position, camera.forward_velocity);
			}


			break;

		}
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
	vect3_t origin = { 0,0,0 };
	vect3_t camera_ray = vect3_sub(origin, a);


	float dot = vect3_dot(camera_ray, normal);


	return dot < 0;

}


//this works fine, but end up calculating all these vectors twice since these are the same steps done for backface culling
//for better performance (though who knows actually how much better in reality), this function is not called and is done manually in the update function instead
float calculate_light_intesnity(vect4_t *transformed_vertices)
{
	float intensity = 0;


	vect3_t vector_a = vec3_from_vec4(transformed_vertices[0]);
	vect3_t vector_b = vec3_from_vec4(transformed_vertices[1]);
	vect3_t vector_c = vec3_from_vec4(transformed_vertices[2]);


	vect3_t vector_ab = vect3_sub(vector_b, vector_a);
	vect3_t vector_ac = vect3_sub(vector_c, vector_a);
	vect3_normalize(&vector_ab);
	vect3_normalize(&vector_ac);

	vect3_t normal = vect3_cross(vector_ab, vector_ac);
	vect3_normalize(&normal);

	//vect3_normalize(&get_light_direction());

	float dot = -vect3_dot(get_light_direction(), normal);

	intensity = dot;

	if (dot < 0.0)
	{
		intensity = 0;

	}

	return intensity;
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

	delta_time = (SDL_GetTicks() - previous_frame_time) / 1000.0f;


	previous_frame_time = SDL_GetTicks();

	num_triangles_to_render = 0;

	//mesh.rotation.x += 0.01;
	//mesh.rotation.y += 0.3 * delta_time;
	//mesh.rotation.z += 0.01;

	//mesh.scale.x += 0.002;
	//mesh.scale.y += 0.001;

	//mesh.translation.x += 0.01;
	mesh.translation.z = 5.0;

	
	vect3_t up_direction = { 0,1,0 };

/*
	this was my implenetation. The thing I was doing wrong was mulitplying the matrixes in the wrong order
	 I was doing rotation yaw x target = result, then rotation pitch x result
	 Instead I need to create a matrix of just the rotations. So start with identity matrix (rotation matrix)
	 then camera pitch x camera rotation, save result in camera rotation, camera yaw x camera rotation, save result in camera rotation
	 THEN camera direction = camera rotation x target
	
	initialize the target looking at the positive z axis
	vect3_t target = { 0,0,1 };
	mat4_t camera_yaw_rotation = mat4_make_rotation_y(camera.yaw);
	mat4_t camera_pitch_rotation = mat4_make_rotation_x(camera.pitch);
	
	camera.direction = vec3_from_vec4(mat4_mul_vec4(camera_yaw_rotation, vec4_from_vec3 (target)));

	camera.direction = vec3_from_vec4(mat4_mul_vec4(camera_pitch_rotation, vec4_from_vec3(camera.direction)));

	//offset the camera position in the direction where the camera is pointing at
	target = vect3_add(camera.position, camera.direction);
*/
	//initialize the target looking at the positive z axis
	vect3_t target = { 0,0,1 };

	mat4_t camera_yaw_rotation = mat4_make_rotation_y(camera.yaw);
	mat4_t camera_pitch_rotation = mat4_make_rotation_x(camera.pitch);


	mat4_t camera_rotation = mat4_identity();
	camera_rotation = mat4_mul_mat4(camera_pitch_rotation, camera_rotation);
	camera_rotation = mat4_mul_mat4(camera_yaw_rotation, camera_rotation);

	vect4_t camera_direction = mat4_mul_vec4(camera_rotation, vec4_from_vec3(target));
	camera.direction = vec3_from_vec4(camera_direction);

	target = vect3_add(camera.position, camera.direction);


	view_matrix = mat4_look_at(camera.position, target, up_direction);



	mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
	mat4_t translation_matrix = mat4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);
	mat4_t rotation_x = mat4_make_rotation_x(mesh.rotation.x);
	mat4_t rotation_y = mat4_make_rotation_y(mesh.rotation.y);
	mat4_t rotation_z = mat4_make_rotation_z(mesh.rotation.z);

	int num_faces = array_length(mesh.faces);

	for (int i = 0; i < num_faces; i++)
	{

		if (i != 4)
		{
			//continue;
		}

		face_t mesh_face = mesh.faces[i];

		vect3_t face_vertices[3];
		face_vertices[0] = mesh.vertices[mesh_face.a];
		face_vertices[1] = mesh.vertices[mesh_face.b];
		face_vertices[2] = mesh.vertices[mesh_face.c];

		

		vect4_t transformed_vertices[3];


		for (int j = 0; j < 3; j++)
		{
			vect4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);


			//  [T] * [R] * [S] * v
			world_matrix = mat4_identity();
			world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
			world_matrix = mat4_mul_mat4(rotation_z, world_matrix);
			world_matrix = mat4_mul_mat4(rotation_y, world_matrix);
			world_matrix = mat4_mul_mat4(rotation_x, world_matrix);

			world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);


			transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);

			transformed_vertex = mat4_mul_vec4(view_matrix, transformed_vertex);

			
			transformed_vertices[j] = transformed_vertex;

		}


		vect3_t vector_a = vec3_from_vec4(transformed_vertices[0]);
		vect3_t vector_b = vec3_from_vec4(transformed_vertices[1]);
		vect3_t vector_c = vec3_from_vec4(transformed_vertices[2]);


		vect3_t vector_ab = vect3_sub(vector_b, vector_a);
		vect3_t vector_ac = vect3_sub(vector_c, vector_a);
		vect3_normalize(&vector_ab);
		vect3_normalize(&vector_ac);

		vect3_t normal = vect3_cross(vector_ab, vector_ac);
		vect3_normalize(&normal);

		//because we will be performing view transforms, the camera is always going to be at the origin (or at least that's how I think it works)
		vect3_t origin = { 0,0,0 };
		vect3_t camera_ray = vect3_sub(origin, vector_a);

		float dot_normal_camera = vect3_dot(normal, camera_ray);


		if (get_cull_mode() == CULL_BACKFACE)
		{

			if (dot_normal_camera < 0)
			{
				continue;
			}
		}


		//if (my_cull_face(vector_a, vector_b, vector_c))
		//{
		//	continue;
		//}
		
		polygon_t polygon = create_polygon_from_triangle(
			vec3_from_vec4(transformed_vertices[0]),
			vec3_from_vec4(transformed_vertices[1]),
			vec3_from_vec4(transformed_vertices[2]),
			mesh_face.a_uv,
			mesh_face.b_uv,
			mesh_face.c_uv);

		clip_polygon(&polygon);


		//triangularize the n-gon we got from clipping the polygon

		triangle_t triangles_after_clipping[MAX_NUM_POLY_TRIANGLES];
		int num_triangles_after_clipping = 0;

		triangles_from_polygon(&polygon, triangles_after_clipping, &num_triangles_after_clipping);


		//for loop and do all the below for each triangle?

		for (int triangle_count = 0; triangle_count < num_triangles_after_clipping; triangle_count++)
		{
			triangle_t triangle_after_clipping = triangles_after_clipping[triangle_count];

			vect4_t projected_points[3];

			for (int j = 0; j < 3; j++)
			{

				projected_points[j] = mat4_mul_vec4_project(projection_matrix, triangle_after_clipping.points[j]);

				//scale into view
				projected_points[j].x *= (get_window_width() / 2.0);
				projected_points[j].y *= (get_window_height() / 2.0);

				//projected_points[j].x *= -1;
				projected_points[j].y *= -1;


				//translate into view
				projected_points[j].x += (get_window_width() / 2.0);
				projected_points[j].y += (get_window_height() / 2.0);

			}

			//vect3_normalize(&light_source.direction);

			float light_intensity_factor = -vect3_dot(get_light_direction(), normal);


			if (light_intensity_factor < 0.0)
			{
				light_intensity_factor = 0.0;

			}

			//float light_intensity_factor = calculate_light_intesnity(transformed_vertices);

			uint32_t triangle_color = light_apply_intensity(mesh_face.color, light_intensity_factor);

			triangle_t triangle_to_render =
			{
				.points = {
					{projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w},
					{projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w},
					{projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w}
				},
				.texcoords = {
					{triangle_after_clipping.texcoords[0].u, triangle_after_clipping.texcoords[0].v},
					{triangle_after_clipping.texcoords[1].u, triangle_after_clipping.texcoords[1].v},
					{triangle_after_clipping.texcoords[2].u, triangle_after_clipping.texcoords[2].v},
				},
				.color = triangle_color,
			};

			

			if (num_triangles_to_render < MAX_TRIANGLES_PER_MESH)
			{
				triangles_to_render[num_triangles_to_render] = triangle_to_render;
				num_triangles_to_render++;
			}

		}


	}

	

}


void render()
{
	//draw_grid();

	clear_color_buffer(0xFF000000);
	clear_z_buffer();


	for (int i = 0; i < num_triangles_to_render; i++) {
		triangle_t triangle = triangles_to_render[i];

		uint32_t wire_color = 0xff0000ff;
		uint32_t fill_color = 0xFFF78228;

		if (get_render_mode() == RENDER_WIREFRAME_AND_RED_DOT_VERTEX)
		{
			draw_triangle(&triangle, wire_color);
			draw_rect(triangle.points[0].x-3, triangle.points[0].y-3, 6, 6, 0xffff0000);
			draw_rect(triangle.points[1].x-3, triangle.points[1].y-3, 6, 6, 0xffff0000);
			draw_rect(triangle.points[2].x-3, triangle.points[2].y, 6, 6, 0xffff0000);
	
		}
		else if (get_render_mode() == RENDER_WIREFRAME)
		{
			draw_triangle(&triangle, wire_color);
		}
		else if (get_render_mode() == RENDER_FILLED_TRIANGLES_SOLID_COLOR)
		{
			draw_filled_triangle(&triangle, triangle.color);
		}
		else if (get_render_mode() == RENDER_FILLED_AND_WIREFRAME)
		{
			draw_filled_triangle(&triangle, triangle.color);

			draw_triangle(&triangle, wire_color);
		}
		else if (get_render_mode() == RENDER_TEXTURED)
		{

			draw_textured_triangle(&triangle, mesh_texture);

			
		}
		else if (get_render_mode() == RENDER_TEXTURED_AND_WIREFRAME)
		{
			draw_textured_triangle(&triangle, mesh_texture);
			draw_triangle(&triangle, wire_color);
		}

	}

	render_color_buffer();

	
}

void free_resources(void)
{
	array_free(mesh.faces);
	array_free(mesh.vertices);
	free(png_texture);
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