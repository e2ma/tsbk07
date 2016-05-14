// Includes vary a bit with platforms.
// MS Windows needs GLEW or glee.
// Mac uses slightly different paths.
#define _LIB
#define FREEGLUT_LIB_PRAGMAS 0
#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GL/freeglut_ext.h"
#include <math.h>
#include "stdbool.h"
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include "MicroGLUT.h"
// A little linking trick for Lightweight IDE
#if 0
#import <Cocoa/Cocoa.h>
#endif
#endif

#include "GL_utilities.h"
#include "GL/loadobj.h"
#include "GL/loadobj.c"
#include "common/LoadTGA.h"
#include "common/LoadTGA.c"
#include "common/VectorUtils3.h"
#include "common/VectorUtils3.c"
//#include "Vertices.h"

// Globals
// Data would normally be read from files

//specular shading START

//Point3D lightSourcesColorsArr[] = { { 1.0f, 0.0f, 0.0f }, // Red light
//{ 0.0f, 1.0f, 0.0f }, // Green light
//{ 0.0f, 0.0f, 1.0f }, // Blue light
//{ 1.0f, 1.0f, 1.0f } }; // White light

Point3D lightSourcesColorsArr[] = { { 1.0f, 1.0f, 1.0f }, // Red light
{ 1.0f, 1.0f, 1.0f }, // Green light
{ 1.0f, 1.0f, 1.0f }, // Blue light
{ 1.0f, 1.0f, 1.0f } }; // White light

GLfloat specularExponent[] = { 10.0, 20.0, 60.0, 5.0 };
GLint isDirectional[] = { 1,1,1,1 };

Point3D lightSourcesDirectionsPositions[] = { { 10.0f, 5.0f, 0.0f }, // Red light, positional
{ 0.0f, 5.0f, 10.0f }, // Green light, positional
{ 0.0f, -1.0f, -1.0f }, // Blue light along X
{ -1.0f, 0.0f, -1.0f } }; // White light along Z

//specular shading END

#define near 1.0
#define far 300.0
#define right 1.0
#define left -1.0
#define top 1.0
#define bottom -1.0

mat4 projectionMatrix;

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


float randval(float min, float max)
{
	return (float)rand() / (float)RAND_MAX * (max - min) + min;

}


GLfloat* DiamondSquare(int width) {

	int vertexCount = width * width;
	int x, z;

	GLfloat *heightArray = malloc(sizeof(GLfloat) * vertexCount);

	for (x = 0; x < width; x++) {
		for (z = 0; z < width; z++) {
			heightArray[(x + z * width)] = 0;
		}
	}

	// Diamond Square algorithm

	//While the length of the side of the squares
	//	is greater than zero{
	//	Pass through the array and perform the diamond
	//	step for each square present.
	//  Reduce the random number range.
	//	Pass through the array and perform the square
	//	step for each diamond present.
	//	Reduce the random number range.
	//}

	// X to the right->
	// Z down
	//  A   .   G   .   B

	//	.   .   .   .   .

	//	F   .   E   .   H

	//	.   .   .   .   .

	//	C   .   I   .   D

	int squares_per_side = 1;
	int size = width; //square size (length of one side)
	int total_size = width; // terrain size (length of one side)
	GLfloat fval, gval, rand;// (x, y = [0 - 256]: 257 vertices)
	int a, b, c, d, e, f, g, hmax, imax;
	rand = 50.0;
	int num = 0; // nb of iterations
	int pos = 0;

	while (size > 2) {
		int x, z;

		// diamond step
		for (x = 0; x < squares_per_side; x++) {
			for (z = 0; z < squares_per_side; z++) {
				a = (x * (size - 1) + z * (size - 1) * total_size);
				b = a + (size - 1);
				c = a + (size - 1) * total_size;
				d = a + (size - 1) + (size - 1) * total_size;
				e = a + (size - 1) / 2 + ((size - 1) / 2) * total_size;

				float randvalue = randval(-rand, rand);
				heightArray[e] = (heightArray[a] + heightArray[b] + heightArray[c] + heightArray[d]) / 4 + randvalue;
			}
		}

		rand = rand / sqrt(2);

		// square step 
		for (x = 0; x < squares_per_side; x++) {
			for (z = 0; z < squares_per_side; z++) {

				a = (x * (size - 1) + z * (size - 1) * total_size);
				b = a + (size - 1);
				c = a + (size - 1) * total_size;
				d = a + (size - 1) + (size - 1) * total_size;
				e = a + (size - 1) / 2 + ((size - 1) / 2) * total_size;

				f = a + ((size - 1) / 2)  * total_size;
				g = a + (size - 1) / 2;

				if (x > 0) {
					pos = e - (size - 1);
					fval = (heightArray[a] + heightArray[e] + heightArray[c] + heightArray[pos]) / 4 + randval(-rand, rand);
				}
				else {

					pos = (((total_size - 1) - (size - 1) / 2) + (z * (size - 1) + (size - 1) / 2) * total_size);
					fval = (heightArray[a] + heightArray[e] + heightArray[c] + heightArray[pos]) / 4 + randval(-rand, rand);

					hmax = ((total_size - 1) + (z*(size - 1) + (size - 1) / 2) * total_size);
					heightArray[hmax] = fval;
				}

				if (z > 0) {
					pos = e - (size - 1) * total_size;
					gval = (heightArray[a] + heightArray[e] + heightArray[b] + heightArray[pos]) / 4 + randval(-rand, rand);
				}
				else {
					pos = (x*(size - 1) + (size - 1) / 2 + ((total_size - 1) - (size - 1) / 2) * total_size);
					gval = (heightArray[a] + heightArray[e] + heightArray[b] + heightArray[pos]) / 4 + randval(-rand, rand);

					imax = (x*(size - 1) + (size - 1) / 2 + (total_size - 1) * total_size);
					heightArray[imax] = gval;
				}

				heightArray[f] = fval;
				heightArray[g] = gval;

			}
		}

		size = (size / 2) + 1;
		squares_per_side *= 2;

		rand = rand / sqrt(2);
		num++;

	}
	printf("Iterations diamondSquare: %d\n", num);
	return heightArray;

}



