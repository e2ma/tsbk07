#version 150

out vec4 outcolor;
in vec3 exNormal; // phong

in float height;

uniform float t;	//psychic teapot
uniform float texflag;
uniform mat4 lookAt;

//textures
in vec2 outTexCoord;
uniform sampler2D texUnit;


//specular shading
uniform vec3 lightSourcesDirPosArr[4];	//dessa ska användas, måste göras om till view
uniform vec3 lightSourcesColorArr[4];
uniform float specularExponent[4];
uniform bool isDirectional[4];			//en if-sats beroende på om det är position eller directional

in vec3 exSurface;

void main(void)
{
	
	vec3 temp;
	vec3 color = vec3(0,0,0);
	float diffuse, specular;
	
	for(int i=0; i < 4;i++){ //for every light source and color
		float shade_temp;

		if(!isDirectional[i]){
			temp =  exSurface - vec3(lookAt*vec4(lightSourcesDirPosArr[i],1.0));  //göra om till view koordinater   vec4, vill behålla translationen!
		}else{
			temp =  mat3(lookAt)*lightSourcesDirPosArr[i];		//vill inte behålla translationen
		}

		vec4 light = vec4(temp ,1);
	

		// diffuse
		diffuse = dot(normalize(exNormal), vec3(normalize(-light)));
		diffuse = max(0.0, diffuse); // no negative light

		// specular
		vec3 r = reflect(vec3(normalize(light)), normalize(exNormal));
		vec3 v = normalize(-exSurface); // view direction
		specular = dot(r, v);

		if (specular > 0.0){
			specular = 1.0 * pow(specular, specularExponent[i]); // specularExponent[i
		}
		specular = max(specular, 0.0);
	
		//diffuse = 0;
		//specular = 0;
		shade_temp = 1.7*diffuse + 1.0*specular;
	
	
		color = color + shade_temp * lightSourcesColorArr[i];
		
	}
	if(texflag == 0){
		outcolor = vec4(color, 1.0);// för modeller som inte har texture, denna ger en färg istället
		//outcolor = texture(texUnit, outTexCoord) * vec4(color, 1.0);
	}else if(texflag == 1){
		outcolor = texture(texUnit, outTexCoord) * vec4(color, 1.0); 	//kanin
	}else if(texflag == 2){
		outcolor = texture(texUnit, outTexCoord); // * vec4(0.1*color, 0.2);	//gräs
	}else{
		outcolor = texture(texUnit, outTexCoord);// * vec4(color, 0.5);	// skybox
	}


}