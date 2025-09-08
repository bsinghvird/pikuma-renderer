#include <math.h>
#include "clipping.h"


#define NUM_PLANES 6
plane_t frustum_planes[NUM_PLANES];

///////////////////////////////////////////////////////////////////////////////
// Frustum planes are defined by a point and a normal vector
///////////////////////////////////////////////////////////////////////////////
// Near plane   :  P=(0, 0, znear), N=(0, 0,  1)
// Far plane    :  P=(0, 0, zfar),  N=(0, 0, -1)
// Top plane    :  P=(0, 0, 0),     N=(0, -cos(fov/2), sin(fov/2))
// Bottom plane :  P=(0, 0, 0),     N=(0, cos(fov/2), sin(fov/2))
// Left plane   :  P=(0, 0, 0),     N=(cos(fov/2), 0, sin(fov/2))
// Right plane  :  P=(0, 0, 0),     N=(-cos(fov/2), 0, sin(fov/2))
///////////////////////////////////////////////////////////////////////////////
//
//           /|\
//         /  | | 
//       /\   | |
//     /      | |
//  P*|-->  <-|*|   ----> +z-axis
//     \      | |
//       \/   | |
//         \  | | 
//           \|/
//
///////////////////////////////////////////////////////////////////////////////
void init_frustum_planes(float fov_x, float fov_y, float z_near, float z_far){
	
	float cos_half_fov_x = cos(fov_x / 2);
	float sin_half_fov_x = sin(fov_x / 2);

	float cos_half_fov_y = cos(fov_y / 2);
	float sin_half_fov_y = sin(fov_y / 2);

			
	frustum_planes[LEFT_FRUSTUM_PLANE].point = vect3_new(0, 0, 0);
	frustum_planes[LEFT_FRUSTUM_PLANE].normal.x = cos_half_fov_x;
	frustum_planes[LEFT_FRUSTUM_PLANE].normal.y = 0;
	frustum_planes[LEFT_FRUSTUM_PLANE].normal.z = sin_half_fov_x;

	frustum_planes[RIGHT_FRUSTUM_PLANE].point = vect3_new(0, 0, 0);
	frustum_planes[RIGHT_FRUSTUM_PLANE].normal.x = -cos_half_fov_x;
	frustum_planes[RIGHT_FRUSTUM_PLANE].normal.y = 0;
	frustum_planes[RIGHT_FRUSTUM_PLANE].normal.z = sin_half_fov_x;

	frustum_planes[TOP_FRUSTUM_PLANE].point = vect3_new(0, 0, 0);
	frustum_planes[TOP_FRUSTUM_PLANE].normal.x = 0;
	frustum_planes[TOP_FRUSTUM_PLANE].normal.y = -cos_half_fov_y;
	frustum_planes[TOP_FRUSTUM_PLANE].normal.z = sin_half_fov_y;

	frustum_planes[BOTTOM_FRUSTUM_PLANE].point = vect3_new(0, 0, 0);
	frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.x = 0;
	frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.y = cos_half_fov_y;
	frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.z = sin_half_fov_y;

	frustum_planes[NEAR_FRUSTUM_PLANE].point = vect3_new(0, 0, z_near);
	frustum_planes[NEAR_FRUSTUM_PLANE].normal.x = 0;
	frustum_planes[NEAR_FRUSTUM_PLANE].normal.y = 0;
	frustum_planes[NEAR_FRUSTUM_PLANE].normal.z = 1;

	frustum_planes[FAR_FRUSTUM_PLANE].point = vect3_new(0, 0, z_far);
	frustum_planes[FAR_FRUSTUM_PLANE].normal.x = 0;
	frustum_planes[FAR_FRUSTUM_PLANE].normal.y = 0;
	frustum_planes[FAR_FRUSTUM_PLANE].normal.z = -1;
}

polygon_t create_polygon_from_triangle(vect3_t v0, vect3_t v1, vect3_t v2, tex2_t t0, tex2_t t1, tex2_t t2)
{
	polygon_t polygon = {
		.vertices = {v0, v1, v2},
		.texcoords = {t0, t1, t2},
		.num_vertices = 3
	};

	return polygon;
}

void clip_polygon(polygon_t* polygon)
{
	clip_polygon_against_plane(polygon, LEFT_FRUSTUM_PLANE);
	clip_polygon_against_plane(polygon, RIGHT_FRUSTUM_PLANE);
	clip_polygon_against_plane(polygon, TOP_FRUSTUM_PLANE);
	clip_polygon_against_plane(polygon, BOTTOM_FRUSTUM_PLANE);
	clip_polygon_against_plane(polygon, NEAR_FRUSTUM_PLANE);
	clip_polygon_against_plane(polygon, FAR_FRUSTUM_PLANE);
}

