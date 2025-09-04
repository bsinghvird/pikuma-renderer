#ifndef CLIPPING_H
#define CLIPPING_H

#include "vector.h"

enum
{
	LEFT_FRUSTUM_PLANE,
	RIGHT_FRUSTUM_PLANE,
	TOP_FRUSTUM_PLANE,
	BOTTOM_FRUSTUM_PLANE,
	NEAR_FRUSTUM_PLANE,
	FAR_FRUSTUM_PLANE,
};

typedef struct
{
	vect3_t point;
	vect3_t normal;
}plane_t;


void init_frustum_planes(float fov, float z_near, float z_far);

#endif // !CLIPPING_H