Model* GenerateTerrain(int width, int bpp, bool diamondSquare)
{

	int vertexCount = width * width;
	printf("vertices: %d\n", vertexCount);
	int triangleCount = (width - 1) * (width - 1) * 2;
	int x, z;

	GLfloat* heightArray;
	GLfloat *vertexArray = malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *normalArray = malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *texCoordArray = malloc(sizeof(GLfloat) * 2 * vertexCount);
	GLuint *indexArray = malloc(sizeof(GLuint) * triangleCount * 3);

	vec3 temp1, temp2, temp3, res;

	if (diamondSquare) { heightArray = DiamondSquare(width); }

	printf("bpp %d\n", bpp);
	for (x = 0; x < width; x++)
		for (z = 0; z < width; z++)
		{
			//		// Vertex array. You need to scale this properly
			vertexArray[(x + z * width) * 3 + 0] = x / 1.0;
			if (diamondSquare) {
				vertexArray[(x + z * width) * 3 + 1] = 1 + heightArray[(x + z * width)];
			}
			else { vertexArray[(x + z * width) * 3 + 1] = 0; }
			vertexArray[(x + z * width) * 3 + 2] = z / 1.0;
		}

for (x = 0; x < width; x++)
	for (z = 0; z < width; z++){

			// Normal vectors. You need to calculate these.
			vec3 a, b, c, n;
			int ax, az, bx, bz, cx, cz;

			if (x == 0 || x == width - 1) {
				//	ax = width - 2;
				//	bx = 1;
				//	cx = width - 2;
				ax = 0;
				bx = 2;
				cx = 0;
		/*		ax = abs((x - 1) % (width - 1));
				bx = abs((x + 1) % (width -1));
				cx = abs((x - 1) % (width - 1));*/


			}
			else { ax = x - 1; bx = x + 1, cx = x - 1; }
			

			if (z == 0 || z == width - 1) {
			/*	az = width - 2;
				bz = z;
				cz = 1;*/
				az = 0;
				bz = 1;
				cz = 2;
				/*az = abs((z - 1) % (width - 1));
				bz = abs(z % (width-1));
				cz = abs((z + 1) % (width-1));*/
			}
			else { az = z - 1; bz = z, cz = z + 1; }

			a = SetVector(vertexArray[(ax + az * width) * 3 + 0], vertexArray[(ax + az * width) * 3 + 1], vertexArray[(ax + az * width) * 3 + 2]);
			b = SetVector(vertexArray[(bx + bz * width) * 3 + 0], vertexArray[(bx + bz * width) * 3 + 1], vertexArray[(bx + bz * width) * 3 + 2]);
			c = SetVector(vertexArray[(cx + cz * width) * 3 + 0], vertexArray[(cx + cz * width) * 3 + 1], vertexArray[(cx + cz * width) * 3 + 2]);

			n = CalcNormalVector(a, b, c);	

			normalArray[(x + z * width) * 3 + 0] = n.x;
			normalArray[(x + z * width) * 3 + 1] = n.y;
			normalArray[(x + z * width) * 3 + 2] = n.z;

			// Texture coordinates. You may want to scale them.

			texCoordArray[(x + z * width) * 2 + 0] = (float)x / (width / 60); //x
			texCoordArray[(x + z * width) * 2 + 1] = (float)z / (width / 60); //y
		}
	for (x = 0; x < width - 1; x++)
		for (z = 0; z < width - 1; z++)
		{
			// Triangle 1
			indexArray[(x + z * (width - 1)) * 6 + 0] = x + z * width;
			indexArray[(x + z * (width - 1)) * 6 + 1] = x + (z + 1) * width;
			indexArray[(x + z * (width - 1)) * 6 + 2] = x + 1 + z * width;
			// Triangle 2
			indexArray[(x + z * (width - 1)) * 6 + 3] = x + 1 + z * width;
			indexArray[(x + z * (width - 1)) * 6 + 4] = x + (z + 1) * width;
			indexArray[(x + z * (width - 1)) * 6 + 5] = x + 1 + (z + 1) * width;
		}

	// End of terrain generation

	// Create Model and upload to GPU:

	Model* model = LoadDataToModel(
		vertexArray,
		normalArray,
		texCoordArray,
		NULL,
		indexArray,
		vertexCount,
		triangleCount * 3);

	return model;
}
	