void clip_polygon_against_plane(polygon_t* polygon, enum plane_location plane)
{
	vect3_t plane_point = frustum_planes[plane].point;
	vect3_t plane_normal = frustum_planes[plane].normal;


	vect3_t inside_vertices[MAX_NUM_POLY_VERTICES];
	tex2_t inside_texcoords[MAX_NUM_POLY_VERTICES];
	int num_inside_vertices = 0;


	//start current with the first vertex, previous with the last vertex
	vect3_t* current_vertex = &polygon->vertices[0]; //Q2
	tex2_t* current_tex_coord = &polygon->texcoords[0];
	
	vect3_t* previous_vertex = &polygon->vertices[polygon->num_vertices-1]; //Q1
	tex2_t* previous_tex_coord = &polygon->texcoords[polygon->num_vertices - 1];

	float current_dot = 0;
	float previous_dot = vect3_dot(vect3_sub(*previous_vertex, plane_point), plane_normal);

	//why no -1??
	//another person made a comment about this and it's as I suspected, since we initialize the vertices array with a size of 10, and it's not possible
	//to clip a triangle to have more than 10 vertices, we never run into this problem. I don't like it, but I guess it's TECHNICALLY fine
	while (current_vertex != &polygon->vertices[polygon->num_vertices])
	{
		current_dot = vect3_dot(vect3_sub(*current_vertex, plane_point), plane_normal);
		
		//if line goes from outside to inside or inside to outside the plane

		if (current_dot * previous_dot < 0)
		{
			//find the interpolation factor t
			

			float t = previous_dot / (previous_dot - current_dot);
			
			//calculate the intersection point I = Q1 +t(Q2-Q1)
			vect3_t intersection_point =
			{
				.x = float_lerp(previous_vertex->x, current_vertex->x, t),
				.y = float_lerp(previous_vertex->y, current_vertex->y, t),
				.z = float_lerp(previous_vertex->z, current_vertex->z, t),
			};
		
			//use the linear interpolationg formula (lerp) to get the interpolated u and v coord
			tex2_t interpolated_tex_coord =
			{
				.u = float_lerp(previous_tex_coord->u, current_tex_coord->u, t),
				.v = float_lerp(previous_tex_coord->v, current_tex_coord->v, t)
			};

			inside_vertices[num_inside_vertices] = vec3_clone(&intersection_point);
			inside_texcoords[num_inside_vertices] = tex2_clone(&interpolated_tex_coord);
			num_inside_vertices++;


		}


		//current vertex is inside plane
		if (current_dot > 0)
		{
			inside_vertices[num_inside_vertices] = vec3_clone(current_vertex);
			inside_texcoords[num_inside_vertices] = tex2_clone(current_tex_coord);
			num_inside_vertices++;
		}


		//move to the next vertex and set the current to the previous
		previous_dot = current_dot;
		previous_vertex = current_vertex;
		previous_tex_coord = current_tex_coord;

		current_vertex++;
		current_tex_coord++;

	}


	for (int i = 0; i < num_inside_vertices; i++)
	{
		polygon->vertices[i] = vec3_clone(&inside_vertices[i]);
		polygon->texcoords[i] = tex2_clone(&inside_texcoords[i]);
	}

	polygon->num_vertices = num_inside_vertices;


}

void triangles_from_polygon(polygon_t* polygon, triangle_t triangles[], int* num_triangles)
{

	for (int i = 0; i < polygon->num_vertices - 2; i++)
	{
		int index0 = 0;
		int index1 = i + 1;
		int index2 = i + 2;

		triangles[i].points[0] = vec4_from_vec3(polygon->vertices[index0]);
		triangles[i].points[1] = vec4_from_vec3(polygon->vertices[index1]);
		triangles[i].points[2] = vec4_from_vec3(polygon->vertices[index2]);

		triangles[i].texcoords[0] = polygon->texcoords[index0];
		triangles[i].texcoords[1] = polygon->texcoords[index1];
		triangles[i].texcoords[2] = polygon->texcoords[index2];

	}

	*num_triangles = polygon->num_vertices - 2;

}

float float_lerp(float a, float b, float t)
{
	return a + t*(b - a);
}

