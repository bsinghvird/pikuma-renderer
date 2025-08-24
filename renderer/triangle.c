#include "triangle.h"
#include "display.h"
#include "swap.h"
//




///////////////////////////////////////////////////////////////////////////////
// Draw a filled a triangle with a flat bottom
///////////////////////////////////////////////////////////////////////////////
//
//        (x0,y0)
//          / \
//         /   \
//        /     \
//       /       \
//      /         \
//  (x1,y1)------(x2,y2)
//
///////////////////////////////////////////////////////////////////////////////
void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
	//not looking for rise over run, we want run over rise since we are plugging in y values and need to find x
	float inverse_slope_1 = (float)(x1 - x0) / (y1 - y0);
	float inverse_slope_2 = (float)(x2 - x0) / (y2 - y0);


	//x_start and x_end start at the top of the triangle with x0, y0 (the tippy top point of the triangle)

	float x_start = x0;
	float x_end = x0;


	for (int y = y0; y <= y2; y++)
	{
		draw_line(x_start, y, x_end, y, color);
		x_start += inverse_slope_1;
		x_end += inverse_slope_2;

	}
}

///////////////////////////////////////////////////////////////////////////////
// Draw a filled a triangle with a flat top
///////////////////////////////////////////////////////////////////////////////
//
//  (x0,y0)------(x1,y1)
//      \         /
//       \       /
//        \     /
//         \   /
//          \ /
//        (x2,y2)
//
///////////////////////////////////////////////////////////////////////////////
void fill_flat_top_triangle( int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
	float inverse_slope_1 = (float)(x2 - x1) / (y2 - y1);
	float inverse_slope_2 = (float)(x2 - x0) / (y2 - y0);

	//x_start and x_end start at the top of the triangle with x2, y2 (the bottom point of the triangle)

	float x_start = x2;
	float x_end = x2;


	for (int y = y2; y >= y0; y--)
	{
		draw_line(x_start, y, x_end, y, color);
		x_start -= inverse_slope_1;
		x_end -= inverse_slope_2;

	}
}


vect3_t barycentric_weights(vect2_t a, vect2_t b, vect2_t c, vect2_t p)
{

	vect2_t ac = vect2_sub(c, a);
	vect2_t ab = vect2_sub(b, a);
	vect2_t pc = vect2_sub(c, p);
	vect2_t pb = vect2_sub(b, p);
	vect2_t ap = vect2_sub(p, a);


	//cross product isn't defined for 2d, so that's why we're doing it manually here. We have an imaginary "z" cross product that goes into the monitor
	float area_parallelogram_abc = (ac.x * ab.y - ac.y * ab.x); // ||AC x AB||

	float alpha = (pc.x * pb.y - pc.y * pb.x) / area_parallelogram_abc;

	float beta = (ac.x * ap.y - ac.y * ap.x) / area_parallelogram_abc;

	float gamma = 1.0 - alpha - beta;

	vect3_t weights = { alpha, beta, gamma };

	return weights;
}


void draw_texel(
	int x, int y, uint32_t* texture,
	vect2_t point_a, vect2_t point_b, vect2_t point_c,
	float u0, float v0, float u1, float v1, float u2, float v2)
{

	vect2_t point_p = { x,y };
	vect3_t weights = barycentric_weights(point_a, point_b, point_c, point_p);


	float alpha = weights.x;
	float beta = weights.y;
	float gamma = weights.z;

	//perform interpolation of all u and v values using barycentric weights
	float interpolated_u = (u0 * alpha) + (u1 * beta) + (u2 * gamma);
	float interpolated_v = (v0 * alpha) + (v1 * beta) + (v2 * gamma);

	//uvs are float values between 0 and 1, so we need to scale by the texture width to get a number within the texture arraay
	int tex_x = abs((int)(interpolated_u * texture_width));
	int tex_y = abs((int)(interpolated_v * texture_height));

	int texture_index = (texture_width * tex_y) + tex_x;

	if (texture_index >= texture_height * texture_width)
	{
		return;
	}


	uint32_t color = texture[(texture_width * tex_y) + tex_x];

	draw_pixel(x, y, color);
}


