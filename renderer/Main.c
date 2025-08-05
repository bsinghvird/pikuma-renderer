#include<stdio.h>
#include "display.h"
#include "vector.h"

#define N_POINTS (9 * 9 * 9)

vect3_t cube_points[N_POINTS]; //9x9x9 cube
vect2_t projected_points[N_POINTS];

vect3_t camera_position = { .x = 0, .y = 0,.z = -5 };

vect3_t cube_rotation = { .x = 0, .y = 0, .z = 0 };

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

	//start loading array of vectors
	//from -1 to 1, 9x9x9 cube

	int point_count = 0;

	for (float x = -1; x <= 1; x += 0.25)
	{
		for (float y = -1; y <= 1; y += 0.25)
		{
			for (float z = -1; z <= 1; z += 0.25)
			{
				vect3_t new_point = { .x = x, .y = y, .z = z };
				cube_points[point_count] = new_point;
				point_count++;
			}
		}
	}

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


	cube_rotation.x += 0.01;
	cube_rotation.y += 0.01;
	cube_rotation.z += 0.01;

	for (int i = 0; i < N_POINTS; i++)
	{
		vect3_t point = cube_points[i];

		vect3_t transformed_point = vec3_rotate_x(point, cube_rotation.x);
		transformed_point = vec3_rotate_y(transformed_point, cube_rotation.y);
		transformed_point = vec3_rotate_z(transformed_point, cube_rotation.z);
		transformed_point.z -= camera_position.z;

		vect2_t projected_point = project(transformed_point);

		projected_points[i] = projected_point;
	}
}


void render()
{
	//draw_grid();

	for (int i = 0; i < N_POINTS; i++)
	{
		vect2_t projected_point = projected_points[i];
		draw_rect(
			projected_point.x +(window_width/2),
			projected_point.y +(window_height/2),
			4, 4, 0xffffff00);
	}

	render_color_buffer();

	clear_color_buffer(0xFF000000);

	SDL_RenderPresent(renderer);
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

	return 0;
}