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

Point3D lightSourcesColorsArr[] = { { 1.0f, 0.0f, 0.0f }, // Red light
{ 0.0f, 1.0f, 0.0f }, // Green light
{ 0.0f, 0.0f, 1.0f }, // Blue light
{ 1.0f, 1.0f, 1.0f } }; // White light

GLfloat specularExponent[] = { 10.0, 20.0, 60.0, 5.0 };
GLint isDirectional[] = { 0,0,1,1 };

Point3D lightSourcesDirectionsPositions[] = { { 10.0f, 5.0f, 0.0f }, // Red light, positional
{ 0.0f, 5.0f, 10.0f }, // Green light, positional
{ -1.0f, 0.0f, 0.0f }, // Blue light along X
{ 0.0f, 0.0f, -1.0f } }; // White light along Z

//specular shading END


#define near 1.0
#define far 300.0
#define right 1.0
#define left -1.0
#define top 1.0
#define bottom -1.0

//GLfloat projectionMatrix[] =
//{ 2.0f*near / (right - left), 0.0f, (right + left) / (right - left), 0.0f,
//0.0f, 2.0f*near / (top - bottom), (top + bottom) / (top - bottom), 0.0f,
//0.0f, 0.0f, -(far + near) / (far - near), -2 * far*near / (far - near),
//0.0f, 0.0f, -1.0f, 0.0f };

mat4 projectionMatrix;

float calcHeight(float x, float z, Model *tex, int tex_width) {
	int tempx = 0, tempz = 0, impx, impz;
	tempx = floor(x);
	tempz = floor(z);

	if(false){

	}else if (fabs(x) >= tex_width - 1 || fabs(z) >= tex_width - 1) {
		return 0;
	}else if (floorf(x) == x && floorf(z) == z){
		return tex->vertexArray[(tempx + tempz * tex_width) * 3 + 1];
	}else{
		vec3 p1, p2, p3;
		float d = 0.0;
		vec3 nvec;

		p1 = SetVector(tex->vertexArray[((tempx + 0) + (tempz + 1) * tex_width) * 3 + 0], 
					   tex->vertexArray[((tempx + 0) + (tempz + 1) * tex_width) * 3 + 1], 
					   tex->vertexArray[((tempx + 0) + (tempz + 1) * tex_width) * 3 + 2]);

		p3 = SetVector(tex->vertexArray[((tempx + 1) + (tempz + 0) * tex_width) * 3 + 0],
					   tex->vertexArray[((tempx + 1) + (tempz + 0) * tex_width) * 3 + 1],
					   tex->vertexArray[((tempx + 1) + (tempz + 0) * tex_width) * 3 + 2]);

		if ( (x - tempx) + (z - tempz) > 1) {

			p2 = SetVector(tex->vertexArray[((tempx + 1) + (tempz + 1) * tex_width) * 3 + 0],
						   tex->vertexArray[((tempx + 1) + (tempz + 1) * tex_width) * 3 + 1],
						   tex->vertexArray[((tempx + 1) + (tempz + 1) * tex_width) * 3 + 2]);

			//res = CrossProduct(VectorSub(temp3, temp2), VectorSub(temp3, temp1));

			nvec = CalcNormalVector(p2, p1, p3);

			d = nvec.x * tex->vertexArray[((tempx + 1) + (tempz + 1) * tex_width) * 3 + 0] +
				nvec.y * tex->vertexArray[((tempx + 1) + (tempz + 1) * tex_width) * 3 + 1] +
				nvec.z * tex->vertexArray[((tempx + 1) + (tempz + 1) * tex_width) * 3 + 2];

			return ((d - nvec.x * x - nvec.z * z) / nvec.y);
			
		}else{ 

			p2 = SetVector(tex->vertexArray[((tempx + 0) + (tempz + 0) * tex_width) * 3 + 0],
				           tex->vertexArray[((tempx + 0) + (tempz + 0) * tex_width) * 3 + 1],
				           tex->vertexArray[((tempx + 0) + (tempz + 0) * tex_width) * 3 + 2]);

			nvec = CalcNormalVector(p2, p1, p3); 

			d = nvec.x * tex->vertexArray[((tempx + 0) + (tempz + 0) * tex_width) * 3 + 0] +
				nvec.y * tex->vertexArray[((tempx + 0) + (tempz + 0) * tex_width) * 3 + 1] +
				nvec.z * tex->vertexArray[((tempx + 0) + (tempz + 0) * tex_width) * 3 + 2];

			return ((d - nvec.x * x - nvec.z * z) / nvec.y);
		}
		
		

	}

}


