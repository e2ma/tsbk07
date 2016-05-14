#ifndef CAMERA_H
#define CAMERA_H

	#include "drawworld.h"
	
	extern GLfloat a, b, siderotation, uprotation;
	extern GLfloat mx, my;
	extern int camstyle, terrain_width;
	extern vec3 p_ws, p_ad, p, v, l;
	extern mat4 trans, total, rot;
	extern Model* tm;


	void movement();
	float calcHeight(float x, float z, Model *tex, int tex_width);
	void mouse(int x, int y);
	void key(char c, int x, int y);
	void antikey(char c, int x, int y);

#endif