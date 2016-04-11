#version 150

in vec3 inPosition;
in vec3 inNormal;
out vec3 exNormal;
out vec3 exSurface; // Phong (specular)
out float shade;

out float height;

uniform mat4 mdlMatrix;
uniform mat4 lookAt;
uniform mat4 projMatrix;

in vec2 inTexCoord;
out vec2 outTexCoord;

void main(void)
{
	outTexCoord = inTexCoord;
	height = 1;

	//Räkna fram normaler!?
	mat3 normalMatrix = mat3(lookAt*mdlMatrix); //normaler till View coordinates
	exNormal = normalMatrix * inNormal;
	//Räkna fram normaler!?


	if(inPosition.y == 0.0){
		height = 0;
	}

	//viewdirection till View coordinates
	exSurface = vec3(lookAt * mdlMatrix * vec4(inPosition, 1.0)); // Don't include projection here - we only want to go to view coordinates, 

	
	gl_Position = projMatrix * lookAt * mdlMatrix * vec4(inPosition, 1.0);

}