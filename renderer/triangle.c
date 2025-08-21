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

void draw_textured_triangle_points(int x0, int y0, float u0, float v0, int x1, int y1, float u1, float v1, int x2, int y2, float u2, float v2, uint32_t *texture)
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
				draw_pixel(x, y, (x%2 == 0 && y % 2 ==0 )? 0xffff00ff : 0xff000000);
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
				draw_pixel(x, y, (x % 2 == 0 && y % 2 == 0) ? 0xffff00ff : 0xff000000);
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




