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
Point3D lightSourcesColorsArr[] = { { 1.0f, 1.0f, 1.0f }, // Red light
{ 1.0f, 1.0f, 1.0f }, // Green light
{ 1.0f, 1.0f, 1.0f }, // Blue light
{ 1.0f, 1.0f, 1.0f } }; // White light

GLfloat specularExponent[] = { 10.0, 20.0, 60.0, 5.0 };
GLint isDirectional[] = { 0,0,1,1 };

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

	//Modulusoperation för att translatera positioner utanför första patchen
	tempx = (tempx % (tex_width - 1)) - (tempx % 1);
	tempz = (tempz % (tex_width - 1)) - (tempz % 1);

	vec3 p1, p2, p3;
	float d = 0.0;
	vec3 nvec;

	//Kan krasha för att 257 vs 256 inte fungerar som det ska?

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
	//return (float)rand() / (float)RAND_MAX * (2*max - min) + min;
	//return (float)rand() / (float)RAND_MAX * (max -0) + 0;
}


GLfloat* DiamondSquare(int width, int bpp) {

	int height = width;
	int vertexCount = width * height;
	printf("vertices: %d\nwidth: %d\n", vertexCount, width);// (256*256) -> changed to 257*257. imageData not valid anymore
	int triangleCount = (width - 1) * (height - 1) * 2;
	int x, z;

	GLfloat *heightArray = malloc(sizeof(GLfloat) * vertexCount);

	for (x = 0; x < width; x++) {
		for (z = 0; z < height; z++) {
			heightArray[(x + z * width)] = 0;
		}
	}

	// Diamond Square algorithm

	//While the length of the side of the squares
	//	is greater than zero{
	//	Pass through the array and perform the diamond
	//	step for each square present.
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
	int size = height; //square size (length of one side)
	int total_size = height; // terrain size (length of one side)
	GLfloat fval, gval, rand;// (x, y = [0 - 256]: 257 vertices)
	int a, b, c, d, e, f, g, hmax, imax;
	rand = 60.0;
	int num = 0; // nb of iterations
	int pos = 0;

	while (size > 0) {
		int x, z;

		// diamond step
		for (x = 0; x < squares_per_side; x++) {
			for (z = 0; z < squares_per_side; z++) {
				//		if (num < 2) {
				a = (x * (size - 1) + z * (size - 1) * total_size);
				b = a + (size - 1);
				c = a + (size - 1) * total_size;
				d = a + (size - 1) + (size - 1) * total_size;
				e = a + (size - 1) / 2 + ((size - 1) / 2) * total_size;

				float randvalue = randval(-rand, rand);
				//	if (num == 0) { randvalue = abs(randvalue); } // uncomment to make the first e-value positive 
				heightArray[e] = (heightArray[a] + heightArray[b] + heightArray[c] + heightArray[d]) / 4 + randvalue;// randval(-rand, rand);

																													 //		}
			}
		}

		// square step 
		rand = rand / sqrt(2);
		for (x = 0; x < squares_per_side; x++) {
			for (z = 0; z < squares_per_side; z++) {
				//if (num < 10) {

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
					// edges != 0
					pos = (((total_size - 1) - (size - 1) / 2) + (z * (size - 1) + (size - 1) / 2) * total_size);
					fval = (heightArray[a] + heightArray[e] + heightArray[c] + heightArray[pos]) / 4 + randval(-rand, rand);

					// edges = 0
					//fval = 0;

					hmax = ((total_size - 1) + (z*(size - 1) + (size - 1) / 2) * total_size);
					heightArray[hmax] = fval;
				}

				if (z > 0) {
					pos = e - (size - 1) * total_size;
					gval = (heightArray[a] + heightArray[e] + heightArray[b] + heightArray[pos]) / 4 + randval(-rand, rand);
				}
				else {
					// edges != 0
					pos = (x*(size - 1) + (size - 1) / 2 + ((total_size - 1) - (size - 1) / 2) * total_size);
					gval = (heightArray[a] + heightArray[e] + heightArray[b] + heightArray[pos]) / 4 + randval(-rand, rand);

					// edges = 0
					//	gval = 0;

					imax = (x*(size - 1) + (size - 1) / 2 + (total_size - 1) * total_size);
					heightArray[imax] = gval;
				}

				heightArray[f] = fval;
				heightArray[g] = gval;
				//	heightArray[(0 * (size - 1) + ((z * (size - 1) + (size - 1) / 2) * total_size))] = 0; // set f=0 again. why otherwize not 0??  // edges = 0



				//		}
			}
		}

		if (size == 2) break;
		size = (size / 2) + 1;
		squares_per_side *= 2;

		rand = rand / sqrt(2);
		num++;

	}
	printf("Iterations diamondSquare: %d\n", num);

	GLfloat h1, h2;
	for (x = 0; x < width; x++) {
		for (z = 0; z < height; z++) {
			h1 = (heightArray[(total_size - 1 + z * width)] + heightArray[(0 + z * width)]) / 2;
			h2 = (heightArray[(x + (total_size - 1) * width)] + heightArray[(x + 0 * width)]) / 2;

			heightArray[(0 + z * width)] = h1;
			heightArray[(x + 0 * width)] = h2;
			heightArray[((total_size - 1) + z * width)] = h1;
			heightArray[(x + (total_size - 1) * width)] = h2;
		}
	}

	return heightArray;

}