mat4 rot, trans, total;
GLfloat a, b, px, py, mx, my, siderotation, uprotation = 0.0;
int camstyle = 0;
vec3 p = { 0,10,20 };
vec3 l = { 0,1,0 };
vec3 v = { 0.0f, 1.0f, 0.0f }; //upvector 
vec3 p_ws;
vec3 p_ad;

// vertex array object
Model *m, *m2, *tm, *m_sphere, *m_bunny, *m_skybox, *water_terrain;

// Reference to shader program
GLuint program, skyboxshader;
GLuint tex1, tex2, sky_tex, water_tex;
int terrain_width = 257;


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

void OnTimer(int value)
{
	glutPostRedisplay();
	glutTimerFunc(20, &OnTimer, value);
}

void init(void)
{
	
	// GL inits
	glClearColor(0.2, 0.2, 0.5, 0);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	printError("GL inits");

	glDisable(GL_CULL_FACE);
	projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 500.0);

	// Load and compile shader
	program = loadShaders("terrain.vert", "terrain.frag");
	skyboxshader = loadShaders("skybox.vert", "skybox.frag");
	glUseProgram(program);
	printError("init shader");
	
	
	//vertex buffer objects, used for uploading vertices 

	dumpInfo();
	m_skybox = LoadModelPlus("skybox.obj");

	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex1);		// Bind Our Texture tex1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex2);		// Bind Our Texture tex2
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, water_tex); // Bind water_tex

	glUniform1i(glGetUniformLocation(program, "tex1"), 0); // Texture unit 0
	glUniform1i(glGetUniformLocation(program, "tex2"), 1); // Texture unit 1
	glUniform1i(glGetUniformLocation(program, "water_tex"), 2); // Texture unit 2
	LoadTGATextureSimple("grass.tga", &tex1);
	LoadTGATextureSimple("dirt.tga", &tex2);
	LoadTGATextureSimple("skybox512.tga", &sky_tex);
	LoadTGATextureSimple("water.tga", &water_tex);


	// Initialize terrain data
	tm = GenerateTerrain(terrain_width, 32, true);
	water_terrain = GenerateTerrain(terrain_width, 32, false);

	//printError("init terrain");

	// End of upload of geometry

	glutTimerFunc(20, &OnTimer, 0);

	printError("init arrays");
}


