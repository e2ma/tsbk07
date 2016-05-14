#version 150
out vec4 outcolor;
//textures
in vec2 outTexCoord;
uniform sampler2D texUnit;

void main(void)
{
	outcolor = texture(texUnit, outTexCoord);	

}