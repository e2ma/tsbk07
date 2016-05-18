#include "drawworld.h"
#include "terrain.c"
#include "camera.c"


// Globals
//specular shading START
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
mat4 rot, trans, total;
GLfloat a, b, px, py, mx, my, siderotation, uprotation = 0.0;
int camstyle = 0;
vec3 p = { 0,10,20 };
vec3 l = { 0,1,0 };
vec3 v = { 0.0f, 1.0f, 0.0f }; //upvector 
vec3 p_ws, p_ad;

// vertex array object
Model *m, *m2, *tm, *m_sphere, *m_bunny, *m_skybox, *water_terrain;

// Reference to shader program
GLuint program, skyboxshader;

//Textures
GLuint tex1, tex2, tex3, sky_tex, water_tex;
int terrain_width = 257; // Needs to be a value 2^(n) + 1 where n is a positive integer

void OnTimer(int value)
{
	glutPostRedisplay();
	glutTimerFunc(20, &OnTimer, value);
}

void init(void)
{
	
	// GL inits
	glClearColor(0.82, 0.94, 0.98, 0);
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
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, tex3);		// Bind Our Texture tex3

	glUniform1i(glGetUniformLocation(program, "tex1"), 0); // Texture unit 0
	glUniform1i(glGetUniformLocation(program, "tex2"), 1); // Texture unit 1
	glUniform1i(glGetUniformLocation(program, "tex3"), 2); // Texture unit 2
	glUniform1i(glGetUniformLocation(program, "water_tex"), 3); // Texture unit 3
	LoadTGATextureSimple("grass.tga", &tex1);
	LoadTGATextureSimple("dirt.tga", &tex2);
	LoadTGATextureSimple("sand2.tga", &tex3);
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

	//moves the camera
	movement();


	//SKYBOX START
	glUseProgram(skyboxshader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sky_tex);
	glDisable(GL_DEPTH_TEST);

	total = S(1.0f, 1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(skyboxshader, "mdlMatrix"), 1, GL_TRUE, total.m);

	mat4 lA = lookAtv(p, l, v);
	lA.m[3] = 0.0;	lA.m[7] = 0.0;	lA.m[11] = 0.0;
	
	glUniformMatrix4fv(glGetUniformLocation(skyboxshader, "lookAt"), 1, GL_TRUE, lA.m);
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

	//printVec3(p);


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex1);		// Bind Our Texture tex1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex2);		// Bind Our Texture tex2
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, tex3);		// sand

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

			trans = T(tx, 0, tz);
			rot = Rx(0);
			total = Mult(trans, rot);
			total = Mult(total, S(1.0f, 1.0f, 1.0f));
			// total = Mult(total, S(1.0f, sin(t), 1.0f)); // demo
			//total = Mult(total, S(1.0f, p.x*0.003+1.0f, 1.0f)); // demo

			glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
			glUniformMatrix4fv(glGetUniformLocation(program, "lookAt"), 1, GL_TRUE, lookAtv(p, l, v).m);
			glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
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

			glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, Mult(T(0, 0.5*sin(t), 0), total).m);
			glUniformMatrix4fv(glGetUniformLocation(program, "lookAt"), 1, GL_TRUE, lookAtv(p, l, v).m);
			glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
			DrawModel(water_terrain, program, "inPosition", "inNormal", "inTexCoord");
		}
	}


	glDisable(GL_BLEND);
	

	printError("display 2");
	glFlush();
}

int main(int argc, char** argv)
{
	printf("** New terrain generated **\nW: Forward\nS: Back\nA: Left\nD: Right\nQ: Rotate left\nE: Rotate right\nI: Look up\nK: Look down\nC: Walk along terrain\nV: Fly mode\n");

	//seed the rand function with a unique number
	srand(time(NULL));

	glutInitWindowSize(1000, 600);
	glutInit(&argc, argv);
	glutCreateWindow("Terrain");

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
