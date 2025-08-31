#ifndef CAMERA_H
#define CAMERA_H
#include "vector.h"


typedef struct
{
	vect3_t position;
	vect3_t direction;
	vect3_t forward_velocity;
	vect3_t side_velocity;
	float yaw;
	float pitch;

}camera_t;


extern camera_t camera;




#endif // !CAMERA_H
