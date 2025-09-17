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
	int x, int y, png_info_t* texture,
	vect4_t point_a, vect4_t point_b, vect4_t point_c,
	tex2_t a_uv, tex2_t b_uv, tex2_t c_uv)
{


	//int buffer_index = (get_window_width() * y) + x;

	//the videos don't have to do this, but kept getting buffer indexes outside of the range of how many pixels there are on the screen. 
	//if (buffer_index >= get_total_num_pixels())
	//{
	//	return;
	//}

	vect2_t p = { x,y };
	vect2_t a = vec2_from_vec4(point_a);
	vect2_t b = vec2_from_vec4(point_b);
	vect2_t c = vec2_from_vec4(point_c);

	vect3_t weights = barycentric_weights(a, b, c, p);

	float alpha = weights.x;
	float beta = weights.y;
	float gamma = weights.z;

	float interpolated_u;
	float interpolated_v;
	float interpolated_reciprocal_w; //"1/w"

	//perform interpolation of all u/w and v/w values using barycentric weights and a factor of 1/w
	interpolated_u = ((a_uv.u / point_a.w) * alpha) + ((b_uv.u / point_b.w) * beta) + ((c_uv.u / point_c.w) * gamma);
	interpolated_v = ((a_uv.v / point_a.w) * alpha) + ((b_uv.v / point_b.w) * beta) + ((c_uv.v / point_c.w) * gamma);

	//interpolate the value of 1/w for the current pixel
	interpolated_reciprocal_w = ((1 / point_a.w) * alpha) + ((1 / point_b.w) * beta) + ((1 / point_c.w) * gamma);

	//divide back both interpolated values by 1/w
	interpolated_u /= interpolated_reciprocal_w;
	interpolated_v /= interpolated_reciprocal_w;


	int texture_width = texture->texture_width;
	int texture_height = texture->texture_height;

	//uvs are float values between 0 and 1, so we need to scale by the texture width to get a number within the texture arraay
	int tex_x = abs((int)(interpolated_u * texture_width)) % texture_width;
	int tex_y = abs((int)(interpolated_v * texture_height)) % texture_height;


	int texture_index = ((texture_width * tex_y) + tex_x);

	/*if (texture_index >= texture_height * texture_width)
	{
		return;
	}*/



	uint32_t* texture_buffer = (uint32_t*)texture->png_image;

	uint32_t color = texture_buffer[texture_index];


	//adjust 1/w so the pixels that are closer to the camera have smaller values
	interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;


	//only draw pixel if depth value is less than the one previously stored in z buffer
	if (interpolated_reciprocal_w < get_z_buffer_at(x,y))
	{
		draw_pixel(x, y, color);
		
		set_z_buffer_at(x, y, interpolated_reciprocal_w);

	}


}

void draw_triangle_pixel(
	int x, int y,
	vect4_t point_a, vect4_t point_b, vect4_t point_c,
	uint32_t color)
{


	//int buffer_index = (get_window_width() * y) + x;

	////the videos don't have to do this, but kept getting buffer indexes outside of the range of how many pixels there are on the screen. 
	//if (buffer_index >= get_total_num_pixels())
	//{
	//	return;
	//}


	vect2_t p = { x,y };
	vect2_t a = vec2_from_vec4(point_a);
	vect2_t b = vec2_from_vec4(point_b);
	vect2_t c = vec2_from_vec4(point_c);

	vect3_t weights = barycentric_weights(a, b, c, p);

	float alpha = weights.x;
	float beta = weights.y;
	float gamma = weights.z;

	float interpolated_reciprocal_w; //"1/w"


	//interpolate the value of 1/w for the current pixel
	interpolated_reciprocal_w = ((1 / point_a.w) * alpha) + ((1 / point_b.w) * beta) + ((1 / point_c.w) * gamma);

	//adjust 1/w so the pixels that are closer to the camera have smaller values
	interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;


	//only draw pixel if depth value is less than the one previously stored in z buffer
	if (interpolated_reciprocal_w < get_z_buffer_at(x, y))
	{
		draw_pixel(x, y, color);

		set_z_buffer_at(x, y, interpolated_reciprocal_w);

	}

}

