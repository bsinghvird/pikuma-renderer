#ifndef DISPLAY_H
#define DISPLAY_H

#include<stdint.h>
#include<SDL.h>
#include<stdbool.h>
#include "triangle.h"

#define FPS 240
#define FRAME_TARGET_TIME (1000 / FPS)



enum Cull_mode
{
	CULL_NONE,
	CULL_BACKFACE
};

enum Render_mode
{
	RENDER_WIREFRAME_AND_RED_DOT_VERTEX,
	RENDER_WIREFRAME,
	RENDER_FILLED_TRIANGLES_SOLID_COLOR,
	RENDER_FILLED_AND_WIREFRAME,
	RENDER_TEXTURED,
	RENDER_TEXTURED_AND_WIREFRAME,
	RENDER_FILLED_PARALLEL
};

//extern SDL_Window* window;
//extern SDL_Renderer* renderer;
//
//extern uint32_t* color_buffer;
//extern float* z_buffer;
//extern SDL_Texture* color_buffer_texture;
//
//extern int window_width;
//extern int window_height;
//extern int total_num_pixels;

bool initialize_window(void);

int get_window_width(void);
int get_window_height(void);
int get_total_num_pixels();


void set_render_mode(enum Render_mode render_mode);
void set_cull_mode(enum Cull_mode cull_mode);
enum Cull_mode get_cull_mode(void);
enum Render_mode get_render_mode(void);

void draw_grid(void);
void draw_pixel(int x, int y, uint32_t color);
void draw_rect(int x, int y, int width, int height, uint32_t color);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
void draw_triangle_points(int x0, int y0, int x1, int y1,int x2, int y2, uint32_t color);
void draw_triangle(triangle_t *triangle, uint32_t color);

void render_color_buffer(void);
void clear_color_buffer(uint32_t color);
void clear_z_buffer(void);

float get_z_buffer_at(int x, int y);
void set_z_buffer_at(int x, int y, float z_value);


void destroy_window(void);

#endif