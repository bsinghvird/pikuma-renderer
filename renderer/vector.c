#include "vector.h"
#include <math.h>

vect2_t vect2_new(float x, float y)
{
	vect2_t result = { x, y };
	return result;
}

float vect2_length(vect2_t v)
{
	//look it's the pythagorean theorem
	return sqrt(v.x * v.x + v.y * v.y);
}


vect2_t vect2_add(vect2_t a, vect2_t b)
{

	vect2_t result =
	{
		.x = a.x + b.x,
		.y = a.y + b.y
	};

	return result;
}


vect2_t vect2_sub(vect2_t a, vect2_t b)
{

	vect2_t result =
	{
		.x = a.x - b.x,
		.y = a.y - b.y
	};

	return result;
}

vect2_t vect2_mul(vect2_t a, float scale_factor)
{
	vect2_t result =
	{
		.x = a.x * scale_factor,
		.y = a.y * scale_factor
	};

	return result;
}

vect2_t vect2_div(vect2_t a, float scale_factor)
{
	vect2_t result =
	{
		.x = a.x / scale_factor,
		.y = a.y / scale_factor
	};

	return result;
}

void vect2_normalize(vect2_t *v)
{
	float length = sqrt(v->x * v->x + v->y * v->y);

	v->x /= length;
	v->y /= length;
}

float vect2_dot(vect2_t a, vect2_t b)
{
	return (a.x * b.x) + (a.y * b.y);
}


vect2_t vec2_from_vec4(vect4_t v)
{
	vect2_t result = { v.x, v.y };

	return result;
}



vect3_t vect3_add(vect3_t a, vect3_t b)
{

	vect3_t result =
	{
		.x = a.x + b.x,
		.y = a.y + b.y,
		.z = a.z + b.z
	};

	return result;
}



vect3_t vect3_sub(vect3_t a, vect3_t b)
{

	vect3_t result =
	{
		.x = a.x - b.x,
		.y = a.y - b.y,
		.z = a.z - b.z
	};

	return result;
}

vect3_t vect3_mul(vect3_t a, float scale_factor)
{
	vect3_t result =
	{
		.x = a.x * scale_factor,
		.y = a.y * scale_factor,
		.z = a.z * scale_factor
	};

	return result;
}

vect3_t vect3_div(vect3_t a, float scale_factor)
{
	vect3_t result =
	{
		.x = a.x / scale_factor,
		.y = a.y / scale_factor,
		.z = a.z / scale_factor
	};

	return result;
}

vect3_t vect3_cross(vect3_t a, vect3_t b)
{
	vect3_t result = {
		.x = a.y * b.z - a.z * b.y,
		.y = a.z * b.x - a.x * b.z,
		.z = a.x * b.y - a.y * b.x,
	};

	return result;
}

float vect3_dot(vect3_t a, vect3_t b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

void vect3_normalize(vect3_t *v)
{
	//I don't really like that we're not using the functions we already made just because we want to do pass by reference
	float length = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);

	v->x /= length;
	v->y /= length;
	v->z /= length;

}




vect3_t vect3_new(float x, float y, float z)
{
	vect3_t result = { x, y, z };
	return result;
}

float vect3_length(vect3_t v)
{
	//look it's the pythagorean theorem again
	return sqrt(v.x * v.x + v.y * v.y + v.z*v.z);
}



vect3_t vec3_rotate_x(vect3_t v, float angle)
{
	vect3_t rotated_vector =
	{
		.x = v.x,
		.y = v.y * cos(angle) - v.z * sin(angle),
		.z = v.y * sin(angle) + v.z * cos(angle)
	};

	return rotated_vector;
}

vect3_t vec3_rotate_y(vect3_t v, float angle)
{
	vect3_t rotated_vector =
	{
		.x = v.x * cos(angle) - v.z * sin(angle),
		.y = v.y,
		.z = v.x * sin(angle) + v.z * cos(angle)
	};

	return rotated_vector;
}


vect3_t vec3_rotate_z(vect3_t v, float angle)
{
	vect3_t rotated_vector =
	{
		.x = v.x * cos(angle) - v.y * sin(angle),
		.y = v.x * sin(angle) + v.y * cos(angle),
		.z = v.z
	};

	return rotated_vector;
}

vect3_t vec3_from_vec4(vect4_t v)
{
	vect3_t result = { v.x, v.y, v.z};

	return result;
}

vect3_t vec3_clone(vect3_t* v)
{
	vect3_t result = { v->x, v->y, v->z };
	return result;
}

vect4_t vec4_from_vec3(vect3_t v)
{
	vect4_t result = { v.x, v.y, v.z, 1.0 };

	return result;
}

int edge_cross(vect4_t* a, vect4_t* b, vect4_t* p)
{
	vect2_t ab = { b->x - a->x, b->y - a->y };
	vect2_t ap = { p->x - a->x, p->y - a->y };
	return ab.x * ap.y - ab.y * ap.x;
}