Model* GenerateTerrain(int width, int bpp, bool diamondSquare)
{

	int vertexCount = width * width;
	printf("vertices: %d\n", vertexCount);// (256*256) -> should be 257*257 -> ok! changed it :) 
	int triangleCount = (width - 1) * (width - 1) * 2;
	int x, z;

	GLfloat* heightArray;
	GLfloat *vertexArray = malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *normalArray = malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *texCoordArray = malloc(sizeof(GLfloat) * 2 * vertexCount);
	GLuint *indexArray = malloc(sizeof(GLuint) * triangleCount * 3);

	vec3 temp1, temp2, temp3, res;

	if (diamondSquare) { heightArray = DiamondSquare(width, width, bpp); }

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



			// Normal vectors. You need to calculate these.
			//temp1 = SetVector(x / 1.0, vertexArray[(x + z * width) * 3 + 0], (z + 1) / 1.0); //upp
			//temp2 = SetVector((x + 1) / 1.0, vertexArray[(x + z * width) * 3 + 1], z / 1.0); //sidan
			//temp3 = SetVector((x - 1) / 1.0, vertexArray[(x + z * width) * 3 + 2], (z - 1) / 1.0); //ner + bakom

			////res = CrossProduct(VectorSub(temp3, temp2), VectorSub(temp3, temp1)); //ner + bakom
			//res = CalcNormalVector(temp3, temp2, temp1);

			////if (x*z != 0 || x == width-1 || x == width-1) {	//om noden inte är på "kanten"
			//normalArray[(x + z * width) * 3 + 0] = res.x;
			//normalArray[(x + z * width) * 3 + 1] = res.y;
			//normalArray[(x + z * width) * 3 + 2] = res.z;
			////}
			vec3 a, b, c, n;

			if (x > 1 && z > 1 && z < width - 1 && x < width - 1) {

				// obs krav x>= 1    // setVector !!!!! 
				a = SetVector(vertexArray[((x - 1) + (z - 1) * width) * 3 + 0], vertexArray[((x - 1) + (z - 1) * width) * 3 + 1], vertexArray[((x - 1) + (z - 1) * width) * 3 + 2]);
				b = SetVector(vertexArray[((x + 1) + (z)* width) * 3 + 0], vertexArray[((x + 1) + (z)* width) * 3 + 1], vertexArray[((x + 1) + (z)* width) * 3 + 2]);
				c = SetVector(vertexArray[((x - 1) + (z + 1) * width) * 3 + 0], vertexArray[((x - 1) + (z + 1) * width) * 3 + 1], vertexArray[((x - 1) + (z + 1) * width) * 3 + 2]);


				// 3 hörnpunkter, hämta från varray 

				n = CalcNormalVector(a, b, c);
				normalArray[(x + z * width) * 3 + 0] = n.x;
				normalArray[(x + z * width) * 3 + 1] = n.y;
				normalArray[(x + z * width) * 3 + 2] = n.z;

			}
			/*	else if (x == 0) {
					a = SetVector(vertexArray[((x - 1) + (z - 1) * width) * 3 + 0], vertexArray[((x - 1) + (z - 1) * width) * 3 + 1], vertexArray[((x - 1) + (z - 1) * width) * 3 + 2]);
					b = SetVector(vertexArray[((x + 1) + (z)* width) * 3 + 0], vertexArray[((x + 1) + (z)* width) * 3 + 1], vertexArray[((x + 1) + (z)* width) * 3 + 2]);
					c = SetVector(vertexArray[((x - 1) + (z + 1) * width) * 3 + 0], vertexArray[((x - 1) + (z + 1) * width) * 3 + 1], vertexArray[((x - 1) + (z + 1) * width) * 3 + 2]);

				}*/

			else {
				normalArray[(x + z * width) * 3 + 0] = 0.0;
				normalArray[(x + z * width) * 3 + 1] = 1.0;
				normalArray[(x + z * width) * 3 + 2] = 0.0;
			}


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
vec3 p = { 40,40,40 };
vec3 l = { 0,1,0 };
vec3 v = { 0.0f, 1.0f, 0.0f }; //uppvektor 
vec3 p_ws;
vec3 p_ad;



// vertex array object
Model *m, *m2, *tm, *m_sphere, *m_bunny, *m_skybox, *water_terrain, *m_tree;
// Reference to shader program
GLuint program, skyboxshader  /*, objectshader*/;
GLuint tex1, dirt_tex, sky_tex, water_tex, plant_tex;
int terrain_width = 257;



// vertex array object
//unsigned int vertexArrayObjID;
unsigned int bunnyVertexArrayObjID;


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
	//objectshader = loadShaders("terrain.vert", "object_shader.frag");
	glUseProgram(program);
	printError("init shader");


	//vertex buffer objects, used for uploading vertices 

	dumpInfo();
	m_sphere = LoadModelPlus("groundsphere.obj");
	m_tree = LoadModelPlus("plant_ball.obj");			//plant win has tga texture
	//m_bunny = LoadModelPlus("bunnyplus.obj");
	m_skybox = LoadModelPlus("skybox.obj");

	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex1);		// Bind Our Texture tex1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, dirt_tex);		// Bind Our Texture dirt
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, water_tex); // Bind water_tex

	glUniform1i(glGetUniformLocation(program, "tex1"), 0); // Texture unit 0
	glUniform1i(glGetUniformLocation(program, "dirt_tex"), 1); // Texture unit 1
	glUniform1i(glGetUniformLocation(program, "water_tex"), 2); // Texture unit 2
	glUniform1i(glGetUniformLocation(program, "plant_tex"), 3); // Texture unit 3
	LoadTGATextureSimple("grass.tga", &tex1);
	LoadTGATextureSimple("dirt.tga", &dirt_tex);
	LoadTGATextureSimple("skybox512.tga", &sky_tex);
	LoadTGATextureSimple("water.tga", &water_tex);
	LoadTGATextureSimple("plant_win.tga", &plant_tex);


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
		l = VectorAdd(p, p_ws);

		//printVec3(p_ws);
	}
	else {			//kameran går längs texturen
	   /*
	   l måste kopplas loss från p_ws när det sker i/k rotation. I det läget ska p_ws inte förändras
	   p_ws ska ändras vid q/e rotation, samt translation, inte vid i/k

	   nvm ovan: ny idé: CS, frammåt vektor som alltid är RAKT fram
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
		p = SetVector(p.x, calcHeight(p.x, p.z, tm, terrain_width) + 20, p.z);
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
	glUniformMatrix4fv(glGetUniformLocation(skyboxshader, "lookAt"), 1, GL_TRUE, lA.m);


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
	//glBlendFunc(GL_ONE, GL_ONE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex1);		// Bind Our Texture tex1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, dirt_tex);		// Bind Our Texture tex2
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, water_tex); // Bind water_tex
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, plant_tex); // Bind water_tex

	float extx = p.x / terrain_width;
	float extz = p.z / terrain_width;

	int tempx = (extx > 0 ? (int)extx : -1 * ceil(fabs(extx)));
	int tempz = (extz > 0 ? (int)extz : -1 * ceil(fabs(extz)));

	//Draw 8 terrain patches around camera, 9 in total
	for (auto xled = (tempx - 1); xled <= (tempx + 1); xled++) {
		for (auto zled = (tempz - 1); zled <= (tempz + 1); zled++) {
			//if (init_xled != xled) { tm = GenerateTerrain(257, 32, true); }
			int tx = xled * (terrain_width - 1);
			int tz = zled * (terrain_width - 1);

			trans = T(tx, 0, tz);
			rot = Rx(0);
			total = Mult(trans, rot);
			total = Mult(total, S(1.0f, 1.0f, 1.0f));

			glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
			glUniformMatrix4fv(glGetUniformLocation(program, "lookAt"), 1, GL_TRUE, lookAtv(p, l, v).m);
			glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
			//glUniform1iv(glGetUniformLocation(program, "camPos"), 3, &p.x);


			//	glBindTexture(GL_TEXTURE_2D, tex1);		// Bind Our Texture tex1
			texflag = 2;
			glUniform1fv(glGetUniformLocation(program, "texflag"), 1, &texflag);
			DrawModel(tm, program, "inPosition", "inNormal", "inTexCoord");


			glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, Mult(T(0, -1, 0), total).m);

			texflag = 1;
			glUniform1fv(glGetUniformLocation(program, "texflag"), 1, &texflag);
			DrawModel(water_terrain, program, "inPosition", "inNormal", "inTexCoord");


			for (auto nr_trees_x = 0; nr_trees_x < 3; nr_trees_x++) {
				for (auto nr_trees_z = 0; nr_trees_z < 3; nr_trees_z++) {

					float tex_height = calcHeight(50 * nr_trees_x + 50, 50 * nr_trees_x + 50, tm, terrain_width);
					if (tex_height > 0) {
						trans = T(50 * nr_trees_x + 50, tex_height, 50 * nr_trees_x + 50);
						rot = Rx(0);
						total = Mult(trans, rot);
						total = Mult(total, S(1.0f, 1.0f, 1.0f));

						glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
						texflag = 3;
						glUniform1fv(glGetUniformLocation(program, "texflag"), 1, &texflag);
						DrawModel(m_tree, program, "inPosition", "inNormal", "inTexCoord");
					}
				}
			}
			
		}
	}


	//boll
	trans = T((20.0*sin(t / 10.0)), calcHeight((20.0*sin(t / 10.0)), (20.0*cos(t / 10.0)), tm, terrain_width), (20.0*cos(t / 10.0)));
	rot = Rx(0);
	total = Mult(trans, rot);
	total = Mult(total, S(1.0f, 1.0f, 1.0f));

	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "lookAt"), 1, GL_TRUE, lookAtv(p, l, v).m);
	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	texflag = 0;
	glUniform1fv(glGetUniformLocation(program, "texflag"), 1, &texflag);
	DrawModel(m_sphere, program, "inPosition", "inNormal", "inTexCoord");


	//bunny
	//glfloat bx, bz;
	//bx = (10 * sin(t / 5));
	//bz = (10 * cos(t / 5));

	//trans = t(100 + bx, calcheight(100 + bx, 100 + bz, tm, ttex.width), 100 + bz);

	//vec3 nrml = setvector(tm->normalarray[((int)(100 + bx) + (int)(100 + bz) * ttex.width) * 3 + 0], 
	//					  tm->normalarray[((int)(100 + bx) + (int)(100 + bz) * ttex.width) * 3 + 1],
	//					  tm->normalarray[((int)(100 + bx) + (int)(100 + bz) * ttex.width) * 3 + 2]);
	//
	//slope dependent rotation
	//antar (1,0,b) , (0, 2, c) -> theta = arctan(b/1), theta = arctan(c/2)
	//2*pi = 360 grader -> 1 grad = pi/180
	// theta = (pi/180) * arctan(a/x), a är komponent i planer, x är längd på komponent??
	// x = 1??

	//testa idé med normal = (1, 2, 3) -> rip?

	//vec3 rot_axis = crossproduct(nrml, v);

	//rot = arbrotate(rot_axis, 0);
	//total = mult(trans, rot);
	//total = mult(total, s(1.0f, 1.0f, 1.0f));

	//gluniformmatrix4fv(glgetuniformlocation(program, "mdlmatrix"), 1, gl_true, total.m);
	//drawmodel(m_bunny, program, "inposition", "innormal", "intexcoord");


	printError("display 2");
	glFlush();
}

int main(int argc, char** argv)
{
	//seed the rand function with a unique number
	srand(time(NULL));

	glutInitWindowSize(600, 600);
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