void draw_textured_triangle(triangle_t* triangle, uint32_t* texture)
{

	draw_textured_triangle_points(
		triangle->points[0].x,
		triangle->points[0].y,
		triangle->texcoords[0].u,
		triangle->texcoords[0].v,
		triangle->points[1].x,
		triangle->points[1].y,
		triangle->texcoords[1].u,
		triangle->texcoords[1].v,
		triangle->points[2].x,
		triangle->points[2].y,
		triangle->texcoords[2].u,
		triangle->texcoords[2].v,
		texture
		);

}

void draw_textured_triangle_points(
	int x0, int y0, float u0, float v0,
	int x1, int y1, float u1, float v1,
	int x2, int y2, float u2, float v2,
	uint32_t *texture)
{

	if (y0 > y1)
	{
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
		float_swap(&u0, &u1);
		float_swap(&v0, &v1);
	}

	if (y1 > y2)
	{
		int_swap(&y1, &y2);
		int_swap(&x1, &x2);
		float_swap(&u1, &u2);
		float_swap(&v1, &v2);

	}

	if (y0 > y1)
	{
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
		float_swap(&u0, &u1);
		float_swap(&v0, &v1);
	}



	vect2_t point_a = { x0, y0 };
	vect2_t point_b = { x1, y1 };
	vect2_t point_c = { x2, y2 };



	//render upper part of the triangle (flat bottom)
	float inv_slope_1 = 0;
	float inv_slope_2 = 0;

	if(y1 != y0) inv_slope_1 = (float)(x1 - x0) / abs(y1 - y0);
	if(y2 != y0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

	if (y1 != y0)
	{
		for (int y = y0; y <= y1; y++)
		{
			int x_start = x1 + (y - y1) * inv_slope_1;
			int x_end = x0 + (y - y0) * inv_slope_2;


			if (x_end < x_start)
			{
				int_swap(&x_end, &x_start);
			}



			for (int x = x_start; x <= x_end; x++)
			{
				draw_texel(x, y, texture, point_a, point_b, point_c, u0, v0, u1, v1, u2, v2);
			}


		}
	}


	//draw bottom part of triangle 



	inv_slope_1 = 0;
	inv_slope_2 = 0;
	//inv_slope2 is the same as above so it does not need to be calculated again but the video does so here we are

	if (y2 != y1) inv_slope_1 = (float)(x2 - x1) / abs(y2 - y1);
	if (y2 != y0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

	if (y2 != y1)
	{
		for (int y = y1; y <= y2; y++)
		{
			int x_start = x1 + (y - y1) * inv_slope_1;
			int x_end = x0 + (y - y0) * inv_slope_2;


			if (x_end < x_start)
			{
				int_swap(&x_end, &x_start);
			}

			for (int x = x_start; x <= x_end; x++)
			{
				draw_texel(x, y, texture, point_a, point_b, point_c, u0, v0, u1, v1, u2, v2);
			}

		}
	}

}







void draw_filled_triangle(triangle_t *triangle, uint32_t color)
{

	draw_filled_triangle_points(
		triangle->points[0].x,
		triangle->points[0].y,
		triangle->points[1].x,
		triangle->points[1].y,
		triangle->points[2].x,
		triangle->points[2].y,
		color);

}

void draw_filled_triangle_points(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
	//look it's bubble sort but super manual for exactly three things
	if (y0 > y1)
	{
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
	}

	if (y1 > y2)
	{
		int_swap(&y1, &y2);
		int_swap(&x1, &x2);
	
	}

	if (y0 > y1)
	{
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
	}


	//avoiding division by 0, in the case there is no "top" or "bottom" of the triangle, so only need to draw the one part. 
	if (y1 == y2)
	{
		fill_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, color);
	}
	else if (y0 == y1)
	{
		fill_flat_top_triangle(x0, y0, x1, y1, x2, y2, color);
	}
	else
	{
		//calculate mx my using triangle simularity

		int My = y1;

		int Mx = ((float)((x2 - x0) * (y1 - y0)) / (float)(y2 - y0)) + x0;


		fill_flat_bottom_triangle(x0, y0, x1, y1, Mx, My, color);

		fill_flat_top_triangle(x1, y1, Mx, My, x2, y2, color);
	}




	



}




