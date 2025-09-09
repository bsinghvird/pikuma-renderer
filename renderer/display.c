#include<stdio.h>
#include "display.h"

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;

static uint32_t* color_buffer = NULL;
static float* z_buffer = NULL;

static SDL_Texture* color_buffer_texture = NULL;

static int window_width = 1920;
static int window_height = 1080;
static int total_num_pixels = 0;

static enum Render_mode render_mode = RENDER_TEXTURED;
static enum Cull_mode cull_mode = CULL_BACKFACE;

int get_window_width(void)
{
	return window_width;
}

int get_window_height(void)
{
	return window_height;
}

int get_total_num_pixels()
{
	return total_num_pixels;
}

void set_cull_mode(enum Cull_mode new_cull_mode)
{
	cull_mode = new_cull_mode;
}

void set_render_mode(enum Render_mode new_render_mode)
{
	render_mode = new_render_mode;

}

bool initialize_window(void)
{



	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		fprintf(stderr, "Error initializing SDL.\n");
		return false;
	}

	SDL_DisplayMode display_mode;
	SDL_GetCurrentDisplayMode(0, &display_mode);

	//window_width = display_mode.w;
	//window_height = display_mode.h;

	total_num_pixels = window_width * window_height;


	window = SDL_CreateWindow(
		NULL,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		window_width,
		window_height,
		SDL_WINDOW_BORDERLESS);

	if (!window)
	{
		fprintf(stderr, "Error creating SDL window.\n ");
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer)
	{
		fprintf(stderr, "Error creating SDL renderer.\n");
		return false;
	}

	//SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);


	//allocate the required memory in bytes to hold the color buffer
	color_buffer = (uint32_t*)malloc(sizeof(uint32_t) * window_width * window_height);
	z_buffer = (float*)malloc(sizeof(float) * window_width * window_height);

	//creating an SDL texture that is used to display the color buffer
	color_buffer_texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_RGBA32,
		SDL_TEXTUREACCESS_STREAMING,
		window_width,
		window_height
	);

	return true;
}

enum Cull_mode get_cull_mode(void)
{
	return cull_mode;
}

enum Render_mode get_render_mode(void)
{
	return render_mode;
}

void draw_grid(void)
{
	for (int y = 0; y < window_height; y += 10)
	{
		for (int x = 0; x < window_width; x += 10)
		{
			color_buffer[(window_width * y) + x] = 0xFFFFFFFF;

			if (x % 10 == 0 || y % 10 == 0)
			{

			}
			//else
			//{
			//	color_buffer[(window_width * y) + x] = 0xFF000000;
			//}

		}
	}
}


void draw_rect(int x, int y, int width, int height, uint32_t color)
{
	int end_point_x = x + width;
	int end_point_y = y + height;

	for (int current_x = x; current_x < end_point_x; current_x++)
	{
		for (int current_y = y; current_y < end_point_y; current_y++)
		{

			draw_pixel(current_x, current_y, color);

		}
	}
}

void draw_pixel(int x, int y, uint32_t color)
{
	if (x < 0 || x >= window_width || y < 0 || y >= window_height)
	{
		return;
	}

	color_buffer[(window_width * y) + x] = color;

}

void draw_line(int x0, int y0, int x1, int y1, uint32_t color)
{
	int delta_x = (x1 - x0);
	int delta_y = (y1 - y0);


	int longest_side_length = abs(delta_x) >= abs(delta_y) ? abs(delta_x) : abs(delta_y);

	//one of these will end up being 1 or -1
	float x_inc = delta_x / (float)longest_side_length;
	float y_inc = delta_y / (float)longest_side_length;

	float current_x = x0;
	float current_y = y0;


	for (int i = 0; i <= longest_side_length; i++)
	{
		draw_pixel(round(current_x), round(current_y), color);
		current_x += x_inc;
		current_y += y_inc;

	
	}


}


void draw_triangle_points(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
	draw_line(x0, y0, x1, y1, color);
	draw_line(x1, y1, x2, y2, color);
	draw_line(x2, y2, x0, y0, color);
}

void draw_triangle(triangle_t *triangle, uint32_t color)
{

	draw_triangle_points(
		triangle->points[0].x,
		triangle->points[0].y,
		triangle->points[1].x,
		triangle->points[1].y,
		triangle->points[2].x,
		triangle->points[2].y,
		color);
}

void render_color_buffer(void)
{
	SDL_UpdateTexture(
		color_buffer_texture,
		NULL,
		color_buffer,
		(int)(window_width * sizeof(uint32_t))
	);

	SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

void clear_color_buffer(uint32_t color)
{
	for (int i = 0; i < window_width * window_height; i++)
	{
		color_buffer[i] = color;
	}

}

void clear_z_buffer(void)
{

	for (int i = 0; i < window_width * window_height; i++)
	{	
		//use 1.0, not 0 because we are using 1/w
		z_buffer[i] = 1.0;
	}
}

float get_z_buffer_at(int x, int y)
{

	if (x < 0 || x >= window_width || y < 0 || y >= window_height)
	{
		return 1.0;
	}

	return z_buffer[(window_width * y) + x];
}

void set_z_buffer_at(int x, int y, float z_value)
{
	if (x < 0 || x >= window_width || y < 0 || y >= window_height)
	{
		return;
	}

	z_buffer[(window_width * y) + x] = z_value;
}

void destroy_window(void)
{
	free(color_buffer);
	free(z_buffer);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

