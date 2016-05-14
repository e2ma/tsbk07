#include "terrain.h"

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
		for (z = 0; z < width; z++) {

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