void display(void)
{
	mat4 total, modelView, camMatrix;

	printError("pre display");

	glUseProgram(program);


	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	GLfloat t = (GLfloat)glutGet(GLUT_ELAPSED_TIME) / 1000;

	GLfloat texflag = 0.0f;

	//MOVEMENT START
	if(camstyle == 0){
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
	p = SetVector(p.x, max(2,max(p.y, 2 + calcHeight(p.x, p.z, tm, terrain_width))), p.z); // emmas
	l = VectorAdd(p, p_ws);

	//printVec3(p_ws);
	}else {			//kameran g�r l�ngs texturen
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
		p = SetVector(p.x, max(0,calcHeight(p.x, p.z, tm, terrain_width)) + 10, p.z);
		l = VectorAdd(p, p_ws);

	}

	//MOVEMENT END		
	//SKYBOX START
	glUseProgram(skyboxshader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sky_tex);
	glDisable(GL_DEPTH_TEST);


	total = S(1.0f, 1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(skyboxshader, "mdlMatrix"), 1, GL_TRUE, total.m);

	mat4 lA = lookAtv(p, l, v);
	lA.m[3] = 0.0;
	lA.m[7] = 0.0;
	lA.m[11] = 0.0;
	glUniformMatrix4fv(glGetUniformLocation(skyboxshader, "lookAt"), 1, GL_TRUE, lA.m); //�r det fel h�r?


	texflag = 3;
	//glUniform1fv(glGetUniformLocation(skyboxshader, "texflag"), 1, &texflag);
	texflag = 0;
	glUniformMatrix4fv(glGetUniformLocation(skyboxshader, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	DrawModel(m_skybox, skyboxshader, "inPosition", NULL, "inTexCoord");

	glEnable(GL_DEPTH_TEST);
	//SKYBOX END	


	//specular shading start
	glUseProgram(program);
	glUniform3fv(glGetUniformLocation(program, "lightSourcesDirPosArr"), 4, &lightSourcesDirectionsPositions[0].x);
	glUniform3fv(glGetUniformLocation(program, "lightSourcesColorArr"), 4, &lightSourcesColorsArr[0].x);
	glUniform1fv(glGetUniformLocation(program, "specularExponent"), 4, specularExponent);
	glUniform1iv(glGetUniformLocation(program, "isDirectional"), 4, isDirectional);
	//specular shading end
	

	// Build matrix

	vec3 cam = { 0, 5, 8 };
	vec3 lookAtPoint = { 2, 0, 2 };
	camMatrix = lookAt(cam.x, cam.y, cam.z,
			   lookAtPoint.x, lookAtPoint.y, lookAtPoint.z,
						 0.0, 1.0, 0.0);
	modelView = IdentityMatrix();
	total = S(1.0f, 1.0f, 1.0f);

	printVec3(p);


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex1);		// Bind Our Texture tex1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex2);		// Bind Our Texture tex2


	float extx = p.x / terrain_width;
	float extz = p.z / terrain_width;

	int tempx = (extx > 0 ? (int)extx : -1 * ceil(fabs(extx)));
	int tempz = (extz > 0 ? (int)extz : -1 * ceil(fabs(extz)));

	//Draw 8 terrain patches around camera, 9 in total
	texflag = 2;
	glUniform1fv(glGetUniformLocation(program, "texflag"), 1, &texflag);
	for (auto xled = (tempx - 1); xled <= (tempx + 1); xled++) {
		for (auto zled = (tempz - 1); zled <= (tempz + 1); zled++) {

			int tx = xled * (terrain_width - 1);
			int tz = zled * (terrain_width - 1);

			float t1_shade, t2_shade, low, high;
			low = 20;
			high = 250;

			if (p.x <= low) { t2_shade = 0.0; }
			else if (p.x >= high) { t2_shade = 1.0; }
			else { t2_shade = (1.0 / (high - low)) * (p.x - low); }
			t1_shade = 1 - t2_shade;

			trans = T(tx, 0, tz);
			rot = Rx(0);
			total = Mult(trans, rot);
			total = Mult(total, S(1.0f, 1.0f, 1.0f));
		

			glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
			glUniformMatrix4fv(glGetUniformLocation(program, "lookAt"), 1, GL_TRUE, lookAtv(p, l, v).m);
			glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
			//glUniform1iv(glGetUniformLocation(program, "camPos"), 3, &p.x);

			DrawModel(tm, program, "inPosition", "inNormal", "inTexCoord");

		}
	}

	//Draw 8 water patches around camera, 9 in total
	texflag = 1;
	glUniform1fv(glGetUniformLocation(program, "texflag"), 1, &texflag);
	for (auto xled = (tempx - 1); xled <= (tempx + 1); xled++) {
		for (auto zled = (tempz - 1); zled <= (tempz + 1); zled++) {

			int tx = xled * (terrain_width - 1);
			int tz = zled * (terrain_width - 1);

			trans = T(tx, 0, tz);
			rot = Rx(0);
			total = Mult(trans, rot);
			total = Mult(total, S(1.0f, 1.0f, 1.0f));

			glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, Mult(T(0, sin(t), 0), total).m);
			glUniformMatrix4fv(glGetUniformLocation(program, "lookAt"), 1, GL_TRUE, lookAtv(p, l, v).m);
			glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
			//glUniform1iv(glGetUniformLocation(program, "camPos"), 3, &p.x);

			
			
			DrawModel(water_terrain, program, "inPosition", "inNormal", "inTexCoord");

		}
	}

	glDisable(GL_BLEND);
	


	printError("display 2");
	glFlush();
}

int main(int argc, char** argv)
{
	//seed the rand function with a unique number
	srand(time(NULL));

	glutInitWindowSize(1000, 600);
	glutInit(&argc, argv);
	glutCreateWindow("Lab 4!");

	glutDisplayFunc(&display);


	glutPassiveMotionFunc(mouse);
	glutKeyboardFunc(key);
	glutKeyboardUpFunc(antikey);

	if (GLEW_OK != glewInit())
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		printf("glewInit failed, aborting.\n");
		exit(1);
	}
	printf("Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	init();
	glutMainLoop();
}
