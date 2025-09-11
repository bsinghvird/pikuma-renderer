#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdint.h>
#include "vector.h"
#include "texture.h"
#include "png_info.h"

typedef struct
{
	int a;
	int b;
	int c;
	tex2_t a_uv;
	tex2_t b_uv;
	tex2_t c_uv;
	uint32_t color;
} face_t;

typedef struct
{
	vect4_t points[3];
	tex2_t texcoords[3];//where each point is on the 0-1 UV map
	uint32_t color;
	png_info_t* texture;
} triangle_t;


vect3_t barycentric_weights(vect2_t a, vect2_t b, vect2_t c, vect2_t p);



void draw_filled_triangle(triangle_t *triangle, uint32_t color);

void draw_filled_triangle_points(
	int x0, int y0, float z0, float w0,
	int x1, int y1, float z1, float w1,
	int x2, int y2, float z2, float w2,
	uint32_t color);


void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int Mx, int My, uint32_t color);

void fill_flat_top_triangle(int x1, int y1, int Mx, int My, int x2, int y2, uint32_t color);


void draw_texel(
	int x, int y, png_info_t* texture,
	vect4_t point_a, vect4_t point_b, vect4_t point_c,
	tex2_t a_uv, tex2_t b_uv, tex2_t c_uv);

void draw_triangle_pixel(
	int x, int y,
	vect4_t point_a, vect4_t point_b, vect4_t point_c,
	uint32_t color);


void draw_textured_triangle(triangle_t* triangle);

void draw_textured_triangle_points(
	int x0, int y0, float z0, float w0, float u0, float v0,
	int x1, int y1, float z1, float w1, float u1, float v1,
	int x2, int y2, float z2, float w2, float u2, float v2,
	png_info_t* texture);

vect3_t get_triangle_normal(vect4_t vertices[3]);

#endif // !TRIANGLE_H
