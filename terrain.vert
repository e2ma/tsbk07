#version 150

in vec3 inPosition;
out vec3 exPosition;
in vec3 inNormal;
out vec3 exNormal;
out vec3 exSurface; // Phong (specular)
out float shade;
uniform float texflag;

out float height;

uniform mat4 mdlMatrix;
uniform mat4 lookAt;
uniform mat4 projMatrix;

in vec2 inTexCoord;
out vec2 outTexCoord;

void main(void)
{
	exPosition = inPosition;
	outTexCoord = inTexCoord;
	height = 1;

	// Calculate normal vectors
	exNormal = inverse(transpose(mat3(lookAt*mdlMatrix))) * inNormal;

	// viewdirection to View coordinates
	exSurface = vec3(lookAt * mdlMatrix * vec4(inPosition, 1.0)); // Don't include projection here - we only want to go to view coordinates, 

	gl_Position = projMatrix * lookAt * mdlMatrix * vec4(inPosition, 1.0);

}