void draw_textured_triangle(triangle_t* triangle)
{

	draw_textured_triangle_points(
		triangle->points[0].x,
		triangle->points[0].y,
		triangle->points[0].z,
		triangle->points[0].w,
		triangle->texcoords[0].u,
		triangle->texcoords[0].v,
		triangle->points[1].x,
		triangle->points[1].y,
		triangle->points[1].z,
		triangle->points[1].w,
		triangle->texcoords[1].u,
		triangle->texcoords[1].v,
		triangle->points[2].x,
		triangle->points[2].y,
		triangle->points[2].z,
		triangle->points[2].w,
		triangle->texcoords[2].u,
		triangle->texcoords[2].v,
		triangle->texture
		);

}

void draw_textured_triangle_points(
	int x0, int y0, float z0, float w0, float u0, float v0,
	int x1, int y1, float z1, float w1, float u1, float v1,
	int x2, int y2, float z2, float w2, float u2, float v2,
	png_info_t* texture)
{

	if (y0 > y1)
	{
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
		
		float_swap(&z0, &z1);
		float_swap(&w0, &w1);
		
		float_swap(&u0, &u1);
		float_swap(&v0, &v1);

	}

	if (y1 > y2)
	{
		int_swap(&y1, &y2);
		int_swap(&x1, &x2);

		float_swap(&z1, &z2);
		float_swap(&w1, &w2);

		float_swap(&u1, &u2);
		float_swap(&v1, &v2);

	}

	if (y0 > y1)
	{
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);

		float_swap(&z0, &z1);
		float_swap(&w0, &w1);

		float_swap(&u0, &u1);
		float_swap(&v0, &v1);
	}


	v0 = 1 - v0;
	v1 = 1 - v1;
	v2 = 1 - v2;


	//create vector points and texture coordinates
	vect4_t point_a = { x0, y0, z0, w0 };
	vect4_t point_b = { x1, y1, z1, w1 };
	vect4_t point_c = { x2, y2, z2, w2 };

	tex2_t a_uv = { u0, v0 };
	tex2_t b_uv = { u1, v1 };
	tex2_t c_uv = { u2, v2 };


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
				draw_texel(x, y, texture, point_a, point_b, point_c, a_uv, b_uv, c_uv);
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
				draw_texel(x, y, texture, point_a, point_b, point_c, a_uv, b_uv, c_uv);
			}

		}
	}

}

vect3_t get_triangle_normal(vect4_t vertices[3])
{
	vect3_t vector_a = vec3_from_vec4(vertices[0]);
	vect3_t vector_b = vec3_from_vec4(vertices[1]);
	vect3_t vector_c = vec3_from_vec4(vertices[2]);


	vect3_t vector_ab = vect3_sub(vector_b, vector_a);
	vect3_t vector_ac = vect3_sub(vector_c, vector_a);
	vect3_normalize(&vector_ab);
	vect3_normalize(&vector_ac);

	vect3_t normal = vect3_cross(vector_ab, vector_ac);
	vect3_normalize(&normal);

	return normal;
}

bool is_top_or_left_edge(vect4_t* start, vect4_t* end)
{
	vect2_t edge = { end->x - start->x, end->y - start->y };

	//this works because we look at edges in a clock wise direction only
	bool is_top_edge = edge.y == 0 && edge.x > 0;
	bool is_left_edge = edge.y < 0;

	return is_top_edge || is_left_edge;
}


void draw_filled_triangle(triangle_t *triangle, uint32_t color)
{

	draw_filled_triangle_points(
		triangle->points[0].x,
		triangle->points[0].y,
		triangle->points[0].z,
		triangle->points[0].w,
		triangle->points[1].x,
		triangle->points[1].y,
		triangle->points[1].z,
		triangle->points[1].w,
		triangle->points[2].x,
		triangle->points[2].y,
		triangle->points[2].z,
		triangle->points[2].w,
		color);

}

