#ifndef TERRAIN_H
#define TERRAIN_H

	#include "drawworld.h"
	float randval(float min, float max);
	Model* GenerateTerrain(int width, int bpp, bool diamondSquare);
	GLfloat* DiamondSquare(int width);
#endif