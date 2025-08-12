#include "triangle.h"
#include "display.h"
//
void int_swap(int* a, int* b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;
}


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
	float inverse_sloap_1 = (float)(x1 - x0) / (y1 - y0);
	float inverse_sloap_2 = (float)(x2 - x0) / (y2 - y0);


	//x_start and x_end start at the top of the triangle with x0, y0 (the tippy top point of the triangle)

	float x_start = x0;
	float x_end = x0;


	for (int y = y0; y <= y2; y++)
	{
		draw_line(x_start, y, x_end, y, color);
		x_start += inverse_sloap_1;
		x_end += inverse_sloap_2;

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
	float inverse_sloap_1 = (float)(x2 - x1) / (y2 - y1);
	float inverse_sloap_2 = (float)(x2 - x0) / (y2 - y0);

	//x_start and x_end start at the top of the triangle with x2, y2 (the bottom point of the triangle)

	float x_start = x2;
	float x_end = x2;


	for (int y = y2; y >= y0; y--)
	{
		draw_line(x_start, y, x_end, y, color);
		x_start -= inverse_sloap_1;
		x_end -= inverse_sloap_2;

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




