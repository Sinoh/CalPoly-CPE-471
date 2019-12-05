#version 330 core
out vec4 color;
in vec3 fragPos;
void main()
{	
	vec3 normal = normalize(vec3(0.4f,0.7f,1.0f));
	color.rgb = normal;
	color.a= 1 - normal.z;	//transparency: 1 .. 100% NOT transparent
	if (fragPos.y < 0){
		color.rgb = normal + fragPos.y;
	}
	if (fragPos.y > 0.7){
		color.rgb = normal + fragPos.y;
	}

}