void draw_filled_triangle_points(
	int x0, int y0, float z0, float w0,
	int x1, int y1, float z1, float w1,
	int x2, int y2, float z2, float w2,
	uint32_t color)
{
	if (y0 > y1)
	{
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);

		float_swap(&z0, &z1);
		float_swap(&w0, &w1);

	}

	if (y1 > y2)
	{
		int_swap(&y1, &y2);
		int_swap(&x1, &x2);

		float_swap(&z1, &z2);
		float_swap(&w1, &w2);

	}

	if (y0 > y1)
	{
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);

		float_swap(&z0, &z1);
		float_swap(&w0, &w1);

	}


	//create vector points and texture coordinates
	vect4_t point_a = { x0, y0, z0, w0 };
	vect4_t point_b = { x1, y1, z1, w1 };
	vect4_t point_c = { x2, y2, z2, w2 };

	//render upper part of the triangle (flat bottom)
	float inv_slope_1 = 0;
	float inv_slope_2 = 0;

	if (y1 - y0 != 0) inv_slope_1 = (float)(x1 - x0) / abs(y1 - y0);
	if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

	if (y1 - y0 != 0) 
	{
		for (int y = y0; y <= y1; y++)
		{
			int x_start = x1 + (y - y1) * inv_slope_1;
			int x_end = x0 + (y - y0) * inv_slope_2;


			if (x_end < x_start)
			{
				int_swap(&x_end, &x_start);
			}

			for (int x = x_start; x < x_end; x++)
			{
				draw_triangle_pixel(x, y, point_a, point_b, point_c, color);
			}


		}
	}


	//draw bottom part of triangle 



	inv_slope_1 = 0;
	inv_slope_2 = 0;
	//inv_slope2 is the same as above so it does not need to be calculated again but the video does so here we are

	if (y2 - y1 != 0) inv_slope_1 = (float)(x2 - x1) / abs(y2 - y1);
	if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

	if (y2 - y1 != 0) 
	{
		for (int y = y1; y <= y2; y++)
		{
			int x_start = x1 + (y - y1) * inv_slope_1;
			int x_end = x0 + (y - y0) * inv_slope_2;


			if (x_end < x_start)
			{
				int_swap(&x_end, &x_start);
			}

			for (int x = x_start; x < x_end; x++)
			{
				draw_triangle_pixel(x, y, point_a, point_b, point_c, color);
			}

		}
	}

}

void draw_filled_triangle_parallel(triangle_t* triangle, uint32_t color)
{

	vect4_t v0 = triangle->points[0];
	vect4_t v1 = triangle->points[1];
	vect4_t v2 = triangle->points[2];



	int x_min = floor(fmin(fmin(triangle->points[0].x, triangle->points[1].x), triangle->points[2].x));
	int y_min = floor(fmin(fmin(triangle->points[0].y, triangle->points[1].y), triangle->points[2].y));

	int x_max = ceil(fmax(fmax(triangle->points[0].x, triangle->points[1].x), triangle->points[2].x));
	int y_max = ceil(fmax(fmax(triangle->points[0].y, triangle->points[1].y), triangle->points[2].y));

	float bias0 = is_top_or_left_edge(&triangle->points[1], &triangle->points[2]) ? 0 : -0.0001;
	float bias1 = is_top_or_left_edge(&triangle->points[2], &triangle->points[0]) ? 0 : -0.0001;
	float bias2 = is_top_or_left_edge(&triangle->points[0], &triangle->points[1]) ? 0 : -0.0001;


	float delta_w0_col = (v1.y - v2.y);
	float delta_w1_col = (v2.y - v0.y);
	float delta_w2_col = (v0.y - v1.y);

	float delta_w0_row = (v2.x - v1.x);
	float delta_w1_row = (v0.x - v2.x);
	float delta_w2_row = (v1.x - v0.x);

	vect2_t p0 = { x_min, y_min };
	float w0_row = edge_cross(&v1, &v2, &p0) + bias0;
	float w1_row = edge_cross(&v2, &v0, &p0) + bias1;
	float w2_row = edge_cross(&v0, &v1, &p0) + bias2;

	for (int y = y_min; y <= y_max; y++) 
	{


		float w0 = w0_row;
		float w1 = w1_row;
		float w2 = w2_row;


		for (int x = x_min; x <= x_max; x++) 
		{

			bool is_inside = w0 >= 0 && w1 >= 0 && w2 >= 0;

			if (is_inside) {
				draw_triangle_pixel(x, y, triangle->points[0], triangle->points[1], triangle->points[2], color);
			}

			w0 += delta_w0_col;
			w1 += delta_w1_col;
			w2 += delta_w2_col;
		}

		w0_row += delta_w0_row;
		w1_row += delta_w1_row;
		w2_row += delta_w2_row;
	}


}
