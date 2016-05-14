#include "camera.h"

void movement() {
	//MOVEMENT START
	if (camstyle == 0) {
		p_ws = VectorSub(l, p);
		p_ad = CrossProduct(p_ws, v);
		p_ad.y = 0;

		trans = T(p.x, p.y, p.z);		//fixa translationen typ?
		rot = Ry((siderotation + mx) / 50);	//rotation av l med q/e!!!
		total = Mult(trans, rot);
		l = MultVec3(total, p_ws);
		p_ws = VectorSub(l, p);


		trans = T(p.x, p.y, p.z);
		rot = ArbRotate(p_ad, (uprotation + my) / 25);  //Ry(siderotation / 10);				//rotation av l med i/k
		total = Mult(trans, rot);
		l = MultVec3(total, p_ws);
		v = Normalize(CrossProduct(p_ad, p_ws));
		p_ws = VectorSub(l, p);

		p = VectorAdd(p, VectorAdd(ScalarMult(p_ws, a / 100), ScalarMult(p_ad, b / 100)));
		p = SetVector(p.x, max(2, max(p.y, 2 + calcHeight(p.x, p.z, tm, terrain_width))), p.z); // emmas
		l = VectorAdd(p, p_ws);

		//printVec3(p_ws);
	}
	else {			//kameran g�r l�ngs texturen
					/*
					l m�ste kopplas loss fr�n p_ws n�r det sker i/k rotation. I det l�get ska p_ws inte f�r�ndras
					p_ws ska �ndras vid q/e rotation, samt translation, inte vid i/k

					nvm ovan: ny id�: CS, framm�t vektor som alltid �r RAKT fram
					*/

		p_ws = VectorSub(l, p);
		p_ad = CrossProduct(p_ws, v);
		p_ad.y = 0;
		vec3 forw = ScalarMult(SetVector(-1.0*p_ad.z, 0, p_ad.x), -1.0); //cs!!!! :D

		trans = T(p.x, p.y, p.z);		//fixa translationen typ?
		rot = Ry((siderotation + mx) / 50);	//rotation av l med q/e!!!
		total = Mult(trans, rot);
		l = MultVec3(total, p_ws);
		p_ws = VectorSub(l, p);

		trans = T(p.x, p.y, p.z);
		rot = ArbRotate(p_ad, (uprotation + my) / 25);  //rotation av l med i/k
		total = Mult(trans, rot);
		l = MultVec3(total, p_ws);
		v = Normalize(CrossProduct(p_ad, p_ws));
		p_ws = VectorSub(l, p);


		p = VectorAdd(p, VectorAdd(ScalarMult(forw, a / 100), ScalarMult(p_ad, b / 100)));
		p = SetVector(p.x, max(0, calcHeight(p.x, p.z, tm, terrain_width)) + 10, p.z);
		l = VectorAdd(p, p_ws);
	}
	//MOVEMENT END	
}


