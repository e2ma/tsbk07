#version 150

in vec3 inPosition;

uniform mat4 mdlMatrix;
uniform mat4 lookAt;
uniform mat4 projMatrix;

in vec2 inTexCoord;
out vec2 outTexCoord;

void main(void)
{
	outTexCoord = inTexCoord;

	gl_Position = projMatrix * lookAt * mdlMatrix * vec4(inPosition, 1.0);

}