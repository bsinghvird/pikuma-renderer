#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdint.h>
#include "vector.h"
#include "texture.h"

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
	vect2_t points[3];
	tex2_t texcoords[3];//where each point is on the 0-1 UV map
	uint32_t color;
	float avg_depth;
} triangle_t;


vect3_t barycentric_weights(vect2_t a, vect2_t b, vect2_t c, vect2_t p);



void draw_filled_triangle(triangle_t *triangle, uint32_t color);

void draw_filled_triangle_points(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);


void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int Mx, int My, uint32_t color);

void fill_flat_top_triangle(int x1, int y1, int Mx, int My, int x2, int y2, uint32_t color);


void draw_texel(
	int x, int y, uint32_t* texture,
	vect2_t point_a, vect2_t point_b, vect2_t point_c,
	float u0, float v0, float u1, float v1, float u2, float v2);

void draw_textured_triangle(triangle_t* triangle, uint32_t* texture);

void draw_textured_triangle_points(
	int x0, int y0, float u0, float v0,
	int x1, int y1, float u1, float v1,
	int x2, int y2, float u2, float v2,
	uint32_t* texture);



#endif // !TRIANGLE_H
