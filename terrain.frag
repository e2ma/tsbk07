#version 150

in vec3 exPosition;
out vec4 outcolor;
in vec3 exNormal; // phong
in float height;
in vec3 exSurface;
in vec3 exWorldPosition;

//uniform vec3 camPos;
uniform float texflag;
uniform mat4 lookAt;


//textures
in vec2 outTexCoord;
uniform sampler2D tex1, tex2, water_tex;

//specular shading
uniform vec3 lightSourcesDirPosArr[4];	
uniform vec3 lightSourcesColorArr[4];
uniform float specularExponent[4];
uniform bool isDirectional[4];		


void main(void)
{
	
	vec3 temp;
	vec3 color = vec3(0,0,0);
	float diffuse, specular;

	//multitexture
	vec4 t1 = texture(tex1, outTexCoord);
	vec4 t2 = texture(tex2, outTexCoord);
	
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
		//diffuse = dot(vec3(0,1,0), vec3(normalize(-light)));
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

	// set apha value to create fog 
	float alpha, far, near; 
	alpha = 1.0;
	far = -285;
	near = -235;
	if(exSurface.z >= near) { alpha = 1.0; }
	else if(exSurface.z < far) { alpha = 0.0; }
	else{ alpha = (1.0/(near - far)) * (exSurface.z - far);}


	if(texflag == 1){ // water
		outcolor = texture(water_tex, outTexCoord) * vec4(color, 0.7*alpha); 	
	}else if(texflag == 2){ // terrain
		
		// multitextured terrain 
		float t1_shade, t2_shade, low, high;
		low = 20;
		high = 25;
	
		if(exWorldPosition.y <= low) { t2_shade = 0.0; }
		else if(exWorldPosition.y >= high) { t2_shade = 1.0; }
		else{ t2_shade = (1.0/(high-low)) * (exWorldPosition.y - low);}
		t1_shade = 1 - t2_shade;
	

		outcolor = (t1 * t1_shade + t2 * t2_shade) * vec4(color, alpha);

	}



}