float calcHeight(float x, float z, Model *tex, int tex_width) {
	int tempx = 0, tempz = 0;

	//For negative numbers
	if (x < 0) {
		float temp = (x / (tex_width - 1));
		float adwwad = fmod(temp, 1);
		x = (tex_width - 1) + fmod(temp, 1)*(tex_width - 1);
	}
	if (z < 0) {
		float temp = (z / (tex_width - 1));
		z = (tex_width - 1) + fmod(temp, 1)*(tex_width - 1);
	}

	tempx = floor(x);
	tempz = floor(z);

	if (floorf(x) == x && floorf(z) == z) {
		return tex->vertexArray[(tempx + tempz * tex_width) * 3 + 1];	//if camera is on an integer position
	}

	//Modulusoperation f�r att translatera positioner utanf�r f�rsta patchen
	tempx = (tempx % (tex_width - 1)) - (tempx % 1);
	tempz = (tempz % (tex_width - 1)) - (tempz % 1);

	vec3 p1, p2, p3;
	float d = 0.0;
	vec3 nvec;

	//Kan krasha f�r att 257 vs 256 inte fungerar som det ska?

	p1 = SetVector(tex->vertexArray[((tempx + 0) + (tempz + 1) * tex_width) * 3 + 0],
		tex->vertexArray[((tempx + 0) + (tempz + 1) * tex_width) * 3 + 1],
		tex->vertexArray[((tempx + 0) + (tempz + 1) * tex_width) * 3 + 2]);

	p3 = SetVector(tex->vertexArray[((tempx + 1) + (tempz + 0) * tex_width) * 3 + 0],
		tex->vertexArray[((tempx + 1) + (tempz + 0) * tex_width) * 3 + 1],
		tex->vertexArray[((tempx + 1) + (tempz + 0) * tex_width) * 3 + 2]);

	float asd = ((tempx + fmod(x, 1)) - tempx);
	float qeqwe = ((tempz + fmod(z, 1)) - tempz);

	if (((tempx + fmod(x, 1)) - tempx) + ((tempz + fmod(z, 1)) - tempz) > 1) {

		p2 = SetVector(tex->vertexArray[((tempx + 1) + (tempz + 1) * tex_width) * 3 + 0],
			tex->vertexArray[((tempx + 1) + (tempz + 1) * tex_width) * 3 + 1],
			tex->vertexArray[((tempx + 1) + (tempz + 1) * tex_width) * 3 + 2]);

		//res = CrossProduct(VectorSub(temp3, temp2), VectorSub(temp3, temp1));

		nvec = CalcNormalVector(p2, p1, p3);

		d = nvec.x * tex->vertexArray[((tempx + 1) + (tempz + 1) * tex_width) * 3 + 0] +
			nvec.y * tex->vertexArray[((tempx + 1) + (tempz + 1) * tex_width) * 3 + 1] +
			nvec.z * tex->vertexArray[((tempx + 1) + (tempz + 1) * tex_width) * 3 + 2];

		return ((d - nvec.x * (tempx + fmod(x, 1)) - nvec.z * (tempz + fmod(z, 1))) / nvec.y);

	}
	else {

		p2 = SetVector(tex->vertexArray[((tempx + 0) + (tempz + 0) * tex_width) * 3 + 0],
			tex->vertexArray[((tempx + 0) + (tempz + 0) * tex_width) * 3 + 1],
			tex->vertexArray[((tempx + 0) + (tempz + 0) * tex_width) * 3 + 2]);

		nvec = CalcNormalVector(p2, p1, p3);

		d = nvec.x * tex->vertexArray[((tempx + 0) + (tempz + 0) * tex_width) * 3 + 0] +
			nvec.y * tex->vertexArray[((tempx + 0) + (tempz + 0) * tex_width) * 3 + 1] +
			nvec.z * tex->vertexArray[((tempx + 0) + (tempz + 0) * tex_width) * 3 + 2];

		return ((d - nvec.x * (tempx + fmod(x, 1)) - nvec.z * (tempz + fmod(z, 1))) / nvec.y);
	}
}


void mouse(int x, int y)
{
	//glutWarpPointer(300,300);
	//mx = x * 0.001;
	//my = y * 0.001;
	glutPostRedisplay();
}

void key(char c, int x, int y)
{
	//printf("hit a key\n");
	switch (c)
	{
	case 'A':
	case 'a': b = -6.0; break;
	case 'D':
	case 'd': b = 6.0; break;
	case 'W':
	case 'w': a = 6.0; break;
	case 'S':
	case 's': a = -6.0; break;
	case 'Q':
	case 'q': siderotation = 2.0; break;
	case 'E':
	case 'e': siderotation = -2.0; break;
	case 'I':
	case 'i': uprotation = 1.0; break;
	case 'K':
	case 'k': uprotation = -1.0; break;
	case 'C':
	case 'c': camstyle = 1; break;
	case 'V':
	case 'v': camstyle = 0; break;

	}
	glutPostRedisplay();
}

void antikey(char c, int x, int y)
{
	//printf("hit a key\n");
	switch (c)
	{
	case 'A':
	case 'a': b = 0.0; break;
	case 'D':
	case 'd': b = 0.0; break;
	case 'W':
	case 'w': a = 0.0; break;
	case 'S':
	case 's': a = 0.0; break;
	case 'Q':
	case 'q': siderotation = 0.0; break;
	case 'E':
	case 'e': siderotation = 0.0; break;
	case 'I':
	case 'i': uprotation = 0.0; break;
	case 'K':
	case 'k': uprotation = 0.0; break;

	}
	glutPostRedisplay();
}