Model* GenerateTerrain(TextureData *tex)
{
	int vertexCount = tex->width * tex->height;
	int triangleCount = (tex->width - 1) * (tex->height - 1) * 2;
	int x, z;

	GLfloat *vertexArray = malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *normalArray = malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *texCoordArray = malloc(sizeof(GLfloat) * 2 * vertexCount);
	GLuint *indexArray = malloc(sizeof(GLuint) * triangleCount * 3);

	vec3 temp1, temp2, temp3, res;
	

	printf("bpp %d\n", tex->bpp);
	for (x = 0; x < tex->width; x++)
		for (z = 0; z < tex->height; z++)
		{
			// Vertex array. You need to scale this properly
			vertexArray[(x + z * tex->width) * 3 + 0] = x / 1.0;
			vertexArray[(x + z * tex->width) * 3 + 1] = tex->imageData[(x + z * tex->width) * (tex->bpp / 8)] / 10.0;
			vertexArray[(x + z * tex->width) * 3 + 2] = z / 1.0;
			
			

			// Normal vectors. You need to calculate these.
			temp1 = SetVector(x / 1.0, vertexArray[(x + z * tex->width) * 3 + 0], (z + 1)/1.0); //upp
			temp2 = SetVector((x + 1) / 1.0, vertexArray[(x + z * tex->width) * 3 + 1], z / 1.0); //sidan
			temp3 = SetVector((x - 1) / 1.0, vertexArray[(x + z * tex->width) * 3 + 2], (z - 1)/ 1.0); //ner + bakom

			//res = CrossProduct(VectorSub(temp3, temp2), VectorSub(temp3, temp1)); //ner + bakom
			res = CalcNormalVector(temp3 ,temp2 ,temp1);

			//if (x*z != 0 || x == tex->width-1 || x == tex->height-1) {	//om noden inte är på "kanten"
				normalArray[(x + z * tex->width) * 3 + 0] = res.x;
				normalArray[(x + z * tex->width) * 3 + 1] = res.y;
				normalArray[(x + z * tex->width) * 3 + 2] = res.z;

			//}


			// Texture coordinates. You may want to scale them.
			
			texCoordArray[(x + z * tex->width) * 2 + 0] = x; // (float)x / tex->width;
			texCoordArray[(x + z * tex->width) * 2 + 1] = z; // (float)z / tex->height;
		}
	for (x = 0; x < tex->width - 1; x++)
		for (z = 0; z < tex->height - 1; z++)
		{
			// Triangle 1
			indexArray[(x + z * (tex->width - 1)) * 6 + 0] = x + z * tex->width;
			indexArray[(x + z * (tex->width - 1)) * 6 + 1] = x + (z + 1) * tex->width;
			indexArray[(x + z * (tex->width - 1)) * 6 + 2] = x + 1 + z * tex->width;
			// Triangle 2
			indexArray[(x + z * (tex->width - 1)) * 6 + 3] = x + 1 + z * tex->width;
			indexArray[(x + z * (tex->width - 1)) * 6 + 4] = x + (z + 1) * tex->width;
			indexArray[(x + z * (tex->width - 1)) * 6 + 5] = x + 1 + (z + 1) * tex->width;
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
vec3 v = { 0.0f, 1.0f, 0.0f }; //uppvektor 
vec3 p_ws;
vec3 p_ad;



// vertex array object
Model *m, *m2, *tm, *m_sphere, *m_bunny, *m_skybox;
// Reference to shader program
GLuint program;
GLuint tex1, tex2, sky_tex;
TextureData ttex; // terrain


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
	printf("hit a key\n");
	switch (c)
	{
	case 'A':
	case 'a': b = -1.0; break;
	case 'D':
	case 'd': b = 1.0; break;
	case 'W':
	case 'w': a = 1.0; break;
	case 'S':
	case 's': a = -1.0; break;
	case 'Q':
	case 'q': siderotation = 1.0; break;
	case 'E':
	case 'e': siderotation = -1.0; break;
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
	printf("hit a key\n");
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
	glUseProgram(program);
	printError("init shader");
	
	
	//vertex buffer objects, used for uploading vertices 

	dumpInfo();

	m_sphere = LoadModelPlus("groundsphere.obj");
	m_bunny = LoadModelPlus("bunnyplus.obj");
	m_skybox = LoadModelPlus("skybox.obj");

	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniform1i(glGetUniformLocation(program, "tex"), 0); // Texture unit 0
	LoadTGATextureSimple("grass.tga", &tex1);
	LoadTGATextureSimple("skybox512.tga", &sky_tex);

	// Load terrain data

	LoadTGATextureData("fft-terrain.tga", &ttex);
	tm = GenerateTerrain(&ttex);
	printError("init terrain");

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
	l = VectorAdd(p, p_ws);

	//printVec3(p_ws);
	}else {			//kameran går längs texturen
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
		p = SetVector(p.x, calcHeight(p.x, p.z, tm, ttex.width) + 2, p.z);
		l = VectorAdd(p, p_ws);

	}

	//MOVEMENT END		

	//SKYBOX START
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sky_tex);
	glDisable(GL_DEPTH_TEST);
	
	total = S(1.0f, 1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
	
	mat4 lA = lookAtv(p, l, v);
	lA.m[3] = 0.0;
	lA.m[7] = 0.0;
	lA.m[11] = 0.0;
	glUniformMatrix4fv(glGetUniformLocation(program, "lookAt"), 1, GL_TRUE, lA.m); //är det fel här?

	
	texflag = 3;
	glUniform1fv(glGetUniformLocation(program, "texflag"), 1, &texflag);
	texflag = 0;
	DrawModel(m_skybox, program, "inPosition", "inNormal", "inTexCoord");
	
	glEnable(GL_DEPTH_TEST);
	//SKYBOX END	


	//specular shading start
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
	

	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "lookAt"), 1, GL_TRUE, lookAtv(p, l, v).m);
	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);

	glBindTexture(GL_TEXTURE_2D, tex1);		// Bind Our Texture tex1
	texflag = 1;
	glUniform1fv(glGetUniformLocation(program, "texflag"), 1, &texflag);
	DrawModel(tm, program, "inPosition", "inNormal", "inTexCoord");

	//boll
	trans = T( fabs(80.0*sin(t/30.0)) , calcHeight( fabs(80.0*sin(t/30.0)) , fabs(80.0*cos(t/30.0)) , tm , ttex.width), fabs(80.0*cos(t/30.0)));
	//trans = T(100, 10, 100);
	rot = Rx(0);
	total = Mult(trans, rot);
	total = Mult(total, S(1.0f, 1.0f, 1.0f));

	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
	//glUniformMatrix4fv(glGetUniformLocation(program, "lookAt"), 1, GL_TRUE, lookAtv(p, l, v).m);
	//glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);

	texflag = 0;
	glUniform1fv(glGetUniformLocation(program, "texflag"), 1, &texflag);
	DrawModel(m_sphere, program, "inPosition", "inNormal", "inTexCoord");


	//bunny
	GLfloat bx, bz;
	bx = (10 * sin(t / 5));
	bz = (10 * cos(t / 5));

	trans = T(100 + bx, calcHeight(100 + bx, 100 + bz, tm, ttex.width), 100 + bz);

	vec3 nrml = SetVector(tm->normalArray[((int)(100 + bx) + (int)(100 + bz) * ttex.width) * 3 + 0], 
						  tm->normalArray[((int)(100 + bx) + (int)(100 + bz) * ttex.width) * 3 + 1],
						  tm->normalArray[((int)(100 + bx) + (int)(100 + bz) * ttex.width) * 3 + 2]);
	
	//slope dependent rotation
	//antar (1,0,b) , (0, 2, c) -> theta = arctan(b/1), theta = arctan(c/2)
	//2*PI = 360 grader -> 1 grad = PI/180
	// theta = (PI/180) * arctan(a/x), a är komponent i planer, x är längd på komponent??
	// x = 1??

	//testa idé med normal = (1, 2, 3) -> RIP?

	vec3 rot_axis = CrossProduct(nrml, v);

	rot = ArbRotate(rot_axis, 0);
	total = Mult(trans, rot);
	total = Mult(total, S(1.0f, 1.0f, 1.0f));

	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
	DrawModel(m_bunny, program, "inPosition", "inNormal", "inTexCoord");


	/*camMatrix = lookAt(cam.x, cam.y, cam.z,
	lookAtPoint.x, lookAtPoint.y, lookAtPoint.z,
	0.0, 1.0, 0.0);
	modelView = IdentityMatrix();
	total = Mult(camMatrix, modelView);
	total = Mult(total, S(1.0f, 1.0f, 1.0f));*/


	printError("display 2");
	glFlush();
}

int main(int argc, char** argv)
{
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
